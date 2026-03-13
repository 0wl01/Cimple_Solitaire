/**
 * @file card.h
 * @brief Definitions and macros for playing cards and decks.
 */

#include <stdint.h>

/** @brief Boolean false value */
#define FALSE 0
/** @brief Boolean true value */
#define TRUE 1

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
        uint8_t suit : 2;  /**< 2 bits used to identify the 4 possible suits */
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
    uint8_t top;  /**< Index of the current top, that is, the current number of elements */
    uint8_t size; /**< Max capacity of the stack; this is set at creation */
    Card cards[]; /**< The flexible array that contains all the cards */
} Deck;

/** @brief Lookup table for the suits symbols */
extern const char *const SUIT[];
/** @brief Lookup table for all playing cards symbols */
extern const char *const CARDS[4][13];

/**
 * @brief Checks whether a given Deck is empty.
 * @param deck Pointer to the Deck struct.
 * @return 1 if empty, or 0 if not empty.
 */
#define IS_EMPTY(deck) !((deck)->top)

/**
 * @brief Creates a Pointer to a Deck struct allocating memory.
 *
 * The Deck created may only have at maximum 255 elements.
 *
 * @param size The size in bytes allocated to the Deck. Each byte supports one
 * Card.
 * @return A pointer to a new empty Deck or NULL if allocation fails.
 *
 * @see Deck
 */
Deck *create_deck(const uint8_t size);

/**
 * @brief Free allocated memory for a Deck.
 *
 * @param deck A Pointer to a Deck you want to free.
 *
 * @see Deck
 */
void eliminate_deck(Deck *deck);

/**
 * @brief Function to remove the last element of a Deck.
 *
 * This pops the top Card of a Deck by decreasing the top var.
 *
 * @param deck A Pointer to a Deck.
 * @return The Card removed.
 *
 * @see Card
 * @see Deck
 * @see push()
 */
Card pop(Deck *deck);

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
uint8_t push(Deck *deck, const Card card);

/**
 * @brief Fills a Deck with cards.
 *
 * Fills a Deck with cards in order (Spades, Hearts, Diamonds, Clubs) 1-13.
 * It will fill the Deck till its max capacity.
 *
 * @param deck Pointer to a Deck.
 *
 * @see Deck
 */
void populate_deck(Deck *deck);

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
void shuffle_deck(Deck *deck);

/**
 * @brief Deals cards from a Deck to another Deck.
 *
 * Deals q Cards from a Deck or every card from the Deck, whichever is smaller.
 * Will stop dealing if the destination is full.
 * * Dealing cards to itself will result in an error.
 *
 * @param d1 Pointer to the origin Deck (The one being taken cards from).
 * @param d2 Pointer to the destination Deck (The one receiving cards).
 * @param q The max quantity of cards to take (MAX: 255).
 *
 * @see Deck
 */
void deal(Deck *restrict d1, Deck *restrict d2, uint8_t q);

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
Card top_card(Deck const *d1);

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
void flip_all(Deck *d1);

/**
 * @brief Deals cards and flips them.
 *
 * Combines functionality of @ref flip_card() and @ref deal().
 *
 * @param d1 Pointer to origin Deck.
 * @param d2 Pointer to destiny Deck.
 * @param q Max quantity of cards to deal (MAX: 255)
 *
 * @see deal()
 * @see flip_card()
 */
void flip_deal(Deck *restrict d1, Deck *restrict d2, uint8_t q);

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
