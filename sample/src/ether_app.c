#include <stdio.h>
#include <assert.h>
#include "exlay.h"
#include "protocol.h"

#define MACSTR "86:fd:5e:fb:fa:ce"

int main(void)
{
	int ep;
	int ret;
	uint8_t mac[6] = {0x86, 0xfd, 0x5e, 0xfb, 0xfa, 0xce};
	int size = 100;

	ep = ex_create_stack(1);
	assert(ep > 0);
	ret = ex_set_binding(ep, 1, "test_ethernet", mac, NULL);
	assert(ret == 0);
	ret = ex_set_remote(ep, 1, NULL);
	assert(ret == 0);
	ret = ex_bind_stack(ep);
	assert(ret == 0);
	ret = ex_dial_stack(ep);
	assert(ret == 0);
	ret = ex_send_stack(ep, size);
	assert(ret >= 0);
	ret = ex_recv_stack(ep, size);
	assert(ret >= 0);
	ret = ex_close_stack(ep);
	assert(ret == 0);


	return 0;
}
