#ifndef CLI_H
#define CLI_H
#include "card.h"
#include "command.h"
#include "game.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Prints a single card to the terminal using Unicode symbols.
 * @details Checks the flip state to show either the back or the face.
 * @param card The Card structure to be rendered.
 */
void print_card(const Card card);

// TODO: docs
void print_invalid_column();

/**
 * @brief Debug function to print all cards in a deck horizontally.
 * @param deck Pointer to the Deck to be printed.
 */
void print_deck(const Deck *deck);

/**
 * @brief Renders the 7 game columns in a vertical, grid-like layout.
 * @note This function handles different column heights by checking the biggest deck.
 * @param decks Array of pointers to the column decks.
 * @param columns Number of columns to display (usually GOLF_COLUMNS).
 */
void print_decks_columns(Deck *restrict decks[], const uint8_t columns);

/**
 * @brief Prints the final game message (Win/Loss).
 * @param win true for win, false for loss.
 */
void print_end(const bool win);

//TODO: docs
void print_unknown_command();

/**
 * @brief Renders the entire Golf game table.
 * @details Displays the column headers (1-7), the columns themselves, 
 * the stock pile, and the waste pile.
 * @param table Pointer to the current game state.
 */
void print_golf_table(const size_t column_size, Deck *restrict columns[], Deck *restrict stock, Deck *restrict waste);
/**
 * @brief Displays the command prompt to the user.
 */
void print_prompt();

/**
 * @brief Displays the help menu with available commands.
 */
void print_golf_help();

// TODO: docs and actual help message.
void print_simon_help();

// TODO: docs
Command game_get_input();

// TODO: docs
char menu_get_input();

// TODO: docs
void print_table(const TableLayout *restrict t);
#endif
