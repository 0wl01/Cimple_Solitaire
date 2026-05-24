#pragma once
#include "command.h"
#include "dsl.h"
#include "game.h"
#include "registry.h"

bool dsl_can_play(void *state);
void dsl_render(void *state);
void dsl_post_turn(void *state);
bool dsl_has_won(void *state);
bool run_dsl_game(const char *path);
LoopSignal dsl_handle_move(void *restrict state, Command cmd);

typedef struct {
    deck_registry *reg;
    game_cfg *cfg;
} dsl_state;
