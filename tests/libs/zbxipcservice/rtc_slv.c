#include "zbxcommon.h"
#include "zbxrtc.h"
#include "zbx_rtc_constants.h"
#include "zbxlog.h"
#include "zbxnix.h"
#include <stdio.h>
#include <string.h>


#define CLIENT_PROCESS_TYPE    ZBX_PROCESS_TYPE_MAIN
#define CLIENT_MESSAGE_CODE    5001
ZBX_GET_CONFIG_VAR2(const char*, const char*, zbx_progname, NULL)
#define zbx_serialize_str_null(buffer)	(memset(buffer, 0, sizeof(zbx_uint32_t)), sizeof(zbx_uint32_t))

#define zbx_serialize_value(buffer, value) (memcpy(buffer, &value, sizeof(value)), sizeof(value))
#define zbx_serialize_int(buffer, value) (memcpy(buffer, (const int *)&value, sizeof(int)), sizeof(int))
#define zbx_serialize_str(buffer, value, len)						\
	(										\
		0 == len ? zbx_serialize_str_null(buffer) :				\
		(									\
			memcpy(buffer, (zbx_uint32_t *)&len, sizeof(zbx_uint32_t)),	\
			memcpy(buffer + sizeof(zbx_uint32_t), value, len),		\
			len + sizeof(zbx_uint32_t)					\
		)									\
	)

int     zbx_rtc_notify_generic(zbx_ipc_async_socket_t *rtc, unsigned char process_type, int process_num,
                zbx_uint32_t code, const char *data, zbx_uint32_t size)
{
        unsigned char   *notify_data, *ptr;
        zbx_uint32_t    notify_data_size;
        int             ret = FAIL;

        /* <process type:uchar><process num:int><code:uint32><data size:uint32><data> */
        notify_data_size = (zbx_uint32_t)(sizeof(process_type) + sizeof(process_num) +  sizeof(code) + 2 * sizeof(size))
                        + size;
        notify_data = (unsigned char *)zbx_malloc(NULL, notify_data_size);

        ptr = notify_data;
        ptr += zbx_serialize_value(ptr, process_type);
        ptr += zbx_serialize_int(ptr, process_num);
        ptr += zbx_serialize_value(ptr, code);
        ptr += zbx_serialize_value(ptr, size);
        (void)zbx_serialize_str(ptr, data, size);

        if (FAIL == zbx_ipc_async_socket_send(rtc, ZBX_RTC_NOTIFY, notify_data, notify_data_size))
        {
                zabbix_log(LOG_LEVEL_CRIT, "cannot send %s notification", get_process_type_string(process_type));
                goto out;
        }

        ret = (int)notify_data_size;
out:
        zbx_free(notify_data);

        return ret;
}


int main(void)
{
    char *error = NULL;
    zbx_ipc_async_socket_t rtc;
    const char payload[] = "hello from rtc slave";

     zbx_init_library_common(zbx_log_impl, get_zbx_progname, zbx_backtrace);
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

    if (0 >= zbx_rtc_notify_generic(&rtc, CLIENT_PROCESS_TYPE, 0, CLIENT_MESSAGE_CODE, payload,
                    (zbx_uint32_t)sizeof(payload)))
    {
        fprintf(stderr, "send notification failed\n");
        zbx_ipc_async_socket_close(&rtc);
        return FAIL;
    }

    if (SUCCEED != zbx_ipc_async_socket_flush(&rtc, 5))
        fprintf(stderr, "flush notification failed\n");

    printf("rtc slave sent notification\n");

    zbx_ipc_async_socket_close(&rtc);
    return SUCCEED;
}
