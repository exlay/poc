#include <stdint.h>

/* exdata: the data structure in exlay
 * */
struct exdata {
	int sock;
	uint8_t *data;
	uint32_t datalen;
	uint8_t *nxt_hdr;
	
	uint8_t *d_lbind; /* pointer to the lbind field of each protocol */
	uint8_t *d_rbind; /* pointer to the rbind firld of each protocol */
	uint8_t *d_upper; /* pointer to the upper firld of each protocol */
	uint32_t bind_s; /* binding size of each protocol */
	uint32_t upper_s; /* upper type size of each protocol */

	uint8_t *a_lbind; /* automatic local bind */
	uint8_t *a_rbind; /* automatic remote bind */
	struct binding_tree *cur;
};

/* 
 * at first, I implement IPv4, ARP, and Ethernet */
/* this is defined by each protocol developer */
struct protobj {
	char *name;
	uint8_t bind_size;
	uint8_t upper_type_size;
	uint32_t hdr_size;
	int (*d_input)(struct exdata *exd, uint32_t datalen);
	int (*d_output)(struct exdata *exd, uint32_t datalen);
	void (*c_push)(uint8_t *table, uint8_t *entry, uint32_t ent_size);
	void (*c_pull)(uint8_t *table, uint8_t *entry, uint32_t *ent_size);
};

/* exlay_ep: exlay endpoint is used in exlay system, not in app. */
struct exlay_ep {
	int ep;
	uint8_t nr_layers;
	struct exlay_layer *top; /* top layer */
	struct exlay_layer *btm; /* buttom layer */
	struct binding_tree *topb; /* current layer */
	struct exlay_ep *fp; /* used by exlay system for an endpoint search */
	struct exlay_ep *bp; /* used by exlay system for an ednpoing search */
};

struct exlay_layer {
	uint8_t layer;
	struct protobj *protob;
	uint8_t *upper; /* upper layer's ID: */
	uint8_t *lbind;
	uint8_t *rbind;
};

/* binding_tree represent the binding information tree to
 * identify the exlay endpoint.
 * e.g., MAC, IP addr, Port, others...*/
struct binding_tree {
	int app_r;
	int app_w;
	uint8_t is_top;
	uint8_t layer;
	struct exlay_layer *entry;
	struct protobj *protob;
	uint8_t *upper;
	struct binding_tree *uplyr; /* point to the upper layer */
	struct binding_tree *lolyr; /* point to the lower layer */
	struct binding_tree *fp; /* point to the next protocol in this layer */
	struct binding_tree *fbind; /* point to the next bind */
};


extern uint8_t *exd_get_lbind(struct exdata *exd, uint32_t *size);
extern uint8_t *exd_get_rbind(struct exdata *exd, uint32_t *size);
extern uint8_t *exd_get_upper(struct exdata *exd, uint32_t *size);
extern int exd_out(struct exdata *exd, uint32_t len);
extern int exd_in(struct exdata *exd, uint32_t len);
