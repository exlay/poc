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

#include "protocol.h"
#include "exlay.h"
#include "exlay_rpc.h" /* generated by rpcgen */

CLIENT *client;
static void init_libexlay() __attribute__((constructor));

void init_libexlay(void)
{
	printf("I am a constructor of libexlay\n");
	client = clnt_create(RPCSERVER, EXLAYPROG, EXLAYVERS, "udp");
	if (client == NULL) {
		clnt_pcreateerror(RPCSERVER);
		exit(EXIT_FAILURE);
	}
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
		int ep,
		unsigned int lyr, 
		char *proto, 
		char *lbind,
		char *upper)
{
	int *res;
	res = ex_set_binding_1(ep, lyr, proto, lbind, upper, client);
	if (res == NULL) {
		clnt_perror(client, RPCSERVER);
		exit(EXIT_FAILURE);
	}
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

int ex_set_remote(int ep, int lyr, char *binding)
{
	int *res;
	res = ex_set_remote_1(ep, lyr, binding, client);
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
