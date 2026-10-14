SRC_DIR := $(ROOT_DIR)/src
TARGET  := $(ROOT_DIR)/$(PROJECT)

SRCS := $(sort $(shell find $(SRC_DIR) -name '*.c'))
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)
JSON := $(OBJS:.o=.json)

ALL_CFLAGS := $(CFLAGS) -std=c99 -I$(SRC_DIR) -MMD -MP

KEYWORDS := syntax/lexer/keywords
INTRINSICS := syntax/lexer/intrinsics

all:  format $(KEYWORDS).h $(TARGET)

$(OBJS): | $(KEYWORDS).h $(INTRINSICS).h

$(KEYWORDS).h: $(KEYWORDS).def
	@printf "$(COLOR_MAGENTA)[gperf] %s$(COLOR_RESET)\n" $(notdir $@)
	@gperf -N get_keyword_kind -t $< > $@

$(INTRINSICS).h: $(INTRINSICS).def
	@printf "$(COLOR_MAGENTA)[gperf] %s$(COLOR_RESET)\n" $(notdir $@)
	@gperf -N get_intrinsic_kind -t $< > $@

$(TARGET): $(OBJS)
	@printf '$(COLOR_GREEN)[+] Linking %s ($(MODE))$(COLOR_RESET)\n' $(notdir $@)
	@$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(BUILD)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@printf '$(COLOR_YELLOW)[CC] %s$(COLOR_RESET)\n' $(patsubst $(SRC_DIR)/%,%,$<)
	@$(CC) $(ALL_CFLAGS) -MJ $(@:.o=.json) -c $< -o $@

compdb: $(OBJS)
	@{ echo '['; cat $(JSON) | sed '$$ s/,$$//'; echo ']'; } > $(ROOT_DIR)/compile_commands.json
	@printf '$(COLOR_MAGENTA)[=] compile_commands.json ($(words $(SRCS)) files)$(COLOR_RESET)\n'

format:
	@echo -e "$(COLOR_BLUE)[clang-format] Formatting source files...$(COLOR_RESET)"
	@clang-format -i $(SRCS)

%.c:
	@:

-include $(DEPS)

$(DEPS): ;

.PHONY: all compdb
