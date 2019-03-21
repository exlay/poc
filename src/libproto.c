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

int exd_in(struct exdata *exd, uint32_t len)
{
	int result = EXIT_FAILURE;
	/*
	 * 受信したパケットから，rbind と upper_type を取得
	 * exd->cur->fbind から，取得した binding に該当するものを探す
	 * */
	struct binding_tree *cb; /* ptr to search the current binding  */
	struct binding_tree *nxt_prot = NULL;
	for (cb = exd->cur->fbind; cb != NULL; cb = cb->fbind) {
		if (memcmp(cb->entry->lbind, exd->d_rbind, exd->bind_s) == 0) {
			/* the binding found */
			/* whether reaching the top of the stack or not */
			if (cb->is_top) {
				int ret;
				ret = write(cb->app_r, exd->data, exd->datalen);
				if (ret < 0) {
					perror("write: exd_in");
				}
			}

			/* 
			 * not the top of the layer 
			 */

		    /* get the upper layer protocol root */
			nxt_prot = cb->uplyr;

			/* search the next protocol by checking upper type */
			/* ptr to search the current next protocol by upper */
			struct binding_tree *np; 
			for (np = nxt_prot->fp; np != NULL; np = np->fp) {
				if (exd->d_upper == NULL) {
					/* upper protocol should be specified by d_rbind */
					exd->d_lbind;
				} else if (memcmp(np->upper, exd->d_upper, exd->upper_s) == 0) {
					/* found the next protocol */
					np->protob->d_input(exd, len);
				}
			}
		}
	}

	/* no such protocol or binding */
	debug_printf("pkt drop in %s in layer %d\n", 
			exd->cur->protob->name,
			exd->cur->layer);

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
