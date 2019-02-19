#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/protocol.h"

static int test_arp_in(uint8_t *data, uint32_t msg_len) {

}

static int test_arp_out(uint8_t *data, uint32_t msg_len) {

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

struct protobj proto_arp = {
	.name = "test_arp",
	.bind_size = 0,
	.d_input = test_arp_in,
	.d_output = test_arp_out,
	.c_push = test_arp_push,
	.c_pull = test_arp_pull,
};

