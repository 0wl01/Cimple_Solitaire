#ifndef CLI_H
#define CLI_H

#include "card.h"

/**
 * @brief Debug function to print a Card.
 *
 * @param card Receives a Card to print.
 *
 * @see Card
 */
void print_card(const Card card);

/**
 * @brief Debug function to print an entire Deck.
 *
 * @param deck Pointer to the Deck to be printed.
 *
 * @see Deck
 */
void print_deck(const Deck *deck);

void print_decks_columns(Deck *restrict decks[], uint8_t columns);

void print_end(const uint8_t win);

void print_golf_table(golf_state *table);

void print_prompt();

void print_help();

char get_input();

#endif