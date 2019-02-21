CC := gcc
CFLAGS := -Wall -Wextra -ggdb -g3 -MMD -MP
CFLAGS += -DDEBUG
LDFLAGS =

BASES := $(filter-out include Makefile lib,$(wildcard *))

LINKDIR := $(CURDIR)/lib

OBJS := $(join $(addsuffix /,$(BASES)),$(BASES:%=%.o))
DEPS := $(OBJS:%.o=%.d)
SRCS := $(OBJS:%.o=%.c)
LIBS := $(OBJS:%.o=%.so)
LINKS := $(addprefix $(LINKDIR)/lib,$(notdir $(OBJS:%.o=%.so)))
LINKNAME := $(notdir $(LINKS))

RM = rm -rf
LN = ln -sf

.PHONY: clean tags

all: $(LINKS)

$(LINKS): $(LIBS)
ifneq "$(wildcard $(LINKDIR))" "$(LINKDIR)"
	@-mkdir $(LINKDIR)
endif
	$(LN) ../$< $@

$(LIBS): %.so: %.o
	$(CC) -shared -Wl,-soname,$(LINKDIR)/lib$(notdir $@) -o $@ $^

$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) -fPIC -o $@ -c $<

tags: tags cscope.out
	ctags -R
	cscope -Rb

clean:
	$(RM) $(OBJS) $(DEPS) $(LINKS) $(LIBS)

-include $(DEPS)
