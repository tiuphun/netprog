# Define the compiler
CC = gcc

# Define the compiler flags
CFLAGS = -Wall -g

# Define the target executable
TARGET = resolver-modified

# Define the source file
SRC = resolver-modified.c

# Default target when running 'make'
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Clean up generated files
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all clean
