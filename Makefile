# Compiler
CC = gcc

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = tests
INC_DIR = include

# Automatically find all subdirectories in src to add them to the include path
# This allows you to #include "bitarr.h" anywhere without needing the full "bit_arr/bitarr.h" path
SRC_SUBDIRS := $(shell find $(SRC_DIR) -type d)
INC_FLAGS := -I$(INC_DIR) $(addprefix -I,$(SRC_SUBDIRS))

BASE_CFLAGS = -std=gnu2x -Wall -Wextra -pedantic $(INC_FLAGS)

# Recursive source discovery
SRCS := $(shell find $(SRC_DIR) -name '*.c')

# Mirror source tree inside build/
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Main executable
TARGET = $(BIN_DIR)/c-litaire

# Build configurations
all: CFLAGS = $(BASE_CFLAGS) -O3 -flto -march=native -DNDEBUG
gdb valgrind test: CFLAGS = $(BASE_CFLAGS) -g -O0
coverage: CFLAGS = $(BASE_CFLAGS) -g -O0 --coverage

# Test objects (exclude main)
TEST_OBJS := $(filter-out $(BUILD_DIR)/main.o,$(OBJS))

TEST_CARD_BIN = $(BIN_DIR)/test_card
TEST_DSL_BIN = $(BIN_DIR)/test_dsl
TEST_REG_BIN = $(BIN_DIR)/test_registry
TEST_GAME_BIN = $(BIN_DIR)/test_dsl_game

.PHONY: all clean run test gdb valgrind coverage

all: $(TARGET)

# Link executable
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@
	@echo "Build successful!"

# Compile source files recursively
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# ---------------- TESTS ----------------

test: $(TEST_CARD_BIN) $(TEST_DSL_BIN) $(TEST_REG_BIN) $(TEST_GAME_BIN)
	@echo "\n--- Running Card Tests ---"
	@./$(TEST_CARD_BIN)
	@echo "\n--- Running DSL Tests ---"
	@./$(TEST_DSL_BIN)
	@echo "\n--- Running Registry Tests ---"
	@./$(TEST_REG_BIN)
	@echo "\n--- Running DSL Game Tests ---"
	@./$(TEST_GAME_BIN)

$(TEST_CARD_BIN): $(TEST_OBJS) $(TEST_DIR)/test_card.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit

$(TEST_DSL_BIN): $(TEST_OBJS) $(TEST_DIR)/test_dsl.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit

$(TEST_REG_BIN): $(TEST_OBJS) $(TEST_DIR)/test_registry.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit

$(TEST_GAME_BIN): $(TEST_OBJS) $(TEST_DIR)/test_dsl_game.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit

# ---------------- UTILITIES ----------------

gdb: all
	gdb ./$(TARGET)

valgrind: all
	valgrind --leak-check=full \
	         --show-leak-kinds=all \
	         --track-origins=yes \
	         ./$(TARGET)

coverage: clean
	@$(MAKE) test CFLAGS="$(BASE_CFLAGS) -g -O0 --coverage"

run: all
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	find . -name "*.gcda" -delete
	find . -name "*.gcno" -delete
	find . -name "*.gcov" -delete
