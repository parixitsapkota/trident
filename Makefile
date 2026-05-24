CMAKE = cmake
BUILD_DIR = build/
BUILD_DIR_DEBUG = $(BUILD_DIR)debug
BUILD_DIR_RELEASE = $(BUILD_DIR)release

.PHONY: all debug release clean set_up

all: debug

# Create Debug build
debug: set_up
	@cd $(BUILD_DIR_DEBUG) && $(CMAKE) -DCMAKE_BUILD_TYPE=Debug ..
	@cd $(BUILD_DIR_DEBUG) && $(MAKE)

# Create Release build
release: set_up
	@cd $(BUILD_DIR_RELEASE) && $(CMAKE) -DCMAKE_BUILD_TYPE=Release ..
	@cd $(BUILD_DIR_RELEASE) && $(MAKE)

dirs:


# Clean all builds
clean:
	rm -rf $(BUILD_DIR_DEBUG) $(BUILD_DIR_RELEASE)

# Format src files and Setup build dirs
set_up:
	@mkdir -p $(BUILD_DIR_RELEASE)
	@mkdir -p $(BUILD_DIR_DEBUG)
	clang-format -i $(wildcard src/*.c) $(wildcard src/*/*.c) $(wildcard src/*.h) $(wildcard src/*/*.h)