# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g -pthread

# Executable name
TARGET = main

# Source files
SRCS = main.c functions.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CC) 	$(CFLAGS) -o $@ $^

# Compile source files into object files
%.o: %.c
	$(CC)	$(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f	$(OBJS)	$(TARGET)

# Phony targets
.PHONY: all clean