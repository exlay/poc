#include <stdio.h>
#include <stdint.h>
#include "test_ipv4.h"
#include "protocol.h"

static int test_ipv4_in(struct exdata *data, uint32_t msg_len) {

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

