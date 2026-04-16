#include "cli.h"
#include "menu.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

// lookup tables for card symbols and suits
// Maybe if the red suits were index 1 and 3 i could use the 2^0 bit to check
// the color
const char *const SUIT[] = {"♠", "♥", "♦", "♣"};
const char *const CARDS[4][13] = {
    {"🂡", "🂢", "🂣", "🂤", "🂥", "🂦", "🂧", "🂨", "🂩", "🂪", "🂫", "🂭", "🂮"}, // 0: Spades
    {"🂱", "🂲", "🂳", "🂴", "🂵", "🂶", "🂷", "🂸", "🂹", "🂺", "🂻", "🂽", "🂾"}, // 1: Hearts
    {"🃁", "🃂", "🃃", "🃄", "🃅", "🃆", "🃇", "🃈", "🃉", "🃊", "🃋", "🃍", "🃎"}, // 2: Diamonds
    {"🃑", "🃒", "🃓", "🃔", "🃕", "🃖", "🃗", "🃘", "🃙", "🃚", "🃛", "🃝", "🃞"}  // 3: Clubs
};

// TODO: Implement way to paint the card red
// probably using ansi escape codes
void print_card(const Card c) {
    if (c.values.flip)
        printf("\U0001F0A0");
    else if (c.values.value < 3)
        printf(" ");
    else {
        const uint8_t val_idx = c.values.value - 3;
        printf("%s", CARDS[c.values.suit][val_idx]);
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

// The top card in the columns is not the top card of the stack. This function reverts the stack.
void print_decks_columns(Deck *restrict decks[], const uint8_t columns) {
    Deck *biggest = get_bigger_deck(decks, columns);
    for (size_t i = 0; i <= biggest->top; ++i) {
        for (size_t j = 0; j < columns; ++j) {
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
}

void print_golf_table(golf_state *table) {
    puts("1 2 3 4 5 6 7\n");
    print_decks_columns(table->columns, GOLF_COLUMNS);
    print_card(top_card(table->stock));
    printf(" ");
    print_card(top_card(table->waste));
    printf("\n");
}

void print_prompt() { printf("(? for help) ~> "); }

void print_golf_help() {
    printf(" s - To deal card from the stock\n 1-7 - To deal cards from the columns\n q - To quit\n ? - For this "
           "screen\n");
}

char game_get_input() {}

char menu_get_input() {
    char buffer[3];
    return !fgets(buffer, sizeof(buffer), stdin) ? buffer[0] : 'q';
}

void print_menu(const size_t games_length, const GameOption *restrict games) {
    printf("\n==================================\n");
    printf("   Cimple-Solitaire   \n");
    printf("==================================\n");

    for (size_t i = 0; i < games_length; ++i)
        printf("  [%zu] %s\n", i + 1, games[i].name);
    printf("  [q] Exit\n");
    printf("==================================\n");
    printf("Pick your game: \n");
}
