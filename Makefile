CC = gcc
CFLAGS = -Wall -Wextra -ggdb -g3 -MMD -MP
CFLAGS += -DDEBUG
LDFLAGS =

TARGETS = exlay_daemon exlay 
SRCDIRS = ./src
OBJDIR = ./build
BINDIR = ./bin
INCDIRS = ./src/include

RM = rm -rf
RMDIR = rmdir

SRCS = $(wildcard $(SRCDIRS)/*.c)
OBJS = $(addprefix $(OBJDIR)/,$(notdir $(SRCS:%.c=%.o)))
DEPS = $(OBJS:%.o=%.d)
BINS = $(addprefix $(BINDIR)/,$(TARGETS))
INCLUDE = $(addprefix -I,$(INCDIRS))

SRCFS = $(notdir $(SRCS))
OBJFS = $(SRCFS:%.c=%.o)
DEPFS = $(SRCFS:%.c=%.d)

.PHONY: clean tag all 
.PRECIOUS: $(OBJS) $(DEPS)

all: 
	make $(BINS) 
	make -C ./protocols

$(BINDIR)/%: $(OBJDIR)/%.o
	if [ ! -d "$(BINDIR)" ]; then mkdir $(BINDIR); fi
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIRS)/%.c
	if [ ! -d "$(OBJDIR)" ]; then mkdir $(OBJDIR); fi
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDE)

tag: tags cscope.out
	ctags -R
	cscope -Rb

clean:
	$(RM) $(OBJS) $(DEPS) $(BINS)
	-$(RMDIR) $(OBJDIR) $(BINDIR)
	make -C ./protocols clean

-include $(DEPS)
