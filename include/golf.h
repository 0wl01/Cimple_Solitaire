#ifndef GOLF_H
#define GOLF_H

#include "card.h"

/**
 *
 * @file
 * @brief Main file for starting the game.
 */

/** @brief Default number of card columns in golf */
#define GOLF_COLUMNS 7
/** @brief Default size of each column in golf */
#define GOLF_COLUMN_SIZE 5

/**
 * @brief Structure holding the complete state of a Golf solitaire game.
 */
typedef struct {
    Deck *stock;                 /**< The draw pile (face-down cards). */
    Deck *waste;                 /**< The discard pile (where cards are played). */
    Deck *columns[GOLF_COLUMNS]; /**< The 7 columns of cards on the tableau. */
} golf_state;

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
