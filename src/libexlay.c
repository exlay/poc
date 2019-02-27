#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <error.h>
#include <dlfcn.h>
#include <string.h>

#include "protocol.h"

struct exlay_ep ep_head = {
	.next = &ep_head,
	.prev = &ep_head,
};

static void add_to_list(struct exlay_ep *p)
{
	p->prev = &ep_head;
	p->next = ep_head.next;
	ep_head.next->prev = p;
	ep_head.next = p;
}

static void del_from_list(struct exlay_ep *p)
{
	p->prev->next = p->next;
	p->next->prev = p->prev;
	p->prev = p->next = NULL;
}

static struct exlay_ep *get_ep_from_sock(int sock)
{
	struct exlay_ep *p;
	for (p = ep_head.next; p != &ep_head; p++) {
		if (sock == p->sock) {
			/* found */
			return p;
		}
	}
	/* if not found */
	return NULL;
}

int exd_out(struct exdata *exd, uint32_t len)
{
	return len;
}

int exd_in(struct exdata *exd, uint32_t len)
{
	return len;
}

static void init_stack(struct exlay_ep *ep, int nr_protos)
{
	int i;
	ep->nr_protos = nr_protos;
	for (i = 0; i < nr_protos; i++) {
		ep->btm[i].layer = i + 1;
		ep->btm[i].ep = ep;
		ep->btm[i].proto = NULL;
	}
}

int ex_create_stack(unsigned int nr_protos)
{
	struct exlay_ep *exep;
	exep = (struct exlay_ep *)malloc(sizeof(struct exlay_ep));
	exep->sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	exep->nr_protos = nr_protos;
	exep->btm = (struct exlay_stack *)malloc(sizeof(struct exlay_stack) * nr_protos);
	exep->top = &exep->btm[nr_protos - 1];
	init_stack(exep, nr_protos);

	if (exep->sock < 0) {
		perror("ex_create_stack: socket:");
	}
	add_to_list(exep);
	return exep->sock;
}

int exlay_to_kern(struct exdata *exd, uint32_t len)
{
	return len;
}

int ex_set_binding(
		int ep,
		unsigned int lyr, 
		char *proto, 
		void *lbind,
		void *for_lower)
{
	struct exlay_ep *exep;
	exep = get_ep_from_sock(ep);
	if (exep == NULL) {
		/* no such exlay endpoint */
		return -1;
	}
	if (exep->nr_protos > lyr || lyr == 0) {
		/* no such layer in the endpoint */
		return -1;
	}
	/* XXX ask daemon to inform the "proto" */
	if (strcmp(proto, "test_ethernet") != 0) {
		return -1;
	}
	/* load library of "proto" by protobj symbol */
	void *handle = dlopen("/home/vagrant/work/poc/protocols/lib/libtest_ethernet.so", RTLD_NOW|RTLD_GLOBAL);
	char *err;
	if ((err = dlerror()) != NULL) {
		fputs(err, stderr);
		putchar('\n');
		return -1;
	}
	/* XXX how should it specify the symbol name of protobj? */
	exep->btm[lyr - 1].proto = (struct protobj *)dlsym(handle, "proto_ethernet");
	if ((err = dlerror()) != NULL) {
		fputs(err, stderr);
		putchar('\n');
		return -1;
	}

	/* set requested binding */
	uint8_t size = exep->btm[lyr - 1].proto->bind_size;
	exep->btm[lyr - 1].lbind = malloc(size);
	memcpy(exep->btm[lyr - 1].lbind, lbind, size);

	exep->btm[lyr - 1].rbind = malloc(size);
	exep->btm[lyr - 1].for_lower = NULL;

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
	struct exlay_ep *p;
	p = get_ep_from_sock(ep);
	if (p == NULL) {
		/* no such exlay endpoint */
		return -1;
	}
	del_from_list(p);
	free(p);
	return 0;
}
