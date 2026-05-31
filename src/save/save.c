#include "save.h"
#include <stdio.h>
#include <string.h>

static game_state_t *init_empty_game_state(const paciencia_game_t *rules) {
    game_state_t *state = calloc(1, sizeof(game_state_t));
    if (state && rules) {
        state->rules = rules;
        state->deck_count = rules->decks_to_create;
        state->decks = calloc(state->deck_count, sizeof(deck_t*));
        for (size_t i = 0; state->decks && i < state->deck_count; ++i) {
            deck_recipe_t *r = &state->rules->deck_recipes[i];
            state->decks[i] = create_deck(r->id, r->metadata, 52);
        }
    }
    return state;
}

bool save_game_file(const game_state_t *state, const char *path, const char *dsl_file) {
    FILE *f = fopen(path, "w");
    if (!f) return false;
    fprintf(f, "%s\n", dsl_file);
    for (size_t i = 0; i < state->deck_count; ++i) {
        deck_t *d = state->decks[i];
        for (size_t j = 0; j < d->top; ++j) {
            if (j > 0) fprintf(f, " ");
            const char *v[] = {"A","2","3","4","5","6","7","8","9","10","J","Q","K"};
            fprintf(f, "%s%c", v[card_value(d->cards[j]) - 3], "SHCD"[card_suit(d->cards[j])]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
    return true;
}

static uint8_t parse_val(const char *str, size_t len) {
    if (str[0] == 'A') return 3;
    if (str[0] == 'J') return 13;
    if (str[0] == 'Q') return 14;
    if (str[0] == 'K') return 15;
    if (len > 2 && str[0] == '1' && str[1] == '0') return 12;
    return (str[0] - '0') + 2;
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
    FILE *f = fopen(path, "r");
    if (!f) return NULL;
    char *line = NULL; size_t len = 0;
    if (getline(&line, &len, f) == -1) { fclose(f); return NULL; }
    game_state_t *st = init_empty_game_state(rules);
    for (size_t i = 0; st && i < st->deck_count; ++i) {
        if (getline(&line, &len, f) != -1) load_deck_line(st->decks[i], line);
    }
    free(line);
    fclose(f);
    return st;
}
