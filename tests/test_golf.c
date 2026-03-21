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
 * 
 * Verifies that a card can be successfully placed on top of another
 * if their values are adjacent according to the rules (e.g., a 3 on a 2).
 * Asserts that can_deal() returns true (1) for this move.
 *
 * @see can_deal()
 */

void test_can_deal_basic(void) {
    Deck *d1 = create_deck(1);
    Deck *d2 = create_deck(1);
    
    // Can a 3 (value 5) go on top of a 2 (value 4)? (Yes, +1)
    d1->cards[0] = (Card){.values = {.value = 5}};
    d1->top = 1;
    d2->cards[0] = (Card){.values = {.value = 4}};
    d2->top = 1;
    CU_ASSERT_TRUE(can_deal(d1, d2));

    eliminate_deck(d1);
    eliminate_deck(d2);
}

/**
 * @brief Tests the cyclic wrapping rule for stacking cards.
 *
 * Verifies the edge case where a King (value 15) can be placed on 
 * an Ace (value 3), or vice versa. Asserts that can_deal() correctly
 * validates this wrap-around logic using the lookup table.
 *
 * @see can_deal()
 */

void test_can_deal_cyclic(void) {
    Deck *d1 = create_deck(1);
    Deck *d2 = create_deck(1);

    // Can a King (value 15) go on top of an Ace (value 3)? (Yes, cyclic rule)
    d1->cards[0] = (Card){.values = {.value = 15}};
    d1->top = 1;
    d2->cards[0] = (Card){.values = {.value = 3}};
    d2->top = 1;
    CU_ASSERT_TRUE(can_deal(d1, d2));

    eliminate_deck(d1);
    eliminate_deck(d2);
}

/**
 * @brief Tests the rejection of an invalid card move.
 *
 * Simulates an attempt to stack two non-adjacent cards (e.g., a 8 on a 3).
 * Asserts that can_deal() successfully blocks the move and returns false (0).
 *
 * @see can_deal()
 */

void test_cant_deal(void) {
    Deck *d1 = create_deck(1);
    Deck *d2 = create_deck(1);
    // Can a 8 go on top of a 3? (No)
    d1->cards[0] = (Card){.values = {.value = 10}};
    d1->top = 1;
    d2->cards[0] = (Card){.values = {.value = 5}};
    d2->top = 1;
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

    eliminate_deck(table.stock);
    eliminate_deck(table.waste);
    for(int i=0; i<GOLF_COLUMNS; i++) eliminate_deck(table.columns[i]);
}

/**
 * @brief Tests the buy function with a valid card move.
 * 
 * Simulates a valid move by forcing a card of value 5 into a column
 * and a card of value 4 into the waste pile. Asserts that the buy()
 * function correctly transfers the card from the column to the waste.
 * 
 * @see buy()
 */

void test_buy_valid_move(void) {
    Deck *col = create_deck(1); // cria memoria para 1 carta
    Deck *waste = create_deck(2); // cria memoria para 2 cartas
    
    col->cards[0] = (Card){.values = {.value = 5}}; // enfia à força 1 carta de valor 5 para col
    col->top = 1; // atualiza o top de col que agora tem 1 carta
    waste->cards[0] = (Card){.values = {.value = 4}}; // enfia à força 1 carta no waste
    waste->top = 1; // atualiza o top de waste que tem 1 carta tambem

    buy(col, waste); // carta vai de col para waste

    CU_ASSERT_EQUAL(col->top, 0); // col should be empty now
    CU_ASSERT_EQUAL(waste->top, 2); // waste has two

    eliminate_deck(col);
    eliminate_deck(waste);
}

/**
 * @brief Tests the game over state (no stock, no valid column moves).
 */

 void test_can_play_game_over(void) {
    golf_state table;
    table.stock = create_deck(0); // Empty Stock
    table.waste = create_deck(1);
    init_columns(table.columns);

    table.waste->cards[0] = (Card){.values = {.value = 5}}; // Waste has a 3 card
    table.waste->top = 1;

    for(int i=0; i<GOLF_COLUMNS; i++) {
        table.columns[i]->cards[0] = (Card){.values = {.value = 13}}; // Every column has a Valet now
        table.columns[i]->top = 1;
    }

    CU_ASSERT_FALSE(can_play(&table)); // Game over, no move possible

    clean_golf(&table);
}

/**
 * @brief Tests if the game continues when stock is empty but a column has a valid move.
 */

void test_can_play_column_move(void) {
    golf_state table;
    table.stock = create_deck(0); // Empty Stock
    table.waste = create_deck(1);
    init_columns(table.columns);

    // Lixo tem um 5
    table.waste->cards[0] = (Card){.values = {.value = 5}}; // Waste receives a 3 Card
    table.waste->top = 1;

    // Coluna 0 tem um 4 (jogada válida num 5!)
    table.columns[0]->cards[0] = (Card){.values = {.value = 4}}; // First Colunm receives a 2 Card
    table.columns[0]->top = 1;

    CU_ASSERT_TRUE(can_play(&table)); // 1 move is still possible!

    clean_golf(&table);
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
        (NULL == CU_add_test(pSuite, "test of can_deal basic", test_can_deal_basic)) ||
        (NULL == CU_add_test(pSuite, "test of can_deal cyclic", test_can_deal_cyclic)) ||
        (NULL == CU_add_test(pSuite, "test of cant_deal", test_cant_deal)) ||
        (NULL == CU_add_test(pSuite, "test of can_play logic", test_can_play_basic)) ||
        (NULL == CU_add_test(pSuite, "test of buy balid move", test_buy_valid_move)) ||
        (NULL == CU_add_test(pSuite, "test of game over", test_can_play_game_over)) ||
        (NULL == CU_add_test(pSuite, "test of column move", test_can_play_column_move))
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