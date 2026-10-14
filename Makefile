# --- Colors ---
COLOR_RESET   := \033[0m
COLOR_RED     := \033[1;31m
COLOR_GREEN   := \033[1;32m
COLOR_YELLOW  := \033[1;33m
COLOR_BLUE    := \033[1;34m
COLOR_MAGENTA := \033[1;35m
export COLOR_RESET COLOR_RED COLOR_GREEN COLOR_YELLOW COLOR_BLUE COLOR_MAGENTA

# --- paths ---
ROOT_DIR  := $(CURDIR)
PREFIX    := /usr/local
MANPREFIX := ${PREFIX}/share/man
export ROOT_DIR

# --- Configuration ---
PROJECT := trident
CC = clang
MAKEFLAGS += --no-print-directory

DEBUG   := -fsanitize=address -g -O0
RELEASE := -O3
CFLAGS  := -Wall -Wextra -Werror
LDFLAGS :=
export PROJECT CC DEBUG RELEASE CFLAGS LDFLAGS

# --- build mode ---
MODE ?= debug
BUILD_PATH ?=

ifeq ($(MODE),release)
CFLAGS += $(RELEASE)
BUILD_PATH := build/release
else
CFLAGS += $(DEBUG)
BUILD_PATH := build/debug
endif

BUILD := $(ROOT_DIR)/$(BUILD_PATH)
export MODE BUILD

# --- Information ---
GIT_TAG := $(shell git describe --tags --abbrev=0 2>/dev/null || echo "0.0.1")
GIT_HASH := $(shell git rev-parse --short HEAD 2>/dev/null || echo "unknown")
VERSION := $(GIT_TAG) $(MODE)-$(shell date "+%d%m%Y")-$(GIT_HASH)
TIME_INFO := $(shell date "+%Y/%m/%d %H:%M:%S:%p")
COMPILER_INFO := $(shell $(CC) --version | head -n 1 | cut -d' ' -f1-3)

CFLAGS += -DVERSION_INFO="\"$(VERSION)\"" \
          -DTIME_INFO="\"$(TIME_INFO)\"" \
          -DCC_INFO="\"$(COMPILER_INFO)\""

# --- Platform ---
PLATFORM ?= linux
OUTPUT ?= $(PROJECT)

ifeq ($(PLATFORM),linux)
CFLAGS += -D_TUX
else ifeq ($(PLATFORM),windows)
CFLAGS += -D_WIN32
OUTPUT := $(PROJECT).exe
else ifeq ($(PLATFORM),macos)
CFLAGS += -D_XOS
else ifeq ($(PLATFORM),freebsd)
CFLAGS += -D_BSD
endif

export OUTPUT

all: compdb
	@$(MAKE) -C src

compdb:
	@$(MAKE) -C src compdb

clean:
	@printf '$(COLOR_BLUE)[-] Cleaning build artifacts...$(COLOR_RESET)\n'
	@rm -rf build/ $(PROJECT)

install: MODE := release
install: all
	@echo "Installing $(OUTPUT)..."
	@mkdir -p $(PREFIX)/bin
	@cp -f $(OUTPUT) $(PREFIX)/bin
	@chmod 755 $(PREFIX)/bin/$(OUTPUT)
	@echo "Installed $(OUTPUT) to $(PREFIX)/.."

uninstall:
	@rm -f $(PREFIX)/bin/$(OUTPUT)
	@echo "Uninstalled $(OUTPUT) from $(PREFIX)/.."

.PHONY: all compdb test clean CLEAN install uninstall
