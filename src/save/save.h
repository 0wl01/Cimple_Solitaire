#pragma once
#include "game_runner.h"
#include <stdbool.h>

/**
 * @brief Saves the current game state to a specified file.
 */
bool save_game_file(const game_state_t *state, const char *path, const char *dsl_file);

/**
 * @brief Loads a game state from a specified file.
 */
game_state_t *load_game_file(const paciencia_game_t *rules, const char *path);
