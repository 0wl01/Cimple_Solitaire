#pragma once

#include "bitarr.h"
#include <iso646.h>
#include <stdint.h>
#include <stdlib.h>

// =============== MACRO CONSTANTS ===============

#define COMMON_DECK_SIZE 52
#define DECK_SIZE_MULTIPLIER 2

#define CARD_SUIT_COUNT 4
#define CARD_VALUE_COUNT 13

#define CARD_VALUE_MASK 0x0F
#define CARD_SUIT_MASK 0x30
#define CARD_SUIT_SHIFT 4
#define CARD_FLIP_MASK 0x40
#define CARD_FLIP_SHIFT 6
#define CARD_COLOR_MASK 0x10
#define CARD_COLOR_SHIFT 4

#define DECK_METADATA_SIZE 4

#define ACE_VALUE 3
#define KING_VALUE 15

// =============== ENUMS ================

typedef enum {
    DECK_FLAG_FACE_UP = 0,
    DECK_FLAG_FIXED_SIZE = 1,
    DECK_FLAG_HIDDEN = 2,
    DECK_FLAG_TOP_VISIBLE = 3,
} deck_flag;

// =============== TYPEDEFS ===============

typedef uint8_t card;
typedef uint_fast16_t card_count;

// flags: idx: 0 = '=', 1 = '1', 2 = '_', 3 = '^'
typedef struct {
    card_count size;
    card_count top;
    bit_arr_t *
        flags; // flags for a deck. '=' mean all cards must be face up, '1' mean the size is fixed to 1, '_' no card is ever visible, '^' only top card is visible.
    char *id;  // name of the deck, may be anything.
    card cards[];
} deck_t;

typedef bool (*card_predicate)(const card, const card);

// ============= CONSTANT EXPRESSIONS =============
// they are in fact used. do not remove.
constexpr size_t CARD_SIZE = sizeof(card);
constexpr size_t DECK_T_SIZE = sizeof(deck_t);

// ============= MACRO FUNCTIONS ===========

#define is_deck_ptr_full(deck_ptr) ((deck_ptr)->top == (deck_ptr)->size)
#define is_deck_ptr_empty(deck_ptr) ((deck_ptr)->top == 0)
#define is_deck_ptr_valid(deck_ptr) ((deck_ptr) != NULL)
#define is_deck_ptr_not_full(deck_ptr) ((deck_ptr)->top < (deck_ptr)->size)
#define is_deck_ptr_not_empty(deck_ptr) ((deck_ptr)->top > 0)

#define is_flipped_card(card) ((card) & CARD_FLIP_MASK)

#define make_card(value, suit, flip)                                                                                   \
    (((value) & CARD_VALUE_MASK) | (((suit) << CARD_SUIT_SHIFT) & CARD_SUIT_MASK) |                                    \
     (((flip) << CARD_FLIP_SHIFT) & CARD_FLIP_MASK))

#define flip_card(card) ((card) ^ CARD_FLIP_MASK)

// receives a flip (true / false) and makes the flip bit that value
#define flip_card_to(card, flip) (((card) & 0xBF) | ((flip) << CARD_FLIP_SHIFT))

#define card_value(card) ((card) & CARD_VALUE_MASK)
#define card_suit(card) (((card) & CARD_SUIT_MASK) >> CARD_SUIT_SHIFT)
#define card_flip(card) (((card) & CARD_FLIP_MASK) >> CARD_FLIP_SHIFT)
#define card_color(card) ((card) & CARD_COLOR_MASK)

// ============= FUNCTION PROTOTYPES =============

/**
 * @brief Creates a new deck with the given id and initial size.
 *
 * @param id The id of the deck.
 * @param start_size The initial size of the deck.
 * @return A pointer to the newly created deck, or NULL if memory allocation fails.
 */
void *create_deck(char const *restrict const deck_name, char const *restrict const metadata, card_count start_size);

/**
 * @brief Shuffles the deck using the Fisher-Yates algorithm.
 *
 * @param deck_ptr The deck to shuffle.
 */
void shuffle_deck(deck_t *deck_ptr);

/**
 * @brief Peeks at the card at the given index of the deck.
 *
 * @param deck_ptr The deck to peek at.
 * @param index The index of the card to peek at.
 * @return The card at the given index.
 */
card peek(const deck_t *restrict const deck_ptr, card_count index);

/**
 * @brief Pushes a card into the deck.
 *
 * @param card The card to push.
 * @param deck_ptr The deck to push the card into.
 * @return true if the card was successfully pushed, false otherwise.
 */
bool push_card_in_deck(const card card, deck_t *restrict const deck_ptr);

/**
 * @brief Pops a card from the deck.
 *
 * @param deck_ptr The deck to pop the card from.
 * @return The card that was popped, or 0 if the deck is empty.
 */
card pop_card_from_deck(deck_t *restrict const deck_ptr);

/**
 * @brief Fills the deck with cards.
 *
 * This function will resize the deck if necessary, and then fill it with cards.
 *
 * @param deck_ptr The deck to fill.
 * @param cards_to_fill The number of cards to fill the deck with.
 */
bool fill_deck_with_cards(deck_t **restrict const deck_ptr, const card_count cards_to_fill);

/**
 * @brief Flips all cards in the deck.
 *
 * @param deck_ptr The deck to flip.
 */
void flip_all_cards(deck_t *restrict const deck_ptr);

/**
 * @brief Unflips all cards in the deck.
 *
 * @param deck_ptr The deck to unflip.
 */
void unflip_all(deck_t *restrict const deck_ptr);

/**
 * @brief Gets a bigger deck within a given deckptr_array.
 *
 * @param size The size of the deck_ptr array.
 * @param deck_ptr The deck_ptr array to get the bigger deck from.
 * @return A pointer to the deck with the highest top value.
 */
deck_t *get_bigger_deck(const card_count size, deck_t *restrict const deck_ptr[size]);

/**
 * @brief Resizes a deck to fit a given size.
 *
 * @param deck_ptr The deck to resize.
 * @param new_size The new size of the deck.
 * @return true if the deck was successfully resized, false otherwise.
 */
bool resize_deck(deck_t **restrict const deck_ptr, const card_count new_size);

/**
 * @brief Splits a deck into two at a given position.
 *
 * @param position The position to split the deck at.
 * @param deck_ptr_source The deck to split.
 * @param deck_ptr_dest The destination deck to store the split deck.
 * @return true if the deck was successfully split, false otherwise.
 */
bool split_deck(const card_count position, deck_t *restrict const deck_ptr_source,
                deck_t **restrict const deck_ptr_dest);

/**
 * @brief Clones a deck.
 *
 * @param deck_ptr The deck to clone.
 * @return A pointer to the cloned deck, or NULL if the clone failed.
 */
deck_t *clone_deck(const deck_t *restrict const deck_ptr);

/**
 * @brief Deals a card from the source deck to the destination deck.
 *
 * @param deck_ptr_src The source deck to deal from.
 * @param deck_ptr_dest The destination deck to deal to.
 * @return The card that was dealt, or 0 if no card was dealt.
 */
card deal(deck_t *restrict const deck_ptr_src, deck_t *restrict const deck_ptr_dest);

// =============== STATIC FUNCTIONS ===============
// DO NOT DELETE

static inline bool card_is_ace(const card card) { return card_value(card) == ACE_VALUE; }

static inline bool card_is_king(const card card) { return card_value(card) == KING_VALUE; }

static inline bool cards_same_suit(const card card1, const card card2) { return card_suit(card1) == card_suit(card2); }

static inline bool cards_same_value(const card card1, const card card2) {
    return card_value(card1) == card_value(card2);
}

static inline bool cards_different_suit(const card card1, const card card2) {
    return card_suit(card1) != card_suit(card2);
}

static inline bool cards_different_color(const card card1, const card card2) {
    return card_color(card1) != card_color(card2);
}

static inline bool cards_same_color(const card card1, const card card2) {
    return card_color(card1) == card_color(card2);
}

static inline bool cards_is_one_less(const card card1, const card card2) {
    return card_value(card1) + 1 == card_value(card2);
}

static inline bool cards_is_one_more(const card card1, const card card2) {
    return card_value(card1) == card_value(card2) + 1;
}

static inline bool cards_are_adjacent(const card card1, const card card2) {
    return cards_is_one_less(card1, card2) || cards_is_one_more(card1, card2);
}
