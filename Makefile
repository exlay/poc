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

all: $(BINS)

$(BINDIR)/%: $(OBJDIR)/%.o
ifneq "$(wildcard $(BINDIR))" "$(BINDIR)"
	@-mkdir $(BINDIR)
endif
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIRS)/%.c
ifneq "$(wildcard $(OBJDIR))" "$(OBJDIR)"
	@-mkdir $(OBJDIR)
endif
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDE)

tag: tags cscope.out
	ctags -R
	cscope -Rb

clean:
	$(RM) $(OBJS) $(DEPS) $(BINS)

-include $(DEPS)
