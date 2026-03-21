#include "card.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

// Should I be using size_t in my loops?
// Search about realloc and check if applicable in this code.

/* Allocates a Deck with a flexible array member for cards. */
Deck *create_deck(uint8_t size) {
    Deck *deck = malloc(sizeof(Deck) + (sizeof(Card) * size));
    if (deck == NULL)
        return NULL;
    deck->top = 0;
    deck->size = size;
    return deck;
}

// A function to free memory of uneeded decks in the game.
void eliminate_deck(Deck *deck) { free(deck); }

// This is a basic pop function to a stack.
// It returns the card popped.
// if deck is empty it returns 0.
Card pop(Deck *deck) {
    assert(deck->top > 0);
    return deck->cards[--deck->top];
}

// This is a basic push function to a stack.
// Returns the exit code 0 for sucess.
uint8_t push(Deck *deck, const Card card) {
    if (deck->top == deck->size) {
        // Deck is full
        return -1;
    }
    deck->cards[(deck->top)] = card;
    deck->top = deck->top + 1;
    return 0;
}

// Fills a stack of cards with Cards.
// I feel like there is a better way to write this thing here.
void populate_deck(Deck *deck) {
    deck->top = 0;
    uint8_t is_full = 0; //control flag
    for (uint8_t s = 0; s < 4 && !is_full; ++s) {
        uint8_t is_red = (s == 1 || s == 2) ? 1 : 0;
        for (uint8_t v = 3; v <= 15 && !is_full; ++v) {
            if (deck->top >= deck->size) {
            is_full = 1;
            } else {
            deck->cards[deck->top++] = (Card){.values = {.flip = 0, .color = is_red, .suit = s, .value = v}};
            }
        }
    }
}

// shuffles the deck array using the Fisher-yater shuffle.
void shuffle_deck(Deck *deck) {
    for (uint8_t i = 1; i < deck->top; ++i) {
        uint8_t ran_num = arc4random_uniform(i + 1);
        // Swap cards
        Card temp = deck->cards[i];
        deck->cards[i] = deck->cards[ran_num];
        deck->cards[ran_num] = temp;
    }
}

// Simple function that takes elements from a deck to another.
// It inverses position and only takes what is available
// I still should probably use memcpy here
void deal(Deck *restrict d1, Deck *restrict d2, uint8_t q) {
    const uint8_t available = (d1->top < q) ? d1->top : q;
    const uint8_t space_left = d2->size - d2->top;
    uint8_t transfer_count = (available < space_left) ? available : space_left;

    while (transfer_count--) {
        d2->cards[d2->top++] = d1->cards[--d1->top];
    }
}

// A function that returns the top card of a card stack
// or returns 0 if the stack is empty.
Card top_card(Deck const *d1) { return d1->top ? d1->cards[d1->top - 1] : (Card){0}; }

// Flips a card by changing the flip bit.
Card flip_card(Card c) {
    c.values.flip ^= 1;
    return c;
}

// flips all cards in a stack.
void flip_all(Deck *d1) {
    Card *end = d1->cards + d1->top;
    for (Card *c = d1->cards; c < end; ++c) {
        c->values.flip ^= 1;
    }
}

// deal from d1 to d2 and flip the card
void flip_deal(Deck *restrict d1, Deck *restrict d2, uint8_t q) {
    const uint8_t available = (d1->top < q) ? d1->top : q;
    const uint8_t space_left = d2->size - d2->top;
    uint8_t transfer_count = (available < space_left) ? available : space_left;

    while (transfer_count--) {
        Card temp = d1->cards[--d1->top];
        temp.values.flip = !temp.values.flip;
        d2->cards[d2->top++] = temp;
    }
}

// Returns the deck with the highest 'top' value from the array.
Deck *get_bigger_deck(Deck *restrict decks[], int8_t n) {
    Deck *biggest = decks[0];
    for (--n; n >= 0; --n) {
        if (biggest->top < decks[n]->top)
            biggest = decks[n];
    }
    return biggest;
}
