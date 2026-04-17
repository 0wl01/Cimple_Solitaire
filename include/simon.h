#ifndef SIMON_H
#define SIMON_H

#include "card.h"
#include "cli.h"
#include <stdint.h>

#define SIMON_COLUMNS 10
#define SIMON_FOUNDATIONS 4
#define SIMON_FOUNDATION_SIZE 13
#define SIMON_COLUMN_SIZE 20

typedef struct {
    Deck *columns[SIMON_COLUMNS];         // The 10 tableau columns
    Deck *foundations[SIMON_FOUNDATIONS]; // The 4 slots for completed suits
} simon_state;

// TODO:  needs docs
bool init_simple_simon();

// TODO: docs
typedef int (*SimonCommandHandler)(simon_state *restrict state, const Command cmd);

// TODO: docs
typedef struct {
    CommandType type;
    SimonCommandHandler handler;
} SimonCommandDispatch;

#endif
