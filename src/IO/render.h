
/**
 * @file render.h
 * @brief Header file for the render module, responsible for rendering the game.
 */

#pragma once
#include "game_runner.h"
#include <stdbool.h>

// ================= IMPORTANT TABLE
// Do not remove

static const char *const VALUES[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
static const char SUITS[] = "SHCD";


// ================= FUNCTIONS PROTOTYPES

/**
 * @brief Prints a card to the console.
 * @param c The card to print.
 */
void print_card(const card c);

/**
 * @brief Prints the prompt to the console.
 */
void print_prompt(void);

/**
 * @brief Prints the end of the game to the console.
 * @param win Whether the player won or lost.
 */
void print_end(const bool win);

/**
 * @brief Prints unknown command error to the console.
 */
void print_unknown_command(void);

/**
 * @brief Prints the game table to the console.
 * @param state The game state to print.
 */
void print_game_table(const game_state_t *state);

/**
 * @brief Prints the game help to the console.
 */
void print_game_help(void);
