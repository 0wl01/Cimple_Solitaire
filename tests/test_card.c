#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdlib.h>

// Include the header from your project's include directory
#include "../include/card.h"

/* Suite initialization and cleanup functions */
int init_suite_card(void) { return 0; }

int clean_suite_card(void) { return 0; }

/* --- TESTS --- */

void test_create_and_eliminate_deck(void) {
    Deck *d = create_deck(52);
    CU_ASSERT_PTR_NOT_NULL(d);
    CU_ASSERT_EQUAL(d->size, 52);
    CU_ASSERT_EQUAL(d->top, 0);
    CU_ASSERT_TRUE(IS_EMPTY(d));

    eliminate_deck(d);
}

void test_push_and_pop(void) {
    Deck *d = create_deck(2);
    Card c1 = {.values = {.flip = 0, .color = 1, .suit = 1, .value = 10}};
    Card c2 = {.values = {.flip = 1, .color = 0, .suit = 0, .value = 14}};

    // Test successful pushes
    CU_ASSERT_EQUAL(push(d, c1), 0);
    CU_ASSERT_EQUAL(d->top, 1);
    CU_ASSERT_FALSE(IS_EMPTY(d));

    CU_ASSERT_EQUAL(push(d, c2), 0);
    CU_ASSERT_EQUAL(d->top, 2);

    // Test pushing to a full deck
    Card c3 = {.card = 0xFF};
    CU_ASSERT_EQUAL(push(d, c3), (uint8_t)-1); // Should fail and return -1

    // Test pop (should pop c2 first, LIFO)
    Card popped = pop(d);
    CU_ASSERT_EQUAL(popped.card, c2.card);
    CU_ASSERT_EQUAL(popped.values.flip, 1);
    CU_ASSERT_EQUAL(d->top, 1);

    eliminate_deck(d);
}

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

    eliminate_deck(d);
}

void test_deal(void) {
    Deck *d1 = create_deck(10);
    Deck *d2 = create_deck(5);

    populate_deck(d1); // d1 has 10 cards now

    // Deal 3 cards from d1 to d2
    deal(d1, d2, 3);
    CU_ASSERT_EQUAL(d1->top, 7);
    CU_ASSERT_EQUAL(d2->top, 3);

    // The top card of d1 (index 9) should now be the bottom card of d2 (index
    // 0) because dealing flips the order (LIFO stack transfer) d1's original
    // index 9 was Spades (0), Value 12
    CU_ASSERT_EQUAL(d2->cards[0].values.suit, 0);
    CU_ASSERT_EQUAL(d2->cards[0].values.value, 12);

    // Attempt to deal 5 more cards (but d2 only has 2 spaces left)
    deal(d1, d2, 5);
    CU_ASSERT_EQUAL(d1->top, 5); // Only 2 cards should be removed
    CU_ASSERT_EQUAL(d2->top, 5); // d2 should be completely full

    eliminate_deck(d1);
    eliminate_deck(d2);
}

void test_top_card(void) {
    Deck *d = create_deck(5);

    // Test empty deck returns {0} safely
    Card empty_top = top_card(d);
    CU_ASSERT_EQUAL(empty_top.card, 0);

    // Test normal top card
    Card c1 = {.values = {.value = 7}};
    push(d, c1);

    Card top = top_card(d);
    CU_ASSERT_EQUAL(top.card, c1.card);
    CU_ASSERT_EQUAL(d->top, 1); // Ensure top_card doesn't pop it!

    eliminate_deck(d);
}

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

    eliminate_deck(d);
}

void test_flip_card(void) {
    Card c = {.values= {.flip = 0, .color = 1, .suit= 1, .value = 10}};
    CU_ASSERT_EQUAL(c.values.flip, 0); // initial state

    c = flip_card(c);
    CU_ASSERT_EQUAL(c.values.flip, 1); // card is now face down

    c = flip_card(c);
    CU_ASSERT_EQUAL(c.values.flip, 0); // card return to initial state
}

void test_flip_deal(void) {
    Deck *d1 = create_deck(5);
    Deck *d2 = create_deck(5);

    Card c = {.values = {.flip = 0, .color = 1, .suit = 1, .value = 10}}; // 2 cards facing up
    
    // cards insert in deck 1
    push(d1, c);
    push(d1, c);

    CU_ASSERT_EQUAL(d1->top, 2); // d1 now has 2 cards
    CU_ASSERT_EQUAL(d2->top, 0);

    flip_deal(d1, d2, 2); // move to d2 while fliping

    CU_ASSERT_EQUAL(d1->top, 0);
    CU_ASSERT_EQUAL(d2->top, 2); // d2 now has 2 cards and they should be flipped

    CU_ASSERT_EQUAL(d2->cards[0].values.flip, 1);
    CU_ASSERT_EQUAL(d2->cards[1].values.flip, 1);

    eliminate_deck(d1);
    eliminate_deck(d2);
}

void test_get_bigger_deck(void) {
    Deck *decks[3];
    decks[0] = create_deck(5);
    decks[1] = create_deck(5);
    decks[2] = create_deck(5);

    Card c = {.card = 0xFF}; // dummy card (11111111) just for size purposes

    // 2. Colocamos cartas em quantidades diferentes
    push(decks[0], c); // decks[0] with 1 card
    
    push(decks[1], c);
    push(decks[1], c);
    push(decks[1], c); // decks[1] with 3 cards
    
    push(decks[2], c);
    push(decks[2], c); // decks[2] with 2 cards

    Deck *biggest = get_bigger_deck(decks, 3);

    CU_ASSERT_PTR_EQUAL(biggest, decks[1]); // should return decks[1] which has 3 cards
    CU_ASSERT_EQUAL(biggest->top, 3);

    eliminate_deck(decks[0]);
    eliminate_deck(decks[1]);
    eliminate_deck(decks[2]);
}

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

    eliminate_deck(d);
}

/* --- MAIN TEST RUNNER --- */

int main(void) {
    CU_pSuite pSuite = NULL;

    // Initialize the CUnit test registry
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    // Add a suite to the registry
    pSuite = CU_add_suite("Card_Test_Suite", init_suite_card, clean_suite_card);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Add the tests to the suite
    if ((NULL == CU_add_test(pSuite, "test of create/eliminate deck",
                             test_create_and_eliminate_deck)) ||
        (NULL ==
         CU_add_test(pSuite, "test of push and pop", test_push_and_pop)) ||
        (NULL ==
         CU_add_test(pSuite, "test of populate_deck", test_populate_deck)) ||
        (NULL == CU_add_test(pSuite, "test of deal", test_deal)) ||
        (NULL == CU_add_test(pSuite, "test of top_card", test_top_card)) ||
        (NULL == CU_add_test(pSuite, "test of flip_card", test_flip_card)) ||
        (NULL == CU_add_test(pSuite, "test of flip_deal", test_flip_deal)) ||
        (NULL == CU_add_test(pSuite, "test of get_bigger_deck", test_get_bigger_deck)) ||
        (NULL == CU_add_test(pSuite, "test of shuffle_deck", test_shuffle_deck)) ||
        (NULL == CU_add_test(pSuite, "test of flip_all", test_flip_all))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Run all tests using the basic interface
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    int fails = CU_get_number_of_failures();

    CU_cleanup_registry();
    return fails > 0 ? 1 : 0; // Return non-zero if tests fail
}
