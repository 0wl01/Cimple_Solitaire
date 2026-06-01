#include "card_engine.h"
#include "game_runner.h"
#include "paciencia_interpreter.h"
#include "save.h"
#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// ==================== TEST HELPERS ====================

static paciencia_game_t *get_mock_rules_for_save(const char *content) {
    const char *filename = "test_save.paciencia";
    FILE *f = fopen(filename, "w");
    if (f) {
        fputs(content, f);
        fclose(f);
    }
    return scan_paciencia_game_file(filename);
}

static void cleanup_save_test(void) {
    remove("test_save.paciencia");
    remove("test_game.save");
}

// ==================== TEST CASES ====================

static void test_save_invalid_path(void) {
    paciencia_game_t *r = get_mock_rules_for_save("INIT A 1\n");
    game_state_t *st = init_game_state(r);

    // Attempt to write to a protected root directory which should fail
    CU_ASSERT_FALSE(save_game_file(st, "/invalid_dir/test.save", "dummy.paciencia"));

    free_game_state(&st);
    free_game_resources(&r);
}

static void test_load_invalid_path(void) {
    paciencia_game_t *r = get_mock_rules_for_save("INIT A 1\n");

    // Try to load a file that does not exist
    game_state_t *st = load_game_file(r, "non_existent_file.save");
    CU_ASSERT_PTR_NULL(st);

    free_game_resources(&r);
}

static void test_save_and_load(void) {
    paciencia_game_t *r = get_mock_rules_for_save("INIT A 1\n");
    game_state_t *st = init_game_state(r);

    // Ensure the save writes successfully
    CU_ASSERT_TRUE(save_game_file(st, "test_game.save", "test_save.paciencia"));

    // Ensure the load succeeds and rebuilds the state
    game_state_t *loaded = load_game_file(r, "test_game.save");
    CU_ASSERT_PTR_NOT_NULL(loaded);

    // Verify that the data loaded matches exactly what was saved
    if (loaded && st) {
        CU_ASSERT_EQUAL(loaded->deck_count, 1);
        CU_ASSERT_EQUAL(loaded->decks[0]->top, 1);
        CU_ASSERT_EQUAL(loaded->decks[0]->cards[0], st->decks[0]->cards[0]);
        free_game_state(&loaded);
    }

    free_game_state(&st);
    free_game_resources(&r);
}

// ==================== REGISTRATION & MAIN ====================

typedef struct {
    const char *name;
    CU_TestFunc func;
} test_case_t;

static void register_tests(CU_pSuite suite) {
    test_case_t tests[] = {{"test_save_invalid_path", test_save_invalid_path},
                           {"test_load_invalid_path", test_load_invalid_path},
                           {"test_save_and_load", test_save_and_load}};
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

    CU_pSuite suite = CU_add_suite("save_suite", NULL, NULL);
    if (!suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    register_tests(suite);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    cleanup_save_test();

    return CU_get_error();
}
