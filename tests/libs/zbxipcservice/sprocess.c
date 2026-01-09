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
#include "mcdef.h"
int main()
{
        char *err = NULL;

        zbx_ipc_service_init_env("/tmp", &err);

#ifndef ASYNC_TEST
        zbx_ipc_socket_t sock;
        zbx_ipc_message_t resp;

        if (SUCCEED != zbx_ipc_socket_open(&sock, "demo", 5, &err)) {
                fprintf(stderr, "open sync socket failed: %s\n", err);
                return FAIL;
        }

        const char payload[] = "sync=hello";
        if (FAIL == zbx_ipc_socket_write(&sock, 1 /*自定义消息码*/, (const unsigned char *)payload, sizeof(payload))) {
                fprintf(stderr, "sync write failed\n");
                zbx_ipc_socket_close(&sock);
                return FAIL;
        }

        zbx_ipc_message_init(&resp);
        if (SUCCEED == zbx_ipc_socket_read(&sock, &resp))
                printf("type=%d, size=%d, %s\n", resp.code, resp.size, resp.data);

        zbx_ipc_message_clean(&resp);
        zbx_ipc_socket_close(&sock);
#else
        zbx_ipc_async_socket_t asocket;
        zbx_ipc_message_t *resp = NULL;

        if (SUCCEED != zbx_ipc_async_socket_open(&asocket, "demo", 5, &err)) {
                fprintf(stderr, "open async socket failed: %s\n", err);
                return FAIL;
        }

        const unsigned char payload[] = "async=hello";
        zbx_ipc_async_socket_send(&asocket, 2, payload, sizeof(payload));

        if (SUCCEED != zbx_ipc_async_socket_flush(&asocket, 5))
                fprintf(stderr, "async flush failed\n");

        if (SUCCEED == zbx_ipc_async_socket_recv(&asocket, 0, &resp) && NULL != resp) {
                printf("type=%d, size=%d, %s\n", resp->code, resp->size, resp->data);
                zbx_ipc_message_free(resp);
        }

        zbx_ipc_async_socket_close(&asocket);
#endif

        zbx_ipc_service_free_env();
}
