#include "golf.h"
#include "cli.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

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
static uint8_t can_deal(Deck *d1, Deck *d2) {
    Card c1 = top_card(d1);
    uint8_t c2_val = top_card(d2).values.value;
    uint8_t possible_stacks = deal_lookup[c1.values.value];
    return possible_stacks && (c2_val == (possible_stacks & 0x0F) || c2_val == possible_stacks >> 4 || !c2_val);
}

/**
 *  TODO: Docs 
 * */
static void buy(Deck *restrict d1, Deck *restrict d2) {
    if (can_deal(d1, d2))
        deal(d1, d2, 1);
}

/**
 * @brief Checks if there's still a play to be made
 *
 * Calls @ref can_deal() to each card column and then checks if there's at least a card in stock.
 *
 * @return 0 if no play is find.
 *
 * @return Any other number if there's at least one play to be made.
 */
static uint8_t can_play(golf_state *table) {
    uint8_t result = table->stock->top;

    for (size_t i = 0; i < GOLF_COLUMNS; ++i)
        if (can_deal(table->columns[i], table->waste))
            result = 1;
    return result;
}

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
    for (uint8_t i = 0; i < GOLF_COLUMNS; ++i) {
        columns[i] = create_deck(GOLF_COLUMN_SIZE);
    }
}

// stop using exit and start returning a True or False here to be checked at the run golf
static void game_loop(char input, golf_state *table) {
    const uint8_t not_playable = !can_play(table);
    if (not_playable) {
        print_end(table->waste->top == 52);
        exit(0);
    }
    if (input == 's') {
        flip_deal(table->stock, table->waste, 1);
    } else if (input == '?') {
        print_help();
    } else if (input == 'q') {
        exit(0);
    } else if (input <= '7' && input >= '1') {
        input -= '1';
        buy(table->columns[(uint8_t)input], table->waste);
    }
}

// change this to a do while game loop is true.
static void run_golf(golf_state *table) {
    while (1) {
        print_golf_table((table));
        game_loop(get_input(), table);
    };
}

void init_golf() {
    golf_state table;
    table.stock = create_deck(52);
    table.waste = create_deck(52);
    init_columns(table.columns);
    populate_deck(table.stock);
    shuffle_deck(table.stock);
    for (size_t i = 0; i < GOLF_COLUMNS; ++i) {
        deal(table.stock, table.columns[i], GOLF_COLUMN_SIZE);
    }

    flip_all(table.stock);
    run_golf(&table);
}
