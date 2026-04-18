// Some bits of code shared by all games

#ifndef GAME_H
#define GAME_H
#include "card.h"
#include "command.h"
#include <stddef.h>

typedef enum { LOOP_CONTINUE = 0, LOOP_RESTART, LOOP_QUIT } LoopSignal;

typedef LoopSignal (*CommandHandler)(void *restrict game_state, const Command cmd);

typedef struct {
    CommandType type;
    CommandHandler handler;
} CommandDispatch;

typedef struct {
    Deck *restrict *columns;
    Deck *restrict *foundations;
    Deck *restrict stock;
    Deck *restrict waste;
    size_t n_columns;
    size_t n_foundations;
} TableLayout;

LoopSignal default_handle_quit(void *state, const Command cmd);
LoopSignal default_handle_restart(void *state, const Command cmd);
LoopSignal default_handle_unknown(void *state, const Command cmd);
LoopSignal default_handle_help(void *state, const Command cmd);
LoopSignal default_handle_hint(void *state, const Command cmd);

LoopSignal dispatch(const CommandDispatch *table, const size_t table_size, void *state, Command cmd);

#endif
