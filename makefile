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
	@./output/app -t $(CURDIR)

testfile:
	@mkdir -p $(OUTPUT_DIR)
	$(CC) $(BASE_CFLAGS) $(HDR_INCLUDE_FLAGS) -g -O0 -DDEBUG $(LIB_SRC_FILES) $(APPLICATION_SRC) -o $(APPLICATION_BIN) $(LDLIBS)
	@./output/app -f $(CURDIR)

help:
	@echo "Available make targets:"
	@echo "  all       - Build release (default)"
	@echo "  debug     - Build with debug flags"
	@echo "  release   - Build optimized release"
	@echo "  example   - Build example binary"
	@echo "  clean     - Remove build output"
	@echo "  testapp   - Build + run in test mode (directory transfer)"
	@echo "  testfile  - Build + run in debug mode (single file transfer)"
	@echo "  help      - Show this help message"
