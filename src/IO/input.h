#pragma once
#include "command.h"

/**
 * @brief Reads user input for simple menu yes/no/quit interactions.
 */
char menu_get_input(void);

/**
 * @brief Reads and parses main game commands (move, save, load, undo, quit).
 */
Command game_get_input(void);
