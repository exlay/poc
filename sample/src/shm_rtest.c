#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>

#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

#include "exlay.h"
#include "protocol.h"

#define STRING "hello, world"
#define LSTRING "HELLO, WORLD"

int main(void)
{
	int fd;
	char *buf;
	struct stat stat;

	fd = shm_open("/shared_mem", O_CREAT|O_RDWR, FILE_MODE);

	if (fd < 0) {
		fprintf(stderr, "shm_open failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	ftruncate(fd, 9216);
	fstat(fd, &stat);

	int i;

	buf = mmap(NULL, stat.st_size, PROT_READ|PROT_WRITE, 
			MAP_SHARED, fd, 0);
	close(fd);

	for (i = 0; i < 7; i++) {
		memset(buf, 0, 100);
		read(fd, buf, strlen(STRING));
		printf("buf = %s\n", buf);
	}
	
	shm_unlink("/shared_mem");
	return 0;
}
