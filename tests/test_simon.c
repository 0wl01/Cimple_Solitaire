#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include "simon.h"
#include "card.h"

#include "simon.c"

/* Suite initialization and cleanup functions */
int init_suite_simon(void) { return 0; }

int clean_suite_simon(void) { return 0; }

/* --- Auxiliary Functions --- */

/**
 * @brief Helper: Creates an Empty Simon State suitable for testing.
 */
static void setup_blank_simon(simon_state *table) {
    setup_foundations(table);
    for (int i = 0; i < SIMON_COLUMNS; i++) {
        table->columns[i] = create_deck(SIMON_COLUMN_SIZE);
    }
}

/* --- TESTS --- */

/**
 * @brief Tests Memory Allocation for the Foundations of Simple Simon State
 */
void test_setup_simon_foundations(void) {
    simon_state table = {0}; // All NULL to avoid seg fault
    
    setup_foundations(&table);

    CU_ASSERT_EQUAL(table.foundations[0]->size, SIMON_FOUNDATION_SIZE);
    CU_ASSERT_EQUAL(table.foundations[0]->top, 0); 
    
    clean_simon_table(&table);
}

/**
 * @brief Tests Memory Allocation for the Colunms of Simple Simon State and their distribution
 */
void test_setup_simon_columns(void) {
    simon_state table = {0}; // All NULL
    
    setup_columns(&table);

    CU_ASSERT_EQUAL(table.columns[0]->size, SIMON_COLUMN_SIZE);
    CU_ASSERT_EQUAL(table.columns[0]->top, 8); // first column
    CU_ASSERT_EQUAL(table.columns[8]->top, 2); // nith
    CU_ASSERT_EQUAL(table.columns[9]->top, 1); // tenth

    clean_simon_table(&table);
}

/**
 * @brief Tests if a Completed Suit is Moved to an Empty Foundation
 */
void test_simon_victory_move(void) {
    simon_state table;
    setup_blank_simon(&table);
    // Creates a Complete Suit from King(15) to Ace(3) and pushes it to Colunm A
    for (uint8_t v = 15; v >= 3; v--) {
        Card c = {.values = {.flip = 0, .color = 0, .suit = SPADES, .value = v}};
        push(table.columns[0], c);
    }

    check_and_move_completed_suits(&table);

    CU_ASSERT_EQUAL(table.columns[0]->top, 0);
    CU_ASSERT_EQUAL(table.foundations[0]->top, 13);

    clean_simon_table(&table);
}

/**
 * @brief Tests a valid move between two columns.
 * Verifies that a card can be successfully moved from the source column
 * to the destination column when the game rules allow it.
 */
void test_simon_valid_move(void) {
    simon_state table;
    setup_blank_simon(&table);
    
    push(table.columns[0], (Card){.values = {.suit = HEARTS, .value = 6}});
    push(table.columns[1], (Card){.values = {.suit = SPADES, .value = 5}});

    Command cmd = {.type = CMD_MOV, .src_col = 'B', .index = 0, .dest_col = 'A'};
    simon_handle_move(&table, cmd);

    CU_ASSERT_EQUAL(table.columns[0]->top, 2); // Colunmn A has got to have 2 cards
    CU_ASSERT_EQUAL(table.columns[1]->top, 0);
    clean_simon_table(&table);
}

/**
 * @brief Tests the rejection of a move with invalid card values.
 * Verifies that the game logic prevents placing a card onto another
 * card if they do not follow the descending numerical order rule.
 */
void test_simon_invalid_value(void) {
    simon_state table;
    setup_blank_simon(&table);
    
    push(table.columns[0], (Card){.values = {.suit = HEARTS, .value = 6}}); // Column A (6)
    push(table.columns[1], (Card){.values = {.suit = CLUBS, .value = 7}});  // Column B (7)

    Command cmd = {.type = CMD_MOV, .src_col = 'B', .index = 0, .dest_col = 'A'};
    simon_handle_move(&table, cmd); // Tries to move a 7 over a 6
    // Both colunms need to be equal to confirm move was refued
    CU_ASSERT_EQUAL(table.columns[0]->top, 1);
    CU_ASSERT_EQUAL(table.columns[1]->top, 1);
    clean_simon_table(&table);
}

/**
 * @brief Tests the handling of out-of-bounds column commands.
 * Verifies that the move handler safely rejects column identifiers 
 * that do not exist on the board (e.g., column 'Z').
 */
void test_simon_invalid_bounds(void) {
    simon_state table;
    setup_blank_simon(&table);

    // Origin Z doesn't exist and is invalid
    Command bad_cmd = {.type = CMD_MOV, .src_col = 'Z', .index = 0, .dest_col = 'A'};
    int result = simon_handle_move(&table, bad_cmd);

    CU_ASSERT_EQUAL(result, 0); // Checks if the move handler returned 0
    clean_simon_table(&table);
}

/**
 * @brief Tests the victory condition when all foundations are full.
 * * Verifies that has_won() returns true when every foundation deck
 * contains exactly 13 cards (a complete suit).
 */
void test_simon_has_won_true(void) {
    simon_state table;
    setup_blank_simon(&table);
    
    // Simulate a win by filling all foundations with 13 cards
    for (int i = 0; i < SIMON_FOUNDATIONS; i++) {
        table.foundations[i]->top = 13; 
    }
    
    CU_ASSERT_TRUE(has_won(&table));
    
    clean_simon_table(&table);
}

/**
 * @brief Tests the victory condition when the game is not yet won.
 * * Verifies that has_won() returns false when the foundations are empty
 * or only partially filled.
 */
void test_simon_has_won_false(void) {
    simon_state table;
    setup_blank_simon(&table);
    
    // Empty foundations should not trigger a win
    CU_ASSERT_FALSE(has_won(&table));
    
    clean_simon_table(&table);
}

/**
 * @brief Tests if the game correctly identifies available moves.
 * * Verifies that has_play_left() returns true when there is at least 
 * one valid move on the table.
 */
void test_simon_has_play_left_true(void) {
    simon_state table;
    setup_blank_simon(&table);
    
    // Setup a valid move: 5 of Hearts can be moved onto a 6 of any suit
    push(table.columns[0], (Card){.values = {.suit = HEARTS, .value = 6}});
    push(table.columns[1], (Card){.values = {.suit = HEARTS, .value = 5}});
    
    CU_ASSERT_TRUE(has_play_left(&table)); 
    
    clean_simon_table(&table);
}

/**
 * @brief Tests if the game correctly identifies when no moves are left.
 * * Verifies that has_play_left() returns false when no valid moves 
 * can be made between the existing columns.
 */
void test_simon_has_play_left_false(void) {
    simon_state table;
    setup_blank_simon(&table);
    
    // Setup an invalid scenario: King (15) and Ace (3) cannot be stacked
    push(table.columns[0], (Card){.values = {.suit = SPADES, .value = 15}});
    push(table.columns[1], (Card){.values = {.suit = HEARTS, .value = 3}});
    
    CU_ASSERT_FALSE(has_play_left(&table)); 
    
    clean_simon_table(&table);
}

typedef struct {
    const char *name;
    CU_TestFunc fn;
} T;

static int add_simon_tests(CU_pSuite s) {
    T t[] = {
        {"test of Simon setup foundations", test_setup_simon_foundations},
        {"test of Simon setup columns and card distribution", test_setup_simon_columns},
        {"test of Simon victory move", test_simon_victory_move},
        {"test of Simon valid move", test_simon_valid_move},
        {"test of Simon invalid value", test_simon_invalid_value},
        {"test of Simon invalid bounds", test_simon_invalid_bounds},
        {"test of Simon win condition (true)", test_simon_has_won_true},
        {"test of Simon win condition (false)", test_simon_has_won_false},
        {"test of Simon play left (true)", test_simon_has_play_left_true},
        {"test of Simon play left (false)", test_simon_has_play_left_false}
    };

    for (size_t i = 0; i < sizeof(t) / sizeof(*t); i++)
        if (!CU_add_test(s, t[i].name, t[i].fn))
            return 0;

    return 1;
}

/* --- MAIN TEST RUNNER --- */
int main(void) {
    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    CU_pSuite s = CU_add_suite("Simon_Test_Suite", init_suite_simon, clean_suite_simon);
    if (!s || !add_simon_tests(s)) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    int fails = CU_get_number_of_failures();
    CU_cleanup_registry();
    return fails > 0;
}
