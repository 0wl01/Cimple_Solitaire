#pragma once
#include "command.h"
#include "dsl.h"
#include "game.h"
#include "registry.h"
#include "render.h"
#include <stdint.h>

constexpr uint8_t MAX_ROOMS_SAVES = 100;

bool dsl_can_play(void *state);
void print_game_table(void *state);
void dsl_post_turn(void *state);
bool dsl_has_won(void *state);
bool run_dsl_game(const char *path);
LoopSignal dsl_handle_move(void *restrict state, Command cmd);

typedef struct {
    deck_registry *reg;
    game_cfg *cfg;
    const char *dsl_filename;
} engine_state;

typedef bool (*FlagCheck)(const Deck *src, const Deck *dst, card_count index);

typedef struct {
    char       flag;
    FlagCheck  check;
} FlagDispatch;

typedef struct {
    engine_state *currS[MAX_ROOMS_SAVES];
    uint8_t currTop;
} Saveroom;
