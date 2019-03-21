#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "exlay.h"
#include "protocol.h"

#define MACSTR "86:fd:5e:fb:fa:ce"
#define STRING "hello, world\n"
#define SRCIP "192.168.10.4"

#define ETHTYPE_ARP 0x0806
#define ETHTYPE_IP4 0x0800
#define ETHTYPE_IP6 0x86dd

int main(void)
{
	int es1;
	int es2;
	int es3;
	int es4;
	int ret;
	uint8_t mac[6] = {0x86, 0xfd, 0x5e, 0xfb, 0xfa, 0xce};
	uint8_t dstmac[6] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc};
	struct in_addr srcip;
	inet_pton(AF_INET, SRCIP, &srcip);
	
	es1 = ex_create_stack(1);
	assert(es1 > 0);
	ret = ex_set_binding(es1, 1, "test_ethernet", mac, 6, htons(0x88b5));
	assert(ret == 0);
	ret = ex_set_remote(es1, 1, dstmac, 6);
	assert(ret == 0);
	ret = ex_bind_stack(es1);
	assert(ret == 0);

	es2 = ex_create_stack(3);
	assert(es2 > 0);
	ret = ex_set_binding(es2, 1, "test_ethernet", mac, 6, htons(ETHTYPE_ARP));
	assert(ret == 0);
	ret = ex_set_binding(es2, 2, "test_arp", NULL, 0, 0);
	assert(ret == 0);
	ret = ex_set_binding(es2, 3, "test_ipv4", &srcip, 4, htons(253));
	assert(ret == 0);
	ret = ex_bind_stack(es2);
	assert(ret == 0);
		
	es3 = ex_create_stack(2);
	assert(es2 > 0);
	ret = ex_set_binding(es3, 1, "test_ethernet", mac, 6, htons(ETHTYPE_IP4));
	assert(ret == 0);
	ret = ex_set_binding(es3, 2, "test_ipv4", &srcip, 4, htons(IPPROTO_ICMP));
	assert(ret == 0);
	ret = ex_bind_stack(es3);
	assert(ret == 0);

	es4 = ex_create_stack(4);
	assert(es2 > 0);
	ret = ex_set_binding(es4, 1, "test_ethernet", mac, 6, htons(ETHTYPE_IP4));
	assert(ret == 0);
	ret = ex_set_binding(es4, 2, "test_ipv4", &srcip, 4, htons(97));
	assert(ret == 0);
	ret = ex_set_binding(es4, 3, "test_ethernet", mac, 6, htons(ETHTYPE_IP4));
	assert(ret == 0);
	ret = ex_set_binding(es4, 4, "test_ipv4", &srcip, 4, htons(IPPROTO_ICMP));
	assert(ret == 0);
	ret = ex_bind_stack(es4);
	assert(ret == 0);

	return 0;
}
