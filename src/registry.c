#include "registry.h"
#include "card.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *find_type_flags(const game_cfg *cfg, const char *name) {
    size_t i = 0;
    while (strcmp(cfg->deck_types[i].name, name))
        ++i;
    return cfg->deck_types[i].flags;
}

static void populate_entry(deck_entry *e, const game_cfg *cfg,
                            Deck *stock, card_count cap, size_t i) {
    strncpy(e->flags, find_type_flags(cfg, e->name), max_deck_t_flags_size - 1);
    bool flipped = strchr(e->flags, '_') || strchr(e->flags, '^');
    e->deck = create_deck(cap);
    deal(stock, e->deck, cfg->instances[i].n_cards, flipped);
    if (strchr(e->flags, '^') && deck_count(e->deck))
        flip_card(deck_top_card(e->deck));
}

static bool populate_entries(deck_registry *restrict reg,
                              const game_cfg *restrict cfg) {
    card_count cap = cfg->bar * DEFAULT_DECK_SIZE;
    Deck *stock = create_deck(cap);
    if (!stock) return false;
    populate_deck(stock);
    shuffle_deck(stock);
    for (size_t i = 0; i < cfg->n_instances; ++i) {
        deck_entry *e = &reg->entries[i];
        strncpy(e->name, cfg->instances[i].deck_t, max_game_name_size - 1);
        populate_entry(e, cfg, stock, cap, i);
    }
    eliminate_deck(&stock);
    return true;
}

deck_registry *build_registry(const game_cfg *cfg) {
    deck_registry *reg = calloc(1, sizeof(deck_registry));
    if (!reg)
        return NULL;

    reg->n_entries = cfg->n_instances;
    reg->entries = calloc(reg->n_entries, sizeof(deck_entry));

    if (!reg->entries) {
        free(reg);
        return NULL;
    }

    if (!populate_entries(reg, cfg)) {
        free_registry(&reg);
        return NULL;
    }

    return reg;
}

void free_registry(deck_registry **reg) {
    if (!*reg)
        return;
    if ((*reg)->entries) {
        for (uint8_t i = 0; i < (*reg)->n_entries; ++i)
            eliminate_deck(&(*reg)->entries[i].deck);
        free((*reg)->entries);
    }
    free(*reg);
    *reg = NULL;
}
