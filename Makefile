# Compiler and Base Flags
CC = gcc
BASE_CFLAGS = -Wall -Wextra -pedantic -I include -I src

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = tests

# Auto-detects all .c files in the src directory
SRCS = $(wildcard $(SRC_DIR)/*.c)
# Translates .c paths into .o paths for the build directory
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Target Executable
TARGET = $(BIN_DIR)/c-litaire

# Release version (Performance optimizations)
all: CFLAGS = $(BASE_CFLAGS) -O3 -flto -march=native -DNDEBUG

# Debugging versions (Debug symbols, no optimizations)
gdb valgrind test: CFLAGS = $(BASE_CFLAGS) -g -O0
coverage: CFLAGS = $(BASE_CFLAGS) -g -O0 --coverage
TEST_OBJS = $(filter-out $(BUILD_DIR)/main.o, $(OBJS))
TEST_BIN = $(BIN_DIR)/test_card
TEST_GOLF_BIN = $(BIN_DIR)/test_golf
TEST_SIMON_BIN = $(BIN_DIR)/test_simon

# Dependencies for Golf tests
GOLF_TEST_DEPS = $(BUILD_DIR)/card.o $(BUILD_DIR)/cli.o $(BUILD_DIR)/game.o
# Dependencias for Simon tests
SIMON_TEST_DEPS = $(BUILD_DIR)/card.o $(BUILD_DIR)/cli.o $(BUILD_DIR)/game.o

.PHONY: all clean run test gdb valgrind coverage

# Default build rule
all: $(TARGET)

# Linking the final executable
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@
	@echo "Build successful! Executable generated at $@"

# Compiling individual object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Test execution rule
test: $(TEST_BIN) $(TEST_GOLF_BIN) $(TEST_SIMON_BIN)
	@echo "\n--- Running Card Tests ---"
	@./$(TEST_BIN)
	@echo "\n--- Running Golf Rules Tests ---"
	@./$(TEST_GOLF_BIN)
	@echo "\n--- Running Simon Rules Tests ---"
	@./$(TEST_SIMON_BIN)

# Building Card tests
$(TEST_BIN): $(TEST_OBJS) $(TEST_DIR)/test_card.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit

# Building Golf tests
$(TEST_GOLF_BIN): $(GOLF_TEST_DEPS) $(TEST_DIR)/test_golf.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit

# Building Simon tests
$(TEST_SIMON_BIN): $(SIMON_TEST_DEPS) $(TEST_DIR)/test_simon.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit

# Start GNU Debugger
gdb: all
	@echo "\n--- Starting GNU Debugger (GDB) ---"
	@echo "Tip: Type 'run' at the prompt to start execution."
	@echo "     If a crash occurs, type 'bt' (backtrace) to pinpoint the exact line of failure."
	gdb ./$(TARGET)

# Start Memory Leaks Tester
valgrind: all
	@echo "\n--- Analyzing Memory Leaks with Valgrind ---"
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

coverage: clean
	@$(MAKE) test CFLAGS="$(BASE_CFLAGS) -g -O0 --coverage" > /dev/null
	@echo "\n--- Relatório de Cobertura ---"
	@find . -name "*.gcda" -exec gcov {} \; | grep -A 1 "File"

# Cleanup rule
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Workspace cleaned successfully!"

# Build and run the main game
run: all
	@echo "\nStarting C-litaire...\n"
	@./$(TARGET)
