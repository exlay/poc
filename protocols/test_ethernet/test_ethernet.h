#ifndef __TEST_ETHERNET__
#define __TEST_ETHERNET__
#include <stdint.h>
#include <net/ethernet.h>

#define MACADDRLEN 6

struct test_ether {
	uint8_t dstmac[MACADDRLEN];
	uint8_t srcmac[MACADDRLEN];
	uint16_t type;
} __attribute__ ((packed));

#define ETH_TYPE_ARP ETHERTYPE_ARP
#define ETH_TYPE_IP4 ETHERTYPE_IP
#define ETH_TYPE_IP6 ETHERTYPE_IPV6
#define ETH_TYPE_VLAN ETHERTYPE_VLAN
#define ETH_TYPE_LOOPBACK ETHERTYPE_LOOPBACK

#define ETH_TYPE_LEN ETHER_TYPE_LEN
#define ETH_CRC_LEN ETHER_CRC_LEN
#define ETH_HDR_LEN ETH_HLEN

#define ETH_MIN_LEN ETHER_MIN_LEN
#define ETH_MAC_LEN ETHER_MAX_LEN

#define __TEST_ETHERNET__

#endif
