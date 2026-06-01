#include "card_engine.h"
#include "game_runner.h"
#include "paciencia_interpreter.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// ==================== TEST HELPERS ====================

static paciencia_game_t *get_mock_rules(const char *content) {
    const char *filename = "test_runner.paciencia";
    FILE *f = fopen(filename, "w");
    if (f) {
        fputs(content, f);
        fclose(f);
    }
    return scan_paciencia_game_file(filename);
}

static void cleanup_runner_test(void) { remove("test_runner.paciencia"); }

// ==================== TEST CASES ====================

static void test_init_and_free(void) {
    paciencia_game_t *r = get_mock_rules("INIT A 5\nINIT B 0\n");
    game_state_t *st = init_game_state(r);

    CU_ASSERT_PTR_NOT_NULL(st);
    if (st) {
        CU_ASSERT_EQUAL(st->deck_count, 2);
        CU_ASSERT_EQUAL(st->decks[0]->top, 5);
        CU_ASSERT_EQUAL(st->decks[1]->top, 0);
    }

    free_game_state(&st);
    free_game_resources(&r);
    CU_ASSERT_PTR_NULL(st);
}

static void test_move_validation(void) {
    paciencia_game_t *r = get_mock_rules("INIT A 1\nINIT B 0\nMOV A B *\n");
    game_state_t *st = init_game_state(r);

    if (st) {
        CU_ASSERT_TRUE(is_move_valid(st, 0, 1, 1));  // Valid move A to B
        CU_ASSERT_FALSE(is_move_valid(st, 1, 0, 1)); // Invalid B to A (no cards)
        CU_ASSERT_FALSE(is_move_valid(st, 0, 2, 1)); // Out of bounds destination
        free_game_state(&st);
    }

    free_game_resources(&r);
}

static void test_execute_move(void) {
    paciencia_game_t *r = get_mock_rules("INIT A 2\nINIT B 0\nMOV A B *\n");
    game_state_t *st = init_game_state(r);

    if (st) {
        CU_ASSERT_TRUE(execute_move(st, 0, 1, 1));
        CU_ASSERT_EQUAL(st->decks[0]->top, 1); // A loses a card
        CU_ASSERT_EQUAL(st->decks[1]->top, 1); // B gains a card
        free_game_state(&st);
    }

    free_game_resources(&r);
}

static void test_win_condition(void) {
    paciencia_game_t *r = get_mock_rules("INIT A 2\nWIN A 0\n");
    game_state_t *st = init_game_state(r);

    if (st) {
        CU_ASSERT_FALSE(check_win_condition_met(st));
        st->decks[0]->top = 0; // Manually trigger win by emptying deck
        CU_ASSERT_TRUE(check_win_condition_met(st));
        free_game_state(&st);
    }

    free_game_resources(&r);
}

static void test_auto_moves(void) {
    // A has 2 cards. B has 0. AUTO moves ANY (*) from A to B.
    paciencia_game_t *r = get_mock_rules("INIT A 2\nINIT B 0\nAUTO A B *\n");
    game_state_t *st = init_game_state(r);

    if (st) {
        execute_auto_moves(st);
        CU_ASSERT_EQUAL(st->decks[0]->top, 0); // All cards should move
        CU_ASSERT_EQUAL(st->decks[1]->top, 2); // B catches them
        free_game_state(&st);
    }

    free_game_resources(&r);
}

// ==================== REGISTRATION & MAIN ====================

typedef struct {
    const char *name;
    CU_TestFunc func;
} test_case_t;

static void register_tests(CU_pSuite suite) {
    test_case_t tests[] = {{"test_init_and_free", test_init_and_free},
                           {"test_move_validation", test_move_validation},
                           {"test_execute_move", test_execute_move},
                           {"test_win_condition", test_win_condition},
                           {"test_auto_moves", test_auto_moves}};
    size_t n = sizeof(tests) / sizeof(tests[0]);
    for (size_t i = 0; i < n; ++i) {
        CU_add_test(suite, tests[i].name, tests[i].func);
    }
}

int main(void) {
    bool init_fail = (CU_initialize_registry() != CUE_SUCCESS);
    if (init_fail) {
        return CU_get_error();
    }

    CU_pSuite suite = CU_add_suite("game_runner_suite", NULL, NULL);
    if (!suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    register_tests(suite);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    cleanup_runner_test();

    return CU_get_error();
}
