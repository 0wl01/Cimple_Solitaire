#include "golf.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>

/** @brief Default number of card columns in golf */
#define COLUMNS 7
/** @brief Default size of each column in golf */
#define COLUMN_SIZE 5

static uint8_t playable;

static struct {
    Deck *stock;
    Deck *waste;
    Deck *columns[COLUMNS];
} table;

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
static uint8_t can_deal(Deck *d1, Deck *d2) {
    Card c1 = top_card(d1);
    uint8_t c2_val = top_card(d2).values.value;
    uint8_t possible_stacks = deal_lookup[c1.values.value];
    return possible_stacks && (c2_val == (possible_stacks & 0x0F) || c2_val == possible_stacks >> 4 || !c2_val);
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
static uint8_t can_play() {
    for (size_t i = 0; i < COLUMNS; ++i)
        if (can_deal(table.columns[i], table.waste))
            return 1;
    return table.stock->top;
}

char get_input() {
    char buffer[10];
    fgets(buffer, 10, stdin);

    return buffer[0];
}

static void render() {
    for (size_t i = 0; i <= COLUMN_SIZE; ++i) {
        for (size_t j = 0; j < COLUMNS; ++j) {
            if (table.columns[j]->top > i)
                print_card(table.columns[j]->cards[i]);
            else
                printf(" ");
        }
        printf("\n");
    }
    printf("\n");
    print_card(top_card(table.stock));
    printf(" ");
    print_card(top_card(table.waste));
    printf("\n");
}

static void buy(uint8_t q) {
    if (can_deal(table.columns[q], table.waste))
        deal(table.columns[q], table.waste, 1);
}

void do_logic(char input) {
    if (input == 's') {
        flip_deal(table.stock, table.waste, 1);
    }
    if (input > '7' || input < '1')
        return;
    input -= '1';
    buy(input);
}

void game_loop() {
    while (1) {

        render();
        do_logic(get_input());
    };
}

// TODO
void start_game() {
    table.stock = create_deck(52);
    table.waste = create_deck(52);
    init_columns(table.columns);
    populate_deck(table.stock);
    shuffle_deck(table.stock);
    for (size_t i = 0; i < COLUMNS; ++i) {
        deal(table.stock, table.columns[i], COLUMN_SIZE);
    }

    flip_all(table.stock);
    game_loop();
}
