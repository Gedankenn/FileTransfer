# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -I.  # -I. to include headers from the current dir

# Source files
LIB_SRC = \
	modules/mysocket/*.c \
	application/*.c

# Use wildcard to expand all .c files
LIB_SRC_FILES = $(wildcard $(LIB_SRC))

EXAMPLE_SRC = examples/example.c
EXAMPLE_BIN = example

# Default target
all: $(EXAMPLE_BIN)

# Build the example binary
$(EXAMPLE_BIN): $(EXAMPLE_SRC) $(LIB_SRC_FILES)
	$(CC) $(CFLAGS) -o $@ $^

# Clean up
clean:
	rm -f $(EXAMPLE_BIN)

# Clangd 
clangd:
	./gen_clangd.sh
