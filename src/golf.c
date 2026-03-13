#include "golf.h"
#include <stdint.h>
#include <stdio.h>

/** @brief Default number of card columns in golf */
#define COLUMNS 7
/** @brief Default size of each column in golf */
#define COLUMN_SIZE 5

/**
 * @brief lookup table for stacking cards.
 *
 * This is a lookup table for stacking cards.
 * Each possible value card 0-15 is indexed here.
 * There are two bytes that each represent a card value.
 * A card can only be on top of a card here represented in its index.
 *
 * I'm using a lookup table because this is O(1) complexity.
 */
static const uint8_t deal_lookup[16] = {
    0x00, /**< 0 is reserved to represent empty space. */
    0x00, /**< 1 is reserved for TBD */
    0x00, /**< 2 is reserved for TBD */
    0xF4, 0x53, 0x64, 0x75, 0x86, 0x97, 0xA8, 0xB9, 0xCA, 0xDB, 0xEC, 0xFD, 0x3E,
};

/**
 * @brief Creates the 7 columns for the game.
 *
 * Allocates 7 bytes of memory for each one of the 7 columns.
 *
 * @param columns List of Pointers to Decks
 *
 * @see Deck
 * @see create_deck()
 */
static void init_columns(Deck *columns[]) {
    for (uint8_t i = 0; i < COLUMNS; ++i) {
        columns[i] = create_deck(COLUMN_SIZE);
    }
}

/**
 * @brief Checks if can deal a card to another deck.
 *
 * This uses the @ref deal_lookup lookup table
 * to see if dealing a card from d1 to d2 is valid.
 *
 * I can probably write this more efficiently.
 *
 * @param d1 Pointer to a Deck
 * @param d2 Pointer to a Deck
 *
 * @return 1 if valid and 0 if not.
 *
 * @see deal_lookup
 * @see Deck
 */
static int8_t can_deal(Deck *d1, Deck *d2) {
    Card c1 = top_card(d1);
    Card c2 = top_card(d2);
    uint8_t possible_stacks = deal_lookup[c1.values.value];
    return c2.values.value == (possible_stacks & 0x0F) || c2.values.value == ((possible_stacks >> 4) & 0x0F);
}

// TODO
void start_game() {
    Deck *stock = create_deck(52);
    Deck *foundation = create_deck(52);
    Deck *columns[COLUMNS];

    populate_deck(stock);
    shuffle_deck(stock);
    init_columns(columns);

    for (uint8_t i = 0; i < COLUMNS; ++i) {
        deal(stock, columns[i], COLUMN_SIZE);
        print_deck(columns[i]);
    }
    flip_all(stock);
    print_deck(stock);
}
