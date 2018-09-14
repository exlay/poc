CC = gcc
CFLAGS = -Wall -Wextra -ggdb
LDFLAGS =

SRC = exlay_daemon.c exlay.c
OBJ = $(%.o:%.c)

TARGETS = exlay_daemon exlay

.c.o:
	$(CC) $(CFLAGS) -c $<

all: $(TARGETS)

clean:
	rm -rf *.o $(TARGETS)
