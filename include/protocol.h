#include <stdint.h>

/* exdata: the data structure in exlay
 * */
struct exdata {
	uint8_t *data;
	uint8_t *nxt_hdr;
};

/* 
 * at first, I implement IPv4, ARP, and Ethernet */
/* this is defined by each protocol developer */
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
	struct exlay_layer *top; /* top layer */
	struct exlay_layer *btm; /* buttom layer */
	struct exlay_layer *cur; /* current layer */
	struct exlay_ep *prev; /* used by exlay system for an endpoint search */
	struct exlay_ep *next; /* used by exlay system for an ednpoing search */
};

struct exlay_layer {
	uint8_t layer;
	struct protobj *proto;
	void *upper; /* upper layer's ID: */
	void *lbind;
	void *rbind;
};

struct binding_tree {
	struct exlay_layer *entry;
	struct binding_tree *upper;
	struct binding_tree *lower;
	struct binding_tree *fp;
};

extern int exd_out(struct exdata *exd, uint32_t len);
extern int exd_in(struct exdata *exd, uint32_t len);
