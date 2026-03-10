#include "card.h"
#include <stdint.h>

/*
 * This is a lookup table for stacking cards.
 * Each possible value card 0-15 is indexed here.
 * There are two bytes that each represent a card value.
 * A card can only be on top of a card here represented in its index.
 *
 * I'm using a lookup table because this is O(1) complexity.
 */
extern const uint8_t deal_lookup[16];

void init_columns(Deck *columns[]);
int8_t can_deal(Deck *d1, Deck *d2);
void flip_deal(Deck *d1, Deck *d2);
void start_game();
