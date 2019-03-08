#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <dlfcn.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <error.h>
#include <dlfcn.h>
#include <rpc/rpc.h>

#include "exlay.h"
#include "protocol.h"
#include "daemon.h"

static struct sockaddr_in daem_addr_in;
static int daem_sock;
static socklen_t sk_len = sizeof(struct sockaddr_in);
static int prot_ctr = 0;
static struct sockaddr_in cli_addr_in;

struct proto_info prinfo_head;
struct binding_tree root;

struct exlay_ep ep_head = {
	.fp = &ep_head,
	.bp = &ep_head,
};

static void reflect_to_binding_tree(struct exlay_ep *exep)
{

}
static int init_exlay(void)
{
	root.upper = &root;
	root.lower = NULL;
	root.fp = &root;
}



static struct exlay_ep *get_ep_from_sock(int sock)
{
	struct exlay_ep *p;
	for (p = ep_head.fp; p != &ep_head; p = p->fp) {
		if (sock == p->sock) {
			/* found */
			return p;
		}
	}
	/* if not found */
	return NULL;
}

static void init_stack(struct exlay_ep *ep, int nr_protos)
{
	int i;
	ep->nr_protos = nr_protos;
	for (i = 0; i < nr_protos; i++) {
		ep->btm[i].layer = i + 1;
		ep->btm[i].proto = NULL;
		ep->btm[i].lbind = NULL;
		ep->btm[i].rbind = NULL;
		ep->btm[i].upper = NULL;
	}
}

static void func_daem_list(void *buf, int len)
{
	debug_printf("cmd = list\n");
	struct exlay_hdr *hdr = (struct exlay_hdr *)buf;
	uint8_t *data = (uint8_t *)buf + EXLAYHDRSIZE;
	int ret;

	if (hdr->code != CODE_REQ) {
		/* invalid request pkt */
		goto OUT;
	} 

	hdr->code = CODE_OK;

	struct proto_info *prt;
	uint8_t *p;
	for (prt = prinfo_head.fp, p = data; prt != &prinfo_head; prt = prt->fp) {
		memcpy(p, prt->name, strlen(prt->name));
		p += strlen(prt->name);
		*p = '\n';
		p++;
	}

OUT:
	ret = sendto(daem_sock, buf, EXLAYHDRSIZE + strlen((char *)data), 0, 
			(struct sockaddr *)&cli_addr_in, sk_len);

	if (ret < 0) {
		perror("sendto");
	}
	return;
}

static void func_daem_add(void *buf, int len)
{
	debug_printf("cmd = add\n");
	struct exlay_hdr *hdr = (struct exlay_hdr *)buf;
	uint8_t *data = (uint8_t *)buf + EXLAYHDRSIZE;
	char prot_name[MAXPROTNAMELEN] = {0};
	char *prot_path = (char *)malloc(sizeof(char) * hdr->len_proto_path);
	memcpy(prot_name, data, hdr->len_proto_name);
	memcpy(prot_path, data + hdr->len_proto_name, hdr->len_proto_path);
	int ret;

	if (hdr->code != CODE_REQ) {
		/* invalid request pkt */
		hdr->code = CODE_INVREQ;
		memset(data, 0, len - EXLAYHDRSIZE);
		/* XXX error message should be written */
		goto OUT;
	} 

	if (prot_ctr >= MAXNRPROT) {
		hdr->code = CODE_NEMPTY;
		memset(data, 0, len - EXLAYHDRSIZE);
		/* XXX error message should be written */
		goto OUT;
	}

	struct proto_info *prt;
	/* whether the protocol name to be added has already exist or not */
	for (prt = prinfo_head.fp; prt != &prinfo_head; prt = prt->fp) {
		if (strcmp(prt->name, prot_name) == 0) {
			hdr->code = CODE_DUP;
			memset(data, 0, len - EXLAYHDRSIZE);
			/* XXX error message should be written */
			goto OUT;
		}
	}
	struct proto_info *new_prt;
	new_prt = (struct proto_info *)malloc(sizeof(struct proto_info));
	if (new_prt == NULL) {
		hdr->code = CODE_NMEM;
		memset(data, 0, len - EXLAYHDRSIZE);
		/* XXX error message should be written */
		goto OUT;
	}
	new_prt->path = (char *)malloc(sizeof(char) * hdr->len_proto_path);
	memcpy(new_prt->name, prot_name, hdr->len_proto_name);
	memcpy(new_prt->path, prot_path, hdr->len_proto_path);
	prot_ctr++;

	void *handle = dlopen(new_prt->path, RTLD_LAZY|RTLD_GLOBAL);
	char *err;
	if ((err = dlerror()) != NULL) {
		fputs(err, stderr);
		putchar('\n');
		hdr->code = CODE_NG;
		goto OUT;
	}
	

	INSERT_TO_LIST_HEAD(&prinfo_head, new_prt);
	debug_printf("prot %s (%s) was successfully added\n", 
			new_prt->name, new_prt->path);

	hdr->code = CODE_OK;

OUT:
	ret = sendto(daem_sock, buf, EXLAYHDRSIZE + strlen((char *)data), 0, 
			(struct sockaddr *)&cli_addr_in, sk_len);

	if (ret < 0) {
		perror("sendto");
	}
	return;
}

static void func_daem_info(void *buf, int len)
{
	debug_printf("cmd = info\n");
	/*
	 * XXX: PENDING
	 * */
}
static void func_daem_del(void *buf, int len)
{
	debug_printf("cmd = del\n");
	struct exlay_hdr *hdr = (struct exlay_hdr *)buf;
	uint8_t *data = (uint8_t *)buf + EXLAYHDRSIZE;
	char *prot_name = (char *)(data);
	int ret;

	if (hdr->code != CODE_REQ) {
		/* invalid request pkt */
		hdr->code = CODE_INVREQ;
		memset(data, 0, len - EXLAYHDRSIZE);
		/* XXX error message should be written */
		goto OUT;
	} 

	struct proto_info *prt;
	/* whether the protocol name to be deleted has really exist or not */
	for (prt = prinfo_head.fp; prt != &prinfo_head; prt = prt->fp) {
		if (strcmp(prt->name, prot_name) == 0) {
			/* found */
			break;
		}
	}

	if (prt == &prinfo_head) {
		hdr->code = CODE_NEXIST;
		goto OUT;
	}

	prot_ctr--;
	REMOVE_FROM_LIST(prt);
	debug_printf("prot %s (%s) was successfully removed\n", 
			prt->name, prt->path);
	free(prt);
	memset(data, 0, hdr->len_proto_name);

	hdr->code = CODE_OK;

OUT:
	ret = sendto(daem_sock, buf, EXLAYHDRSIZE + strlen((char *)data), 0, 
			(struct sockaddr *)&cli_addr_in, sk_len);

	if (ret < 0) {
		perror("sendto");
	}
	return;
}
static void func_daem_update(void *buf, int len)
{
	debug_printf("cmd = update\n");
	struct exlay_hdr *hdr = (struct exlay_hdr *)buf;
	uint8_t *data = (uint8_t *)buf + EXLAYHDRSIZE;
	char prot_name[MAXPROTNAMELEN] = {0};
	char prot_path[MAXPROTPATHLEN] = {0};
	memcpy(prot_name, data, hdr->len_proto_name);
	memcpy(prot_path, data + hdr->len_proto_name, hdr->len_proto_path);
	int ret;

	if (hdr->code != CODE_REQ) {
		/* invalid request pkt */
		hdr->code = CODE_INVREQ;
		memset(data, 0, len - EXLAYHDRSIZE);
		/* XXX error message should be written */
		goto OUT;
	} 

	struct proto_info *prt;
	/* whether the protocol name to be added has already exist or not */
	for (prt = prinfo_head.fp; prt != &prinfo_head; prt = prt->fp) {
		if (strcmp(prt->name, prot_name) == 0) {
			/* found */
			break;
		}
	}

	if (prt == &prinfo_head) {
		hdr->code = CODE_NEXIST;
		goto OUT;
	}

	memcpy(prt->path, prot_path, hdr->len_proto_path);
	debug_printf("prot %s (%s) was successfully updated\n", 
			prt->name, prt->path);

	hdr->code = CODE_OK;

OUT:
	ret = sendto(daem_sock, buf, EXLAYHDRSIZE + strlen((char *)data), 0, 
			(struct sockaddr *)&cli_addr_in, sk_len);

	if (ret < 0) {
		perror("sendto");
	}
	return;
}


struct daem_cmd {
	uint8_t cmd;
	void (*cmd_func)(void *buf, int len);
} daem_cmd_table[NR_DAEM_CMDS + 1] = {
	{CMD_LIST,    func_daem_list},
	{CMD_ADD,     func_daem_add},
	{CMD_INFO,    func_daem_info},
	{CMD_DEL,     func_daem_del},
	{CMD_UPDATE,  func_daem_update},
	{CMD_UNKNOWN, NULL}
};


int init_daemon(void)
{
	if ((daem_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		return errno;
	}

	memset(&daem_addr_in, 0, sizeof(struct sockaddr_in));
	daem_addr_in.sin_family = AF_INET;
	daem_addr_in.sin_port = DAEMON_PORT;
	daem_addr_in.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(daem_sock, (struct sockaddr *)&daem_addr_in, sizeof(struct sockaddr_in)) < 0) {
		perror("bind");
		return errno;
	}

	prinfo_head.fp = &prinfo_head;

	return 0;
}

int main(void)
{
	int ret;
	uint8_t buf[MAXPKTSIZE] = {0};

	if (init_daemon() != 0) {
		return EXIT_FAILURE;
	}
	debug_printf("init_daemon: finish...\n");

	for (;;) {
		int i;
		struct exlay_hdr *p;

		memset(buf, 0, MAXPKTSIZE);
		ret = recvfrom(daem_sock, buf, MAXPKTSIZE, 0, 
				(struct sockaddr *)&cli_addr_in, &sk_len);
		if (ret < 0) {
			perror("recvfrom");
		}

		p = (struct exlay_hdr *)buf;
		print_exlay_hdr(p);

		for (i = 0; daem_cmd_table[i].cmd != CMD_UNKNOWN; i++) {
			if (daem_cmd_table[i].cmd == p->cmd) {
				daem_cmd_table[i].cmd_func(buf, ret);
				break;
			}
		}
		if (daem_cmd_table[i].cmd == CMD_UNKNOWN) {
			fprintf(stderr, "unknown command\n");
		}
	}

	return 0;
}

/* RPC svc */

int *ex_create_stack_1_svc(unsigned int nr_layers, struct svc_req *rqstp)
{
	static int result;
	struct exlay_ep *exep;
	exep = (struct exlay_ep *)malloc(sizeof(struct exlay_ep));
	exep->sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	exep->nr_protos = nr_layers;
	exep->btm = (struct exlay_layer *)malloc(sizeof(struct exlay_layer) * nr_layers);
	exep->top = &exep->btm[nr_layers - 1];
	init_stack(exep, nr_layers);

	if (exep->sock < 0) {
		perror("ex_create_stack: socket:");
	}
	INSERT_TO_LIST_HEAD(&ep_head, exep);
	//return exep->sock;
	return &result;
}

int *ex_set_binding_1_svc(
		int ep, 
		unsigned int lyr, 
		char *proto, 
		char *lbind, 
		char *upper, 
		struct svc_req *rqstp)
{
	static int result = 0;

	struct exlay_ep *exep;
	char buf[strlen(proto)+6]; /* store name of "libXX.so" */
	memset(buf, 0, sizeof(buf));
	strncat(buf, "lib", 3);
	strncat(buf, proto, strlen(proto));
	strncat(buf, ".so", 3);

	exep = get_ep_from_sock(ep);
	if (exep == NULL) {
		/* no such exlay endpoint */
		result = -1;
		goto OUT;
	}
	if (exep->nr_protos > lyr || lyr == 0) {
		/* no such layer in the endpoint */
		result = -1;
		goto OUT;
	}
	/* XXX if ex_set_binding is already called before, notify */

	/* load library of "proto" by protobj symbol */
	void *handle = dlopen(buf, RTLD_NOW|RTLD_GLOBAL);
	char *err;
	if ((err = dlerror()) != NULL) {
		fputs(err, stderr);
		putchar('\n');
		result = -1;
		goto OUT;
	}
	/* XXX how should it specify the symbol name of protobj? */
	exep->btm[lyr-1].proto = (struct protobj *)dlsym(handle, proto);
	if ((err = dlerror()) != NULL) {
		fputs(err, stderr);
		putchar('\n');
		result = -1;
		goto OUT;
	}

	/* set requested binding */
	uint8_t size = exep->btm[lyr-1].proto->bind_size;
	uint8_t uplyr_type_s = exep->btm[lyr-1].proto->upper_type_size;
	exep->btm[lyr-1].lbind = malloc(size);
	memcpy(exep->btm[lyr-1].lbind, lbind, size);

	if (upper != NULL) {
		exep->btm[lyr-1].upper = malloc(uplyr_type_s);
		memcpy(exep->btm[lyr-1].upper, upper, uplyr_type_s);
	} else {
		exep->btm[lyr-1].upper = NULL;
	}

OUT:
	return &result;

}

int *ex_bind_stack_1_svc(int ep, struct svc_req *rqstp)
{
	static int result = 0;
	struct exlay_ep *exep;
	exep = get_ep_from_sock(ep);
	if (exep == NULL) {
		/* no such endpoint */
		result = -1;
		goto OUT;
	}
	/* reflect stack to binding_tree */
	reflect_to_binding_tree(exep);

OUT:
	return &result;
}
int *ex_set_remote_1_svc(int ep, int lyr, char *binding, struct svc_req *rqstp)
{
	static int result = 0;

	struct exlay_ep *exep;
	exep = get_ep_from_sock(ep);
	if (exep == NULL) {
		/* no such endpoint */
		result = -1;
		goto OUT;
	}
	uint8_t size = exep->btm[lyr-1].proto->bind_size;
	if (binding == NULL) {
		free(exep->btm[lyr-1].rbind);
	} else {
		if (exep->btm[lyr-1].rbind == NULL) {
		/* ex_set_remote is called for the first time */
			if ((exep->btm[lyr-1].rbind = malloc(size)) == NULL) {
				fprintf(stderr, "ex_set_remote: malloc: error: %d\n", errno);
				result = errno;
				goto OUT;
			}
		}
		memcpy(exep->btm[lyr-1].rbind, binding, size);
	}

OUT:
	return &result;
}
int *ex_dial_stack_1_svc(int ep, struct svc_req *rqstp)
{
	static int result;
	return &result;
}

int *ex_listen_stack_1_svc(int ep, struct svc_req *rpstp)
{
	static int result;
	return &result;
}
int *ex_close_stack_1_svc(int ep, struct svc_req *rpstp)
{
	static int result = 0;
	struct exlay_ep *p;
	p = get_ep_from_sock(ep);
	if (p == NULL) {
		/* no such exlay endpoint */
		result = -1;
		goto OUT;
	}
	REMOVE_FROM_LIST(p);
	free(p);

OUT:
	return &result;
}
