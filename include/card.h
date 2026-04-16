#ifndef CARD_H
#define CARD_H

/**
 * @file 
 * @brief Definitions and macros for playing cards and decks.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Suit order constant
 */
typedef enum : uint8_t { SPADES = 0, HEARTS = 1, DIAMONDS = 2, CLUBS = 3 } Suit;

/**
 * @brief Represents a playing card.
 * @details The card value is packed into an 8-bit unsigned int.
 * Because of how a bit field works, it is not possible to strictly know which
 * position each value occupies across different compilers.
 */
typedef union {
    uint8_t card; /**< Raw byte repr of the card */
    struct {
        uint8_t flip : 1;  /**< This bit stores if the card is flipped down */
        uint8_t color : 1; /**< Used to check the card color (may be useless if suit order changes) */
        Suit suit : 2;     /**< 2 bits used to identify the 4 possible suits */
        uint8_t value : 4; /**< The value of the card (3-15). Values (0-2) aren't used. */
    } values;              /**< Bitfield struct for easy access of the card properties */
} Card;

/**
 * @brief A stack that represents a deck of cards.
 * @details This uses a flexible array member meaning you must allocate memory
 * using @ref create_deck().
 * @see create_deck()
 * @see Card
 */
typedef struct {
    size_t top;   /**< Index of the current top, that is, the current number of elements */
    size_t size;  /**< Max capacity of the stack; this is set at creation */
    Card cards[]; /**< The flexible array that contains all the cards */
} Deck;

// TODO
// needs documentations
// this is basically a pointer to a function that receives two cards
typedef bool (*CardPairPredicate)(const Card, const Card);

/**
 * @brief Checks whether a given Deck is empty.
 * @param deck Pointer to the Deck struct.
 * @return 1 if empty, or 0 if not empty.
 */
#define IS_EMPTY(deck) !((deck)->top)

/**
 * @brief Check whether a given deck is full.
 * @param deck Pointer to a Deck struct.
 * @return 1 if full 0 if not full.
 */
#define IS_FULL(deck) ((deck)->top == (deck)->size)

/**
 * @brief Creates a Pointer to a Deck struct allocating memory.
 *
 * The Deck created may only have at maximum size_t elements.
 *
 * @param size The size in bytes allocated to the Deck. That is the amount of cards that the deck supports. 
 * @return A pointer to a new empty Deck or NULL if allocation fails.
 *
 * @see Deck
 */
Deck *create_deck(const size_t size);

/**
 * @brief Free allocated memory for a Deck.
 *
 * @param deck A Pointer to a Deck pointer you want to free.
 *
 * @see Deck
 */
void eliminate_deck(Deck **deck);

/**
 * @brief Function to remove the last element of a Deck.
 *
 * This pops the top Card of a Deck by decreasing the top var.
 *
 * @param deck A Pointer to a Deck.
 * @return The Card removed if the deck is empty it return an empty card.
 *
 * @see Card
 * @see Deck
 * @see push()
 */
Card pop(Deck *restrict deck);

/**
 * @brief Inserts a Card in a Deck.
 *
 * This function will put a card in the top position of a Deck stack that isn't
 * full.
 *
 * @param deck Pointer to deck.
 * @param card A Card to insert in the deck.
 *
 * @return Returns 0 if successful.
 *
 * @see Deck
 * @see Card
 */
bool push(Deck *restrict deck, const Card card);

/**
 * @brief Fills a Deck with cards.
 *
 * Fills a Deck with cards in order (Spades, Hearts, Diamonds, Clubs) 1-13 (value 3-15).
 * It will fill the Deck till its max capacity. A deck with size 13 will only get the cards of spades.
 *
 * @param deck Pointer to a Deck.
 *
 * @see Deck
 */
void populate_deck(Deck *restrict deck);

/**
 * @brief Shuffle a Deck.
 *
 * Shuffles a Deck using the Fisher-Yates Shuffle algorithm.
 * Uses arc4random_uniform() to generate random numbers.
 *
 * @param deck Pointer to a Deck.
 *
 * @see Deck
 */
void shuffle_deck(Deck *restrict deck);

/**
 * @brief Deals cards from a Deck to another Deck.
 *
 * Deals q Cards from a Deck or every card from the Deck, whichever is smaller.
 * Will stop dealing if the destination is full.
 * Flipping an already flipped card will flip it face up.
 * * Dealing cards to itself will result in an error.
 *
 * @param d1 Pointer to the origin Deck (The one being taken cards from).
 * @param d2 Pointer to the destination Deck (The one receiving cards).
 * @param q The max quantity of cards to take .
 * @param flip If the card should be flipped or not.
 *
 * @see Deck
 */
void deal(Deck *restrict d1, Deck *restrict d2, const size_t q, const bool flip);

/**
 * @brief The top Card of a Deck.
 *
 * @param d1 Pointer to a Deck.
 *
 * @return The Card at the top of that Deck.
 *
 * @see Deck
 * @see Card
 */
Card top_card(const Deck *restrict d1);

/**
 * @brief Flips a Card.
 *
 * Flips the flip bit of a Card.
 *
 * @param c Card to flip.
 * @return The Card flipped.
 *
 * @see Card
 */
Card flip_card(Card c);

/**
 * @brief Flips all cards from a Deck.
 *
 * Flips all cards from a Deck but does not alter their positions.
 *
 * @param d1 Pointer to a Deck.
 *
 * @see flip_card()
 * @see Deck
 */
void flip_all(Deck *restrict d1);

/**
 * @brief Logic for finding the deck with the highest occupancy.
 * This implementation iterates through the provided array and compares
 * the 'top' field of each Deck.
 * @param decks Array of pointers to Deck structures.
 * @param n The number of decks to evaluate.
 *
 * @pre n > 0
 * @pre All elements decks[0...n] must be non NULL 
 * @return Pointer to the Deck with the highest number of cards.
 */
Deck *get_bigger_deck(Deck *restrict decks[], const size_t n);

/**
 * @brief Splits a deck from a position to the top to another deck.
 *
 * @param src Deck which the card will be taken from.
 * @param dest Deck which the cards will be placed on.
 * @param pos Position to start taking the cards from.
 * @return Returns true if possible and false if not possible.
 */
bool split_deck(Deck *restrict src, Deck *restrict dest, const size_t pos);

/**
 * @brief Gets a card in a given position of a Deck
 *
 * @param deck Pointer to a Deck.
 * @param pos size_t arg with the position of an element. 0 indexed.
 *
 * @return The card accessed. if the position is invalid returns the empty card.
 */
Card peek(Deck *restrict deck, const size_t pos);

// TODO
// needs documentation
bool same_suit(const Card a, const Card b);

/**
 * @brief Checks if a sequence of a certain number of cards are all the same suit 
 * @param deck The deck of cards checked,
 * @param start_pos Starting position of the sequence.
 * @param end_pos End position of the sequence.
 */
bool sequence_same_suit(Deck *restrict deck, const size_t start_pos, const size_t end_pos);

// TODO
// Needs docs
bool is_one_less(const Card a, const Card b);

// TODO
// Needs docs
bool sequence_is_decreasing(Deck *restrict deck, const size_t start_pos, const size_t end_pos);

// TODO
// Needs docs
bool sequence_is_decreasing_hierarchy(Deck *restrict deck, const size_t start_pos, const size_t end_pos);

// TODO
// Needs docs
bool one_less_same_suit(const Card a, const Card b);

#endif
