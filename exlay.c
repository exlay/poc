#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>

#include "exlay.h"

static void func_exlay_help(int largc, char **largv)
{
	if (largc == 1) {
		fprintf(stderr, "error: need more arguments.\n");
		goto HELP;
	}

	if (largc <= 1 || 5 <= largc || strcmp(largv[1], "help") != 0) {
		fprintf(stderr, "error: invalid command or arguments.\n");
		goto HELP;
	}

	if (strcmp(largv[1], "help") == 0 && largc != 2) {
		fprintf(stderr, "error: invalid arguments.\n");
		goto HELP;
	}

HELP:
	fprintf(stderr, "usage:                             \n");
	fprintf(stderr, "    exlay help                  -- show this message\n");
	fprintf(stderr, "    exlay list                  -- show all protocols added to the daemon\n");
	fprintf(stderr, "    exlay add <proto> <path>    -- add the protocol to the daemon\n");
	fprintf(stderr, "    exlay del <proto>           -- delete the protocol from the daemon\n");
	fprintf(stderr, "    exlay info <proto>          -- show the description of the protocol\n");
	fprintf(stderr, "    exlay update <proto> <path> -- update the protocol\n");
}

static void func_exlay_list(int largc, char **largv)
{
	if (largc != 2) {
		func_exlay_help(largc, largv);
		goto OUT;
	}
	struct exlay_hdr hdr = {
		.cmd = CMD_LIST,
		.code = CODE_REQ,
		.len_proto_name = 0,
		.len_proto_path = 0,
	};

OUT:
	return;
}

static void func_exlay_add(int largc, char **largv)
{
	if (largc != 4) {
		func_exlay_help(largc, largv);
		goto OUT;
	}
	struct exlay_hdr hdr = {
		.cmd = CMD_ADD,
		.code = CODE_REQ,
		.len_proto_name = strlen(largv[2]),
		.len_proto_path = strlen(largv[3]),
	};

OUT:
	return;
}

static void func_exlay_info(int largc, char **largv)
{
	if (largc != 3) {
		func_exlay_help(largc, largv);
		goto OUT;
	}
	struct exlay_hdr hdr = {
		.cmd = CMD_INFO,
		.code = CODE_REQ,
		.len_proto_name = strlen(largv[2]),
		.len_proto_path = 0,
	};

OUT:
	return;

}

static void func_exlay_del(int largc, char **largv)
{
	if (largc != 3) {
		func_exlay_help(largc, largv);
		goto OUT;
	}
	struct exlay_hdr hdr = {
		.cmd = CMD_DEL,
		.code = CODE_REQ,
		.len_proto_name = strlen(largv[2]),
		.len_proto_path = 0,
	};

OUT:
	return;
}

static void func_exlay_update(int largc, char **largv)
{
	if (largc != 4) {
		func_exlay_help(largc, largv);
		goto OUT;
	}
	struct exlay_hdr hdr = {
		.cmd = CMD_UPDATE,
		.code = CODE_REQ,
		.len_proto_name = strlen(largv[2]),
		.len_proto_path = strlen(largv[3]),
	};

OUT:
	return;
}

struct exlay_cmd {
	char *cmd;
	void (*cmd_func)(int largc, char **largv);
} cmd_table[NR_CMDS + 1] = {
	{"help", func_exlay_help},
	{"list", func_exlay_list},
	{"add", func_exlay_add},
	{"info", func_exlay_info},
	{"del", func_exlay_del},
	{"update", func_exlay_update},
	{NULL, NULL}
};

static struct sockaddr_in daem_addr_in;
static int cli_sock;
static char cmd_buf[MAXCMDLEN + 1];

static int largc;
static char **largv;

int main(int argc, char **argv) 
{
	int i;

	if ((cli_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		return errno;
	}

	/*
	 * argc must be 2, 3, or 4
	 * i.e.,
	 *
	 * exlay help
	 * exlay list 
	 * exlay add <path>
	 * exlay info <proto>
	 * exlay del <proto>
	 * exlay update <proto> <path>
	 *
	 * */
	if (argc <= 1 || 5 <= argc) {
		func_exlay_help(argc, argv);
		return EXIT_FAILURE;
	}

	for (i = 0; cmd_table[i].cmd != NULL; i++) {
		if (strcmp(cmd_table[i].cmd, argv[1]) == 0) {
			debug_printf(stderr, "cmd = %s\n", argv[1]);
			cmd_table[i].cmd_func(argc, argv);
			break;
		}
	}

	/* unkown command is specified */
	if (i == NR_CMDS) {
		func_exlay_help(argc, argv);
	}
	

	close(cli_sock);
	return 0;
	
}
