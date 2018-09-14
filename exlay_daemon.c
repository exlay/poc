#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>

#include "exlay.h"

static void func_daem_help(struct exlay_hdr *ndr)
{
}
static void func_daem_list(struct exlay_hdr *ndr)
{
}

static void func_daem_add(struct exlay_hdr *ndr)
{
}

static void func_daem_info(struct exlay_hdr *ndr)
{
}
static void func_daem_del(struct exlay_hdr *ndr)
{
}
static void func_daem_update(struct exlay_hdr *ndr)
{
}

static struct sockaddr_in daem_addr_in;
static struct sockaddr_in cli_addr_in;
static int daem_sock;
static char cmd_buf[MAXCMDLEN];
static socklen_t len = sizeof(struct sockaddr_in);

struct daem_cmd {
	uint8_t cmd;
	void (*cmd_func)(struct exlay_hdr *hdr);
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

	memset(cmd_buf, 0, MAXCMDLEN);

	return 0;
}

int main(void)
{
	int ret;

	if (init_daemon() != 0) {
		return EXIT_FAILURE;
	}
	debug_printf("init_daemon: finish...\n");

	for (;;) {
		int i;
		struct exlay_hdr *p;

		ret = recvfrom(daem_sock, cmd_buf, MAXCMDLEN, 0, 
				(struct sockaddr *)&cli_addr_in, &len);
		if (ret < 0) {
			perror("recvfrom");
		}

		p = (struct exlay_hdr *)cmd_buf;

		for (i = 0; daem_cmd_table[i].cmd != CMD_UNKNOWN; i++) {
			if (daem_cmd_table[i].cmd == p->cmd) {
				fprintf(stderr, "cmd = %d\n", p->cmd);
				break;
			}
		}
	}

	return 0;
}
