APP := jedi
CC := gcc
LD := gcc
CPPFLAGS := -Iinclude -MD
CFLAGS := -Wall -O2
LDLIBS := -lrt
OBJS :=			\
	src/main.o	\
	src/menu.o	\
	src/test.o	\
	src/tools.o	\
	src/timer2.o

# Be silent per default, but 'make V=1' will show all compiler calls
ifneq ($(V),1)
  Q = @
else
  Q =
endif

BUILD ?= debug
ifeq ($(BUILD),release)
  CPPFLAGS += -DNDEBUG
  CFLAGS += -s
else ifeq ($(BUILD),debug)
  CFLAGS += -g
else
  $(error Incorrect BUILD variable)
endif

all: $(APP)

$(APP): $(OBJS)
	@printf "  LD      $@\n"
	$(Q)$(LD) $(LDFLAGS) $(OBJS) -o $(APP) $(LDLIBS)

%.o: %.c
	@printf "  CC      $(*).c\n"
	$(Q)$(CC) $(CPPFLAGS) $(CFLAGS) $< -c -o $@

clean:
	@printf "  CLEAN\n"
	$(Q)-rm -f $(APP)
	$(Q)-rm -f $(OBJS)
	$(Q)find src/ -name '*.d' -exec rm -f {} \;
	$(Q)-rm -f cscope.*
	$(Q)-rm -f tags

index:
	$(Q)find . -name '*.[ch]' > cscope.files
	$(Q)cscope -b -q
	$(Q)ctags -L cscope.files

.PHONY: all clean index

-include $(OBJS:.o=.d)
