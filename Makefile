CC = gcc
CFLAGS = -Wall -Wextra -ggdb -g3 -MMD -MP -fPIC
CFLAGS += -DDEBUG
LDFLAGS =

TARGETS = exlay_daemon exlay 
SRCDIRS = ./src
OBJDIR = ./build
BINDIR = ./bin
INCDIRS = ./include
LIBDIR = $(CURDIR)/lib

RM = rm -rf
RMDIR = rmdir

SRCS = $(wildcard $(SRCDIRS)/*.c)
OBJS = $(addprefix $(OBJDIR)/,$(notdir $(SRCS:%.c=%.o)))
DEPS = $(OBJS:%.o=%.d)
BINS = $(addprefix $(BINDIR)/,$(TARGETS))
INCLUDE = $(addprefix -I,$(INCDIRS))

LIB = $(LIBDIR)/libexlay.so
PLIBDIR = $(CURDIR)/protocols/lib

SRCFS = $(notdir $(SRCS))
OBJFS = $(SRCFS:%.c=%.o)
DEPFS = $(SRCFS:%.c=%.d)

.PHONY: clean tag all 
.PRECIOUS: $(OBJS) $(DEPS)

all: 
	+make $(BINS) 
	+make $(LIB)
	+make -C ./protocols
	+make -C ./sample

$(BINDIR)/%: $(OBJDIR)/%.o
	if [ ! -d "$(BINDIR)" ]; then mkdir $(BINDIR); fi
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIRS)/%.c
	if [ ! -d "$(OBJDIR)" ]; then mkdir $(OBJDIR); fi
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDE)

$(LIBDIR)/%.so: $(OBJDIR)/%.o
	if [ ! -d $(LIBDIR) ]; then mkdir $(LIBDIR); fi
	$(CC) -shared -Wl,-soname,$(LIBDIR)/$(notdir $@),-rpath,$(PLIBDIR) -o $@ $^


tag: tags cscope.out
	ctags -R
	cscope -Rb

clean:
	$(RM) $(OBJS) $(DEPS) $(BINS) $(LIB)
	-$(RMDIR) $(OBJDIR) $(BINDIR) $(LIBDIR)
	make -C ./protocols clean
	make -C ./sample clean

-include $(DEPS)
