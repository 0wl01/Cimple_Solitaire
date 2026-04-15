#ifndef SIMON_H
#define SIMON_H

#include "card.h"
#include <stdint.h>

typedef struct {
    Deck *columns[10];     // The 10 tableau columns
    Deck *foundations[4];  // The 4 slots for completed suits
} simon_state;

#endif