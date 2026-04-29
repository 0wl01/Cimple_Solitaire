#include "card.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Allocates a Deck with a flexible array member for cards. */
Deck *create_deck(const size size) {
    Deck *deck = malloc(sizeof(Deck) + (sizeof(Card) * size));
    if (unlikely(deck == NULL))
        return NULL;
    deck->top = 0;
    deck->size = size;
    return deck;
}

// A function to free memory of uneeded decks in the game.
void eliminate_deck(Deck **deck) {
    free(*deck);
    *deck = NULL;
}

// This is a basic pop function to a stack.
// It returns the card popped.
Card pop(Deck *restrict deck) {
    assert(deck != NULL);
    return deck->top > 0 ? deck->cards[--deck->top] : (Card){0};
}

// This is a basic push function to a stack.
// Returns the exit code 0 for sucess.
bool push(Deck *deck, const Card card) {
    assert(deck != NULL);
    if (IS_FULL(deck)) {
        // Deck is full
        return false;
    }
    deck->cards[(deck->top++)] = card;
    return true;
}

// Fills a stack of cards with Cards.
void populate_deck(Deck *restrict deck) {
    assert(deck != NULL);
    deck->top = 0;
    for (uint8_t s = 0; s < 4; ++s) {
        uint8_t is_red = (s == DIAMONDS || s == HEARTS) ? 1 : 0;
        for (uint8_t v = 3; v <= 15; ++v) {
            if (deck->top >= deck->size)
                return;
            deck->cards[deck->top++] = (Card){.values = {.flip = 0, .color = is_red, .suit = s, .value = v}};
        }
    }
}

// shuffles the deck array using the Fisher-yater shuffle.
void shuffle_deck(Deck *restrict deck) {
    assert(deck != NULL);
    for (size i = 1; i < deck->top; ++i) {
        size ran_num = arc4random_uniform(i + 1);
        // Swap cards
        Card temp = deck->cards[i];
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
        Card temp = d1->cards[--d1->top];
        temp.values.flip ^= flip;
        d2->cards[d2->top++] = temp;
    }
}

// A function that returns the top card of a card stack
// or returns 0 if the stack is empty.
Card top_card(const Deck *restrict d1) {
    assert(d1 != NULL);
    return d1->top ? d1->cards[d1->top - 1] : (Card){0};
}

// Flips a card by changing the flip bit.
Card flip_card(Card c) {
    c.values.flip ^= 1;
    return c;
}

// flips all cards in a stack.
void flip_all(Deck *restrict d1) {
    assert(d1 != NULL);
    Card *end = d1->cards + d1->top;
    for (Card *c = d1->cards; c < end; ++c) {
        c->values.flip ^= 1;
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
    memcpy(dest->cards + dest->top, src->cards + pos, sizeof(Card) * (cards_to_copy));
    src->top -= cards_to_copy;
    dest->top += cards_to_copy;
    return true;
}

// given a position returns the card at the position
// non destructive
// assumes pos starts at 0
Card peek(Deck *restrict deck, const size pos) {
    assert(deck != NULL);
    return pos >= deck->top ? (Card){0} : deck->cards[pos];
}

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
inline static bool all_pairs_match(Deck *restrict deck, size start_pos, const size end_pos, CardPairPredicate pred) {
    assert(deck != NULL && pred != NULL);
    if (end_pos < start_pos || start_pos >= deck->top || end_pos >= deck->top || start_pos == end_pos)
        return start_pos == end_pos;

    for (; start_pos < end_pos && pred(deck->cards[start_pos], deck->cards[start_pos + 1]); ++start_pos)
        ;
    return start_pos == end_pos;
}

bool same_suit(const Card a, const Card b) { return a.values.suit == b.values.suit; }

// will check if a sequence of cards from start_pos to end_pos have equal suits
bool sequence_same_suit(Deck *restrict deck, const size start_pos, const size end_pos) {
    return all_pairs_match(deck, start_pos, end_pos, same_suit);
}

// The empty card 0 is always one less.
bool is_one_less(const Card a, const Card b) { return a.values.value == b.values.value + 1 || a.card == 0; }

// basically checks if a sequence of cards is in decreasing order
bool sequence_is_decreasing(Deck *restrict deck, const size start_pos, const size end_pos) {
    return all_pairs_match(deck, start_pos, end_pos, is_one_less);
}

bool one_less_same_suit(const Card a, const Card b) { return same_suit(a, b) && is_one_less(a, b); }

bool sequence_is_decreasing_hierarchy(Deck *restrict deck, const size start_pos, const size end_pos) {
    return all_pairs_match(deck, start_pos, end_pos, one_less_same_suit);
}
