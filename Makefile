CC = gcc
CFLAGS = -Wall -Wextra -ggdb -g3 -MMD -MP -fPIC
CFLAGS += -DDEBUG
LDFLAGS = -lexlay -ldl
LIBRARY = -L./lib

TOPDIR = $(CURDIR)
TARGETS = exlay_daemon exlay 
SRCDIR = ./src
OBJDIR = ./build
BINDIR = ./bin
INCDIR = ./include
LIBDIR = $(CURDIR)/lib

XDRSRC = exlay_rpc.x
XDRSVC = $(XDRSRC:%.x=%_svc.c)
XDRCLNT = $(XDRSRC:%.x=%_clnt.c)
XDRXDR = $(XDRSRC:%.x=%_xdr.c)
XDRHDR = $(XDRSRC:%.x=%.h)

XDROUTS = $(INCDIR)/$(XDRHDR) \
		  $(SRCDIR)/$(XDRSVC) \
		  $(SRCDIR)/$(XDRCLNT) \
		  $(SRCDIR)/$(XDRXDR) \
		  $(OBJDIR)/$(XDRSVC:%.c=%.o) \
		  $(OBJDIR)/$(XDRCLNT:%.c=%.o) \
		  $(OBJDIR)/$(XDRXDR:%.c=%.o) 

EXLAYCLI = $(BINDIR)/exlay
CLIOBJS = $(addprefix $(OBJDIR)/,$(XDRXDR:%.c=%.o) $(XDRCLNT:%.c=%.o) exlay.o)

EXLAYDAEMON = $(BINDIR)/exlay_daemon
DAEMONOBJS = $(addprefix $(OBJDIR)/,$(XDRXDR:%.c=%.o) $(XDRSVC:%.c=%.o) exlay_daemon.o)

RM = rm -rf
RMDIR = rmdir

TAGS = tags cscope.out

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(addprefix $(OBJDIR)/,$(notdir $(SRCS:%.c=%.o)))
DEPS = $(OBJS:%.o=%.d)
BINS = $(addprefix $(BINDIR)/,$(TARGETS))
INCLUDE = $(addprefix -I,$(INCDIR))

LIB = $(LIBDIR)/libexlay.so $(LIBDIR)/libproto.so
PLIBDIR = $(CURDIR)/protocols/lib

CONF_LIBPATHS = $(wildcard $(PLIBDIR)/*.so)
CONF_LIBNAMES = $(notdir $(basename $(CONF_LIBPATHS)))
CONF_TMP = $(addsuffix "\t",$(CONF_LIBNAMES))
CONF_PAIRS = $(join $(CONF_TMP), $(CONF_LIBPATHS))

SRCFS = $(notdir $(SRCS))
OBJFS = $(SRCFS:%.c=%.o)
DEPFS = $(SRCFS:%.c=%.d)

.PHONY: clean tag all rpc sample protocols exlay
.PRECIOUS: $(OBJS) $(DEPS)

all: exlay protocols sample

exlay: $(XDROUTS) $(LIB) $(BINS)

protocols:
	+make -C ./protocols

sample:
	+make -C ./sample

$(EXLAYCLI): $(CLIOBJS)
	if [ ! -d "$(BINDIR)" ]; then mkdir $(BINDIR); fi
	$(CC) $(CFLAGS) $(LIBRARY) -o $@ $^ $(LDFLAGS)

$(EXLAYDAEMON): $(DAEMONOBJS)
	if [ ! -d "$(BINDIR)" ]; then mkdir $(BINDIR); fi
	$(CC) $(CFLAGS) $(LIBRARY) -o $@ $^ $(LDFLAGS) -lproto -pthread

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	if [ ! -d "$(OBJDIR)" ]; then mkdir $(OBJDIR); fi
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDE)

$(LIBDIR)/%.so: $(OBJDIR)/%.o $(OBJDIR)/$(XDRCLNT:%.c=%.o) $(OBJDIR)/$(XDRXDR:%.c=%.o)
	if [ ! -d $(LIBDIR) ]; then mkdir $(LIBDIR); fi
	$(CC) -shared -Wl,-soname,$(LIBDIR)/$(notdir $@),-rpath,$(PLIBDIR) -o $@ $^

$(INCDIR)/%.h: $(SRCDIR)/%.x
	cd $(SRCDIR) && rpcgen -Nh $(notdir $<) > ../$@

$(SRCDIR)/%_svc.c: $(SRCDIR)/%.x
	cd $(SRCDIR) && rpcgen -Nm $(notdir $<) > ../$@

$(SRCDIR)/%_clnt.c: $(SRCDIR)/%.x
	cd $(SRCDIR) && rpcgen -Nl $(notdir $<) > ../$@

$(SRCDIR)/%_xdr.c: $(SRCDIR)/%.x
	cd $(SRCDIR) && rpcgen -Nc $(notdir $<) > ../$@

config: exlay.conf
	@echo $(CONF_PAIRS) | xargs -n2 > $<

tag:
	ctags -R
	cscope -Rb

clean:
	$(RM) $(OBJS) $(DEPS) $(BINS) $(LIB) $(XDROUTS) $(TAGS)
	-$(RMDIR) $(OBJDIR) $(BINDIR) $(LIBDIR)
	-echo -n "" > exlay.conf
	make -C ./protocols clean
	make -C ./sample clean

-include $(DEPS)
