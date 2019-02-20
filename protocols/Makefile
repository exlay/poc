CC := gcc
CFLAGS := -Wall -Wextra -ggdb -g3 -MMD -MP
CFLAGS += -DDEBUG
LDFLAGS =

BASES := $(filter-out include Makefile lib,$(wildcard *))

LINKDIR := $(CURDIR)/lib
SRCDIRS := $(BASES)
OBJDIRS := $(BASES)
LIBDIRS := $(BASES)

DEPS := $(BASES:%=%.d)
OBJS := $(BASES:%=%.o)
SRCS := $(BASES:%=%.c)
LIBS := $(BASES:%=%.so)
LINKS := $(addprefix $(LINKDIR),$(BASES:%=lib%.so))

RM = rm -rf

all: $(LINKS)
ifneq "$(LINKDIR)" "$(LINKDIR)"
	@-mkdir $(LINKDIR)
endif

$(LIBDIRS)/%.so: $(OBJDIRS)/%.o
	$(CC) -shared -Wl,-soname,$@ -o $@ $^

$(OBJDIRS)/%.o: $(SRCDIRS)/%.c
	$(CC) $(CFLAGS) -fPIC -o $@ -c $<

tag: tags cscope.out
	ctags -R
	cscope -Rb

print:
	@echo LIBS = $(LIBS)
	@echo LINKS = $(LINKS)

clean:
	$(RM) $(OBJS) $(DEPS) $(LINKS) $(LIBS)

-include $(DEPS)
