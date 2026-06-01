#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "registry.h"
#include "dsl.h"

/* SUITE: REGISTRY BUILDER */

static const char *dummy_file = "test_registry.paciencia";

/**
 * @brief Creates a temporary test rules file to generate a configuration.
 * @details Uses a single fputs call to respect the 15-instruction limit.
 */
static void create_reg_dummy_file(void) {
    FILE *f = fopen(dummy_file, "w");
    if (!f) return;

    /* A simple setup with 1 deck, 2 tableaus (3 cards each) and 1 foundation */
    fputs("JOGO RegistryTest\n"
          "BARALHOS 1\n"
          "TIPO tab =\n"
          "TIPO fund V\n"
          "INIT tab 3\n"
          "INIT tab 3\n"
          "INIT fund 0\n", f);

    fclose(f);
}

/**
 * @brief Deletes the temporary file after tests are done.
 */
static void delete_reg_dummy_file(void) {
    remove(dummy_file);
}

int init_suite_registry(void) {
    create_reg_dummy_file();
    return 0;
}

int clean_suite_registry(void) {
    delete_reg_dummy_file();
    return 0;
}

/**
 * @brief Tests if the registry correctly allocates memory and builds decks.
 * @details Checks if the number of allocated entries matches the INIT commands,
 * and if the decks have the expected initial sizes.
 */
void test_build_registry(void) {
    game_cfg *cfg = scan_game_file(dummy_file);
    deck_registry *reg;

    CU_ASSERT_PTR_NOT_NULL_FATAL(cfg);

    reg = build_registry(cfg);
    CU_ASSERT_PTR_NOT_NULL_FATAL(reg);

    /* We had 3 INIT commands, so we must have 3 entries */
    CU_ASSERT_EQUAL(reg->n_entries, 3);

    /* First entry is 'tab' with 3 cards */
    CU_ASSERT_STRING_EQUAL(reg->entries[0].name, "tab");
    CU_ASSERT_EQUAL(reg->entries[0].deck->top, 3);

    /* Third entry is 'fund' with 0 cards */
    CU_ASSERT_STRING_EQUAL(reg->entries[2].name, "fund");
    CU_ASSERT_EQUAL(reg->entries[2].deck->top, 0);

    free_registry(&reg);
    free_game_cfg(&cfg);
}

/**
 * @brief Tests if the registry internal logic (find_type_flags/populate) works.
 * @details Instead of searching for the deck, we check if the built registry
 * holds the correct properties and card counts assigned by the new internal logic.
 */
void test_registry_internal_logic(void) {
    game_cfg *cfg = scan_game_file(dummy_file);
    deck_registry *reg = build_registry(cfg);
    
    CU_ASSERT_PTR_NOT_NULL_FATAL(reg);

    /* 1. Check if 'tab' has the correct flags assigned by find_type_flags */
    /* Note: In our dummy_file, 'TIPO tab =' means flag '=' */
    CU_ASSERT_STRING_EQUAL(reg->entries[0].flags, "=");

    /* 2. Check if the initial cards were populated (populate_entry) */
    /* We expect 3 cards in 'tab' as per our INIT command */
    CU_ASSERT_EQUAL(reg->entries[0].deck->top, 3);

    free_registry(&reg);
    free_game_cfg(&cfg);
}

/**
 * @brief Tests if memory freeing handles edge cases safely.
 */
void test_free_registry_null(void) {
    deck_registry *reg = NULL;
    
    /* Should not segfault */
    free_registry(&reg); 
    CU_ASSERT_PTR_NULL(reg);
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
    CU_pSuite s_reg;
    int fails;

    if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();

    s_reg = CU_add_suite("Registry_Builder_Suite", init_suite_registry, clean_suite_registry);
    
    T t_reg[] = {
        {"test build_registry allocation", test_build_registry},
        {"test registry internal logic (flags/init)", test_registry_internal_logic},
        {"test free_registry safety", test_free_registry_null}
    };

    if (!s_reg || !add_tests(s_reg, t_reg, sizeof(t_reg) / sizeof(t_reg[0]))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    fails = CU_get_number_of_failures();
    CU_cleanup_registry();

    return fails > 0 ? 1 : 0;
}