#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdlib.h>

// Include the header from your project's include directory
#include "../src/golf.c"

/* Suite initialization and cleanup functions */
int init_suite_golf(void) { return 0; }

int clean_suite_golf(void) { return 0; }

/* --- TESTS --- */

/**
 * @brief Tests the logic for stacking cards based on the lookup table.
 */
void test_can_deal_rules(void) {
    Deck *d1 = create_deck(1);
    Deck *d2 = create_deck(1);
    
    // Can a 5 (value 5) go on top of a 4 (value 4)? (Yes, +1)
    d1->cards[0] = (Card){.values = {.value = 5}};
    d1->top = 1;
    d2->cards[0] = (Card){.values = {.value = 4}};
    d2->top = 1;
    CU_ASSERT_TRUE(can_deal(d1, d2));

    // Can a King (value 15) go on top of an Ace (value 3)? (Yes, cyclic rule)
    d1->cards[0] = (Card){.values = {.value = 15}};
    d2->cards[0] = (Card){.values = {.value = 3}};
    CU_ASSERT_TRUE(can_deal(d1, d2));

    // Can a 10 go on top of a 5? (No)
    d1->cards[0] = (Card){.values = {.value = 10}};
    d2->cards[0] = (Card){.values = {.value = 5}};
    CU_ASSERT_FALSE(can_deal(d1, d2));

    eliminate_deck(d1);
    eliminate_deck(d2);
}

/**
 * @brief Tests if the game correctly identifies when moves are still possible.
 */
void test_can_play_basic(void) {
    golf_state table;
    table.stock = create_deck(1);
    table.waste = create_deck(1);
    for(int i=0; i<GOLF_COLUMNS; i++) table.columns[i] = create_deck(1);

    // If there are cards in the stock, the player can always draw (can play)
    table.stock->top = 1;
    CU_ASSERT_TRUE(can_play(&table));

    // Limpeza
    eliminate_deck(table.stock);
    eliminate_deck(table.waste);
    for(int i=0; i<GOLF_COLUMNS; i++) eliminate_deck(table.columns[i]);
}

/* --- MAIN TEST RUNNER --- */

int main(void) {
    CU_pSuite pSuite = NULL;

    // Initialize the CUnit test registry
    if (CUE_SUCCESS != CU_initialize_registry()) {
        return CU_get_error();
    }

    // Add a suite to the registry
    pSuite = CU_add_suite("Golf_Test_Suite", init_suite_golf, clean_suite_golf);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // Add the tests to the suite
    if (
        (NULL == CU_add_test(pSuite, "test of can_deal rules", test_can_deal_rules)) ||
        (NULL == CU_add_test(pSuite, "test of can_play logic", test_can_play_basic))
    ) {
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