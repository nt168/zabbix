/*
** Copyright (C) 2001-2025 Zabbix SIA
**
** This program is free software: you can redistribute it and/or modify it under the terms of
** the GNU Affero General Public License as published by the Free Software Foundation, version 3.
**
** This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
** without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU Affero General Public License for more details.
**
** You should have received a copy of the GNU Affero General Public License along with this program.
** If not, see <https://www.gnu.org/licenses/>.
*/

#include "zbxcommon.h"
#include "zbxipcservice.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char *service_name;
static const char *peer_name;

static void     fail(const char *context, const char *error)
{
        if (NULL != error)
                fprintf(stderr, "%s: %s\n", context, error);
        else
                fprintf(stderr, "%s\n", context);

        exit(EXIT_FAILURE);
}

static void     send_reply(zbx_ipc_service_t *service, zbx_ipc_client_t *client, const zbx_ipc_message_t *message)
{
        const char              *payload = NULL;
        zbx_ipc_client_t        *found;

        switch (message->code)
        {
                case 1:
                        payload = "sync-pong";
                        break;
                case 2:
                        payload = "async-pong";
                        break;
                case 3:
                        payload = "exchange-pong";
                        break;
                default:
                        payload = "unknown";
                        break;
        }

        zbx_ipc_client_set_userdata(client, &service_name);
        found = zbx_ipc_client_by_id(service, zbx_ipc_client_id(client));

        if (client != found || SUCCEED != zbx_ipc_client_send(client, message->code + 100,
                        (const unsigned char *)payload, strlen(payload) + 1))
        {
                fail("Cannot send reply", NULL);
        }

        zbx_ipc_client_addref(client);
        zbx_ipc_client_release(client);
}

static void     handle_requests(zbx_ipc_service_t *service)
{
        int                     received = 0;
        zbx_ipc_client_t        *client;
        zbx_ipc_message_t       *message;
        zbx_timespec_t          timeout = {.sec = 5, .ns = 0};

        while (received < 3)
        {
                int state;

                state = zbx_ipc_service_recv(service, &timeout, &client, &message);
                if (ZBX_IPC_RECV_TIMEOUT == state)
                        continue;

                if (NULL == client || NULL == message)
                        fail("Incomplete IPC payload", NULL);

                if (SUCCEED != zbx_ipc_client_connected(client))
                        fail("Client disconnected", NULL);

                if (0 > zbx_ipc_client_get_fd(client))
                        fail("Invalid socket descriptor", NULL);

                send_reply(service, client, message);
                zbx_ipc_message_free(message);

                zbx_ipc_service_recv(service, &(zbx_timespec_t){.sec = 0, .ns = 0}, &client, &message);
                zbx_ipc_client_close(client);

                received++;
        }
}

static void     send_to_peer(void)
{
        zbx_ipc_socket_t        csocket;
        zbx_ipc_message_t       message;
        char                    *error = NULL;

        if (SUCCEED != zbx_ipc_socket_open(&csocket, peer_name, 5, &error))
                fail("Cannot open peer socket", error);

        if (FAIL == zbx_ipc_socket_write(&csocket, 200, (const unsigned char *)"peer-ping", 10))
                fail("Cannot send peer ping", NULL);

        zbx_ipc_message_init(&message);
        if (FAIL == zbx_ipc_socket_read(&csocket, &message))
                fail("Cannot read peer response", NULL);

        zbx_ipc_message_clean(&message);
        zbx_ipc_socket_close(&csocket);
}

int     main(int argc, char **argv)
{
        char                    *error = NULL;
        zbx_ipc_service_t       service;

        if (4 != argc)
                fail("Usage: sprocess <root> <service> <peer>", NULL);

        service_name = argv[2];
        peer_name = argv[3];

        zbx_init_library_ipcservice(ZBX_PROGRAM_TYPE_SERVER);

        if (SUCCEED != zbx_ipc_service_init_env(argv[1], &error))
                fail("Cannot init IPC environment", error);

        if (FAIL == zbx_ipc_service_start(&service, service_name, &error))
                fail("Cannot start IPC service", error);

        handle_requests(&service);
        send_to_peer();

        zbx_ipc_service_close(&service);
        zbx_ipc_service_free_env();

        return EXIT_SUCCESS;
}
