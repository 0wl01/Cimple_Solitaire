# Vars
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g -flto -march=native -O3 -I include

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

.PHONY = all clean run test

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@
	@echo "Done!"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_BIN)
	@echo "Running tests..."
	@./$(TEST_BIN)

$(TEST_BIN): $(TEST_OBJS) $(TEST_DIR)/test_card.c
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ -lcunit

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Cleaned!"

run: all
	@echo "Starting..."
	@./$(TARGET)

