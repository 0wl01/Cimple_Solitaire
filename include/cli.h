#ifndef CLI_H
#define CLI_H

/**
 * @file 
 * @brief Communication between the client and the game
 */

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

/**
 * @brief Prints an error message for invalid column selection.
 */
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

/**
 * @brief Prints an error message for unrecognized commands.
 */
void print_unknown_command();

/**
 * @brief Renders the entire Golf game table.
 * @details Displays the column headers (1-7), the columns themselves, 
 * the stock pile, and the waste pile.
 * @param table Pointer to the current game state.
 */
void print_golf_table(const size column_size, Deck *restrict columns[], Deck *restrict stock, Deck *restrict waste);

/**
 * @brief Displays the command prompt to the user.
 */
void print_prompt();

/**
 * @brief Displays the help menu with available commands.
 */
void print_golf_help();

/**
 * @brief Prints all the inputs to play Simon.
 */
void print_simon_help();

/**
 * @brief Get the commands from the player inside a card game.
 *
 * If the first character of the input is a 'm' then the output changes depending on what card game you are playing on, since
 * is one command they share but have different output.
 *  
 * @see Command
 */
Command game_get_input();

/**
 * @brief Reads a single character command from standard input.
 * @return The first character of the input, or 'q' if reading fails.
 */
char menu_get_input();

/** * @brief Renders the generic table layout for any game.
 * @details Displays the top row (stock, waste, foundations) and the column grid.
 * @param t Pointer to the TableLayout structure containing the game's state.
 */
void print_table(const TableLayout *restrict t);
#endif
