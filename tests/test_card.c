#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stddef.h>
#include <stdlib.h>

// Include the header from your project's include directory
#include "card.h"

/* Suite initialization and cleanup functions */
int init_suite_card(void) { return 0; }

int clean_suite_card(void) { return 0; }

/* --- TESTS --- */

/**
 * @brief Tests the allocation and basic initialization of a Deck.
 * * Verifies if create_deck() returns a non-null pointer, sets the correct 
 * size, and starts with an empty stack (top = 0).
 * * @see create_deck()
 * @see eliminate_deck()
 */
void test_create_and_eliminate_deck() {
    Deck *d = create_deck(52);
    CU_ASSERT_PTR_NOT_NULL(d);
    CU_ASSERT_EQUAL(d->size, 52);
    CU_ASSERT_EQUAL(d->top, 0);
    CU_ASSERT_TRUE(IS_EMPTY(d));

    eliminate_deck(&d);

    CU_ASSERT_PTR_NULL(d);
}

/**
 * @brief Tests a standard push operation on a deck with available space.
 * * Ensures that cards are correctly added to the stack and that the 
 * 'top' index increments as expected.
 * * @see push()
 */
void test_push_normal() {
    Deck *d = create_deck(2);
    Card c1 = {.values = {.flip = 0, .color = 1, .suit = 1, .value = 10}};
    Card c2 = {.values = {.flip = 1, .color = 0, .suit = 0, .value = 14}};

    // Test successful pushes
    CU_ASSERT_TRUE(push(d, c1));
    CU_ASSERT_EQUAL(d->top, 1);
    CU_ASSERT_FALSE(IS_EMPTY(d));

    CU_ASSERT_TRUE(push(d, c2));
    CU_ASSERT_EQUAL(d->top, 2);

    eliminate_deck(&d);
}

/**
 * @brief Tests pushing a card to a deck that has reached its maximum capacity.
 * * Asserts that the function returns (uint8_t)-1 and that the deck's 
 * top index remains unchanged.
 * * @see push()
 */
void test_push_full(void) {
    Deck *d = create_deck(2);
    Card c1 = {.values = {.flip = 0, .color = 1, .suit = 1, .value = 10}};
    Card c2 = {.values = {.flip = 1, .color = 0, .suit = 0, .value = 14}};

    push(d, c1);
    push(d, c2); // deck is now full

    // Test pushing to a full deck
    Card c3 = {.card = 0xFF};
    CU_ASSERT_FALSE(push(d, c3));

    eliminate_deck(&d);
}

/**
 * @brief Tests the Last-In, First-Out (LIFO) behavior of the pop function.
 * * Ensures that the last card pushed is the first one retrieved and 
 * that the 'top' index decrements correctly.
 * * @see pop()
 */
void test_pop(void) {
    Deck *d = create_deck(2);
    Card c1 = {.values = {.flip = 0, .color = 1, .suit = 1, .value = 10}};
    Card c2 = {.values = {.flip = 1, .color = 0, .suit = 0, .value = 14}};

    push(d, c1);
    push(d, c2); // c2 last in and d->top = 2

    // Test pop (should pop c2 first, LIFO)
    Card popped = pop(d);
    CU_ASSERT_EQUAL(popped.card, c2.card);
    CU_ASSERT_EQUAL(popped.values.flip, 1);
    CU_ASSERT_EQUAL(d->top, 1);

    eliminate_deck(&d);
}

/**
 * @brief Validates the deck population logic and card value offsets.
 * * Checks if the deck contains 52 cards and verifies specific card 
 * properties (suit, color, value) at different positions to ensure 
 * the nested loops work correctly.
 * * @see populate_deck()
 */
void test_populate_deck(void) {
    Deck *d = create_deck(52);
    populate_deck(d);

    CU_ASSERT_EQUAL(d->top, 52);

    // Test first card (Should be Spades (0), Black (0), Value 3)
    CU_ASSERT_EQUAL(d->cards[0].values.suit, 0);
    CU_ASSERT_EQUAL(d->cards[0].values.color, 0);
    CU_ASSERT_EQUAL(d->cards[0].values.value, 3);
    CU_ASSERT_EQUAL(d->cards[0].values.flip, 0);

    // Test a middle card (Should be Hearts (1), Red (1), Value 10)
    CU_ASSERT_EQUAL(d->cards[22].values.suit, 1);
    CU_ASSERT_EQUAL(d->cards[22].values.color, 1);
    CU_ASSERT_EQUAL(d->cards[22].values.value, 12);

    // Test last card (Should be Clubs (3), Black (0), Value 15) -> index 51
    CU_ASSERT_EQUAL(d->cards[51].values.suit, 3);
    CU_ASSERT_EQUAL(d->cards[51].values.color, 0);
    CU_ASSERT_EQUAL(d->cards[51].values.value, 15);

    eliminate_deck(&d);
}

/**
 * @brief Tests a standard card transfer between two decks.
 * * Verifies that the correct number of cards is moved and that the 
 * LIFO order is maintained during the transfer.
 * * @see deal()
 */
void test_deal_normal(void) {
    Deck *d1 = create_deck(10);
    Deck *d2 = create_deck(5);

    populate_deck(d1); // d1 has 10 cards now

    // Deal 3 cards from d1 to d2
    deal(d1, d2, 3, false);
    CU_ASSERT_EQUAL(d1->top, 7);
    CU_ASSERT_EQUAL(d2->top, 3);

    // The top card of d1 (index 9) should now be the bottom card of d2 (index
    // 0) because dealing flips the order (LIFO stack transfer) d1's original
    // index 9 was Spades (0), Value 12
    CU_ASSERT_EQUAL(d2->cards[0].values.suit, 0);
    CU_ASSERT_EQUAL(d2->cards[0].values.value, 12);

    eliminate_deck(&d1);
    eliminate_deck(&d2);
}

/**
 * @brief Tests the deal function's behavior when the destination deck is full.
 * * Ensures that the function only transfers the amount of cards that fit 
 * in the destination, preventing memory corruption.
 * * @see deal()
 */
void test_deal_overflow(void) {
    Deck *d1 = create_deck(10);
    Deck *d2 = create_deck(2);

    populate_deck(d1);

    // Attempt to deal 5 cards (but d2 only has 2 spaces)
    deal(d1, d2, 5, false);
    CU_ASSERT_EQUAL(d1->top, 8); // Only 2 cards should be removed
    CU_ASSERT_EQUAL(d2->top, 2); // d2 should be completely full

    eliminate_deck(&d1);
    eliminate_deck(&d2);
}

/**
 * @brief Tests the safety of the top_card function on an empty deck.
 * * Confirms that calling top_card() on a deck with no elements returns 
 * a null-initialized Card structure instead of crashing.
 * * @see top_card()
 */
void test_top_card_empty(void) {
    Deck *d = create_deck(5);

    // Test empty deck returns {0} safely
    Card empty_top = top_card(d);
    CU_ASSERT_EQUAL(empty_top.card, 0);

    eliminate_deck(&d);
}

/**
 * @brief Tests retrieving the top card without removing it.
 * * Verifies that top_card() returns the correct data and that the 
 * deck's 'top' index remains unchanged.
 * * @see top_card()
 */
void test_top_card_normal(void) {
    Deck *d = create_deck(5);

    // Test normal top card
    Card c1 = {.values = {.value = 7}};
    push(d, c1);

    Card top = top_card(d);
    CU_ASSERT_EQUAL(top.card, c1.card);
    CU_ASSERT_EQUAL(d->top, 1); // Ensure top_card doesn't pop it!

    eliminate_deck(&d);
}

/**
 * @brief Tests the bulk flipping of all cards in a deck.
 * * Ensures that the flip bit is toggled for every card in the stack 
 * and can be toggled back.
 * * @see flip_all()
 */
void test_flip_all(void) {
    Deck *d = create_deck(3);
    populate_deck(d); // Contains 3 unflipped cards

    CU_ASSERT_EQUAL(d->cards[0].values.flip, 0);
    CU_ASSERT_EQUAL(d->cards[2].values.flip, 0);

    flip_all(d);

    CU_ASSERT_EQUAL(d->cards[0].values.flip, 1);
    CU_ASSERT_EQUAL(d->cards[2].values.flip, 1);

    // Test toggle off
    flip_all(d);
    CU_ASSERT_EQUAL(d->cards[0].values.flip, 0);

    eliminate_deck(&d);
}

/**
 * @brief Tests the individual card flipping logic.
 * * Verifies that the XOR operation on the flip bit correctly toggles 
 * the card's visibility state.
 * * @see flip_card()
 */
void test_flip_card(void) {
    Card c = {.values = {.flip = 0, .color = 1, .suit = 1, .value = 10}};

    c = flip_card(c);
    CU_ASSERT_EQUAL(c.values.flip, 1); // card is now face down

    c = flip_card(c);
    CU_ASSERT_EQUAL(c.values.flip, 0); // card return to initial state
}

/**
 * @brief Tests the combined functionality of dealing and flipping.
 * * Validates that cards are moved between decks and their 'flip' 
 * state is inverted in a single operation.
 * * @see flip_deal()
 */
void test_flip_deal(void) {
    Deck *d1 = create_deck(5);
    Deck *d2 = create_deck(5);

    Card c = {.values = {.flip = 0, .color = 1, .suit = 1, .value = 10}}; // 2 cards facing up

    // cards inserted in deck 1
    push(d1, c);
    push(d1, c);

    CU_ASSERT_EQUAL(d1->top, 2); // d1 now has 2 cards
    CU_ASSERT_EQUAL(d2->top, 0);

    deal(d1, d2, 2, true); // move to d2 while fliping

    CU_ASSERT_EQUAL(d1->top, 0);
    CU_ASSERT_EQUAL(d2->top, 2); // d2 now has 2 cards and they should be flipped

    CU_ASSERT_EQUAL(d2->cards[0].values.flip, 1);
    CU_ASSERT_EQUAL(d2->cards[1].values.flip, 1);

    eliminate_deck(&d1);
    eliminate_deck(&d2);
}

/**
 * @brief Helper function to create and populate 3 decks for testing.
 * @param d Array of 3 Deck pointers to be initialized.
 * @param c The card to push into the decks.
 */
static void mk(Deck *d[3], Card c) {
    for (size_t i = 0; i < 3; i++)
        for (d[i] = create_deck(3); d[i]->top <= i; push(d[i], c));
}

/**
 * @brief Helper function to safely eliminate an array of 3 decks.
 * @param d Array of 3 Deck pointers to be freed.
 */
static void rm(Deck *d[3]) {
    for (size_t i = 0; i < 3; eliminate_deck(&d[i++]));
}
/**
 * @brief Tests the logic for identifying the largest deck in a collection.
 * * Compares multiple decks with different card counts to ensure the 
 * function returns the pointer to the one with the highest occupancy.
 * * @see get_bigger_deck()
 */
void test_get_bigger_deck(void) {
    Deck *d[3];
    Card c = {.card = 0xFF};
    mk(d, c);
    CU_ASSERT_PTR_EQUAL(get_bigger_deck(d, 3), d[2]);
    CU_ASSERT_EQUAL(get_bigger_deck(d, 3)->top, 3);
    rm(d);
}

/**
 * @brief Verifies the randomness and integrity of the shuffle algorithm.
 * * Ensures that after a shuffle, the deck still contains 52 cards 
 * but in a different order than the initial state.
 * * @see shuffle_deck()
 */
void test_shuffle_deck(void) {
    Deck *d = create_deck(52);
    populate_deck(d); // Fills deck in order

    Card first_before = d->cards[0];
    Card last_before = d->cards[51];

    shuffle_deck(d);

    CU_ASSERT_EQUAL(d->top, 52); // Must keep all cards

    // Checks if the first or last card changed
    // False Positive very unlikely
    CU_ASSERT_TRUE(d->cards[0].card != first_before.card || d->cards[51].card != last_before.card);

    eliminate_deck(&d);
}

/**
 * @brief Tests the attempt to split a deck at an invalid position.
 * * @details Verifies that the `split_deck` function returns false and prevents 
 * the split when the provided index is greater than the current number 
 * of cards in the deck (above the `top`).
 */
void test_split_deck_invalid_position(void) {
    Deck *d1 = create_deck(5);
    Deck *d2 = create_deck(5);

    // Insert exactly 5 cards (values 3, 4, 5, 6, 7)
    for(uint8_t i = 3; i <= 7; i++) push(d1, (Card){.values = {.value = i}});

    // split attempt above top card
    CU_ASSERT_FALSE(split_deck(d1, d2, 6));

    eliminate_deck(&d1);
    eliminate_deck(&d2);
}

/**
 * @brief Tests the successful scenario of splitting a deck.
 * * @details Verifies that, given a valid index, the function correctly divides 
 * the cards between the two decks and updates their respective tops (`top`) 
 * to the expected sizes.
 */
void test_split_deck_valid(void) {
    Deck *d1 = create_deck(5);
    Deck *d2 = create_deck(5);

    for(uint8_t i = 3; i <= 7; i++) push(d1, (Card){.values = {.value = i}});

    CU_ASSERT_TRUE(split_deck(d1, d2, 2));
    CU_ASSERT_EQUAL(d1->top, 2);
    CU_ASSERT_EQUAL(d2->top, 3);

    eliminate_deck(&d1);
    eliminate_deck(&d2);
}

/**
 * @brief Tests the overflow protection on the destination deck.
 * * @details Verifies that the function fails safely when the number of cards 
 * to be moved exceeds the maximum allocated capacity of the destination deck.
 */
void test_split_deck_overflow(void) {
    Deck *d1 = create_deck(5);
    Deck *d2 = create_deck(1); // reduced capacity forces overflow

    for(uint8_t i = 3; i <= 7; i++) push(d1, (Card){.values = {.value = i}});

    // too many cards to d2
    CU_ASSERT_FALSE(split_deck(d1, d2, 2));

    eliminate_deck(&d1);
    eliminate_deck(&d2);
}

typedef struct {
    const char *name;
    CU_TestFunc fn;
} T;

static int add_all(CU_pSuite s) {
    T t[] = {{"test of create/eliminate deck", test_create_and_eliminate_deck},
             {"test of a normal push", test_push_normal},
             {"test of push on a full deck", test_push_full},
             {"test of push and pop", test_pop},
             {"test of populate_deck", test_populate_deck},
             {"test of a normal deal", test_deal_normal},
             {"test of a overflow deal", test_deal_overflow},
             {"test of top_card on empty deck", test_top_card_empty},
             {"test of top_card", test_top_card_normal},
             {"test of flip_card", test_flip_card},
             {"test of flip_deal", test_flip_deal},
             {"test of get_bigger_deck", test_get_bigger_deck},
             {"test of shuffle_deck", test_shuffle_deck},
             {"test of flip_all", test_flip_all},
             {"test of an invalid deck split", test_split_deck_invalid_position},
             {"test of an valid deck split", test_split_deck_valid},
             {"test of overflow protection", test_split_deck_overflow}
            };

    for (size_t i = 0; i < (sizeof(t) / sizeof(*t)); i++)
        if (!CU_add_test(s, t[i].name, t[i].fn))
            return 0;

    return 1;
}

int setup_card_suite(void) {
    CU_pSuite s = CU_add_suite("Card_Test_Suite", init_suite_card, clean_suite_card);
    return s && add_all(s);
}

/* --- MAIN TEST RUNNER --- */

int main(void) {
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    if (!setup_card_suite()) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    int fails = CU_get_number_of_failures();
    CU_cleanup_registry();

    return fails > 0 ? 1 : 0;
}
