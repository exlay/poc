#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <error.h>
#include <dlfcn.h>
#include <linux/if.h>
#include <sys/ioctl.h>

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <arpa/inet.h>

#include "exlay.h"
#include "protocol.h"

#define MACSTR "86:fd:5e:fb:fa:ce"
#define STRING "hello, world\n Cybozu Labyouth is SAIKOU\n"
#define SRCIP "192.168.10.4"

// 5e:a6:27:62:5f:9c

int main(int argc, char **argv)
{
	int exsock;
	uint8_t mac[6] = {0x5e, 0xa6, 0x27, 0x62, 0x5f, 0x9c};
	uint8_t buf[100];
	uint8_t type[2] = {0x88, 0xb5};

	if (argc != 2) {
		fprintf(stderr, "usage: bind_test <ifname>\n");
		exit(1);
	}

	int es1;
	int es2;
	int es3;
	int es4;
	int ret;
	uint8_t local_mac[6] = {0};
	uint8_t dstmac[6] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc};
	uint8_t upper[2] = {0x88, 0xb5};
	uint8_t type_arp[2] = {0x08,0x06};
	uint8_t proto_exp = 253;
	struct in_addr srcip;
	inet_pton(AF_INET, SRCIP, &srcip);

	int fd;
	struct ifreq ifr2;

	fd = socket(AF_INET, SOCK_DGRAM, 0);

	ifr2.ifr_addr.sa_family = AF_INET;
	strncpy(ifr2.ifr_name, argv[1], IFNAMSIZ-1);

	ioctl(fd, SIOCGIFHWADDR, &ifr2);

	close(fd);

	memcpy(local_mac, ifr2.ifr_hwaddr.sa_data, 6);
	exsock = ex_create_stack(1);
	assert(exsock > 0);
	ret = ex_set_binding(exsock, 1, "test_ethernet", local_mac, 6, type, 2);
	assert(ret == 0);
	ret = ex_set_remote(exsock, 1, dstmac, 6);
	assert(ret == 0);
	ret = ex_bind_stack(exsock);
	assert(ret == 0);
	ret = ex_dial_stack(exsock);
	assert(ret == 0);
	ret = ex_send_stack(exsock, STRING, strlen(STRING), 0);
	assert(ret >= 0);
	while (1) {
		memset(buf, 0, sizeof(buf));
		ret = ex_recv_stack(exsock, buf, 100, 0);
		printf("recv: %s (%d byte))", buf, ret);
	}


	return 0;
}
