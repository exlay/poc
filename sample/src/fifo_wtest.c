#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define PATHNAME "/tmp/myfifo"
#define STR1 "hello, world"
#define STR2 "HELLO, INL"
#define STR3 "My name is nelio"

#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

int main(void)
{
	int fd;
	if (mkfifo(PATHNAME, FILE_MODE) < 0) {
		perror("mkfifo");
		exit(EXIT_FAILURE);
	}

	if ((fd = open(PATHNAME, O_RDWR)) < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	write(fd, STR1, strlen(STR1));
	write(fd, STR2, strlen(STR2));
	write(fd, STR3, strlen(STR3));


	close(fd);
}
