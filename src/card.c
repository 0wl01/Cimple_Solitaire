#include "card.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// lookup tables for card symbols and suits
const char *const SUIT[] = {"♠", "♥", "♦", "♣"};
const char *const CARDS[4][13] = {
    {"🂡", "🂢", "🂣", "🂤", "🂥", "🂦", "🂧", "🂨", "🂩", "🂪", "🂫", "🂭",
     "🂮"}, // 0: Spades
    {"🂱", "🂲", "🂳", "🂴", "🂵", "🂶", "🂷", "🂸", "🂹", "🂺", "🂻", "🂽",
     "🂾"}, // 1: Hearts
    {"🃁", "🃂", "🃃", "🃄", "🃅", "🃆", "🃇", "🃈", "🃉", "🃊", "🃋", "🃍",
     "🃎"}, // 2: Diamonds
    {"🃑", "🃒", "🃓", "🃔", "🃕", "🃖", "🃗", "🃘", "🃙", "🃚", "🃛", "🃝",
     "🃞"} // 3: Clubs
};

Deck *create_deck(uint8_t size) {
    Deck *deck = malloc(sizeof(Deck) + (sizeof(Card) * size));
    if (deck == NULL)
        return NULL;
    deck->top = 0;
    deck->size = size;
    return deck;
}

void eliminate_deck(Deck *deck) { free(deck); }

// if deck is empty it returns 0
Card pop(Deck *deck) {
    assert(deck->top > 0);
    return deck->cards[--deck->top];
}

uint8_t push(Deck *deck, const Card card) {
    if (deck->top == deck->size) {
        // Deck is full
        return -1;
    }
    deck->cards[(deck->top)] = card;
    deck->top = deck->top + 1;
    return 0;
}

// I feel like there is a better way to write this thing here.
void populate_deck(Deck *deck) {
    deck->top = 0;
    for (uint8_t s = 0; s < 4; ++s) {
        uint8_t is_red = (s == 1 || s == 2) ? 1 : 0;
        for (uint8_t v = 3; v <= 15; ++v) {
            if (deck->top >= deck->size)
                return;
            deck->cards[deck->top++] = (Card){
                .values = {.flip = 0, .color = is_red, .suit = s, .value = v}};
        }
    }
}

void shuffle_deck(Deck *deck) {
    for (uint8_t i = 1; i < deck->top; ++i) {
        uint8_t ran_num = arc4random_uniform(i + 1);
        if (ran_num == i)
            continue;
        // Swap cards
        Card temp = deck->cards[i];
        deck->cards[i] = deck->cards[ran_num];
        deck->cards[ran_num] = temp;
    }
}

// This one here was rewritten by gemini and I need to take a look at it
// I still should probably use memcpy here
void deal(Deck *restrict d1, Deck *restrict d2, uint8_t q) {
    uint8_t available = (d1->top < q) ? d1->top : q;
    uint8_t space_left = d2->size - d2->top;
    uint8_t transfer_count = (available < space_left) ? available : space_left;

    while (transfer_count--) {
        d2->cards[d2->top++] = d1->cards[--d1->top];
    }
}

Card top_card(Deck const *d1) {
    return d1->top ? d1->cards[d1->top - 1] : (Card){0};
}

Card flip_card(Card c) {
    c.values.flip = c.values.flip ^ 1;
    return c;
}

void flip_all(Deck *d1) {
    for (int8_t i = 0; i < d1->top; ++i) {
        d1->cards[i].values.flip = !d1->cards[i].values.flip;
    }
}

// deal from d1 to d2 and flip the card
void flip_deal(Deck *restrict d1, Deck *restrict d2) {

    Card c = pop(d1);
    if (push(d2, flip_card(c)) == 0)
        return;
    // Deck d2 is full
    // Send card back
    push(d1, c);
}

void print_card(const Card c) {
    if (c.values.flip) {
        printf("\U0001F0A0");
        return;
    }
    const uint8_t val_idx = (c.values.value >= 3) ? c.values.value - 3 : 0;

    // TODO: Implement way to paint the card red
    // probably using ansi escape codes
    // if (IS_RED(card));

    printf("%s", CARDS[c.values.suit][val_idx]);
}

void print_deck(Deck const *deck) {
    for (int8_t i = deck->top - 1; i >= 0; --i) {
        printf("(%d: ", i);
        print_card(deck->cards[i]);
        printf("%d), ", deck->cards[i].values.value);
    }
    putchar('\n');
}
