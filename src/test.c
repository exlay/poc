#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/udp.h>

#include "exlay.h"

int main(void)
{
	uint16_t udp_port = htons(40000);
	uint8_t ipv4_addr[4];
	inet_pton(AF_INET, "192.168.10.11", ipv4_addr);
	uint8_t ipv6_addr[16];
	inet_pton(AF_INET6, "2001:200:0:6813:abcd::3", ipv6_addr);
	uint8_t ipv4_protocol = htons(17);
	uint8_t ipv6_protocol = htons(17);
	uint16_t ether_type = htons(0x0800);
	
	struct ifreq ifr;
	int fd;
	fd = socket(AF_INET, SOCK_DGRAM, 0);

	ifr.ifr_addr.sa_family = AF_INET;
	strncpy(ifr.ifr_name, "tap0", IFNAMSIZ-1);
	
	ioctl(fd, SIOCGIFHWADDR, &ifr);
	close(fd);

	struct exlay_ep_node root;
	struct exlay_ep_node ether;
	struct exlay_ep_node wifi;
	struct exlay_ep_node arp;
	struct exlay_ep_node ipv4;
	struct exlay_ep_node ipv6;
	struct exlay_ep_node icmp;
	struct exlay_ep_node icmpv6;
	struct exlay_ep_node tcp;
	struct exlay_ep_node udp;

	udp.bind_size = 2; /* port number is 16-bit field */
	udp.binding = (uint8_t *)malloc(udp.bind_size);
	memcpy(udp.binding, &udp_port, udp.bind_size);

	ipv4.bind_size = 4; /* ipv4 address is 32-bit field */
	ipv4.binding = (uint8_t *)malloc(ipv4.bind_size);
	memcpy(ipv4.binding, ipv4_addr, ipv4.bind_size);
	ipv4.nxt_type_size = 1; /* ipv4 protocol is 8-bit field */
	ipv4.nxt_type = (uint8_t *)malloc(ipv4.nxt_type_size);
	memcpy(ipv4.nxt_type, &ipv4_protocol, ipv4.nxt_type_size);

	ipv6.bind_size = 16; /* ipv6 address is 128-bit field */
	ipv6.binding = (uint8_t *)malloc(ipv6.bind_size);
	memcpy(ipv6.binding, ipv6_addr, ipv6.bind_size);
	ipv6.nxt_type_size = 1; /* ipv6 protocol is 8-bit field */
	ipv6.nxt_type = (uint8_t *)malloc(ipv6.nxt_type_size);
	memcpy(ipv6.nxt_type, &ipv6_protocol, ipv6.nxt_type_size);

	ether.bind_size = 6; /* mac address is 48-bit field */
	ether.binding = (uint8_t *)malloc(ether.bind_size);
	memcpy(ether.binding, ifr.ifr_hwaddr.sa_data, ether.bind_size);
	ether.nxt_type_size = 2; /* ether protocol is 16-bit field */
	ether.nxt_type = (uint8_t *)malloc(ether.nxt_type_size);
	memcpy(ether.nxt_type, &ether_type, ether.nxt_type_size);

	for (unsigned int i = 0; i < udp.bind_size; i++) {
		printf("%0x ", udp.binding[i]);
	}
	putchar('\n');

	for (unsigned int i = 0; i < ipv4.bind_size; i++) {
		printf("%02x ", ipv4.binding[i]);
	}
	putchar('\n');

	for (unsigned int i = 0; i < ipv6.bind_size; i++) {
		printf("%02x ", ipv6.binding[i]);
	}
	putchar('\n');
	
	for (unsigned int i = 0; i < ether.bind_size; i++) {
		printf("%02x ", ether.binding[i]);
	}
	putchar('\n');


}
