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


int main()
{
	zbx_ipc_service_t svc = {0};
	char *err = NULL;
//	char *socket_path;
	zbx_ipc_service_init_env("/tmp", &err);      // 选择有读写权限的目录
//	socket_path = ipc_make_path("demo", &err);
	zbx_ipc_service_start(&svc, "demo", &err);   // 会生成 /tmp/zabbix_demo.sock

	for (;;) {
		zbx_timespec_t to = {0, 0};              // 1s 超时循环
    		zbx_ipc_client_t *cli;
    		zbx_ipc_message_t *msg;

    		if (ZBX_IPC_RECV_TIMEOUT == zbx_ipc_service_recv(&svc, &to, &cli, &msg))
        		continue;                            // 等待客户端
    		if (NULL == msg) {                       // 客户端断开
        		zbx_ipc_client_release(cli);
        		continue;
    		}

    		// 处理消息并回显
		printf("%s\n", msg->data);
    		zbx_ipc_client_send(cli, msg->code, msg->data, msg->size);
    		zbx_ipc_message_free(msg);
    		zbx_ipc_client_release(cli);             // recv 已经 addref 过
	}

	zbx_ipc_service_close(&svc);
	zbx_ipc_service_free_env();
}	
