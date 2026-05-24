#include "card.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Allocates a Deck with a flexible array member for cards. */
Deck *create_deck(const card_count size) {
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
    for (card_count i = 1; i < deck->top; ++i) {
        card_count ran_num = arc4random_uniform(i + 1);
        // Swap cards
        card temp = deck->cards[i];
        deck->cards[i] = deck->cards[ran_num];
        deck->cards[ran_num] = temp;
    }
}

// Simple function that takes elements from a deck to another.
// It inverses position and only takes what is available
void deal(Deck *restrict d1, Deck *restrict d2, const card_count q, const bool flip) {
    assert(d1 != NULL && d2 != NULL);
    const card_count available = (d1->top < q) ? d1->top : q;
    const card_count space_left = d2->size - d2->top;
    card_count transfer_count = (available < space_left) ? available : space_left;

    while (transfer_count--) {
        card temp = d1->cards[--d1->top];
        temp ^= flip << 6;
        d2->cards[d2->top++] = temp;
    }
}

// flips all cards in a stack.
void flip_all(Deck *restrict d1) {
    assert(d1 != NULL);
    for (card_count i = 0; i < d1->top; ++i) {
        d1->cards[i] ^= 1 << 6;
    }
}

void unflip_all(Deck *restrict d1) {
    assert(d1 != NULL);
    for (card_count i = 0; i < d1->top; ++i)
        if (card_flipped(d1->cards[i]))
            flip_card(d1->cards[i]);
}

// Returns the deck with the highest 'top' value from the array.
Deck *get_bigger_deck(Deck *restrict decks[], const card_count n) {
    assert(decks != NULL && n > 0);
    Deck *biggest = *decks;
    for (card_count i = 1; i < n; ++i)

        if (biggest->top < decks[i]->top)
            biggest = decks[i];
    return biggest;
}

// Assuming pos starts at 0
// calculating src->top - pos twice because I can't use more than two returns.
bool split_deck(Deck *restrict src, Deck *restrict dest, const card_count pos) {
    assert(src != NULL && dest != NULL);
    const card_count cards_to_copy = src->top - pos;
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
 * @brief Runs a function through the whole deck, from top to bottom, testing if
 * one card and the one above fits parameters.
 *
 * @param deck A pointer to a Deck.
 * @param start_pos Index from the bottom of the potential sequence.
 * @param end_pos Index from the top of the potential sequence.
 * @param pred funtion that return a bool value, such as is_one_less and
 * one_less_same_suit.
 *
 * @see one_less_same_suit
 * @see is_one_less
 */
// basically runs a two cards function to a sequence of cards.
// a single card sequence always return true.
static inline bool all_pairs_match(const Deck *restrict deck, card_count start_pos, const card_count end_pos,
                                   CardPairPredicate pred) {
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

static inline bool is_one_more(const card a, const card b) { return cards_is_one_more(a, b); }

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

static inline bool is_same_color(const card a, const card b) { return cards_same_color(a, b); }

static inline bool is_alternating_color(const card a, const card b) { return !cards_same_color(a, b); }

static inline bool is_alternating_suit(const card a, const card b) { return !cards_same_suit(a, b); }

bool sequence_alternating_suit(const Deck *restrict deck, const card_count start_pos, const card_count end_pos) {
    return all_pairs_match(deck, start_pos, end_pos, is_alternating_suit);
}

bool sequence_alternating_color(const Deck *restrict deck, const card_count start_pos, const card_count end_pos) {
    return all_pairs_match(deck, start_pos, end_pos, is_alternating_color);
}

bool sequence_same_color(const Deck *restrict deck, const card_count start_pos, const card_count end_pos) {
    return all_pairs_match(deck, start_pos, end_pos, is_same_color);
}

// will check if a sequence of cards from start_pos to end_pos have equal suits
bool sequence_same_suit(const Deck *restrict deck, const card_count start_pos, const card_count end_pos) {
    return all_pairs_match(deck, start_pos, end_pos, same_suit);
}

// basically checks if a sequence of cards is in decreasing order
bool sequence_is_decreasing(const Deck *restrict deck, const card_count start_pos, const card_count end_pos) {
    return all_pairs_match(deck, start_pos, end_pos, is_one_less);
}

bool sequence_is_increasing(const Deck *restrict deck, const card_count start_pos, const card_count end_pos) {
    return all_pairs_match(deck, start_pos, end_pos, is_one_more);
}

bool sequence_is_decreasing_hierarchy(const Deck *restrict deck, const card_count start_pos, const card_count end_pos) {
    return all_pairs_match(deck, start_pos, end_pos, one_less_same_suit);
}

card_count sequence_length(const Deck *restrict deck, const card_count start_pos, CardPairPredicate pred) {

    assert(deck != NULL && pred != NULL);
    card_count len = 1;
    const card_count top = deck->top;
    const card *const cs = deck->cards;

    if (unlikely(start_pos >= top))
        return 0;
    while (start_pos + len < top && pred(cs[start_pos + len - 1], cs[start_pos + len])) {
        ++len;
    }

    return len;
}

Deck *clone_deck(const Deck *restrict sample) {
    auto target = create_deck(sample->size);
    if (!target)
        return nullptr;
    target->top = sample->top;
    memcpy(target->cards, sample->cards, sizeof(card) * target->top);
    return target;
}

Deck **clone_deckArray(Deck *sample[], size_t size) {
    Deck **target = malloc(size * sizeof(Deck *));
    if (unlikely(!target))
        return nullptr;
    for (size_t i = 0; i < size; ++i) {
        target[i] = clone_deck(sample[i]);
        if (unlikely(!target[i])) {
            for (size_t j = 0; j < i; ++j)
                eliminate_deck(&target[j]);
            free(target);
            return nullptr;
        }
    }
    return target;
}
