#include <stdint.h>
// search realloc
#define FALSE 0
#define TRUE 1

/*
    The card value works in a fun way.
    The msb is the flip bit if it is 1 the card is flipped.
    after that the 7th bit it the color bit, if it is 1 color it red.
    then we got the 2 bits used for the suit.
    The other 4 bits are used to calculate the value 3-15 (that makes it 1-13).
*/
typedef union {
    uint8_t card;
    struct {
        uint8_t flip : 1;
        uint8_t color : 1;
        uint8_t suite : 2;
        uint8_t value : 4;
    } values;
} Card;

typedef struct {
    uint8_t top;
    uint8_t size;
    Card cards[];
} Deck;

extern const char *const SUIT[];
extern const char *const CARDS[4][13];

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
