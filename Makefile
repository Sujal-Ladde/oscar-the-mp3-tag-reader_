# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -g

# Include directory
INC_DIR = include

# Source directory
SRC_DIR = src

# Main source directory
MAIN_DIR = main

# Executable name
EXECUTABLE = a.out

# Output directory for object files
BIN_DIR = bin

# Create the bin directory if it doesn't exist
MKDIR_BIN = mkdir -p $(BIN_DIR)

# Source files
COMMON_SRC = $(SRC_DIR)/common.c
EDIT_SRC = $(SRC_DIR)/edit.c
VIEW_SRC = $(SRC_DIR)/view.c
MAIN_SRC = $(MAIN_DIR)/main.c

# Object files in the bin directory
COMMON_OBJ = $(BIN_DIR)/common.o
EDIT_OBJ = $(BIN_DIR)/edit.o
VIEW_OBJ = $(BIN_DIR)/view.o
MAIN_OBJ = $(BIN_DIR)/main.o

# Default target: compile and link
all: $(BIN_DIR) $(EXECUTABLE)

$(BIN_DIR):
	$(MKDIR_BIN)

# Compile object files and place them in the bin directory
$(BIN_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@

$(COMMON_OBJ): $(COMMON_SRC) $(INC_DIR)/common.h
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@

$(EDIT_OBJ): $(EDIT_SRC) $(INC_DIR)/edit.h $(INC_DIR)/common.h
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@

$(VIEW_OBJ): $(VIEW_SRC) $(INC_DIR)/view.h $(INC_DIR)/common.h
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@

$(MAIN_OBJ): $(MAIN_SRC) $(INC_DIR)/edit.h $(INC_DIR)/view.h $(INC_DIR)/common.h
	$(CC) $(CFLAGS) -I $(INC_DIR) -c $< -o $@

# Link object files from the bin directory to create the executable in the current directory
$(EXECUTABLE): $(COMMON_OBJ) $(EDIT_OBJ) $(VIEW_OBJ) $(MAIN_OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Clean target: remove object files from the bin directory and the executable
clean:
	rm -rf $(BIN_DIR) $(EXECUTABLE)

# Prepare input/output directories
# DATA_DIR = data
# ENCODE_INP_DIR = $(DATA_DIR)/encode_input
# ENCODE_OP_DIR = $(DATA_DIR)/encode_output
# DECODE_INP_DIR = $(DATA_DIR)/decode_input
# DECODE_OP_DIR = $(DATA_DIR)/decode_output

# prepare-dirs: $(BIN_DIR)
# 	mkdir -p $(ENCODE_INP_DIR) $(ENCODE_OP_DIR) $(DECODE_INP_DIR) $(DECODE_OP_DIR)

.PHONY: all clean prepare-dirs