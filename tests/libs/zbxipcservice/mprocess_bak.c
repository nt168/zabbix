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
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SERVICE_MPROCESS "ipc_mprocess"
#define SERVICE_SPROCESS "ipc_sprocess"

static void     fail(const char *context, const char *error)
{
        if (NULL != error)
                fprintf(stderr, "%s: %s\n", context, error);
        else
                fprintf(stderr, "%s\n", context);

        exit(EXIT_FAILURE);
}

static void     ensure(int condition, const char *context)
{
        if (0 == condition)
                fail(context, NULL);
}

static void     cleanup_path(const char *path)
{
        if (NULL != path)
                rmdir(path);
}

static void     ensure_service_ready(const char *service_name)
{
        char                    *error = NULL;
        zbx_ipc_socket_t        csocket;
        int                     attempts;

        for (attempts = 0; attempts < 50; attempts++)
        {
                if (SUCCEED == zbx_ipc_socket_open(&csocket, service_name, 1, &error))
                {
                        zbx_ipc_socket_close(&csocket);
                        return;
                }

                zbx_free(error);
                error = NULL;
                usleep(100000);
        }

        fail("Service did not start", error);
}

static void     handle_response(zbx_ipc_message_t *message, zbx_uint32_t expected_code, const char *expected)
{
        char    *formatted = NULL;

        ensure(NULL != message, "Missing IPC message");
        ensure(message->code == expected_code, "Unexpected response code");
        ensure(NULL != message->data, "Missing IPC payload");
        ensure(0 == strcmp((char *)message->data, expected), "Unexpected IPC payload");

        zbx_ipc_message_format(message, &formatted);
        zbx_free(formatted);
}

static void     communicate_with_server(void)
{
        char            *error = NULL;
        zbx_ipc_socket_t csocket;
        zbx_ipc_message_t message;

        if (SUCCEED != zbx_ipc_socket_open(&csocket, SERVICE_SPROCESS, 5, &error))
                fail("Cannot open synchronous socket", error);

        ensure(SUCCEED == zbx_ipc_socket_connected(&csocket), "Synchronous socket not connected");

        if (FAIL == zbx_ipc_socket_write(&csocket, 1, (const unsigned char *)"sync-ping", 10))
                fail("Cannot write sync payload", NULL);

        zbx_ipc_message_init(&message);
        if (FAIL == zbx_ipc_socket_read(&csocket, &message))
                fail("Cannot read sync response", NULL);

        handle_response(&message, 101, "sync-pong");
        zbx_ipc_message_clean(&message);
        zbx_ipc_socket_close(&csocket);
}

static void     communicate_async_manual(void)
{
        char                    *error = NULL;
        zbx_ipc_async_socket_t  asocket;
        zbx_ipc_message_t       *message = NULL;
        int                     rc;

        if (SUCCEED != zbx_ipc_async_socket_open(&asocket, SERVICE_SPROCESS, 5, &error))
                fail("Cannot open async socket", error);

        ensure(SUCCEED == zbx_ipc_async_socket_connected(&asocket), "Async socket not connected");

        if (FAIL == zbx_ipc_async_socket_send(&asocket, 2, (const unsigned char *)"async-ping", 11))
                fail("Cannot send async payload", NULL);

        rc = zbx_ipc_async_socket_check_unsent(&asocket);
        ensure(SUCCEED == rc || FAIL == rc, "Unexpected check_unsent result");

        if (SUCCEED != zbx_ipc_async_socket_flush(&asocket, 5))
                fail("Async flush failed", NULL);

        if (SUCCEED != zbx_ipc_async_socket_recv(&asocket, 5, &message))
                fail("Async receive failed", NULL);

        handle_response(message, 102, "async-pong");
        zbx_ipc_message_free(message);

        ensure(SUCCEED == zbx_ipc_async_socket_connected(&asocket), "Async socket disconnected too early");

        zbx_ipc_async_socket_close(&asocket);
}

static void     communicate_async_exchange(void)
{
        char            *error = NULL;
        unsigned char   *out = NULL;

        if (SUCCEED != zbx_ipc_async_exchange(SERVICE_SPROCESS, 3, 5, (const unsigned char *)"exchange-ping",
                        14, &out, &error))
        {
                fail("Async exchange failed", error);
        }

        ensure(0 == strcmp((char *)out, "exchange-pong"), "Unexpected exchange payload");
        zbx_free(out);
}

static void     receive_from_peer(zbx_ipc_service_t *service)
{
        zbx_ipc_client_t        *client = NULL;
        zbx_ipc_message_t       *message = NULL;
        zbx_timespec_t          timeout = {.sec = 5, .ns = 0};
        int                     state;

        state = zbx_ipc_service_recv(service, &timeout, &client, &message);
        ensure(ZBX_IPC_RECV_TIMEOUT != state, "No peer message received");
        ensure(NULL != client && NULL != message, "Missing peer data");

        ensure(SUCCEED == zbx_ipc_client_connected(client), "Peer disconnected");
        ensure(0 <= zbx_ipc_client_get_fd(client), "Invalid peer FD");

        zbx_ipc_client_set_userdata(client, service);
        ensure(service == zbx_ipc_client_get_userdata(client), "Userdata mismatch");

        ensure(client == zbx_ipc_client_by_id(service, zbx_ipc_client_id(client)), "Client lookup failed");

        zbx_ipc_client_addref(client);
        ensure(SUCCEED == zbx_ipc_client_send(client, 201, (const unsigned char *)"peer-pong", 10),
                        "Cannot reply to peer");
        zbx_ipc_client_release(client);

        char    *formatted = NULL;

        zbx_ipc_message_format(message, &formatted);
        zbx_free(formatted);
        zbx_ipc_message_free(message);

        zbx_ipc_client_t        *flush_client = NULL;
        zbx_ipc_message_t       *flush_message = NULL;

        zbx_ipc_service_recv(service, &(zbx_timespec_t){.sec = 0, .ns = 0}, &flush_client, &flush_message);

        if (NULL != flush_message)
        {
                zbx_ipc_message_free(flush_message);
                zbx_ipc_client_close(flush_client);
        }

        zbx_ipc_client_close(client);
        zbx_ipc_service_alert(service);
}

int     main(void)
{
        char                    temp_template[] = "/tmp/zbx_ipc_testXXXXXX";
        char                    *error = NULL;
        zbx_ipc_service_t       service;
        pid_t                   pid;
        int                     status;

        if (NULL == mkdtemp(temp_template))
                fail("Cannot create temporary directory", strerror(errno));

        zbx_init_library_ipcservice(ZBX_PROGRAM_TYPE_SERVER);

        if (SUCCEED != zbx_ipc_service_init_env(temp_template, &error))
                fail("Cannot init IPC env", error);

        if (FAIL == zbx_ipc_service_start(&service, SERVICE_MPROCESS, &error))
                fail("Cannot start IPC service", error);

        pid = fork();
        if (-1 == pid)
                fail("Cannot fork", strerror(errno));
        else if (0 == pid)
        {
                execl("./sprocess", "sprocess", temp_template, SERVICE_SPROCESS, SERVICE_MPROCESS, (char *)NULL);
                perror("execl");
                _exit(EXIT_FAILURE);
        }

        ensure_service_ready(SERVICE_SPROCESS);

        communicate_with_server();
        communicate_async_manual();
        communicate_async_exchange();
        receive_from_peer(&service);

        if (-1 == waitpid(pid, &status, 0) || !WIFEXITED(status) || EXIT_SUCCESS != WEXITSTATUS(status))
                fail("sprocess failed", NULL);

        zbx_ipc_service_close(&service);
        zbx_ipc_service_free_env();
        cleanup_path(temp_template);

        return EXIT_SUCCESS;
}
