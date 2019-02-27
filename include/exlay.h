#ifndef __EXLAY__
#define __EXLAY__

#include <limits.h>
#include <time.h>
#include <stdint.h>

#define DAEMON_PORT 11017

#ifdef DEBUG
#define debug_printf(fmt, ...) fprintf(stderr, "%s(%d)debug: " fmt, __func__, __LINE__, ## __VA_ARGS__)
#else
#define debug_printf(fmt, ...)
#endif

#define MAXCMDLEN 6

#define MAXBUFLEN 65536
#define MAXNRPROT 256
#define MAXPROTNAMELEN 256
#define MAXPROTPATHLEN 256
#define EXLAYHDRSIZE (sizeof(struct exlay_hdr))

#define MAXPAYLSIZE ( ((MAXPROTNAMELEN * MAXNRPROT) > (MAXPROTNAMELEN + MAXBUFLEN)) ? \
	   	(MAXPROTNAMELEN * MAXNRPROT) : (MAXPROTNAMELEN + MAXBUFLEN) )



struct proto_info {
	struct proto_info *next;
	struct proto_info *prev;
	char name[MAXPROTNAMELEN];
	char *path;
	time_t ctime;
	char desc[MAXBUFLEN];
	//struct netdev_ops *exlay_ops;
};

/* exlay_hdr: packet header for communication between 
 * exlay cli and exlay daemon
 * */
struct exlay_hdr {
	uint8_t cmd;
	uint8_t code;
	uint16_t len_proto_name; /* or len. of protocol list in the case of CMD_LIST */
	uint16_t len_proto_path; /* or info. of a protocol in the case of CMD_INFO */
	uint16_t reserved;
};

/*
 * MAX data size of exlay payload from daemon to cli in each cmd
 *
 * CMD_LIST:	MAXPROTNAMELEN * MAXNRPROT (= 65536)
 * CMD_ADD:		MAXPROTNAMELEN + MAXPROTPATHLEN (= 512)
 * CMD_INFO:	MAXPROTNAMELEN + MAXBUFLEN (= 65792)
 * CMD_UPDATE:	MAXPROTNAMELEN + MAXPROTPATHLEN (= 512)
 *   
 * */
#define MAXPKTSIZE (MAXPAYLSIZE + EXLAYHDRSIZE)


#define CMD_HELP	0x00
#define CMD_LIST	0x01
#define CMD_ADD		0x02
#define CMD_INFO	0x03
#define CMD_DEL		0x04
#define CMD_UPDATE	0x05
#define CMD_UNKNOWN 0xff

#define CODE_REQ 0x00
#define CODE_OK  0x01
#define CODE_INVREQ 0x02
#define CODE_NEMPTY 0x03
#define CODE_DUP 0x04
#define CODE_NMEM 0x05
#define CODE_NEXIST 0x06
#define CODE_NG  0xff

#define NR_CMDS 6
#define NR_DAEM_CMDS 5

void print_exlay_hdr(struct exlay_hdr *hdr)
{
	debug_printf("========== exlay header ==========\n");
	switch (hdr->cmd) {
		case CMD_LIST:
			debug_printf("cmd: CMD_LIST\n");
			break;
		case CMD_ADD:
			debug_printf("cmd: CMD_ADD\n");
			break;
		case CMD_INFO:
			debug_printf("cmd: CMD_INFO\n");
			break;
		case CMD_DEL:
			debug_printf("cmd: CMD_DEL\n");
			break;
		case CMD_UPDATE:
			debug_printf("cmd: CMD_UPDATE\n");
			break;
		default:
			debug_printf("cmd: unknown command\n");
	}
	switch (hdr->code) {
		case CODE_REQ:
			debug_printf("code: CODE_REQ\n");
			break;
		case CODE_OK:
			debug_printf("code: CODE_OK\n");
			break;
		case CODE_NG:
			debug_printf("code: CODE_NG\n");
			break;
		default:
			debug_printf("code: unknown code\n");
	}
	debug_printf("len_proto_name: %d\n", hdr->len_proto_name);
	debug_printf("len_proto_path: %d\n", hdr->len_proto_path);
	debug_printf("==================================\n");
}
#endif



/* exlay_ep_node: a node in the tree structure of protocols in exlay */
/* this is for exlay system developer */
struct exlay_ep_node {
	uint8_t *binding;
	uint8_t bind_size;
	uint8_t *nxt_type;
	uint8_t nxt_type_size;
	struct exlay_ep_node *fp;
	struct exlay_ep_node *bp;
};

int ex_create_stack(int nr_layer);

int ex_set_binding(
		int ep, 
		unsigned int layer, 
		char *proto, 
		void *lbind,
		void *for_lower);

int ex_bind_stack(int ep);

int ex_set_remote(int ep, int layer, void *binding);

int ex_dial_stack(int ep);

int ex_send_stack(int ep, uint32_t size);

int ex_recv_stack(int ep, uint32_t size);

int ex_close_stack(int ep);

