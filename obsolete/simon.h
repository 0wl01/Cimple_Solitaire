#ifndef SIMON_H
#define SIMON_H

#include "card.h"
#include <stdint.h>

/** @brief Number of tableau columns in Simple Simon. */
#define SIMON_COLUMNS 10
/** @brief Number of foundations in Simple Simon. */
#define SIMON_FOUNDATIONS 4
/** @brief Maximum size of a foundation deck. */
#define SIMON_FOUNDATION_SIZE 13
/** @brief Initial maximum size allocated for a column deck. */
#define SIMON_COLUMN_SIZE 20

/**
 * @brief Structure holding the complete state of a Simple Simon game.
 */
typedef struct {
    Deck *columns[SIMON_COLUMNS];         // The 10 tableau columns
    Deck *foundations[SIMON_FOUNDATIONS]; // The 4 slots for completed suits
} simon_state;

/**
 * @brief Initializes and runs the Simple Simon game.
 * @return true if the user wants to restart, false to quit.
 */
bool init_simple_simon();

#endif
