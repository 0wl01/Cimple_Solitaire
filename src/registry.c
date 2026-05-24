#include "registry.h"
#include "card.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

deck_entry *find_deck(const deck_registry *reg, const char *name, uint8_t index) {
    uint8_t seen = 0;
    for (uint8_t i = 0; i < reg->n_entries; ++i) {
        if (strcmp(reg->entries[i].name, name) == 0) {
            if (seen == index)
                return &reg->entries[i];
            ++seen;
        }
    }
    return NULL;
}

static bool populate_entries(deck_registry *restrict reg, const game_cfg *restrict cfg) {
    card_count cap = cfg->bar * DEFAULT_DECK_SIZE;
    Deck *stock = create_deck(cap);
    if (!stock)
        return false;

    for (size_t b = 0; b < cfg->bar; ++b)
        populate_deck(stock);
    shuffle_deck(stock);

    for (size_t i = 0; i < cfg->n_instances; ++i) {
        deck_entry *e = &reg->entries[i];
        strncpy(e->name, cfg->instances[i].deck_t, max_game_name_size - 1);
        size_t type_idx = 0;
        // This assumes the type is present in the deck types list
        // if not, type_idx will be out of bounds and cause UB
        while (strcmp(cfg->deck_types[type_idx].name, e->name))
            ++type_idx;
        strncpy(e->flags, cfg->deck_types[type_idx].flags, max_deck_t_flags_size - 1);
        bool flipped = strchr(e->flags, '_') != NULL || strchr(e->flags, '^') != NULL;
        e->deck = create_deck(cap);
        if (!e->deck)
            abort();

        deal(stock, e->deck, cfg->instances[i].n_cards, flipped);

        if(strchr(e->flags, '^')) flip_card(deck_top_card(e->deck));

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
