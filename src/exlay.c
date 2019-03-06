#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "exlay.h"

static struct sockaddr_in daem_addr_in;
static int cli_sock;

static void print_code(uint8_t code)
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
		case CODE_NG:
			fprintf(stderr, "error: Something wrong with exlay daemon\n");
			break;
		default:
			fprintf(stderr, "error: Unknown error code\n");
	}
}

static void print_list_data(struct exlay_hdr *hdr, uint8_t *data)
{
	fprintf(stdout, "Protocol List: \n%s", data);
}

static void print_info_data(struct exlay_hdr *hdr, uint8_t *data)
{
	uint8_t *name = data;
	uint8_t *info = data + hdr->len_proto_name;

	fprintf(stdout, "Protocol Name: %s\n", name);
	fprintf(stdout, "Description:\n\n%s", info);
}

static void print_add_data(struct exlay_hdr *hdr, uint8_t *data)
{
	fprintf(stderr, "%s\n", (char *)data);
}
static void print_data(struct exlay_hdr *hdr, uint8_t *data)
{
	switch (hdr->cmd) {
		case CMD_LIST:
			print_list_data(hdr, data);
			break;
		case CMD_INFO:
			print_info_data(hdr, data);
			break;
		case CMD_ADD:
			print_add_data(hdr, data);
			break;
		default:
			fprintf(stderr, "unknown commmand: %d\n", hdr->cmd);
	}
}

static int send_and_recv_pkt(
		struct exlay_hdr *hdr, 
		uint8_t *data, 
		int *data_len)
{
	int ret;
	socklen_t sk_len = sizeof(struct sockaddr_in);
	uint8_t pkt[MAXPKTSIZE] = {0};

	memcpy(pkt, hdr, EXLAYHDRSIZE);
	memcpy(pkt + EXLAYHDRSIZE, data, *data_len);

	ret = sendto(cli_sock, pkt, EXLAYHDRSIZE + *data_len, 0,
			(struct sockaddr *)&daem_addr_in, sk_len);

	if (ret < 0) {
		perror("sendto");
		goto OUT;
	}

	memset(pkt, 0, MAXPAYLSIZE);

	ret = recvfrom(cli_sock, pkt, MAXPKTSIZE, 0,
			(struct sockaddr *)&daem_addr_in, &sk_len);

	*data_len = ret;

	if (ret < 0) {
		perror("recvfrom");
		goto OUT;
	}

   	memset(hdr, 0, EXLAYHDRSIZE);
	memset(data, 0, ret - EXLAYHDRSIZE);

	memcpy(hdr, pkt, EXLAYHDRSIZE);
	memcpy(data, pkt + EXLAYHDRSIZE, ret);

OUT:
	return ret;
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

	int ret;
	uint8_t data[MAXPKTSIZE] = {0};
	int data_len = 0;

	ret = send_and_recv_pkt(&hdr, data, &data_len);

	if (hdr.cmd != CMD_LIST) {
		fprintf(stderr, "invalid response: %d\n", hdr.cmd);
		goto OUT;
	}

	switch (hdr.code) {
		case CODE_OK:
			print_data(&hdr, data);
			break;
		default:
			print_code(hdr.code);
	}
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
		.len_proto_name = strlen(largv[2]) + 1,
		.len_proto_path = strlen(largv[3]) + 1,
	};

	int ret;
	uint8_t data[MAXPKTSIZE] = {0};
	int data_len = hdr.len_proto_name + hdr.len_proto_path;
	
	memcpy(data, largv[2], hdr.len_proto_name);
	memcpy(data + hdr.len_proto_name, largv[3], hdr.len_proto_path);

	ret = send_and_recv_pkt(&hdr, data, &data_len);

	if (hdr.cmd != CMD_ADD) {
		fprintf(stderr, "operation not supported\n");
		goto OUT;
	}

	switch (hdr.code) {
		case CODE_OK:
			break;
		default:
			print_code(hdr.code);
	}

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

	int ret;
	uint8_t data[MAXPKTSIZE] = {0};
	int data_len = hdr.len_proto_name + hdr.len_proto_path;
	
	memcpy(data, largv[2], hdr.len_proto_name);

	ret = send_and_recv_pkt(&hdr, data, &data_len);

	if (hdr.cmd != CMD_DEL) {
		fprintf(stderr, "operation not supported\n");
		goto OUT;
	}

	switch (hdr.code) {
		case CODE_OK:
			break;
		default:
			print_code(hdr.code);
	}

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

	int ret;
	uint8_t data[MAXPKTSIZE] = {0};
	int data_len = hdr.len_proto_name + hdr.len_proto_path;
	
	memcpy(data, largv[2], hdr.len_proto_name);
	memcpy(data + hdr.len_proto_name, largv[3], hdr.len_proto_path);

	ret = send_and_recv_pkt(&hdr, data, &data_len);

	if (hdr.cmd != CMD_UPDATE) {
		fprintf(stderr, "operation not supported\n");
		goto OUT;
	}

	switch (hdr.code) {
		case CODE_OK:
			break;
		default:
			print_code(hdr.code);
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

	if ((cli_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket");
		return errno;
	}

	daem_addr_in.sin_family = AF_INET;
	daem_addr_in.sin_port = DAEMON_PORT;
	inet_aton("127.0.0.1", &daem_addr_in.sin_addr);

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
	

	close(cli_sock);
	return 0;
	
}
