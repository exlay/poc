#include <stdio.h>
#include <stdint.h>
#include "test_ipv4.h"
#include "protocol.h"

static int test_ipv4_in(struct exdata *data, uint32_t msg_len) {
	/* 1. header analyze */
	/* 2. search routing table for dst IP */
	/* 2.1 if the pacekt is routed to other hosts */
	/* 2.1.1 decriment TTL */
	/* 2.1.2 resolve option */
	/* 2.1.3 search arptable for next host: send an event to ARP via arptable handler */
	/* 2.1.3 set automatic {l,r}bind in exdata (i.e., smac and dmac ) */
	/* 2.1.4 exd_out */
	/* 2.2 else if the packet is for local system */
	/* 2.2.1  reasemble the packets */
	/* 2.2.2  goto the upper layer: call exd_in()  */
	/* 2.3 else: no route to host: drop */
}

static int test_ipv4_out(struct exdata *data, uint32_t msg_len) {

}

static void test_ipv4_push(
		uint8_t *table, 
		uint8_t *entry, 
		uint32_t ent_size) {

}

static void test_ipv4_pull(
		uint8_t *table, 
		uint8_t *entry, 
		uint32_t *ent_size) {
}

struct protobj test_ipv4 = {
	.name = "test_ipv4",
	.bind_size = 4,
	.upper_type_size = 1, /* size of the Protocol field in the IPv4 header */
	.d_input = test_ipv4_in,
	.d_output = test_ipv4_out,
	.c_push = test_ipv4_push,
	.c_pull = test_ipv4_pull,
};

