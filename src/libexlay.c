/* libexlay.c: library for user Apps. using exlay
 *
 * TODO: 
 * 		add configure API for each protocol (setsockopt-like)	
 * 		support multi-thread for send/recv
 * */
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
#include <rpc/rpc.h>
#include <time.h>

#include "protocol.h"
#include "exlay.h"
#include "exlay/error.h"
#include "exlay_rpc.h" /* generated by rpcgen */

CLIENT *client;
static void init_libexlay() __attribute__((constructor));
struct timeval to = {3600, 0};
struct timeval rto = {3600, 0};

void init_libexlay(void)
{
	struct timeval dbg;
	client = clnt_create(RPCSERVER, EXLAYPROG, EXLAYVERS, "udp");
	if (client == NULL) {
		clnt_pcreateerror(RPCSERVER);
		exit(EXIT_FAILURE);
	}
	clnt_control(client, CLGET_TIMEOUT, (char *)&dbg);
	if (clnt_control(client, CLSET_TIMEOUT, (char *)&to) < 0) {
		fprintf(stderr, "clnt_control: failed to set timeout\n");
		exit(EXIT_FAILURE);
	}
	clnt_control(client, CLGET_TIMEOUT, (char *)&dbg);
	clnt_control(client, CLGET_RETRY_TIMEOUT, (char *)&dbg);
	if (clnt_control(client, CLSET_RETRY_TIMEOUT, (char *)&rto) < 0) {
		fprintf(stderr, "clnt_control: failed to set retry timeout\n");
		exit(EXIT_FAILURE);
	}
	clnt_control(client, CLGET_RETRY_TIMEOUT, (char *)&dbg);
}

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
	int *res;
	res = ex_create_stack_1(nr_protos, client);
	if (res == NULL) {
		clnt_perror(client, RPCSERVER);
		exit(EXIT_FAILURE);
	}
	return *res;
}

int exlay_to_kern(struct exdata *exd, uint32_t len)
{
	return len;
}

int ex_set_binding(
		int exsock,
		unsigned int lyr, 
		char *proto, 
		void *lbind,
		unsigned int bsize,
		int upper)
{
	int *res;
	binding b;
	b.binding_len = bsize;
	b.binding_val = (char *)lbind;
	res = ex_set_binding_1(exsock, lyr, proto, b, bsize, upper, client);
	if (res == NULL) {
		clnt_perror(client, RPCSERVER);
		exit(EXIT_FAILURE);
	}
	print_err(*res);
	return *res;
}

int ex_bind_stack(int ep)
{
	int *res;
	res = ex_bind_stack_1(ep, client);
	if (res == NULL) {
		clnt_perror(client, RPCSERVER);
		exit(EXIT_FAILURE);
	}
	return *res;
}

int ex_set_remote(int ep, int lyr, void *rbind, unsigned int bsize)
{
	int *res;
	binding rb;
	rb.binding_len = bsize;
	rb.binding_val = (char *)rbind;
	res = ex_set_remote_1(ep, lyr, rb, bsize, client);
	if (res == NULL) {
		clnt_perror(client, RPCSERVER);
		exit(EXIT_FAILURE);
	}
	return *res;
}

int ex_dial_stack(int ep)
{
	int *res;
	res = ex_dial_stack_1(ep, client);
	if (res == NULL) {
		clnt_perror(client, RPCSERVER);
		exit(EXIT_FAILURE);
	}
	return *res;
}

int ex_send_stack(int ep, char *buf, uint32_t size)
{
	return 0;
}

int ex_recv_stack(int ep, char *buf, uint32_t size)
{
	return 0;
}

int ex_close_stack(int ep)
{
	int *res;
	res = ex_dial_stack_1(ep, client);
	if (res == NULL) {
		clnt_perror(client, RPCSERVER);
		exit(EXIT_FAILURE);
	}
	return *res;
}
