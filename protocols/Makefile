CC := gcc
CFLAGS := -Wall -Wextra -ggdb -g3 -MMD -MP
CFLAGS += -DDEBUG
LDFLAGS = -lproto
LIBDIR = ../lib

INCDIRS := ../include
INCLUDE = $(addprefix -I,$(INCDIRS))

#LIBLIST := $(filter-out include Makefile lib protolib.map,$(wildcard *))
LIBLIST := \
	test_arp \
	test_ethernet \
	test_ipv4

LINKDIR := $(CURDIR)/lib

OBJS := $(join $(addsuffix /,$(LIBLIST)),$(LIBLIST:%=%.o))
DEPS := $(OBJS:%.o=%.d)
SRCS := $(OBJS:%.o=%.c)
LIBS := $(OBJS:%.o=%.so)
LINKS := $(addprefix $(LINKDIR)/lib,$(notdir $(OBJS:%.o=%.so)))

RM = rm -rf
RMDIR = rmdir
LN = ln -sf

.PHONY: clean tags

all: $(LIBS)

$(LIBS): %.so: %.o
	if [ ! -d $(LINKDIR) ]; then mkdir $(LINKDIR); fi
	$(CC) -shared -Wl,-soname,$(LINKDIR)/lib$(notdir $@) -o $@ $^ -L../lib $(LDFLAGS)
	$(LN) ../$@ $(LINKDIR)/lib$(notdir $@) 

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -fPIC -o $@ -c $<

tags: tags cscope.out
	ctags -R
	cscope -Rb

clean:
	$(RM) $(OBJS) $(DEPS) $(LINKS) $(LIBS)
	if [ -d $(LINKDIR) ]; then $(RMDIR) $(LINKDIR); fi


-include $(DEPS)
