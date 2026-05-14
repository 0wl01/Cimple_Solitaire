#include "card.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Allocates a Deck with a flexible array member for cards. */
Deck *create_deck(const size size) {
    Deck *deck = malloc(sizeof(Deck) + (sizeof(card) * size));
    if (unlikely(deck == NULL))
        return NULL;
    deck->top = 0;
    deck->size = size;
    return deck;
}

// This is a basic push function to a stack.
// Returns the exit code 0 for sucess.
bool push(Deck *restrict deck, const card c) {
    assert(deck != NULL);
    if (unlikely(is_deck_full(deck)))
        return false;
    deck->cards[(deck->top++)] = c;
    return true;
}

// Fills a stack of cards with cards.
void populate_deck(Deck *restrict deck) {
    assert(deck != NULL);
    deck->top = 0;
    for (card s = 0; s < 4; ++s) {
        for (card v = 3; v <= 15; ++v) {
            if (!push(deck, make_card(s, v)))
                return;
        }
    }
}

// shuffles the deck array using the Fisher-yater shuffle.
void shuffle_deck(Deck *restrict deck) {
    assert(deck != NULL);
    for (size i = 1; i < deck->top; ++i) {
        size ran_num = arc4random_uniform(i + 1);
        // Swap cards
        card temp = deck->cards[i];
        deck->cards[i] = deck->cards[ran_num];
        deck->cards[ran_num] = temp;
    }
}

// Simple function that takes elements from a deck to another.
// It inverses position and only takes what is available
void deal(Deck *restrict d1, Deck *restrict d2, const size q, const bool flip) {
    assert(d1 != NULL && d2 != NULL);
    const size available = (d1->top < q) ? d1->top : q;
    const size space_left = d2->size - d2->top;
    size transfer_count = (available < space_left) ? available : space_left;

    while (transfer_count--) {
        card temp = d1->cards[--d1->top];
        temp ^= flip << 6;
        d2->cards[d2->top++] = temp;
    }
}

// flips all cards in a stack.
void flip_all(Deck *restrict d1) {
    assert(d1 != NULL);
    for (size i = 0; i < d1->top; ++i) {
        d1->cards[i] ^= 1 << 6;
    }
}

// Returns the deck with the highest 'top' value from the array.
Deck *get_bigger_deck(Deck *restrict decks[], const size n) {
    assert(decks != NULL && n > 0);
    Deck *biggest = *decks;
    for (size i = 1; i < n; ++i)

        if (biggest->top < decks[i]->top)
            biggest = decks[i];
    return biggest;
}

// Assuming pos starts at 0
// calculating src->top - pos twice because I can't use more than two returns.
bool split_deck(Deck *restrict src, Deck *restrict dest, const size pos) {
    assert(src != NULL && dest != NULL);
    const size cards_to_copy = src->top - pos;
    if (pos >= src->top || cards_to_copy > dest->size - dest->top)
        return false;
    memcpy(dest->cards + dest->top, src->cards + pos, sizeof(card) * (cards_to_copy));
    src->top -= cards_to_copy;
    dest->top += cards_to_copy;
    return true;
}

// given a position returns the card at the position
// non destructive
// assumes pos starts at 0


/**
 * @brief Runs a function through the whole deck, from top to bottom, testing if one card and the one above fits parameters.
 *
 * @param deck A pointer to a Deck.
 * @param start_pos Index from the bottom of the potential sequence.
 * @param end_pos Index from the top of the potential sequence.
 * @param pred funtion that return a bool value, such as is_one_less and one_less_same_suit.
 *
 * @see one_less_same_suit
 * @see is_one_less
 */
// basically runs a two cards function to a sequence of cards.
// a single card sequence always return true.
static inline bool all_pairs_match(Deck *restrict deck, size start_pos, const size end_pos, CardPairPredicate pred) {
    assert(deck != NULL && pred != NULL);
    if (end_pos < start_pos || start_pos >= deck->top || end_pos >= deck->top || start_pos == end_pos)
        return start_pos == end_pos;

    for (; start_pos < end_pos && pred(deck->cards[start_pos], deck->cards[start_pos + 1]); ++start_pos)
        ;
    return start_pos == end_pos;
}

/**
 * @brief Chekcs if two cards share the same suit.
 *
 * @param a A card...
 * @param b Another... card...
 */
static inline bool same_suit(const card a, const card b) { return cards_same_suit(a, b); }

/**
 * @brief Checks if the hierarchy order is correct (Kings > Queens > ... > Aces)
 * @param b card of bigger value.
 * @param a card of smaller value.
 */
static inline bool is_one_less(const card a, const card b) { return cards_is_one_less(a, b); }

/**
 * @brief Checks if two cards are of same suit and follows the stated hierarchy.
 *
 * @param b card of bigger value and of suit X.
 * @param a card of smaller value and of suit X.
 *
 * @see is_one_less
 * @see same_suit
 */
static inline bool one_less_same_suit(const card a, const card b) {
    return cards_same_suit(a, b) && cards_is_one_less(a, b);
}

// will check if a sequence of cards from start_pos to end_pos have equal suits
bool sequence_same_suit(Deck *restrict deck, const size start_pos, const size end_pos) {
    return all_pairs_match(deck, start_pos, end_pos, same_suit);
}

// basically checks if a sequence of cards is in decreasing order
bool sequence_is_decreasing(Deck *restrict deck, const size start_pos, const size end_pos) {
    return all_pairs_match(deck, start_pos, end_pos, is_one_less);
}

bool sequence_is_decreasing_hierarchy(Deck *restrict deck, const size start_pos, const size end_pos) {
    return all_pairs_match(deck, start_pos, end_pos, one_less_same_suit);
}

size sequence_length(const Deck *restrict deck, const size start_pos, CardPairPredicate pred) {
    assert(deck != NULL && pred != NULL);
    size len = 1;
    const size top = deck->top;
    const card *const cs = deck->cards;

    if (unlikely(start_pos >= top))
        return 0;
    while (start_pos + len < top && pred(cs[start_pos + len - 1], cs[start_pos + len])) {
        ++len;
    }

    return len;
}
