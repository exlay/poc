#include <stdint.h>

/* exdata: the data structure in exlay
 * */
struct exdata {
	uint8_t *data;
	uint8_t *nxt_hdr;
};

/* 
 * at first, I implement IPv4, ARP, and Ethernet */
/* this is for protocol developer */
struct protobj {
	char *name;
	uint8_t bind_size;
	uint8_t upper_type_size;
	int (*d_input)(struct exdata *exd, uint32_t datalen);
	int (*d_output)(struct exdata *exd, uint32_t datalen);
	void (*c_push)(uint8_t *table, uint8_t *entry, uint32_t ent_size);
	void (*c_pull)(uint8_t *table, uint8_t *entry, uint32_t *ent_size);
};

/* exlay_ep: exlay endpoint is used in exlay system, not in app. */
struct exlay_ep {
	int sock;
	uint8_t nr_protos;
	char *ifname;
	struct exlay_stack *top; /* top layer */
	struct exlay_stack *btm; /* buttom layer */
	struct exlay_stack *cur; /* current layer */
	struct exlay_ep *prev;
	struct exlay_ep *next;
};

struct exlay_stack {
	uint8_t layer;
	struct exlay_ep *ep; /* exlay endpoint */
	struct protobj *proto;
	void *for_lower; /* upper layer's ID: */
	void *lbind;
	void *rbind;
};


extern int exd_out(struct exdata *exd, uint32_t len);
extern int exd_in(struct exdata *exd, uint32_t len);
