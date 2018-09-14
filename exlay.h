#define DAEMON_PORT 11017

#ifdef DEBUG
#define debug_printf(fmt, ...) fprintf(stderr, "%s(%d)debug: " fmt, __func__, __LINE__, ## __VA_ARGS__)
#else
#define debug_printf(fmt, ...)
#endif

#define MAXCMDLEN 256

struct proto_info {
	char *name;
	struct netdev_ops *exlay_ops;
};

struct exlay_hdr {
	uint8_t cmd;
	uint8_t code;
	uint8_t len_proto_name;
	uint8_t len_proto_path;
};

#define CMD_HELP	0x00
#define CMD_LIST	0x01
#define CMD_ADD		0x02
#define CMD_INFO	0x03
#define CMD_DEL		0x04
#define CMD_UPDATE	0x05
#define CMD_UNKNOWN 0xff

#define CODE_REQ 0x00
#define CODE_OK  0x01
#define CODE_NG  0xff

#define NR_CMDS 6
#define NR_DAEM_CMDS 5

