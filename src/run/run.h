

/**
 * @file run.h
 * @brief Header file for the run module, responsible for running the game.
 */

#pragma once
#include <stdbool.h>

/**
 * @brief Instantiates the game state and starts the main command loop.
 * * @param path The filepath to the .paciencia DSL script.
 * @return true if the game wants to restart, false if quitting.
 */
bool run_dsl_game(const char *path);
