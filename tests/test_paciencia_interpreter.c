#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "paciencia_interpreter.h"
#include "bitarr.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// ==================== TEST HELPERS ====================

// Helper to create a temporary DSL file for the scanner to read
static const char* create_temp_dsl(const char *content) {
    const char *filename = "temp_test.paciencia";
    FILE *f = fopen(filename, "w");
    if (f) {
        fputs(content, f);
        fclose(f);
    }
    return filename;
}

// ==================== TEST CASES ====================

static void test_invalid_file(void) {
    paciencia_game_t *game = scan_paciencia_game_file("does_not_exist.paciencia");
    CU_ASSERT_PTR_NULL(game);
}

static void test_dsl_counting(void) {
    const char *dsl = "INIT TAB 5\nINIT FUND 0\nWIN TAB 0\nMOV TAB FUND *\nAUTO TAB FUND *\n";
    const char *filename = create_temp_dsl(dsl);
    
    paciencia_game_t *game = scan_paciencia_game_file(filename);
    CU_ASSERT_PTR_NOT_NULL(game);
    
    if (game) {
        CU_ASSERT_EQUAL(game->decks_to_create, 2);
        CU_ASSERT_EQUAL(game->win_conditions_count, 1);
        CU_ASSERT_EQUAL(game->move_rules_count, 1);
        CU_ASSERT_EQUAL(game->auto_move_rules_count, 1);
        free_game_resources(&game);
    }
    remove(filename);
}

static void test_deck_metadata(void) {
    const char *dsl = "TIPO TAB 1=\nINIT TAB 5\n";
    const char *filename = create_temp_dsl(dsl);
    
    paciencia_game_t *game = scan_paciencia_game_file(filename);
    CU_ASSERT_PTR_NOT_NULL(game);
    
    if (game && game->decks_to_create > 0) {
        CU_ASSERT_STRING_EQUAL(game->deck_recipes[0].metadata, "1=");
        CU_ASSERT_STRING_EQUAL(game->deck_recipes[0].id, "TAB");
        free_game_resources(&game);
    }
    remove(filename);
}

static void test_move_flags_parsing(void) {
    const char *dsl = "MOV A B +[m<\n"; // Sequence, Descending, Same Suit, Less
    const char *filename = create_temp_dsl(dsl);
    
    paciencia_game_t *game = scan_paciencia_game_file(filename);
    CU_ASSERT_PTR_NOT_NULL(game);
    
    if (game && game->move_rules_count > 0) {
        bit_arr_t *f = game->move_rules[0].flags;
        CU_ASSERT_EQUAL(access_bit_arr(MOV_SEQUENCE, f), 1);
        CU_ASSERT_EQUAL(access_bit_arr(MOV_SEQ_DEC, f), 1);
        CU_ASSERT_EQUAL(access_bit_arr(MOV_SEQ_SUIT, f), 1);
        CU_ASSERT_EQUAL(access_bit_arr(MOV_TOP_LESS, f), 1);
        CU_ASSERT_EQUAL(access_bit_arr(MOV_ANY, f), 0);
        free_game_resources(&game);
    }
    remove(filename);
}

static void test_win_conditions(void) {
    const char *dsl = "WIN TAB 0\nWIN FUND 13\n";
    const char *filename = create_temp_dsl(dsl);
    
    paciencia_game_t *game = scan_paciencia_game_file(filename);
    CU_ASSERT_PTR_NOT_NULL(game);
    
    if (game && game->win_conditions_count == 2) {
        CU_ASSERT_STRING_EQUAL(game->win_conditions[0].id, "TAB");
        CU_ASSERT_EQUAL(game->win_conditions[0].card_count, 0);
        CU_ASSERT_STRING_EQUAL(game->win_conditions[1].id, "FUND");
        CU_ASSERT_EQUAL(game->win_conditions[1].card_count, 13);
        free_game_resources(&game);
    }
    remove(filename);
}

// ==================== REGISTRATION & MAIN ====================

typedef struct {
    const char *name;
    CU_TestFunc func;
} test_case_t;

static void register_tests(CU_pSuite suite) {
    test_case_t tests[] = {
        {"test_invalid_file", test_invalid_file},
        {"test_dsl_counting", test_dsl_counting},
        {"test_deck_metadata", test_deck_metadata},
        {"test_move_flags_parsing", test_move_flags_parsing},
        {"test_win_conditions", test_win_conditions}
    };
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
    
    CU_pSuite suite = CU_add_suite("dsl_interpreter_suite", NULL, NULL);
    if (!suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    register_tests(suite);
    
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    
    return CU_get_error();
}