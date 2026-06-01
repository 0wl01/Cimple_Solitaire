#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dsl.h"

/* SUITE: DSL FILE PARSER */
static const char *dummy_file = "test_dummy_rules.paciencia";

/**
 * @brief Creates a temporary test rules file.
 * @details Generates a `.paciencia` file with standard rules and comments.
 * Uses a single fputs call to respect the 15-instruction limit.
 */
static void create_dummy_file(void) {
    FILE *f = fopen(dummy_file, "w");
    if (!f) return;

    /* A single function call to write the entire file */
    fputs("# This is a comment that should be ignored by the parser\n"
          "JOGO CimpleTest\n"
          "BARALHOS 2\n"
          "TIPO fundacao V\n"
          "TIPO coluna 1^\n"
          "INIT fundacao 0\n"
          "INIT coluna 7\n"
          "WIN fundacao 52\n"
          "MOV coluna fundacao <D\n"
          "AUTO coluna fundacao *\n", f);

    fclose(f);
}

/**
 * @brief Deletes the temporary file after tests are done.
 */
static void delete_dummy_file(void) {
    remove(dummy_file);
}

int init_suite_dsl(void) {
    create_dummy_file();
    return 0;
}

int clean_suite_dsl(void) {
    delete_dummy_file();
    return 0;
}

/* ==================================================
   THE TESTS
   ================================================== */

/**
 * @brief Tests if the parser can handle a non-existent file.
 * @details It should safely return NULL and not crash the program (segfault).
 */
void test_file_not_found(void) {
    game_cfg *cfg = scan_game_file("phantom_file_that_does_not_exist.paciencia");
    CU_ASSERT_PTR_NULL(cfg);
}

/**
 * @brief Tests if the block counts (number of types, rules, etc.) are correct.
 */
void test_parse_counts(void) {
    game_cfg *cfg = scan_game_file(dummy_file);
    
    CU_ASSERT_PTR_NOT_NULL(cfg);
    if (!cfg) return;

    CU_ASSERT_STRING_EQUAL(cfg->game_name, "CimpleTest");
    CU_ASSERT_EQUAL(cfg->bar, 2);
    CU_ASSERT_EQUAL(cfg->n_decks_types, 2);
    CU_ASSERT_EQUAL(cfg->n_instances, 2);
    CU_ASSERT_EQUAL(cfg->n_winconds, 1);
    CU_ASSERT_EQUAL(cfg->n_move_rules, 1);
    CU_ASSERT_EQUAL(cfg->n_auto_rules, 1);

    free_game_cfg(&cfg);
}

/**
 * @brief Tests if deck types and initializations were parsed correctly.
 */
void test_parse_declarations(void) {
    game_cfg *cfg = scan_game_file(dummy_file);
    
    CU_ASSERT_PTR_NOT_NULL(cfg);
    if (!cfg) return;

    /* Type reading */
    CU_ASSERT_STRING_EQUAL(cfg->deck_types[0].name, "fundacao");
    CU_ASSERT_STRING_EQUAL(cfg->deck_types[0].flags, "V");

    /* Init reading */
    CU_ASSERT_STRING_EQUAL(cfg->instances[1].deck_t, "coluna");
    CU_ASSERT_EQUAL(cfg->instances[1].n_cards, 7);

    free_game_cfg(&cfg);
}

/**
 * @brief Tests if game rules (win, move, auto) were parsed correctly.
 */
void test_parse_rules(void) {
    game_cfg *cfg = scan_game_file(dummy_file);
    
    CU_ASSERT_PTR_NOT_NULL(cfg);
    if (!cfg) return;

    /* Win condition reading */
    CU_ASSERT_STRING_EQUAL(cfg->conditions[0].deck_name, "fundacao");
    CU_ASSERT_EQUAL(cfg->conditions[0].n, 52);

    /* Move rule reading */
    CU_ASSERT_STRING_EQUAL(cfg->mov_rules[0].deck_src, "coluna");
    CU_ASSERT_STRING_EQUAL(cfg->mov_rules[0].deck_dst, "fundacao");
    CU_ASSERT_STRING_EQUAL(cfg->mov_rules[0].flags, "<D");

    /* Auto rule reading */
    CU_ASSERT_STRING_EQUAL(cfg->auto_rules[0].deck_src, "coluna");
    CU_ASSERT_STRING_EQUAL(cfg->auto_rules[0].deck_dst, "fundacao");
    CU_ASSERT_STRING_EQUAL(cfg->auto_rules[0].flags, "*");

    free_game_cfg(&cfg);
}

/* ==================================================
   MAIN RUNNER
   ================================================== */
typedef struct {
    const char *name;
    CU_TestFunc fn;
} T;

static int add_tests(CU_pSuite suite, T *tests, size_t count) {
    size_t i;
    for (i = 0; i < count; i++)
        if (!CU_add_test(suite, tests[i].name, tests[i].fn))
            return 0;
    return 1;
}

int main(void) {
    CU_pSuite s_dsl;
    int fails;

    if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();

    s_dsl = CU_add_suite("DSL_Parser_Suite", init_suite_dsl, clean_suite_dsl);
    
    T t_dsl[] = {
        {"test file not found handling", test_file_not_found},
        {"test correct block counts", test_parse_counts},
        {"test type and init declarations", test_parse_declarations},
        {"test game rules extraction", test_parse_rules}
    };

    if (!s_dsl || !add_tests(s_dsl, t_dsl, sizeof(t_dsl) / sizeof(t_dsl[0]))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    fails = CU_get_number_of_failures();
    CU_cleanup_registry();

    return fails > 0 ? 1 : 0;
}