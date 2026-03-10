#include "card.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

void swap_cards(Card *c1, Card *c2) {
    Card tmp = *c1;
    *c1 = *c2;
    *c2 = tmp;
}

/*
    The card value works in a fun way.
    The msb is the flip bit if it is 1 the card is flipped.
    after that the 7th bit it the color bit, if it is 1 color it red.
    then we got the 2 bits used for the suit.
    The other 4 bits are used to calculate the value 3-15 (that makes it 1-13).
*/
void print_card(Card card) {
    switch (VALUE_INDEX(card)) {
    case 0:
        // A definir
        return;
    case 1:
        // A definir
        return;
    case 2:
        // A definir
        return;
    }

    if (IS_FLIPPED(card)) {
        printf("\U0001F0A0");
        return;
    }

    // TODO: Implement way to paint the card red
    // if (IS_RED(card));

    printf("%s", CARDS[SUIT_INDEX(card)][VALUE_INDEX(card - 3)]);
}

Deck *create_deck(uint8_t size) {
    Deck *deck = malloc(sizeof(Deck) + size);
    if (deck == NULL)
        return NULL;
    deck->top = 0;
    deck->size = size;
    return deck;
}

void eliminate_deck(Deck *deck) { free(deck); }

// if deck is empty it returns 0
Card pop(Deck *deck) {
    if (deck->top <= 0) {
        return 0;
    }
    return deck->cards[--deck->top];
}

uint8_t push(Deck *deck, Card card) {
    if (deck->top == deck->size) {
        // Deck is full
        return -1;
    }
    deck->cards[(deck->top)] = card;
    deck->top = deck->top + 1;
    return 0;
}

void print_deck(Deck *deck) {
    for (int8_t i = deck->top - 1; i >= 0; --i) {
        print_card(deck->cards[i]);
        printf(", ");
    }
    putchar('\n');
}

// Rewrite this later
void populate_deck(Deck *deck) {
    Card temp_card = 3;
    for (int i = 0; i < (deck->size); ++i) {
        if (VALUE_INDEX(temp_card) == 0)
            temp_card += 3;
        push(deck, temp_card++);
    }
}

void shuffle_deck(Deck *deck) {
    uint8_t random_int;
    for (int i = 1; i < deck->top; ++i) {
        arc4random_buf(&random_int, 1);
        random_int = random_int % (i + 1);
        swap_cards(deck->cards + i, deck->cards + random_int);
    }
}

// rewrite this using memcpy
void deal(Deck *d1, Deck *d2, uint8_t q) {
    Card temp;
    for (; q > 0 && d2->top < d2->size; --q) {
        temp = pop(d1);
        if (temp)
            push(d2, temp);
    }
}

Card top_card(Deck *d1) { return d1->top ? d1->cards[d1->top - 1] : 0; }

void flip_all(Deck *d1) {
    for (int8_t i = 0; i < d1->top; ++i) {
        d1->cards[i] = FLIP_CARD(d1->cards[i]);
    }
}
