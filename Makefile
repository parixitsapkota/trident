# --- Configuration ---
PROJECT := trident
CC      := clang

DEBUG   := -fsanitize=address -g -O0
RELEASE := -O3
CFLAGS  := -Wall -Wextra

MODE    ?= debug
BUILD   := build

ifeq ($(MODE),release)
    CFLAGS += $(RELEASE)
    BUILD  := build/release
else
    CFLAGS += $(DEBUG)
    BUILD  := build/debug
endif

C_SOURCES := $(wildcard src/*.c) $(wildcard src/*/*.c)
H_HEADERS := $(wildcard src/*.h) $(wildcard src/*/*.h)
SRCFILES  := $(C_SOURCES) $(H_HEADERS)

OBJECTS   := $(patsubst src/%.c, $(BUILD)/%.o, $(C_SOURCES))

.PHONY: all clean format

all: format $(PROJECT)

$(PROJECT): $(OBJECTS)
	@echo "Linking $(PROJECT) ($(MODE) mode)..."
	@$(CC) $(CFLAGS) $(OBJECTS) -o $(PROJECT)

$(BUILD)/%.o: src/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling $< -> $@"
	@$(CC) $(CFLAGS) -c $< -o $@

# Clean build
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf build $(PROJECT)

# Format c files
format:
	@echo "Formatting source files..."
	@clang-format -i $(SRCFILES)
