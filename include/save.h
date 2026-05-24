#pragma once
#include "dsl.h"
#include "registry.h"

bool save_game(const deck_registry *reg, const game_cfg *cfg, const char *dsl_filename);
bool load_game(deck_registry **reg, const game_cfg *cfg, const char *save_path);
