#include "card.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Allocates a Deck with a flexible array member for cards. */
Deck *create_deck(const size_t size) {
    Deck *deck = malloc(sizeof(Deck) + (sizeof(Card) * size));
    if (deck == NULL)
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
Card pop(Deck *restrict deck) { return (deck && deck->top > 0) ? deck->cards[--deck->top] : (Card){0}; }

// This is a basic push function to a stack.
// Returns the exit code 0 for sucess.
bool push(Deck *deck, const Card card) {
    if (!deck || IS_FULL(deck)) {
        // Deck is full
        return false;
    }
    deck->cards[(deck->top++)] = card;
    return true;
}

// Fills a stack of cards with Cards.
void populate_deck(Deck *restrict deck) {
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
    for (size_t i = 1; i < deck->top; ++i) {
        size_t ran_num = arc4random_uniform(i + 1);
        // Swap cards
        Card temp = deck->cards[i];
        deck->cards[i] = deck->cards[ran_num];
        deck->cards[ran_num] = temp;
    }
}

// Simple function that takes elements from a deck to another.
// It inverses position and only takes what is available
void deal(Deck *restrict d1, Deck *restrict d2, const size_t q, const bool flip) {
    if (d1 == d2)
        return;
    const size_t available = (d1->top < q) ? d1->top : q;
    const size_t space_left = d2->size - d2->top;
    size_t transfer_count = (available < space_left) ? available : space_left;

    while (transfer_count--) {
        Card temp = d1->cards[--d1->top];
        temp.values.flip ^= flip;
        d2->cards[d2->top++] = temp;
    }
}

// A function that returns the top card of a card stack
// or returns 0 if the stack is empty.
Card top_card(const Deck *restrict d1) { return (d1 && d1->top) ? d1->cards[d1->top - 1] : (Card){0}; }

// Flips a card by changing the flip bit.
Card flip_card(Card c) {
    c.values.flip ^= 1;
    return c;
}

// flips all cards in a stack.
void flip_all(Deck *restrict d1) {
    Card *end = d1->cards + d1->top;
    for (Card *c = d1->cards; c < end; ++c) {
        c->values.flip ^= 1;
    }
}

// Returns the deck with the highest 'top' value from the array.
Deck *get_bigger_deck(Deck *restrict decks[], const size_t n) {
    if (!decks || n == 0)
        return NULL;
    Deck *biggest = *decks;
    for (size_t i = 1; i < n; ++i)
        if (biggest->top < decks[i]->top)
            biggest = decks[i];
    return biggest;
}

// Assuming pos starts at 0
// calculating src->top - pos twice because I can't use more than two returns.
bool split_deck(Deck *restrict src, Deck *restrict dest, const size_t pos) {
    if (pos >= src->top || src->top - pos > dest->size - dest->top)
        return false;
    const size_t cards_to_copy = src->top - pos;
    memcpy(dest->cards + dest->top, src->cards + pos, sizeof(Card) * (cards_to_copy));
    src->top -= cards_to_copy;
    dest->top += cards_to_copy;
    return true;
}

// given a position returns the card at the position
// non destructive
// assumes pos starts at 0
Card peek(Deck *restrict deck, const size_t pos) {
    if (!deck || pos >= deck->top)
        return (Card){0};
    return deck->cards[pos];
}

// TODO
// needs documentation
// basically runs a two cards function to a sequence of cards.
static bool all_pairs_match(Deck *restrict deck, size_t start_pos, const size_t end_pos, CardPairPredicate pred) {
    if (!deck || end_pos < start_pos || start_pos >= deck->top || end_pos >= deck->top || start_pos == end_pos)
        return start_pos == end_pos;

    for (; start_pos < end_pos && pred(deck->cards[start_pos], deck->cards[start_pos + 1]); ++start_pos)
        ;
    return start_pos > end_pos;
}

bool same_suit(const Card a, const Card b) { return a.values.suit == b.values.suit; }

// will check if a sequence of cards from start_pos to end_pos have equal suits
bool sequence_same_suit(Deck *restrict deck, const size_t start_pos, const size_t end_pos) {
    return all_pairs_match(deck, start_pos, end_pos, same_suit);
}

bool is_one_less(const Card a, const Card b) { return a.values.value == b.values.value + 1; }

// basically checks if a sequence of cards is in decreasing order
bool sequence_is_decreasing(Deck *restrict deck, const size_t start_pos, const size_t end_pos) {
    return all_pairs_match(deck, start_pos, end_pos, is_one_less);
}

bool one_less_same_suit(const Card a, const Card b) { return same_suit(a, b) && is_one_less(a, b); }

bool sequence_is_decreasing_hierarchy(Deck *restrict deck, const size_t start_pos, const size_t end_pos) {
    return all_pairs_match(deck, start_pos, end_pos, one_less_same_suit);
}
