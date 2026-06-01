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

TEST_BITARR_BIN = $(BIN_DIR)/test_bitarr
TEST_CARDNEGINE_BIN = $(BIN_DIR)/test_card_engine
TEST_PACIENCINE_BIN = $(BIN_DIR)/test_paciencia_interpreter
TEST_IO_BIN = $(BIN_DIR)/test_io
TEST_RUN_BIN = $(BIN_DIR)/test_run
TEST_RUNNER_BIN = $(BIN_DIR)/test_runner
TEST_SAVE_BIN = $(BIN_DIR)/test_save

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

test: $(TEST_BITARR_BIN) $(TEST_CARDNEGINE_BIN) $(TEST_PACIENCINE_BIN) $(TEST_IO_BIN) $(TEST_RUN_BIN) $(TEST_RUNNER_BIN) $(TEST_SAVE_BIN)
	@echo "\n--- Running Bit_Arr Tests ---"
	@./$(TEST_BITARR_BIN)
	@echo "\n--- Running Card Engine Tests ---"
	@./$(TEST_CARDNEGINE_BIN)
	@echo "\n--- Running Paciencia Tests ---"
	@./$(TEST_PACIENCINE_BIN)
	@echo "\n--- Running IO Tests ---"
	@./$(TEST_IO_BIN)
	@echo "\n--- Running Run Tests ---"
	@./$(TEST_RUN_BIN)
	@echo "\n--- Running Runner Tests ---"
	@./$(TEST_RUNNER_BIN)
	@echo "\n--- Running Save Tests ---"
	@./$(TEST_SAVE_BIN)

$(TEST_BITARR_BIN): $(TEST_OBJS) $(TEST_DIR)/test_bitarr.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit

$(TEST_CARDNEGINE_BIN): $(TEST_OBJS) $(TEST_DIR)/test_card_engine.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit

$(TEST_PACIENCINE_BIN): $(TEST_OBJS) $(TEST_DIR)/test_paciencia_interpreter.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit

$(TEST_IO_BIN): $(TEST_OBJS) $(TEST_DIR)/test_IO.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit

$(TEST_SAVE_BIN): $(TEST_OBJS) $(TEST_DIR)/test_save.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit

$(TEST_RUN_BIN): $(TEST_OBJS) $(TEST_DIR)/test_run.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit

$(TEST_RUNNER_BIN): $(TEST_OBJS) $(TEST_DIR)/test_runner.c
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
