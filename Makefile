# ==============================================================================
# Directories
# ==============================================================================
SRC_DIR     := src
INCLUDE_DIR := include
OBJ_DIR     := obj
BIN_DIR     := bin
TESTS_DIR   := tets
# ==============================================================================
# Compiler and Flags
# ==============================================================================
CC       := gcc
CFLAGS   := -Wall -Wextra -Wpedantic -std=c11 -O2 -I$(INCLUDE_DIR)
ifdef DEBUG
  CFLAGS += -DDEBUG
endif
LDFLAGS  := 
# Automatic dependency tracking flags (without matching target bugs)
DEPFLAGS  = -MT $@ -MMD -MP -MF $(OBJ_DIR)/$*.d


# ==============================================================================
# Files
# ==============================================================================
SOURCES := $(wildcard $(SRC_DIR)/*.c)
# Convert src/file.c into obj/file.o
OBJECTS := $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS    := $(OBJECTS:.o=.d)

TARGET  := $(BIN_DIR)/rv32i-as

# ==============================================================================
# Build Rules
# ==============================================================================
.PHONY: all clean test directories

all: directories $(TARGET)

# Link the final executable into the bin directory
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

# Rule to compile handwritten C source files and generate dependencies
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | directories
	$(CC) $(DEPFLAGS) $(CFLAGS) -c -o $@ $<

# Create directories dynamically if they do not exist
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Clean up all build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# ==============================================================================
# Tests
# ==============================================================================
# Look for test scripts inside your tets/ directory
TEST_FILES := $(wildcard $(TESTS_DIR)/*.al)

test: all
	@echo "Running tests..."
	@for file in $(TEST_FILES); do \
		echo "Testing $$file..."; \
		./$(TARGET) $$file; \
	done

# Include the generated dependency files safely
-include $(wildcard $(OBJ_DIR)/*.d)