#include "macros.h"
#include "save.h"
#include "dsl.h"
#include "registry.h"
#include <stdio.h>
#include <string.h>

static const char *const VALUES[] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
static const char SUITS[] = "SHCD";

static void print_card_code(card c, FILE *f) { fprintf(f, "%s%c", VALUES[card_value(c) - 3], SUITS[card_suit(c)]); }

static void save_deck(const Deck *deck, FILE *f) {
    for (card_count i = 0; i < deck->top; ++i) {
        if (i > 0)
            fprintf(f, " ");
        print_card_code(deck->cards[i], f);
    }
    putc('\n', f);
}

bool save_game(const deck_registry *reg, const game_cfg *cfg, const char *dsl_filename) {
    FILE *f = fopen("save.paciencia", "w");
    if (!f)
        return false;
    fprintf(f, "%s\n", dsl_filename);
    for (uint8_t i = 0; i < reg->n_entries; ++i)
        save_deck(reg->entries[i].deck, f);
    fclose(f);
    return true;
}

static uint8_t parse_suit(char c) {
    const struct { char key; uint8_t suit; } map[] = {
        {'S', SPADES}, {'H', HEARTS}, {'C', CLUBS}, {'D', DIAMONDS}
    };
    uint8_t i = 0;
    while (i < 4 && map[i].key != c) ++i;
    return i < 4 ? map[i].suit : 0;
}

static uint8_t parse_value(const char *code, size_t len) {
    static const char *const vals[] = {
        "A","2","3","4","5","6","7","8","9","10","J","Q","K"
    };
    char val[4] = {0};
    strncpy(val, code, len - 1);
    for (uint8_t i = 0; i < 13; ++i)
        if (strcmp(val, vals[i]) == 0) return i + CARD_ACE;
    return 0;
}

static card parse_card_code(const char *token) {
    size_t len = strlen(token);
    uint8_t suit = parse_suit(token[len - 1]);
    uint8_t value = parse_value(token, len);
    return make_card(suit, value);
}

static void parse_token(deck_entry *e, const char *token, bool flipped) {
    card c = parse_card_code(token);
    if (flipped) flip_card(c);
    push(e->deck, c);
}

static void load_deck(deck_entry *e, char *line) {
    clear_deck(e->deck);
    bool flipped = strchr(e->flags, '_') != NULL;
    char *token = strtok(line, " \n");
    while (token) {
        parse_token(e, token, flipped);
        token = strtok(NULL, " \n");
    }
    if (strchr(e->flags, '^') && e->deck->top > 0)
        flip_card(e->deck->cards[e->deck->top - 1]);
}

static void load_decks(deck_registry *reg, FILE *f) {
    char *line cleanup(mfree) = NULL;
    size_t len = 0;
    uint8_t i = 0;
    while (i < reg->n_entries && getline(&line, &len, f) != -1) {
        load_deck(&reg->entries[i], line);
        ++i;
    }
}

bool load_game(deck_registry **reg, const game_cfg *cfg, const char *save_path) {
    FILE *f cleanup(close_file) = fopen(save_path, "r");
    if (!f) { perror(save_path); return false; }
    char *line cleanup(mfree) = NULL;
    size_t len = 0;
    getline(&line, &len, f);  // skip dsl filename
    free_registry(reg);
    *reg = build_registry(cfg);
    load_decks(*reg, f);
    return true;
}
