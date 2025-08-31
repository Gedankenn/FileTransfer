# Compiler and flags
CC := gcc
BASE_CFLAGS := -Wall -Wextra
DEPFLAGS := 
LDLIBS := -lcurl

# Output dir
OUTPUT_DIR := output

# Header files and include flags
LIB_HDR_FILES := $(wildcard modules/*.h modules/mysocket/*.h application/*.h)
LIB_HDR_DIRS := $(sort $(dir $(LIB_HDR_FILES)))
HDR_INCLUDE_FLAGS := $(addprefix -I,$(LIB_HDR_DIRS))

# Sources
LIB_SRC_FILES := $(wildcard modules/mysocket/*.c application/*.c modules/*.c)
APPLICATION_SRC := main.c
EXAMPLE_SRC := examples/example.c

APPLICATION_BIN := $(OUTPUT_DIR)/app
EXAMPLE_BIN := $(OUTPUT_DIR)/example

.PHONY: all debug release example clean

all: release

debug:
	@mkdir -p $(OUTPUT_DIR)
	$(CC) $(BASE_CFLAGS) $(HDR_INCLUDE_FLAGS) -g -O0 -DDEBUG $(LIB_SRC_FILES) $(APPLICATION_SRC) -o $(APPLICATION_BIN) $(LDLIBS)

release:
	@mkdir -p $(OUTPUT_DIR)
	$(CC) $(BASE_CFLAGS) $(HDR_INCLUDE_FLAGS) -O2 $(LIB_SRC_FILES) $(APPLICATION_SRC) -o $(APPLICATION_BIN) $(LDLIBS)

example:
	@mkdir -p $(OUTPUT_DIR)
	$(CC) $(BASE_CFLAGS) $(HDR_INCLUDE_FLAGS) $(LIB_SRC_FILES) $(EXAMPLE_SRC) -o $(EXAMPLE_BIN) $(LDLIBS)

clean:
	rm -rf $(OUTPUT_DIR)

testapp:
	@mkdir -p $(OUTPUT_DIR)
	$(CC) $(BASE_CFLAGS) $(HDR_INCLUDE_FLAGS) -O2 $(LIB_SRC_FILES) $(APPLICATION_SRC) -o $(APPLICATION_BIN) $(LDLIBS)
	@./output/app -t /home/sabo/github/file_transfer_c/ 

testfile:
	@mkdir -p $(OUTPUT_DIR)
	$(CC) $(BASE_CFLAGS) $(HDR_INCLUDE_FLAGS) -g -O0 -DDEBUG $(LIB_SRC_FILES) $(APPLICATION_SRC) -o $(APPLICATION_BIN) $(LDLIBS)
	@./output/app -f /home/sabo/github/file_transfer_c/
