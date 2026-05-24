#include "cli.h"
#include "registry.h"
#include "game_engine.h"
#include "card.h"
#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <inttypes.h>

// lookup tables for card symbols and suits
// Maybe if the red suits were index 1 and 3 i could use the 2^0 bit to check
// the color
const char *const SUIT[] = {"♠", "♥", "♣", "♦"};
const char *const CARDS[4][13] = {
    {"🂡", "🂢", "🂣", "🂤", "🂥", "🂦", "🂧", "🂨", "🂩", "🂪", "🂫", "🂭",
     "🂮"}, // 0: Spades
    {"🂱", "🂲", "🂳", "🂴", "🂵", "🂶", "🂷", "🂸", "🂹", "🂺", "🂻", "🂽",
     "🂾"}, // 1: Hearts
    {"🃑", "🃒", "🃓", "🃔", "🃕", "🃖", "🃗", "🃘", "🃙", "🃚", "🃛", "🃝",
     "🃞"}, // 2: Clubs
    {"🃁", "🃂", "🃃", "🃄", "🃅", "🃆", "🃇", "🃈", "🃉", "🃊", "🃋", "🃍",
     "🃎"}, // 3: Diamonds
};

// TODO: Implement way to paint the card red
// probably using ansi escape codes
void print_card(const card c) {
  if (card_flipped(c))
    printf("\U0001F0A0 ");
  else if (card_value(c) < 3)
    printf(" ");
  else {
    const uint8_t val_idx = card_value(c) - 3;
    printf("%s ", CARDS[card_suit(c)][val_idx]);
  }
}

void print_deck(Deck const *deck) {
  for (int8_t i = deck->top - 1; i >= 0; --i) {
    // printf("(%d: ", i);
    print_card(deck->cards[i]);
    printf(", ");
    // printf("%d), ", deck->cards[i].values.value);
  }
  putchar('\n');
}

// The top card in the columns is not the top card of the stack. This function
// reverts the stack.
void print_decks_columns(Deck *restrict decks[], const uint8_t columns) {
  Deck *biggest = get_bigger_deck(decks, columns);
  for (card_count i = 0; i <= biggest->top; ++i) {
    for (card_count j = 0; j < columns; ++j) {
      if (decks[j]->top > i) {
        print_card(decks[j]->cards[i]);
      } else
        printf(" ");

      printf(" ");
    }
    printf("\n");
  }
}

void print_end(const bool win) {
  if (win)
    printf("You Win!\n");
  else
    printf("You Lose! HAHA\n");
  printf("Do you want to keep playing? (y/n)\n");
}

void print_prompt() { printf("(? for help) ~> "); }

void print_invalid_column() { printf("Invalid column!\n"); }

void print_unknown_command() { printf("Unknown command!\n"); }

static char index_to_col(uint8_t i) {
    return i < 26 ? 'a' + i : 'A' + (i - 26);
}

static void render_headers(const deck_registry *reg) {
    printf("    ");
    for (uint8_t i = 0; i < reg->n_entries; ++i)
        printf("%c  ", index_to_col(i));
    putchar('\n');
}

static void render_row(const deck_registry *reg, card_count row) {
    printf("%2"PRIuFAST16"  ", row);
    for (uint8_t i = 0; i < reg->n_entries; ++i) {
        const deck_entry *e = &reg->entries[i];
        if (e->deck->top > row) print_card(e->deck->cards[row]);
        else                    printf("  ");
        printf(" ");
    }
    putchar('\n');
}

static Deck *get_tallest(const deck_registry *reg) {
    Deck *tallest = reg->entries[0].deck;
    for (uint8_t i = 1; i < reg->n_entries; ++i)
        if (reg->entries[i].deck->top > tallest->top)
            tallest = reg->entries[i].deck;
    return tallest;
}

void dsl_render(void *state) {
    dsl_state *s = state;
    render_headers(s->reg);
    Deck *tallest = get_tallest(s->reg);
    for (card_count i = 0; tallest && i < tallest->top; ++i)
        render_row(s->reg, i);
}
