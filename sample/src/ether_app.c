#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "exlay.h"
#include "protocol.h"

#define MACSTR "86:fd:5e:fb:fa:ce"
#define STRING "hello, world\n"

int main(void)
{
	int exsock;
	int ret;
	uint8_t mac[6] = {0x86, 0xfd, 0x5e, 0xfb, 0xfa, 0xce};
	uint8_t dstmac[6] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc};

	exsock = ex_create_stack(1);
	assert(exsock > 0);
	ret = ex_set_binding(exsock, 1, "test_ethernet", mac, 6, 0);
	assert(ret == 0);
	ret = ex_set_remote(exsock, 1, dstmac, 6);
	assert(ret == 0);
	ret = ex_bind_stack(exsock);
	assert(ret == 0);
	ret = ex_dial_stack(exsock);
	assert(ret == 0);
	ret = ex_send_stack(exsock, STRING, strlen(STRING), 0);
	assert(ret >= 0);
	ret = ex_recv_stack(exsock, STRING, strlen(STRING), 0);
	assert(ret >= 0);
	ret = ex_close_stack(exsock);
	assert(ret == 0);


	return 0;
}
