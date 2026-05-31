#pragma once
#include "card.h"
#include "dsl.h"

typedef struct {
    name_t name;
    Deck  *deck;
    char   flags[max_deck_t_flags_size];
} deck_entry;

typedef struct {
    deck_entry *entries;
    uint8_t     n_entries;
} deck_registry;

deck_registry *build_registry(const game_cfg *cfg);
deck_entry    *find_deck(const deck_registry *reg, const char *name, uint8_t index);
void           free_registry(deck_registry **reg);
