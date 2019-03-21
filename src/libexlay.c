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

/* for ex_{send,recv}_stack */
static int rd;
static int wd;

struct cli_ios {
	int ep;
	int rd;
	int wd;
	cli_io cio;
	struct cli_ios *fp;
	struct cli_ios *bp;
};

static struct cli_ios cli_ios_head;

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

	cli_ios_head.fp = cli_ios_head.bp = &cli_ios_head;
}

int ex_create_stack(unsigned int nr_protos)
{
	int *res;
	res = ex_create_stack_1(nr_protos, client);
	if (res == NULL) {
		clnt_perror(client, RPCSERVER);
		exit(EXIT_FAILURE);
	}
	
	struct cli_ios *io;
	io = (struct cli_ios *)malloc(sizeof(struct cli_ios));
	if (io == NULL) {
		*res = -CODE_NMEM;
		goto OUT;
	}

	memset(io, 0, sizeof(struct cli_ios));
	io->ep = *res;
	
	INSERT_TO_LIST_HEAD(&cli_ios_head, io);
OUT:
	return *res;
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

static struct cli_ios *get_cli_from_ep(int ep)
{
	struct cli_ios *p;
	for (p = cli_ios_head.fp; p != &cli_ios_head; p = p->fp) {
		if (ep == p->ep) {
			/* found */
			return p;
		}
	}
	/* if not found */
	return NULL;
}

int ex_bind_stack(int ep)
{
	int res = 0;
	struct cli_ios *p;
	cli_io *tmp;
	p = get_cli_from_ep(ep);
	if (p == NULL) {
		fprintf(stderr, "no soch exlay endpoint\n");
		res = -CODE_NEXIST;
		goto OUT;
	}
	tmp = ex_bind_stack_1(ep, client);
	if (tmp == NULL) {
		clnt_perror(client, RPCSERVER);
		exit(EXIT_FAILURE);
	}
	memcpy(&p->cio, tmp, sizeof(cli_io)); 
	p->rd = open(p->cio.rpath, O_RDWR);
	if (p->rd < 0) {
		perror("open: read fifo");
		res = -errno;
		goto OUT;
	}

	p->wd = open(p->cio.wpath, O_RDWR);
	if (p->wd < 0) {
		perror("open: write fifo");
		res = -errno;
	}

	res = p->cio.code;
OUT:
	return res;
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

int ex_send_stack(int ep, void *buf, uint32_t size, int opt)
{
	int *res;
	msg msgbuf;
	msgbuf.msg_len = size;
	msgbuf.msg_val = (char *)buf;
	res = ex_send_stack_1(ep, msgbuf, size, client);
	if (res == NULL) {
		clnt_perror(client, RPCSERVER);
		exit(EXIT_FAILURE);
	}
	print_err(*res);
	
	return *res;
}

int ex_recv_stack(int ep, void *buf, uint32_t size, int opt)
{
	int res = 0;
	struct cli_ios *p;
	p = get_cli_from_ep(ep);
	if (p == NULL) {
		fprintf(stderr, "no soch exlay endpoint\n");
		res = -CODE_NEXIST;
		goto OUT;
	}
	res = read(p->rd, buf, size);

OUT:
	return res;
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
