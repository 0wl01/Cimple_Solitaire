#include "golf.h"
#include <stdint.h>

#define COLUMNS 7
#define COLUMN_SIZE 5

const uint8_t deal_lookup[16] = {
    0x00, // 0 is reserved to represent empty space.
    0x00, // TBD
    0x00, // TBD
    0xF4, 0x53, 0x64, 0x75, 0x86, 0x97, 0xA8,
    0xB9, 0xCA, 0xDB, 0xEC, 0xFD, 0x3E,
};

void init_columns(Deck *columns[]) {
    for (uint8_t i = 0; i < COLUMNS; ++i) {
        columns[i] = create_deck(COLUMN_SIZE);
    }
}

// is this the best way to write this?
int8_t can_deal(Deck *d1, Deck *d2) {
    uint8_t possible_stacks = deal_lookup[VALUE_INDEX(top_card(d1))];
    return VALUE_INDEX(top_card(d2)) == (possible_stacks & 0x0F) ||
           VALUE_INDEX(top_card(d2)) == ((possible_stacks >> 4) & 0x0F);
}

// deal from d1 to d2 and flip the card
void flip_deal(Deck *d1, Deck *d2) {

    deal(d1, d2, 1);
    d2->cards[d2->top - 1] = FLIP_CARD(top_card(d2));
}

// TODO
void start_game() {
    Deck *stock = create_deck(52);
    Deck *foundation = create_deck(52);
    Deck *columns[COLUMNS];

    populate_deck(stock);
    init_columns(columns);
    shuffle_deck(stock);

    for (uint8_t i = 0; i < COLUMNS; ++i)
        deal(stock, columns[i], COLUMN_SIZE);
    flip_all(stock);
    for (uint8_t i = 0; i < COLUMNS; ++i)
        print_deck(columns[i]);
    print_deck(stock);
}
