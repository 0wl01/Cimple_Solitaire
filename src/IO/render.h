#pragma once
#include "game_runner.h"
#include <stdbool.h>

void print_card(const card c);
void print_prompt(void);
void print_end(const bool win);
void print_unknown_command(void);
void print_game_table(const game_state_t *state);
void print_game_help(void);
