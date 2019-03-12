#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <rpc/rpc.h>

#include "exlay.h"
#include "exlay_rpc.h"

CLIENT *client;

static void check_result(int code)
{
	switch (code) {
		case CODE_INVREQ:
			fprintf(stderr, "error: Invalid request\n");
			break;
		case CODE_NEMPTY:
			fprintf(stderr, "error: Some protocols should be deleted from the list\n");
			break;
		case CODE_DUP:
			fprintf(stderr, "error: Dupulicate protocol name\n");
			break;
		case CODE_NMEM:
			fprintf(stderr, "error: Daemon cannot allocate memory\n");
			break;
		case CODE_NEXIST:
			fprintf(stderr, "error: No sucn protocol in the list\n");
			break;
		case CODE_NFND:
			fprintf(stderr, "error: No sucn file or directory\n");
			break;
		case CODE_NG:
			fprintf(stderr, "error: Something wrong with exlay daemon\n");
			break;
		case CODE_OK:
			break;
		default:
			fprintf(stderr, "error: Unknown error code (%d)\n", code);
	}
}

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
	char **res;
	if (largc != 2) {
		func_exlay_help(largc, largv);
		goto OUT;
	}
	res = exlay_list_1(client);

	if (res == NULL) {
		clnt_perror(client, RPCSERVER);
		exit(EXIT_FAILURE);
	}

	printf("========== Protocol List ==========\n%s", *res);
	printf("===================================\n");

OUT:
	return;
}

static void func_exlay_add(int largc, char **largv)
{
	int *res;
	if (largc != 4) {
		func_exlay_help(largc, largv);
		goto OUT;
	}
	res = exlay_add_1(largv[2], client);
	if (res == NULL) {
		clnt_perror(client, RPCSERVER);
		exit(EXIT_FAILURE);
	}

	check_result(*res);

OUT:
	return;
}

static void func_exlay_info(int largc, char **largv)
{
	if (largc != 3) {
		func_exlay_help(largc, largv);
		goto OUT;
	}

OUT:
	return;

}

static void func_exlay_del(int largc, char **largv)
{
	int *res;
	if (largc != 3) {
		func_exlay_help(largc, largv);
		goto OUT;
	}
	res = exlay_del_1(largv[2], client);
	if (res == NULL) {
		clnt_perror(client, RPCSERVER);
		exit(EXIT_FAILURE);
	}
	check_result(*res);

OUT:
	return;
}

static void func_exlay_update(int largc, char **largv)
{
	if (largc != 4) {
		func_exlay_help(largc, largv);
		goto OUT;
	}
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

int main(int argc, char **argv) 
{
	int i;

	client = clnt_create(RPCSERVER, EXLAYPROG, EXLAYVERS, "udp");
	if (client == NULL) {
		clnt_pcreateerror(RPCSERVER);
		exit(EXIT_FAILURE);
	}
	struct timeval to = {3600, 0};
	struct timeval rto = {3600, 0};
	if (clnt_control(client, CLSET_TIMEOUT, (char *)&to) < 0) {
		fprintf(stderr, "clnt_control: failed to set timeout\n");
		exit(EXIT_FAILURE);
	}
	if (clnt_control(client, CLSET_RETRY_TIMEOUT, (char *)&rto) < 0) {
		fprintf(stderr, "clnt_control: failed to set retry timeout\n");
		exit(EXIT_FAILURE);
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
			debug_printf("cmd = %s\n", argv[1]);
			cmd_table[i].cmd_func(argc, argv);
			break;
		}
	}

	/* unkown command is specified */
	if (i == NR_CMDS) {
		func_exlay_help(argc, argv);
	}
	return 0;
}
