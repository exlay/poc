#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "exlay.h"

int main(void)
{
	uint16_t udp_port = htons(40000);
	uint8_t ipv4_addr[4];
	inet_pton(AF_INET, "192.168.10.11", ipv4_addr);
	uint8_t ipv4_protocol = htons(17);
	

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

	udp.bind_size = 2;
	udp.binding = (uint8_t *)malloc(udp.bind_size);
	memcpy(udp.binding, &udp_port, udp.bind_size);

	ipv4.bind_size = 4;
	ipv4.binding = (uint8_t *)malloc(ipv4.bind_size);
	memcpy(ipv4.binding, ipv4_addr, ipv4.bind_size);
	ipv4.nxt_type_size = 1;
	ipv4.nxt_type = (uint8_t *)malloc(ipv4.nxt_type_size);
	memcpy(ipv4.nxt_type, &ipv4_protocol, ipv4.nxt_type_size);

	for (unsigned int i = 0; i < sizeof(uint16_t); i++) {
		printf("%0x ", udp.binding[i]);
	}
	putchar('\n');

	for (unsigned int i = 0; i < 4; i++) {
		printf("%02x ", ipv4.binding[i]);
	}
	putchar('\n');


}
