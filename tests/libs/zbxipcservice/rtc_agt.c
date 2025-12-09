#include "zbxcommon.h"
#include "zbxrtc.h"
#include "zbx_rtc_constants.h"

#include <stdio.h>

#define CLIENT_PROCESS_TYPE    ZBX_PROCESS_TYPE_MAIN
#define CLIENT_MESSAGE_CODE    5001

int main(void)
{
    char *error = NULL;
    zbx_ipc_async_socket_t rtc;
    zbx_uint32_t messages[] = {CLIENT_MESSAGE_CODE};

    if (SUCCEED != zbx_ipc_service_init_env("/tmp", &error))
    {
        fprintf(stderr, "init ipc env failed: %s\n", error);
        zbx_free(error);
        return FAIL;
    }

    if (SUCCEED != zbx_ipc_async_socket_open(&rtc, ZBX_IPC_SERVICE_RTC, 5, &error))
    {
        fprintf(stderr, "open rtc socket failed: %s\n", error);
        zbx_free(error);
        return FAIL;
    }

    zbx_rtc_subscribe(CLIENT_PROCESS_TYPE, 0, messages, ARRSIZE(messages), 5, &rtc);

    printf("rtc agent subscribed, waiting for CLIENT messages...\n");

    for (;;)
    {
        zbx_uint32_t cmd;
        unsigned char *data;

        if (SUCCEED != zbx_rtc_wait(&rtc, NULL, &cmd, &data, 1))
        {
            fprintf(stderr, "rtc wait failed\n");
            break;
        }

        if (0 == cmd)
            continue;

        if (CLIENT_MESSAGE_CODE == cmd)
        {
            printf("received client message: %s\n", data);
            zbx_free(data);
            break;
        }

        zbx_free(data);
    }

    zbx_ipc_async_socket_close(&rtc);
    return SUCCEED;
}
