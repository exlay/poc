#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "test_arp.h"
#include "protocol.h"

static int test_arp_in(struct exdata *data, uint32_t msg_len) {

}

static int test_arp_out(struct exdata *data, uint32_t msg_len) {

}

static void test_arp_push(
		uint8_t *table, 
		uint8_t *entry, 
		uint32_t ent_size) {

}

static void test_arp_pull(
		uint8_t *table, 
		uint8_t *entry, 
		uint32_t *ent_size) {
}

struct protobj test_arp = {
	.name = "test_arp",
	.upper_type_size = 0,
	.bind_size = 0,
	.d_input = test_arp_in,
	.d_output = test_arp_out,
	.c_push = test_arp_push,
	.c_pull = test_arp_pull,
};

