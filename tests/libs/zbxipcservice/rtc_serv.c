#include "zbxcommon.h"
#include "zbxrtc.h"
#include "zbx_rtc_constants.h"

#include <stdio.h>

#define CLIENT_PROCESS_TYPE    ZBX_PROCESS_TYPE_MAIN
#define CLIENT_MESSAGE_CODE    5001

int main(void)
{
    char *error = NULL;
    zbx_rtc_t rtc;
    zbx_timespec_t timeout = {1, 0};

    if (SUCCEED != zbx_ipc_service_init_env("/tmp", &error))
    {
        fprintf(stderr, "init ipc env failed: %s\n", error);
        zbx_free(error);
        return FAIL;
    }

    if (SUCCEED != zbx_rtc_init(&rtc, &error))
    {
        fprintf(stderr, "rtc service start failed: %s\n", error);
        zbx_free(error);
        return FAIL;
    }

    printf("rtc service started, waiting for requests...\n");

    for (;;)
    {
        zbx_ipc_client_t *client = NULL;
        zbx_ipc_message_t *message = NULL;
        int ret;

        ret = zbx_ipc_service_recv(&rtc.service, &timeout, &client, &message);

        if (ZBX_IPC_RECV_TIMEOUT == ret)
            continue;

        if (NULL != message)
        {
            zbx_rtc_dispatch(&rtc, client, message, NULL);
            zbx_ipc_message_free(message);
        }

        if (NULL != client)
            zbx_ipc_client_release(client);
    }

    return SUCCEED;
}
