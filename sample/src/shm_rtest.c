#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>

#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

#include "exlay.h"
#include "protocol.h"

int main(void)
{
	int fd;
	int ret;

	fd = shm_open("/shared_mem", O_CREAT|O_RDWR, FILE_MODE);

	if (fd < 0) {
		fprintf(stderr, "shm_open failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (ret < 0) {
		fprintf(stderr, "ftruncate failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}


	return 0;
}
