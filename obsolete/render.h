#pragma once
#include "card.h"
#include <stdbool.h>
#include <stdint.h>

void print_card(const card c);
void print_prompt(void);
void print_end(const bool win);
void print_unknown_command(void);
void print_game_table(void *state);
void print_game_help(void);
