#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <error.h>

#include "protocol.h"

int exd_out(struct exdata *exd, uint32_t len)
{
	return len;
}

int exd_in(struct exdata *exd, uint32_t len)
{
	return len;
}

int ex_create_stack(unsigned int nr_protos)
{
	struct exlay_ep *exep;
	exep = (struct exlay_ep *)malloc(sizeof(struct exlay_ep));
	exep->sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (exep->sock < 0) {
		perror("ex_create_stack: socket:");
	}
	return exep->sock;
}

int exlay_to_kern(struct exdata *exd, uint32_t len)
{
	return len;
}

int ex_set_binding(
		int ep,
		unsigned int layer, 
		char *proto, 
		void *lbind,
		void *for_lower)
{
	return 0;
}

int ex_bind_stack(int ep)
{
	return 0;
}

int ex_set_remote(int ep, int layer, void *binding)
{
	return 0;
}

int ex_dial_stack(int ep)
{
	return 0;
}

int ex_send_stack(int ep, uint32_t size)
{
	return 0;
}

int ex_recv_stack(int ep, uint32_t size)
{
	return 0;
}

int ex_close_stack(int ep)
{
	return 0;
}
