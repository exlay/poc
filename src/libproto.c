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
	exd->cur = exd->cur->lower;
	if (exd->cur == NULL) {
		fprintf(stderr, "exd_out: invalid layer\n");
		return -1;
	}

	if (exd->cur->lower == NULL) {
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
	return len;
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
