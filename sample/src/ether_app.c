#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "exlay.h"
#include "protocol.h"

#define MACSTR "86:fd:5e:fb:fa:ce"
#define STRING "hello, world\n"

int main(void)
{
	int ep;
	int ret;
	uint8_t mac[6] = {0x86, 0xfd, 0x5e, 0xfb, 0xfa, 0xce};
	uint8_t dstmac[6] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc};
	int size = 100;

	ep = ex_create_stack(1);
	assert(ep > 0);
	ret = ex_set_binding(ep, 1, "test_ethernet", mac, NULL);
	assert(ret == 0);
	ret = ex_set_remote(ep, 1, NULL);
	assert(ret == 0);
	ret = ex_set_remote(ep, 1, dstmac);
	assert(ret == 0);
	ret = ex_set_remote(ep, 1, dstmac);
	assert(ret == 0);
	ret = ex_set_remote(ep, 1, NULL);
	assert(ret == 0);
	ret = ex_bind_stack(ep);
	assert(ret == 0);
	ret = ex_dial_stack(ep);
	assert(ret == 0);
	ret = ex_send_stack(ep, STRING, strlen(STRING));
	assert(ret >= 0);
	ret = ex_recv_stack(ep, STRING, strlen(STRING));
	assert(ret >= 0);
	ret = ex_close_stack(ep);
	assert(ret == 0);


	return 0;
}
