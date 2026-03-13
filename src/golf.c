#include "golf.h"
#include <stdint.h>
#include <stdio.h>

#define COLUMNS 7
#define COLUMN_SIZE 5

static const uint8_t deal_lookup[16] = {
    0x00, // 0 is reserved to represent empty space.
    0x00, // TBD
    0x00, // TBD
    0xF4, 0x53, 0x64, 0x75, 0x86, 0x97, 0xA8,
    0xB9, 0xCA, 0xDB, 0xEC, 0xFD, 0x3E,
};

static void init_columns(Deck *columns[]) {
    for (uint8_t i = 0; i < COLUMNS; ++i) {
        columns[i] = create_deck(COLUMN_SIZE);
    }
}

// is this the best way to write this?
// Check if can deal from d1 to d2.
static int8_t can_deal(Deck *d1, Deck *d2) {
    Card c1 = top_card(d1);
    Card c2 = top_card(d2);
    uint8_t possible_stacks = deal_lookup[c1.values.value];
    return c2.values.value == (possible_stacks & 0x0F) ||
           c2.values.value == ((possible_stacks >> 4) & 0x0F);
}

// TODO
void start_game() {
    Deck *stock = create_deck(52);
    Deck *foundation = create_deck(52);
    Deck *columns[COLUMNS];

    populate_deck(stock);
    print_deck(stock);
    printf("\n");
    init_columns(columns);
    shuffle_deck(stock);

    for (uint8_t i = 0; i < COLUMNS; ++i)
        deal(stock, columns[i], COLUMN_SIZE);
    flip_all(stock);
    for (uint8_t i = 0; i < COLUMNS; ++i)
        print_deck(columns[i]);
    print_deck(stock);
}
