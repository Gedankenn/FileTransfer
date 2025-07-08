#!/bin/bash

# Output file
CLANGD_FLAGS_FILE="compile_flags.txt"

# Base flags
FLAGS="-Wall -Wextra -std=c99"

# Find include directories
INCLUDE_DIRS=$(find modules include -type d)

# Start the compile_flags.txt file
echo "$FLAGS" > $CLANGD_FLAGS_FILE

# Add each -I include path
for dir in $INCLUDE_DIRS; do
  echo "-I$dir" >> $CLANGD_FLAGS_FILE
done

echo "Generated $CLANGD_FLAGS_FILE with:"
cat $CLANGD_FLAGS_FILE

