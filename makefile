# Compiler and flags
CC := gcc
BASE_CFLAGS := -Wall -Wextra
DEPFLAGS := -MMD -MP
LDLIBS := -lcurl

# Build mode: release (default) or debug (set by phony targets via recursive make)
BUILD ?= release

# Dirs
OUTPUT_DIR := output
BUILD_DIR  := .build/$(BUILD)
TEST_DIR   := modules/test

# Header files and include flags (auto-discover)
LIB_HDR_FILES := $(wildcard modules/*.h modules/mysocket/*.h application/*.h)
LIB_HDR_DIRS  := $(sort $(dir $(LIB_HDR_FILES)))
HDR_INCLUDE_FLAGS := $(addprefix -I,$(LIB_HDR_DIRS))

# Sources
LIB_SRC_FILES   := $(wildcard modules/mysocket/*.c application/*.c modules/*.c)
APPLICATION_SRC := main.c
EXAMPLE_SRC     := examples/example.c
TEST_SRC        := $(TEST_DIR)/test.c

# Objects (mirror source tree under BUILD_DIR)
LIB_OBJS     := $(LIB_SRC_FILES:%.c=$(BUILD_DIR)/%.o)
APP_OBJ      := $(BUILD_DIR)/$(APPLICATION_SRC:.c=.o)
EXAMPLE_OBJ  := $(BUILD_DIR)/$(EXAMPLE_SRC:.c=.o)
TEST_OBJ     := $(BUILD_DIR)/$(TEST_SRC:.c=.o)

# Binaries
APPLICATION_BIN := $(OUTPUT_DIR)/app
EXAMPLE_BIN     := $(OUTPUT_DIR)/example
TEST_BIN        := $(TEST_DIR)/test

# CFLAGS per build type
CFLAGS := $(BASE_CFLAGS)
ifeq ($(BUILD),debug)
  CFLAGS += -g -O0 -DDEBUG
else
  CFLAGS += -O2
endif

.PHONY: all debug release example clean help test runtest testapp testfile app

# Default = release
all: release

# High-level targets dispatch to the correct BUILD
debug:
	@$(MAKE) BUILD=debug app

release:
	@$(MAKE) BUILD=release app

example:
	@$(MAKE) BUILD=$(BUILD) $(EXAMPLE_BIN)

test: $(TEST_BIN)

app: $(APPLICATION_BIN)

# Link rules
$(APPLICATION_BIN): $(LIB_OBJS) $(APP_OBJ) | $(OUTPUT_DIR)
	$(CC) $(CFLAGS) $(LIB_OBJS) $(APP_OBJ) -o $@ $(LDLIBS)

$(EXAMPLE_BIN): $(LIB_OBJS) $(EXAMPLE_OBJ) | $(OUTPUT_DIR)
	$(CC) $(CFLAGS) $(LIB_OBJS) $(EXAMPLE_OBJ) -o $@ $(LDLIBS)

$(TEST_BIN): $(LIB_OBJS) $(TEST_OBJ) | $(TEST_DIR)
	$(CC) $(CFLAGS) $(LIB_OBJS) $(TEST_OBJ) -o $@ $(LDLIBS)

# Compile C -> object, generating .d dependency files
# Creates directories as needed (both for BUILD_DIR and mirrored subdirs)
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(DEPFLAGS) $(HDR_INCLUDE_FLAGS) -c $< -o $@

# Ensure dirs exist before linking
$(OUTPUT_DIR):
	@mkdir -p $(OUTPUT_DIR)

$(TEST_DIR):
	@mkdir -p $(TEST_DIR)

# Convenience: build + run
testapp:
	@$(MAKE) BUILD=release app
	@./$(APPLICATION_BIN) -t $(CURDIR)

testfile:
	@$(MAKE) BUILD=debug app
	@./$(APPLICATION_BIN) -f $(CURDIR)

runtest: test
	@./$(TEST_BIN)

# Cleanup
clean:
	rm -rf $(OUTPUT_DIR) .build

help:
	@echo "Available make targets:"
	@echo "  all        - Build release (default)"
	@echo "  debug      - Build with debug flags (-g -O0 -DDEBUG)"
	@echo "  release    - Build optimized release (-O2)"
	@echo "  example    - Build example binary"
	@echo "  app        - Build application binary"
	@echo "  test       - Build test binary ($(TEST_BIN))"
	@echo "  runtest    - Build test binary and run it"
	@echo "  testapp    - Build + run app in test mode (directory transfer)"
	@echo "  testfile   - Build + run app in debug mode (single file transfer)"
	@echo "  clean      - Remove build output"
	@echo "  help       - Show this help message"

# Include auto-generated dependency files (safe even if they don't exist yet)
-include $(LIB_OBJS:.o=.d) $(APP_OBJ:.o=.d) $(EXAMPLE_OBJ:.o=.d) $(TEST_OBJ:.o=.d)

