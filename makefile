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

# Header files and include flags (auto-discover)
LIB_HDR_FILES := $(wildcard modules/*.h modules/mysocket/*.h application/*.h)
LIB_HDR_DIRS  := $(sort $(dir $(LIB_HDR_FILES)))
HDR_INCLUDE_FLAGS := $(addprefix -I,$(LIB_HDR_DIRS))

# Sources
LIB_SRC_FILES   := $(wildcard modules/mysocket/*.c application/*.c modules/*.c)
APPLICATION_SRC := main.c
EXAMPLE_SRC     := examples/example.c

# Objects (mirror source tree under BUILD_DIR)
LIB_OBJS     := $(LIB_SRC_FILES:%.c=$(BUILD_DIR)/%.o)
APP_OBJ      := $(BUILD_DIR)/$(APPLICATION_SRC:.c=.o)
EXAMPLE_OBJ  := $(BUILD_DIR)/$(EXAMPLE_SRC:.c=.o)

# Binaries
APPLICATION_BIN := $(OUTPUT_DIR)/app
EXAMPLE_BIN     := $(OUTPUT_DIR)/example

# CFLAGS per build type
CFLAGS := $(BASE_CFLAGS)
ifeq ($(BUILD),debug)
  CFLAGS += -g -O0 -DDEBUG
else
  CFLAGS += -O2
endif

.PHONY: all debug release example clean help testapp testfile app

# Default = release
all: release

# High-level targets dispatch to the correct BUILD
debug:
	@$(MAKE) BUILD=debug app

release:
	@$(MAKE) BUILD=release app

example:
	@$(MAKE) BUILD=$(BUILD) $(EXAMPLE_BIN)

app: $(APPLICATION_BIN)

# Link rules
$(APPLICATION_BIN): $(LIB_OBJS) $(APP_OBJ) | $(OUTPUT_DIR)
	$(CC) $(CFLAGS) $(LIB_OBJS) $(APP_OBJ) -o $@ $(LDLIBS)

$(EXAMPLE_BIN): $(LIB_OBJS) $(EXAMPLE_OBJ) | $(OUTPUT_DIR)
	$(CC) $(CFLAGS) $(LIB_OBJS) $(EXAMPLE_OBJ) -o $@ $(LDLIBS)

# Compile C -> object, generating .d dependency files
# Creates directories as needed (both for BUILD_DIR and mirrored subdirs)
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(DEPFLAGS) $(HDR_INCLUDE_FLAGS) -c $< -o $@

# Ensure output dir exists before linking
$(OUTPUT_DIR):
	@mkdir -p $(OUTPUT_DIR)

# Convenience: build + run
testapp:
	@$(MAKE) BUILD=release app
	@./$(APPLICATION_BIN) -t $(CURDIR)

testfile:
	@$(MAKE) BUILD=debug app
	@./$(APPLICATION_BIN) -f $(CURDIR)

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
	@echo "  testapp    - Build + run in test mode (directory transfer)"
	@echo "  testfile   - Build + run in debug mode (single file transfer)"
	@echo "  clean      - Remove build output"
	@echo "  help       - Show this help message"

# Include auto-generated dependency files (safe even if they don't exist yet)
-include $(LIB_OBJS:.o=.d) $(APP_OBJ:.o=.d) $(EXAMPLE_OBJ:.o=.d)
