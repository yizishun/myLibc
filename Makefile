MALLOC_HOME = $(shell pwd)
BUILD = $(MALLOC_HOME)/builds
SHARE = $(BUILD)/share

CSRCS = $(shell find $(MALLOC_HOME)/src -name "*.c")
INC_PATH = $(MALLOC_HOME)/include
INCFLAGS = $(addprefix -I, $(INC_PATH))
CFLAGS += -g -O2 -Wall -Werror $(INCFLAGS) -fno-builtin

run:
	@mkdir -p $(BUILD)
	gcc $(CSRCS) $(CFLAGS) -o $(MALLOC_HOME)/builds/tb
	$(MALLOC_HOME)/builds/tb

clean:
	rm -rf $(BUILD)