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

	zbx_ipc_socket_t sock;
	char *err = NULL;
	zbx_ipc_message_t resp;
 	zbx_ipc_service_init_env("/tmp", &err);
	zbx_ipc_socket_open(&sock, "demo", 5, &err); // 5s 连接超时
	const char payload[] = "helloxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\
				========================================================================================================================";
	zbx_ipc_socket_write(&sock, 1 /*自定义消息码*/, (const unsigned char *)payload, sizeof(payload));

	if (SUCCEED == zbx_ipc_socket_read(&sock, &resp)) {
    		// resp.code / resp.data / resp.size 即服务端回显
    		zbx_ipc_message_free(&resp);
	}
	zbx_ipc_socket_close(&sock);

}
