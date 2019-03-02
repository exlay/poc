#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <error.h>
#include <errno.h>
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
		ep->btm[i].lbind = NULL;
		ep->btm[i].rbind = NULL;
		ep->btm[i].upper = NULL;
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
	char buf[strlen(proto)+6]; /* store name of "libXX.so" */
	memset(buf, 0, sizeof(buf));
	strncat(buf, "lib", 3);
	strncat(buf, proto, strlen(proto));
	strncat(buf, ".so", 3);

	exep = get_ep_from_sock(ep);
	if (exep == NULL) {
		/* no such exlay endpoint */
		return -1;
	}
	if (exep->nr_protos > lyr || lyr == 0) {
		/* no such layer in the endpoint */
		return -1;
	}
	/* XXX if ex_set_binding is already called before, notify */

	/* load library of "proto" by protobj symbol */
	void *handle = dlopen(buf, RTLD_NOW|RTLD_GLOBAL);
	char *err;
	if ((err = dlerror()) != NULL) {
		fputs(err, stderr);
		putchar('\n');
		return -1;
	}
	/* XXX how should it specify the symbol name of protobj? */
	exep->btm[lyr-1].proto = (struct protobj *)dlsym(handle, proto);
	if ((err = dlerror()) != NULL) {
		fputs(err, stderr);
		putchar('\n');
		return -1;
	}

	/* set requested binding */
	uint8_t size = exep->btm[lyr - 1].proto->bind_size;
	uint8_t uplyr_type_s = exep->btm[lyr - 1].proto->upper_type_size;
	exep->btm[lyr - 1].lbind = malloc(size);
	memcpy(exep->btm[lyr - 1].lbind, lbind, size);

	if (for_lower != NULL) {
		exep->btm[lyr - 1].upper = malloc(uplyr_type_s);
		memcpy(exep->btm[lyr - 1].upper, for_lower, uplyr_type_s);
	} else {
		exep->btm[lyr - 1].upper = NULL;
	}

	return 0;
}

int ex_bind_stack(int ep)
{
	return 0;
}

int ex_set_remote(int ep, int lyr, void *binding)
{
	struct exlay_ep *exep;
	exep = get_ep_from_sock(ep);
	if (exep == NULL) {
		/* no such endpoint */
		return -1;
	}
	uint8_t size = exep->btm[lyr-1].proto->bind_size;
	if (binding == NULL) {
		free(exep->btm[lyr-1].rbind);
	} else {
		if (exep->btm[lyr-1].rbind == NULL) {
		/* ex_set_remote is called for the first time */
			if ((exep->btm[lyr-1].rbind = malloc(size)) == NULL) {
				fprintf(stderr, "ex_set_remote: malloc: error: %d\n", errno);
				exit(errno);
			}
		}
		memcpy(exep->btm[lyr-1].rbind, binding, size);
	}
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
