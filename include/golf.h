#ifndef GOLF_H
#define GOLF_H

#include "card.h"

/**
 *
 * @file
 * @brief Main file for starting the game.
 */

/**
 * @brief Starts the Golf game session.
 * Initializes the table, shuffles the deck, deals cards, and enters the game loop.
 */
void init_golf();

/**
 * @brief Recursively frees all decks within a golf_state.
 * @param table Pointer to the game state to be deallocated.
 */
void clean_golf(golf_state *table);

#endif