# Compiler and Base Flags
CC = gcc
BASE_CFLAGS = -std=gnu2x -Wall -Wextra -pedantic -I include -I src -Wno-unused-parameter

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

# --- TEST CONFIGURATIONS ---
# Inclui todos os objetos gerados, exceto o main.o (para evitar 2 funções main nos testes)
TEST_OBJS = $(filter-out $(BUILD_DIR)/main.o, $(OBJS))

# Binários de Teste
TEST_CARD_BIN = $(BIN_DIR)/test_card
TEST_DSL_BIN = $(BIN_DIR)/test_dsl

# Já preparados para as Fases 2 e 3
TEST_REG_BIN = $(BIN_DIR)/test_registry
TEST_GAME_BIN = $(BIN_DIR)/test_dsl_game

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

# --- TEST RULES ---

# Test execution rule
test: $(TEST_CARD_BIN) $(TEST_DSL_BIN) $(TEST_REG_BIN)
	@echo "\n--- Running Card Tests ---"
	@./$(TEST_CARD_BIN)
	@echo "\n--- Running DSL Parser Tests ---"
	@./$(TEST_DSL_BIN)
	@echo "\n--- Running Registry Builder Tests ---"
	@./$(TEST_REG_BIN)

# Building Card tests
$(TEST_CARD_BIN): $(TEST_OBJS) $(TEST_DIR)/test_card.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit

# Building DSL tests
$(TEST_DSL_BIN): $(TEST_OBJS) $(TEST_DIR)/test_dsl.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit

# Building Registry tests
$(TEST_REG_BIN): $(BUILD_DIR)/registry.o $(TEST_OBJS) $(TEST_DIR)/test_registry.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit

# Building DSL Game tests (TODO)
$(TEST_GAME_BIN): $(TEST_OBJS) $(TEST_DIR)/test_dsl_game.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit

# --- UTILITIES ---

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

# Coverage Report
coverage: clean
	@$(MAKE) test CFLAGS="$(BASE_CFLAGS) -g -O0 --coverage" > /dev/null
	@echo "\n--- Relatório de Cobertura ---"
	@find . -name "*.gcda" -exec gcov {} \; | grep -A 1 "File 'src/" | grep -v "0.00%"

# Cleanup rule
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	rm -f *.gcov *.gcda *.gcno
	find . -name "*.gcda" -delete
	find . -name "*.gcno" -delete
	@echo "Workspace cleaned successfully!"

# Build and run the main game
run: all
	@echo "\nStarting C-litaire...\n"
	@./$(TARGET)
