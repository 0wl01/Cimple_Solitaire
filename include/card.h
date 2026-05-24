#ifndef CARD_H
#define CARD_H

/**
 * @file
 * @brief Definitions and macros for playing cards and decks.
 */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * @brief The biggest size any structure may have in this code.
 */
typedef uint_fast16_t card_count;

/**
 * @brief Suit order constant
 */
typedef enum { SPADES = 0, HEARTS = 1, CLUBS = 2, DIAMONDS = 3 } suit;

/**
 * @brief a default deck size
 */
constexpr uint8_t DEFAULT_DECK_SIZE = 52;
constexpr uint8_t MASK_VALUE = 0x0F;
constexpr uint8_t MASK_SUIT = 0x30;
constexpr uint8_t MASK_COLOR = 0x10;
constexpr uint8_t MASK_FLIPPED = 0x40;

constexpr uint8_t CARD_ACE  = 3;
constexpr uint8_t CARD_JACK = 13;
constexpr uint8_t CARD_QUEEN = 14;
constexpr uint8_t CARD_KING = 15;

/**
 * @brief Represents a playing card.
 * @details The card value is packed into an 8-bit unsigned int.
 */
typedef uint8_t card;

#define make_card(s, v) (((card)(s) << 4) | ((v) & MASK_VALUE))

#define card_value(c) ((c) & MASK_VALUE)
#define card_suit(c) (((c) & MASK_SUIT) >> 4)
#define card_color(c) (((c) & MASK_COLOR) >> 4)
#define card_flipped(c) (((c) & MASK_FLIPPED) >> 6)

/**
 * @brief Flips a card.
 *
 * Flips the flip bit of a card.
 *
 * @param c card to flip.
 * @return The card flipped.
 *
 * @see card
 */
#define flip_card(c) ((c) ^= 1 << 6)

#define cards_same_suit(c1, c2) ((((c1) & MASK_SUIT) >> 4) == (((c2) & MASK_SUIT) >> 4))

#define cards_is_one_less(c1, c2) (((c1) + 1) == (c2))
#define cards_one_less_same_suit(c1, c2) ((cards_is_one_less((c1), (c2))) && (cards_same_suit((c1),(c2))))
#define cards_different_colors(c1, c2) (card_color(c1) ^ card_color(c2))
#define cards_same_color(c1, c2) (card_color(c1) == card_color(c2))
#define cards_same_value(c1, c2) (card_value(c1) == card_value(c2))
#define cards_one_less_same_color(c1, c2) ((cards_is_one_less((c1), (c2))) && (cards_same_color((c1),(c2))))
#define cards_is_one_more(c1, c2) (((c2) + 1) == (c1))
#define cards_one_more_same_color(c1, c2) ((cards_is_one_more((c1), (c2))) && (cards_same_color((c1),(c2))))

/**
 * @brief A stack that represents a deck of cards.
 * @details This uses a flexible array member meaning you must allocate memory
 * using @ref create_deck().
 * @see create_deck()
 * @see card
 */
typedef struct {
    card_count top;  /**< Index of the current top, that is, the current number of
                      elements */
    card_count size; /**< Max capacity of the stack; this is set at creation */
    card cards[];    /**< The flexible array that contains all the cards */
} Deck;

// TODO docs
#define clear_deck(d) ((d)->top = 0)
#define deck_count(d) ((d)->top)
#define deck_top_card(d) ((d)->cards[(d)->top - 1])

/**
 * @brief Checks whether a given Deck is empty.
 * @param deck Pointer to the Deck struct.
 * @return 1 if empty, or 0 if not empty.
 */
#define is_deck_empty(deck) !((deck)->top)

/**
 * @brief Check whether a given deck is full.
 * @param deck Pointer to a Deck struct.
 * @return 1 if full 0 if not full.
 */
#define is_deck_full(deck) ((deck)->top == (deck)->size)

/**
 * @brief Typedef for a function pointer that compares two cards.
 * @param a The first card.
 * @param b The second card.
 * @return true if the cards meet the predicate's condition, false otherwise.
 */
typedef bool (*CardPairPredicate)(const card, const card);

/**
 * @brief Creates a Pointer to a Deck struct allocating memory.
 *
 * The Deck created may only have at maximum size elements.
 *
 * @param size The size in bytes allocated to the Deck. That is the amount of
 * cards that the deck supports.
 * @return A pointer to a new empty Deck or NULL if allocation fails.
 *
 * @see Deck
 */
Deck *create_deck(const card_count size);

/**
 * @brief Free allocated memory for a Deck.
 *
 * @param deck A Pointer to a Deck pointer you want to free.
 *
 * @see Deck
 */
static inline void eliminate_deck(Deck **deck) {
    free(*deck);
    *deck = nullptr;
}

/**
 * @brief Function to remove the last element of a Deck.
 *
 * This pops the top card of a Deck by decreasing the top var.
 *
 * @param deck A Pointer to a Deck.
 * @return The card removed if the deck is empty it return an empty card.
 *
 * @see card
 * @see Deck
 * @see push()
 */
static inline card pop(Deck *restrict deck) {
    assert(deck != NULL);
    return deck->top > 0 ? deck->cards[--deck->top] : (card){0};
}

/**
 * @brief Inserts a card in a Deck.
 *
 * This function will put a card in the top position of a Deck stack that isn't
 * full.
 *
 * @param deck Pointer to deck.
 * @param card A card to insert in the deck.
 *
 * @return Returns 0 if successful.
 *
 * @see Deck
 * @see card
 */
bool push(Deck *restrict deck, const card card);

/**
 * @brief Fills a Deck with cards.
 *
 * Fills a Deck with cards in order (Spades, Hearts, Diamonds, Clubs) 1-13
 * (value 3-15). It will fill the Deck till its max capacity. A deck with size
 * 13 will only get the cards of spades.
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
 * Deals q cards from a Deck or every card from the Deck, whichever is smaller.
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
void deal(Deck *restrict d1, Deck *restrict d2, const card_count q,
          const bool flip);

/**
 * @brief The top card of a Deck.
 *
 * @param d1 Pointer to a Deck.
 *
 * @return The card at the top of that Deck.
 *
 * @see Deck
 * @see card
 */
static inline card top_card(const Deck *restrict d1) {
    assert(d1 != NULL);
    return d1->top ? d1->cards[d1->top - 1] : (card){0};
}

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

//TODO: docs
void unflip_all(Deck *restrict d1);

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
Deck *get_bigger_deck(Deck *restrict decks[], const card_count n);

/**
 * @brief Splits a deck from a position to the top to another deck.
 *
 * @param src Deck which the card will be taken from.
 * @param dest Deck which the cards will be placed on.
 * @param pos Position to start taking the cards from.
 * @return Returns true if possible and false if not possible.
 */
bool split_deck(Deck *restrict src, Deck *restrict dest, const card_count pos);

/**
 * @brief Gets a card in a given position of a Deck
 *
 * @param deck Pointer to a Deck.
 * @param pos size arg with the position of an element. 0 indexed.
 *
 * @return The card accessed. if the position is invalid returns the empty card.
 */
static inline card peek(Deck *restrict deck, const card_count pos) {
    assert(deck != NULL);
    return pos >= deck->top ? (card){0} : deck->cards[pos];
}

/**
 * @brief Checks if a sequence of a certain number of cards are all the same
 * suit
 * @param deck The deck of cards checked,
 * @param start_pos Starting position of the sequence.
 * @param end_pos End position of the sequence.
 */
bool sequence_same_suit(const Deck *restrict deck, const card_count start_pos,
                        const card_count end_pos);

/**
 * @brief Given a deck and two indexes, chekcs if the cards follow the stated
 * hierarchy.
 *
 * @param deck Pointer to a Deck.
 * @param start_pos Index from the bottom of the potential sequence.
 * @param end_pos Index from the top of the potential sequence.
 *
 * @see is_one_less
 */
bool sequence_is_decreasing(const Deck *restrict deck, const card_count start_pos,
                            const card_count end_pos);

/**
 * @brief Given a deck and two indexes, chekcs if the cards follow the stated
 * hierarchy and are all of same suit.
 *
 * @param deck Pointer to a Deck.
 * @param start_pos Index from the bottom of the potential sequence.
 * @param end_pos Index from the top of the potential sequence.
 *
 * @see one_less_same_suit
 */
bool sequence_is_decreasing_hierarchy(const Deck *restrict deck,
                                      const card_count start_pos,
                                      const card_count end_pos);

card_count sequence_length(const Deck *restrict deck,
                           const card_count start_pos, CardPairPredicate pred);

Deck *clone_deck(const Deck *restrict sample);

bool sequence_is_increasing(const Deck *restrict deck, const card_count start_pos,
                            const card_count end_pos);
bool sequence_same_color(const Deck *restrict deck, const card_count start_pos,
                         const card_count end_pos);
bool sequence_alternating_color(const Deck *restrict deck, const card_count start_pos,
                                const card_count end_pos);
bool sequence_alternating_suit(const Deck *restrict deck, const card_count start_pos,
                               const card_count end_pos);


#endif
