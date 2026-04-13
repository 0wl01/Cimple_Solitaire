# Vars
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g -march=native -O3 -I include -I src

# Dirs
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
BIN_DIR = bin
TEST_DIR = tests

# Files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

TARGET = $(BIN_DIR)/golf

# Tests stuff
TEST_OBJS = $(filter-out $(BUILD_DIR)/main.o, $(OBJS))
TEST_BIN = $(BIN_DIR)/test_card
TEST_GOLF_BIN = $(BIN_DIR)/test_golf
GOLF_TEST_DEPS = $(BUILD_DIR)/card.o $(BUILD_DIR)/cli.o

.PHONY = all clean run test

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@
	@echo "Done!"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_BIN) $(TEST_GOLF_BIN)
	@echo "--- Running Card Tests ---"
	@./$(TEST_BIN)
	@echo "\n--- Running Golf Rules Tests ---"
	@./$(TEST_GOLF_BIN)

$(TEST_BIN): $(TEST_OBJS) $(TEST_DIR)/test_card.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit

$(TEST_GOLF_BIN): $(GOLF_TEST_DEPS) $(TEST_DIR)/test_golf.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Cleaned!"

run: all
	@echo "Starting..."
	@./$(TARGET)
