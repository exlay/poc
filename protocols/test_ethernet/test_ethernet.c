#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "test_ethernet.h"
#include "protocol.h"

static int test_ethernet_in(uint8_t *data, uint32_t msg_len) {

}

static int test_ethernet_out(uint8_t *data, uint32_t msg_len) {

}

static void test_ethernet_push(
		uint8_t *table, 
		uint8_t *entry, 
		uint32_t ent_size) {

}

static void test_ethernet_pull(
		uint8_t *table, 
		uint8_t *entry, 
		uint32_t *ent_size) {
}

struct protobj proto_ethernet = {
	.name = "test_ethernet",
	.bind_size = 0,
	.d_input = test_ethernet_in,
	.d_output = test_ethernet_out,
	.c_push = test_ethernet_push,
	.c_pull = test_ethernet_pull,
};

