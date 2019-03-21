#ifndef __EXLAY__
#define __EXLAY__

#include <limits.h>
#include <time.h>
#include <stdint.h>
#include <fcntl.h>

#define DAEMON_PORT 11017

#ifdef DEBUG
#define debug_printf(fmt, ...) fprintf(stderr, "%s(%d)debug: " fmt, __func__, __LINE__, ## __VA_ARGS__)
#define debug_printf2(fmt, ...) fprintf(stderr, fmt, ## __VA_ARGS__)
#else
#define debug_printf(fmt, ...)
#define debug_printf2(fmt, ...)
#endif

#define INSERT_TO_LIST_HEAD(h, p) \
		do {\
				(p)->bp = (h); \
				(p)->fp = (h)->fp; \
				(h)->fp->bp = (p); \
				(h)->fp = (p);	\
		   	} while (0)

#define INSERT_TO_LIST_TAIL(h, p) \
		do {\
				(p)->bp = (h)->bp; \
				(p)->fp = (h)->fp->bp; \
				(h)->bp->fp = (p);	\
				(h)->bp = (p); \
		   	} while (0)

#define REMOVE_FROM_LIST(p) \
		do {\
				(p)->bp->fp = (p)->fp; \
				(p)->fp->bp = (p)->bp; \
				(p)->fp = (p)->bp = NULL;	\
		   	} while (0)

#define MAXCMDLEN 6

#define MAXBUFLEN 65536
#define MAXNRPROT 256
#define MAXPROTNAMELEN 256
#define MAXPROTPATHLEN 256
#define EXLAYHDRSIZE (sizeof(struct exlay_hdr))

struct proto_info {
	struct proto_info *fp;
	struct proto_info *bp;
	char *name;
	char *path;
	time_t ctime;
	//struct netdev_ops *exlay_ops;
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

#define CODE_OK 0x00
#define CODE_INVREQ 0x02
#define CODE_NEMPTY 0x03
#define CODE_DUP 0x04
#define CODE_NMEM 0x05
#define CODE_NEXIST 0x06
#define CODE_NFND 0x07
#define CODE_NEXEP 0x08
#define CODE_NLYR 0x09
#define CODE_NPRTLIB 0x0a
#define CODE_EDLOPEN 0x0b
#define CODE_EDLSHM 0x0c
#define CODE_EDIFFBS 0x0d
#define CODE_EMKFIFO 0x0e
#define CODE_EBIND 0x0f
#define CODE_NG  0xff

#define NR_CMDS 6

//#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#define FILE_MODE 0777
#define RDPATHPREFIX "/tmp/r_"
#define WRPATHPREFIX "/tmp/w_"

#define MAX_PATHLEN 16

#define CONFIG_FILE "./exlay.conf"
#define MAX_CFG_LINELEN 512

int ex_create_stack(unsigned int nr_layer);
int ex_set_binding(
		int ep, 
		unsigned int layer, 
		char *proto, 
		void *lbind,
		unsigned int bsize, /* binding size */
		int upper);
int ex_bind_stack(int ep);
int ex_set_remote(int ep, int layer, void *binding, unsigned int bsize);
int ex_dial_stack(int ep);
int ex_listen_stack(int ep);
int ex_send_stack(int ep, void *buf, uint32_t size, int opt);
int ex_recv_stack(int ep, void *buf, uint32_t size, int opt);
int ex_close_stack(int ep);

#define RPCSERVER "127.0.0.1" 
#endif
