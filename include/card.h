#include <stdint.h>

#define FALSE 0
#define TRUE 1

typedef uint8_t Card;
typedef struct {
    uint8_t top;
    uint8_t size;
    Card cards[];
} Deck;

extern const char *const SUIT[];
extern const char *const CARDS[4][13];

#define SUIT_INDEX(card) ((card) >> 4) & 0x03
#define VALUE_INDEX(card) ((card) & 0x0F)
#define IS_FLIPPED(card) ((card) >> 7) & 0x01
#define IS_RED(card) ((card) >> 6) & 0x01

#define FLIP_CARD(card) (card) + 0x80

#define IS_EMPTY(deck) !((deck)->top)

Deck *create_deck(uint8_t size);
void eliminate_deck(Deck *deck);
Card pop(Deck *deck);
uint8_t push(Deck *deck, Card card);
void swap_cards(Card *c1, Card *c2);
void print_card(Card card);
void print_deck(Deck *deck);
void populate_deck(Deck *deck);
void shuffle_deck(Deck *deck);
void deal(Deck *d1, Deck *d2, uint8_t q);
Card top_card(Deck *d1);
void flip_all(Deck *d1);
