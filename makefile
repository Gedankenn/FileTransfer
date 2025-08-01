# Compiler and base flags
CC = gcc
BASE_CFLAGS = -Wall -Wextra -I.
LIBS = -lcurl

# Header files
LIB_HDR_FILES = $(wildcard modules/*.h modules/mysocket/*.h application/*.h)

# Extrai diretórios únicos dos headers
LIB_HDR_DIRS := $(sort $(dir $(LIB_HDR_FILES)))

# Monta flags -I para os diretórios de headers
HDR_INCLUDE_FLAGS := $(addprefix -I,$(LIB_HDR_DIRS))

# CFLAGS finais juntando flags base + includes
CFLAGS = $(BASE_CFLAGS) $(HDR_INCLUDE_FLAGS)

# Source files
LIB_SRC_FILES = $(wildcard modules/mysocket/*.c application/*.c modules/*.c)

# Example build
EXAMPLE_SRC = examples/example.c
EXAMPLE_BIN = example

# Application build
APPLICATION_SRC = application/main.c
APPLICATION_BIN = app

# Default target
all: $(APPLICATION_BIN)

# Build application binary
$(APPLICATION_BIN): $(APPLICATION_SRC) $(LIB_SRC_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Build example binary
example: $(EXAMPLE_BIN)

$(EXAMPLE_BIN): $(EXAMPLE_SRC) $(LIB_SRC_FILES) $(LIB_HDR_FILES)
	$(CC) $(CFLAGS) -o $@ $^

# Clean up
clean:
	rm -f $(EXAMPLE_BIN) $(APPLICATION_BIN)


# Clangd
clangd:
	./gen_clangd.sh

