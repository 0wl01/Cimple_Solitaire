#include "render.h"
#include <stdio.h>



void print_card(const card c) {
    if (card_flip(c)) {
        printf("?? ");
    } else {
        uint8_t v = card_value(c);
        uint8_t s = card_suit(c);
        if (v >= 3 && v <= 15 && s < 4) {
            printf("%s%c ", VALUES[v - 3], SUITS[s]);
        }
    }
}

void print_prompt(void) { printf("(? for help) ~> "); }

void print_end(const bool win) {
    printf(win ? "You Win!\n" : "You Lose!\n");
    printf("Play again? (y/n)\n");
}

void print_unknown_command(void) { printf("Unknown command!\n"); }

void print_game_help(void) {
    printf("m<src> <idx> <dst> - move cards (e.g., m A 1 B)\n");
    printf("s <file> - save game\n");
    printf("l <file> - load game\n");
    printf("u - undo last move\n");
    printf("? - help\n");
    printf("q - quit\n");
}

static void print_deck_row(size_t i, deck_t *deck) {
    char prefix = i < 26 ? 'A' + i : 'a' + (i - 26);
    printf("%c: ", prefix);
    for (size_t j = 0; deck && j < deck->top; ++j) {
        print_card(deck->cards[j]);
    }
    printf("\n");
}

void print_game_table(const game_state_t *state) {
    for (size_t i = 0; state && i < state->deck_count; ++i) {
        print_deck_row(i, state->decks[i]);
    }
}
