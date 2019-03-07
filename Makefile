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
XDRSVC = $(SRCDIR)/$(XDRSRC:%.x=%_svc.c)
XDRCLNT = $(SRCDIR)/$(XDRSRC:%.x=%_clnt.c)
XDRXDR = $(SRCDIR)/$(XDRSRC:%.x=%_xdr.c)
XDRHDR = $(INCDIR)/$(XDRSRC:%.x=%.h)

RM = rm -rf
RMDIR = rmdir

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(addprefix $(OBJDIR)/,$(notdir $(SRCS:%.c=%.o)))
DEPS = $(OBJS:%.o=%.d)
BINS = $(addprefix $(BINDIR)/,$(TARGETS))
INCLUDE = $(addprefix -I,$(INCDIR))

LIB = $(LIBDIR)/libexlay.so
PLIBDIR = $(CURDIR)/protocols/lib

SRCFS = $(notdir $(SRCS))
OBJFS = $(SRCFS:%.c=%.o)
DEPFS = $(SRCFS:%.c=%.d)

.PHONY: clean tag all rpc
.PRECIOUS: $(OBJS) $(DEPS)

all: 
	+make $(XDRHDR) $(XDRSVC) $(XDRCLNT) $(XDRXDR)
	+make $(LIB)
	+make $(BINS) 
	+make -C ./protocols
	+make -C ./sample

$(BINDIR)/%: $(OBJDIR)/%.o
	if [ ! -d "$(BINDIR)" ]; then mkdir $(BINDIR); fi
	$(CC) $(CFLAGS) $(LIBRARY) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	if [ ! -d "$(OBJDIR)" ]; then mkdir $(OBJDIR); fi
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDE)

$(LIBDIR)/%.so: $(OBJDIR)/%.o $(XDRCLNT:%.c=%.o) $(XDRXDR:%.c=%.o)
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

tag: tags cscope.out
	ctags -R
	cscope -Rb


clean:
	$(RM) $(OBJS) $(DEPS) $(BINS) $(LIB) $(XDRSVC) $(XDRCLNT) $(XDRXDR) $(XDRHDR)
	-$(RMDIR) $(OBJDIR) $(BINDIR) $(LIBDIR)
	make -C ./protocols clean
	make -C ./sample clean

-include $(DEPS)
