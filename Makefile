CC = gcc
CFLAGS := -Wall
LDFLAGS =

SRC = exlay_daemon.c
OBJ = $(%.o:%.c)

TARGETS = exlay_daemon

.c.o:
	$(CC) $(CFLAGS) -c $<

all: $(TARGETS)

clean:
	rm -rf *.o $(TARGETS)
