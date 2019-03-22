/* libproto.c: exlay protocol library: for exlay_daemon, each protocol
 *
 * TODO: 
 * */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <error.h>
#include <errno.h>
#include <dlfcn.h>
#include <string.h>
#include <rpc/rpc.h>
#include <time.h>

#include "protocol.h"
#include "exlay.h"
#include "exlay/error.h"


int exd_out(struct exdata *exd, uint32_t len)
{
	int result = EXIT_FAILURE;
	/* set the current layer down */
	exd->cur = exd->cur->lolyr;
	if (exd->cur == NULL) {
		fprintf(stderr, "exd_out: invalid layer\n");
		return -1;
	}

	if (exd->cur->lolyr == NULL) {
		/* reach the buttom of this stack */
		result = write(exd->sock, exd->data, len);
	} else {
		/* go to lower layer */
		result = exd->cur->protob->d_output(exd, len);
	}

	return result;
}

int upper_cmp(uint8_t *u1, uint8_t *u2, uint32_t upsize)
{
	int result = 0;
	if (u1 == u2 && u1 == NULL) {
		result = 0;
		goto OUT;
	}

	if (u1 != NULL && u2 != NULL) {
		result = memcmp(u1, u2, upsize);
		if (upsize == 0) {
			result = 1;
		}
	} else {
		result = 1;
	}

OUT:
	return result;
}

int bind_cmp(uint8_t *b1, uint8_t *b2, uint32_t bsize)
{
	int result = 0;
	if (b1 == b2 && b1 == NULL) {
		result = 0;
		goto OUT;
	}

	if (b1 != NULL && b2 != NULL) {
		result = memcmp(b1, b2, bsize);
		if (bsize == 0) {
			result = 1;
		}
	} else {
		result = 1;
	}

OUT:
	return result;
}

int exd_in(struct exdata *exd, uint32_t len)
{
	int result = EXIT_FAILURE;
	/*
	 * 受信したパケットから，rbind と upper_type を取得
	 * exd->cur->fbind から，取得した binding に該当するものを探す
	 * */
	struct binding_tree *cb; /* ptr to search the current binding  */
	int ret;
	for (cb = exd->cur->fbind; cb != NULL; cb = cb->fbind) {
		if ((ret = bind_cmp(cb->entry->lbind, exd->d_rbind, exd->bind_s)) == 0 &&
				(ret = upper_cmp(cb->entry->upper, exd->d_upper, exd->upper_s)) == 0) {
			/* the binding found */
			/* whether reaching the top of the stack or not */
			int ret;
			if (cb->is_top) {
				ret = write(cb->app_r, exd->data, exd->datalen);
				if (ret < 0) {
					perror("write: exd_in");
				}
			}

			/* 
			 * not the top of the layer 
			 */

			if (cb->is_top && cb->uplyr == NULL) {
				debug_printf2("%s --> App\n", cb->entry->protob->name);
				return ret;
			}

			/* found the next protocol */
			debug_printf2("%s --> %s", 
					cb->protob->name, 
					cb->uplyr->fp->protob->name);
			ret = cb->uplyr->fp->protob->d_input(exd, len);
			return ret;
		}
	}

	/* no such protocol or binding */
	debug_printf2("===== pkt drop in %s in layer %d =====\n", 
			exd->cur->protob->name,
			exd->cur->layer);
	unsigned int i;
	for (i = 0; i < exd->datalen; i++) {
		if ((i % 16) == 0 && i != 0) {
			debug_printf2("\n");
		}
		debug_printf2("%02X ", exd->data[i]);
	}
	debug_printf2("\n===============================================\n");

	return result;
}

/* intput: exd
 * output: size
 * */
uint8_t *exd_get_lbind(struct exdata *exd, uint32_t *size)
{
	*size = exd->cur->entry->protob->bind_size;
	return exd->cur->entry->lbind;
}
uint8_t *exd_get_rbind(struct exdata *exd, uint32_t *size)
{
	*size = exd->cur->entry->protob->bind_size;
	return exd->cur->entry->rbind;
}
uint8_t *exd_get_upper(struct exdata *exd, uint32_t *size)
{
	*size = exd->cur->entry->protob->upper_type_size;
	return exd->cur->entry->upper;
}
