#include <stdint.h>

/* 
 * at first, I implement IPv4, ARP, and Ethernet */
/* this is for protocol developer */
struct protobj {
	uint8_t *name;
	uint8_t bind_size;
	int (*d_input)(uint8_t *data, uint32_t msg_len);
	int (*d_output)(uint8_t *data, uint32_t msg_len);
	void (*c_push)(uint8_t *table, uint8_t *entry, uint32_t ent_size);
	void (*c_pull)(uint8_t *table, uint8_t *entry, uint32_t *ent_size);
};

/* exlay_ep: exlay endpoint is used by user program */
/* this is for application developer */
struct exlay_ep {
	uint8_t *binding;
	uint8_t bind_size;
	uint8_t *nxt_type;	/* IP: protocol 8bit, Ethernet: type 16bit */
	uint8_t nxt_type_size;
};

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
