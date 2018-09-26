#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>

#include "exlay.h"

static struct sockaddr_in daem_addr_in;
static int daem_sock;
static socklen_t sk_len = sizeof(struct sockaddr_in);
static int prot_ctr = 0;
static struct sockaddr_in cli_addr_in;

struct proto_info proto_info_list[MAXNRPROT] = {0};


static void func_daem_list(void *buf)
{
	debug_printf("cmd = list\n");
	struct exlay_hdr *hdr = (struct exlay_hdr *)buf;
	uint8_t *data = (uint8_t *)buf + EXLAYHDRSIZE;

	if (hdr->code != CODE_REQ) {
		/* invalid request pkt */
		goto OUT;
	} 

	hdr->code = CODE_OK;

	int i;
	uint8_t *p;
	for (i = 0, p = data; i < prot_ctr; i++) {
		memcpy(p, proto_info_list[i].name, strlen(proto_info_list[i].name));
		p += strlen(proto_info_list[i].name);
	}

	int ret;
	ret = sendto(daem_sock, buf, EXLAYHDRSIZE + strlen((char *)data), 0, 
			(struct sockaddr *)&cli_addr_in, sk_len);

	if (ret < 0) {
		perror("sendto");
	}

OUT:
	return;
}

static void func_daem_add(void *buf)
{
	debug_printf("cmd = add\n");
	struct exlay_hdr *hdr = (struct exlay_hdr *)buf;
	uint8_t *data = (uint8_t *)buf + EXLAYHDRSIZE;

	if (hdr->code != CODE_REQ) {
		/* invalid request pkt */
		goto OUT;
	} 

	hdr->code = CODE_OK;

	int i;
	uint8_t *p;
	for (i = 0, p = data; i < prot_ctr; i++) {
		memcpy(p, proto_info_list[i].name, strlen(proto_info_list[i].name));
		p += strlen(proto_info_list[i].name);
	}

	int ret;
	ret = sendto(daem_sock, buf, EXLAYHDRSIZE + strlen((char *)data), 0, 
			(struct sockaddr *)&cli_addr_in, sk_len);

	if (ret < 0) {
		perror("sendto");
	}

OUT:
	return;
}

static void func_daem_info(void *buf)
{
	debug_printf("cmd = info\n");
}
static void func_daem_del(void *buf)
{
	debug_printf("cmd = del\n");
}
static void func_daem_update(void *buf)
{
	debug_printf("cmd = update\n");
}


struct daem_cmd {
	uint8_t cmd;
	void (*cmd_func)(void *buf);
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

		ret = recvfrom(daem_sock, buf, MAXPKTSIZE, 0, 
				(struct sockaddr *)&cli_addr_in, &sk_len);
		if (ret < 0) {
			perror("recvfrom");
		}

		p = (struct exlay_hdr *)buf;
		print_exlay_hdr(p);

		for (i = 0; daem_cmd_table[i].cmd != CMD_UNKNOWN; i++) {
			if (daem_cmd_table[i].cmd == p->cmd) {
				daem_cmd_table[i].cmd_func(buf);
				break;
			}
		}
	}

	return 0;
}
