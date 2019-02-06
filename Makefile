CC = gcc
CFLAGS = -Wall -Wextra -ggdb -g3
CFLAGS += -DDEBUG
LDFLAGS =

SRC = exlay_daemon.c exlay.c test.c
OBJ = $(%.o:%.c)

.PHONY: clean tag all

TARGETS = exlay_daemon exlay test

.c.o:
	$(CC) $(CFLAGS) -c $<

all: $(TARGETS)

tag: tags cscope.out
	ctags -R
	cscope -Rb

clean:
	rm -rf *.o $(TARGETS)

test.o: exlay.h
