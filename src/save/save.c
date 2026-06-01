#include "save.h"
#include "game_runner.h"
#include "macros.h"
#include "render.h"
#include <stdio.h>
#include <string.h>

static game_state_t *init_empty_game_state(const paciencia_game_t *rules) {
    game_state_t *state = calloc(1, sizeof(game_state_t));
    if (state && rules) {
        state->rules = rules;
        state->deck_count = rules->decks_to_create;
        state->decks = calloc(state->deck_count, sizeof(deck_t *));
        for (size_t i = 0; state->decks && i < state->deck_count; ++i) {
            deck_recipe_t *r = &state->rules->deck_recipes[i];
            state->decks[i] = create_deck(r->id, r->metadata, 52);
        }
    }
    return state;
}

bool save_game_file(const game_state_t *restrict const state, const char *path, const char *dsl_file) {
    bool result_code = false;
    FILE *f cleanup(close_file) = fopen(path, "w");
    if (f) {
        fprintf(f, "%s\n", dsl_file);
        for (size_t i = 0; i < state->deck_count; ++i) {
            deck_t *d = state->decks[i];
            for (size_t j = 0; j < d->top; ++j) {
                fprintf(f, "%s%c ", VALUES[card_value(d->cards[j]) - 3], SUITS[card_suit(d->cards[j])]);
            }
            fprintf(f, "\n");
        }
        result_code = true;
    }
    return result_code;
}

static uint8_t parse_val(const char *str, size_t len) {
    card return_val = 0;
    if (str[0] == 'A')
        return_val = 3;
    if (str[0] == 'J')
        return_val = 13;
    if (str[0] == 'Q')
        return_val = 14;
    if (str[0] == 'K')
        return_val = 15;
    if (len > 2 && str[0] == '1' && str[1] == '0')
        return_val = 12;
    return_val = (str[0] - '0') + 2;
    return return_val;
}

static card parse_token(const char *t) {
    size_t l = strlen(t);
    uint8_t v = parse_val(t, l);
    char s = t[l - 1];
    uint8_t suit = (s == 'H') ? 1 : (s == 'C') ? 2 : (s == 'D') ? 3 : 0;
    return make_card(v, suit, 0);
}

static void load_deck_line(deck_t *deck, char *line) {
    char *tok = strtok(line, " \n");
    while (tok) {
        push_card_in_deck(parse_token(tok), deck);
        tok = strtok(NULL, " \n");
    }
}

game_state_t *load_game_file(const paciencia_game_t *rules, const char *path) {
    game_state_t *st = NULL;
    FILE *f cleanup(close_file) = fopen(path, "r");
    if (likely(f)) {
        char *line cleanup(mfree) = NULL;
        size_t len = 0;
        if (likely(getline(&line, &len, f) != -1)) {
            st = init_empty_game_state(rules);
            for (size_t i = 0; st && i < st->deck_count; ++i) {
                if (likely(getline(&line, &len, f) != -1))
                    load_deck_line(st->decks[i], line);
            }
        }
    }
    return st;
}
