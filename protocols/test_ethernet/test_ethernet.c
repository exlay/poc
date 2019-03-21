#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#include "test_ethernet.h"
#include "protocol.h"

static uint8_t bcmac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static uint8_t zeromac[6] = {0};

struct test_ether eth_h;

static int test_ethernet_in(struct exdata *exd, uint32_t datalen)
{
	int ret;
	uint32_t bsize;
	uint32_t upsize;
	uint32_t up;

	struct test_ether *eh;
	eh = (struct test_ether *)exd->data;
	exd->d_rbind = eh->dstmac;
	exd->d_lbind = eh->srcmac;
	exd->d_upper = (uint8_t *)&eh->type;
	exd->nxt_hdr = (uint8_t *)(exd->data + ETH_HDR_LEN);

	return exd_in(exd, datalen);
}

static int test_ethernet_out(struct exdata *exd, uint32_t datalen) 
{
	int ret;
	uint32_t bsize;
	uint32_t upsize;
	uint32_t up;

	uint8_t *lbindp = exd_get_lbind(exd, &bsize);
	uint8_t *rbindp = exd_get_rbind(exd, &bsize);
	uint8_t *upperp = exd_get_upper(exd, &upsize);
	memcpy(&up, upperp, upsize);

	struct test_ether *hp = (struct test_ether *)exd->nxt_hdr;
	uint8_t *eth_data_sp = (uint8_t *)(exd->nxt_hdr + ETH_HDR_LEN);
	uint8_t *eth_data_ep = eth_data_sp + datalen;
	memcpy(hp->srcmac, lbindp, bsize);
	memcpy(hp->dstmac, rbindp, bsize);
	hp->type = htons(up);

	if (datalen < ETH_ZLEN) {
		memset(eth_data_ep, 0, ETH_ZLEN - datalen);
		ret = exd_out(exd, ETH_ZLEN);
	} else {
		ret = exd_out(exd, datalen);
	}

	return ret;
}


struct protobj test_ethernet = {
	.name = "test_ethernet",
	.upper_type_size = ETH_TYPE_LEN, /* ethernet frame type field */
	.bind_size = MACADDRLEN,
	.hdr_size = ETH_HDR_LEN,
	.d_input = test_ethernet_in,
	.d_output = test_ethernet_out,
	.c_push = NULL,
	.c_pull = NULL,
};
