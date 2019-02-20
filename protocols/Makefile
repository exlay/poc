CC := gcc
CFLAGS := -Wall -Wextra -ggdb -g3 -MMD -MP
CFLAGS += -DDEBUG
LDFLAGS =
PROTLIBDIR := ./lib

SRCDIRS = $(filter-out include Makefile,$(wildcard *))


RM = rm -rf

all:
	@echo $(SRCDIRS)
	@-mkdir $(PROTLIBDIR)

tag: tags cscope.out
	ctags -R
	cscope -Rb


clean:
	$(RM) $(OBJS) $(DEPS) $(BINS)

-include $(DEPS)
