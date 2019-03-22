#include <sys/types.h>
#include <stdint.h>

struct t_iphdr {
#if __BYTE_ORDER == __LITTLE_ENDIAN
	unsigned int ihl:4;
	unsigned int version:4;
#elif __BYTE_ORDER == __BIG_ENDIAN
	unsigned int version:4;
	unsigned int ihl:4;
#else
#endif
	uint8_t tos;
	uint16_t tot_len;
	uint16_t id;
	uint16_t frag_off; /* fragment offset */
	uint8_t ttl;
	uint16_t protocol;
	uint16_t check;
	uint32_t saddr;
	uint32_t daddr;
};
