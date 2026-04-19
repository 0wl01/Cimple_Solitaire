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

void test_simon_invalid_bounds(void) {
    simon_state table;
    setup_blank_simon(&table);

    // Origin Z doesn't exist and is invalid
    Command bad_cmd = {.type = CMD_MOV, .src_col = 'Z', .index = 0, .dest_col = 'A'};
    int result = simon_handle_move(&table, bad_cmd);

    CU_ASSERT_EQUAL(result, 0); // Checks if the move handler returned 0
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
        {"test of Simon invalid bounds", test_simon_invalid_bounds}
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
