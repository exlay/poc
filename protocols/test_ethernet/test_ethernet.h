#include <stdint.h>
struct test_ether {
	uint8_t dstmac[6];
	uint8_t srcmac[6];
	uint16_t type;
};
