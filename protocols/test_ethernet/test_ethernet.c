#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "test_ethernet.h"
#include "protocol.h"

struct test_ether eth_h;

static int test_ethernet_in(struct exdata *exd, uint32_t frame_len) {
	exd_in(exd, frame_len);
}

static int test_ethernet_out(struct exdata *exd, uint32_t frame_len) {
	exd_out(exd, frame_len);
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

struct protobj test_ethernet = {
	.name = "test_ethernet",
	.upper_type_size = 2, /* ethernet frame type field */
	.bind_size = 6,
	.d_input = test_ethernet_in,
	.d_output = test_ethernet_out,
	.c_push = test_ethernet_push,
	.c_pull = test_ethernet_pull,
};
