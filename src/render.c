#include "render.h"
#include "card.h"
#include "game_engine.h"
#include <inttypes.h>
#include <stdio.h>

static const char *const CARDS[4][13] = {
    {"🂡", "🂢", "🂣", "🂤", "🂥", "🂦", "🂧", "🂨", "🂩", "🂪", "🂫", "🂭", "🂮"},
    {"🂱", "🂲", "🂳", "🂴", "🂵", "🂶", "🂷", "🂸", "🂹", "🂺", "🂻", "🂽", "🂾"},
    {"🃑", "🃒", "🃓", "🃔", "🃕", "🃖", "🃗", "🃘", "🃙", "🃚", "🃛", "🃝", "🃞"},
    {"🃁", "🃂", "🃃", "🃄", "🃅", "🃆", "🃇", "🃈", "🃉", "🃊", "🃋", "🃍", "🃎"},
};

static char index_to_col(uint8_t i) { return i < 26 ? 'a' + i : 'A' + (i - 26); }

void print_card(const card c) {
    if (card_flipped(c))
        printf("🂠 ");
    else if (card_value(c) < 3)
        printf("   ");
    else
        printf("%s ", CARDS[card_suit(c)][card_value(c) - 3]);
}

void print_prompt(void) { printf("(? for help) ~> "); }

void print_end(const bool win) {
    printf(win ? "You Win!\n" : "You Lose!\n");
    printf("Play again? (y/n)\n");
}

void print_unknown_command(void) { printf("Unknown command!\n"); }

static void print_row(uint8_t i, const deck_entry *e) {
    printf("%c: ", index_to_col(i));
    for (card_count j = 0; j < e->deck->top; ++j)
        print_card(e->deck->cards[j]);
    putchar('\n');
}

void print_game_table(void *state) {
    engine_state *s = state;
    for (uint8_t i = 0; i < s->reg->n_entries; ++i)
        print_row(i, &s->reg->entries[i]);
}

void print_game_help(void) {
    printf("m<src><index><dst> - move cards (e.g. ma3b)\n");
    printf("s - save game\n");
    printf("l - load game\n");
    printf("? - help\n");
    printf("r - restart\n");
    printf("q - quit\n");
}
