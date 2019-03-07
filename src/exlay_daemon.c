#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/mman.h>
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

struct proto_info list_head;
struct binding_tree root;

static int init_exlay(void)
{
	root.upper = &root;
	root.lower = NULL;
	root.fp = &root;
}

void add_to_list(struct proto_info *p)
{
	p->prev = &list_head;
	p->next = list_head.next;
	list_head.next->prev = p;
	list_head.next = p;
}

void del_from_list(struct proto_info *p)
{
	p->prev->next = p->next;
	p->next->prev = p->prev;
	p->prev = p->next = NULL;
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
	for (prt = list_head.next, p = data; prt != &list_head; prt = prt->next) {
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
	for (prt = list_head.next; prt != &list_head; prt = prt->next) {
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
	

	add_to_list(new_prt);
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
	for (prt = list_head.next; prt != &list_head; prt = prt->next) {
		if (strcmp(prt->name, prot_name) == 0) {
			/* found */
			break;
		}
	}

	if (prt == &list_head) {
		hdr->code = CODE_NEXIST;
		goto OUT;
	}

	prot_ctr--;
	del_from_list(prt);
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
	for (prt = list_head.next; prt != &list_head; prt = prt->next) {
		if (strcmp(prt->name, prot_name) == 0) {
			/* found */
			break;
		}
	}

	if (prt == &list_head) {
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

	list_head.next = &list_head;

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
