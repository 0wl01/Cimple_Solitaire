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
void eliminate_deck(Deck *restrict deck) { free(deck); }

// This is a basic pop function to a stack.
// It returns the card popped.
Card pop(Deck *restrict deck) { return (deck && deck->top > 0) ? deck->cards[--deck->top] : (Card){0}; }

// This is a basic push function to a stack.
// Returns the exit code 0 for sucess.
bool push(Deck *deck, const Card card) {
    if (deck || deck->top == deck->size) {
        // Deck is full
        return false;
    }
    deck->cards[(deck->top++)] = card;
    return true;
}

// Fills a stack of cards with Cards.
// I feel like there is a better way to write this thing here.
void populate_deck(Deck *restrict deck) {
    deck->top = 0;
    for (uint8_t s = 0; s < 4; ++s) {
        uint8_t is_red = (s == 1 || s == 2) ? 1 : 0;
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

// will check if a sequence of cards from start_pos to end_pos have equal suits
bool same_suit(Deck *restrict deck, size_t start_pos, const size_t end_pos) {
    if (!deck || end_pos < start_pos || start_pos >= deck->top || end_pos >= deck->top)
        return false;
    Suit first_suit = deck->cards[start_pos].values.suit;
    // reusing start_pos as an index to avoid creating a new variable
    for (++start_pos; start_pos <= end_pos && deck->cards[start_pos].values.suit == first_suit; ++start_pos)
        ;
    return start_pos > end_pos;
}

// basically checks if a sequence of cards is in decreasing order
bool is_decreasing(Deck *restrict deck, size_t start_pos, const size_t end_pos) {
    if (!deck || end_pos < start_pos || start_pos >= deck->top || end_pos >= deck->top)
        return false;
    uint8_t last_value = deck->cards[start_pos].values.value;

    for (++start_pos; start_pos <= end_pos && last_value - 1 == deck->cards[start_pos].values.value;
         last_value = deck->cards[start_pos++].values.value) {
    }

    return start_pos > end_pos;
}
