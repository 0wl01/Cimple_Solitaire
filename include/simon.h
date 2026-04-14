#ifndef SIMON_H
#define SIMON_H

#include "card.h"
#include <stdint.h>

/**
 * @brief Represents the game state for Simple Simon.
 */
typedef struct {
    Deck *columns[10];     // The 10 tableau columns
    Deck *foundations[4];  // The 4 slots for completed suits
} simon_state;

/**
 * @brief Starts the Simple Simon game loop.
 * Must take (void) to be compatible with the main menu function pointers.
 */
void init_simple_simon(void);

#endif