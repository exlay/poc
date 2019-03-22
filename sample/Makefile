CC = gcc
CFLAGS = -Wall -Wextra -ggdb -g3 -MMD -MP
CFLAGS += -DDEBUG
LDFLAGS = -lexlay -lrt

SRCDIRS = ./src
OBJDIR = ./build
BINDIR = ./bin
INCDIRS = ../include
LIBDIRS = $(CURDIR)/../lib

TARGETS = ether_app bind_test shm_rtest fifo_rtest write_ether

# User should add the name of the exlay application
TARGETS += 

RM = rm -rf
RMDIR = rmdir

SRCS = $(wildcard $(SRCDIRS)/*.c)
OBJS = $(addprefix $(OBJDIR)/,$(notdir $(SRCS:%.c=%.o)))
DEPS = $(OBJS:%.o=%.d)
BINS = $(addprefix $(BINDIR)/,$(TARGETS))
INCLUDE = $(addprefix -I,$(INCDIRS))
LIBRARY = $(addprefix -L,$(LIBDIRS))

SRCFS = $(notdir $(SRCS))
OBJFS = $(SRCFS:%.c=%.o)
DEPFS = $(SRCFS:%.c=%.d)

.PHONY: clean tag all 
.PRECIOUS: $(OBJS) $(DEPS)

all: $(BINS)

$(BINDIR)/%: $(OBJDIR)/%.o
	if [ ! -d "$(BINDIR)" ]; then mkdir $(BINDIR); fi
	$(CC) $(CFLAGS) $(LIBRARY) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIRS)/%.c
	if [ ! -d "$(OBJDIR)" ]; then mkdir $(OBJDIR); fi
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDE) 

tag: tags cscope.out
	ctags -R
	cscope -Rb

clean:
	$(RM) $(OBJS) $(DEPS) $(BINS)
	-$(RMDIR) $(OBJDIR) $(BINDIR)

-include $(DEPS)
