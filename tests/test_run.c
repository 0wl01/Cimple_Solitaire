#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "run.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// ==================== TEST HELPERS ====================

// Creates a tiny, temporary DSL file to instantiate a valid game state
static const char* create_mock_dsl(void) {
    const char *content = "INIT TAB 5\nINIT FUND 0\nWIN TAB 0\nMOV TAB FUND *\n";
    FILE *f = fopen("mock_run.paciencia", "w");
    if (f) {
        fputs(content, f);
        fclose(f);
    }
    return "mock_run.paciencia";
}

// Injects a string of commands into stdin for the game loop to read
static void inject_run_input(const char *str) {
    FILE *f = fopen("test_run_in.tmp", "w");
    if (f) {
        fputs(str, f);
        fclose(f);
        freopen("test_run_in.tmp", "r", stdin);
    }
}

// Cleans up all temporary testing files
static void cleanup_run_test(void) {
    remove("test_run_in.tmp");
    remove("mock_run.paciencia");
    remove("test_run.save");
}

// ==================== TEST CASES ====================

static void test_run_invalid_file(void) {
    // If given a bad path, the runner should fail gracefully and return false
    CU_ASSERT_FALSE(run_dsl_game("does_not_exist.paciencia"));
}

static void test_run_immediate_quit(void) {
    const char *dsl = create_mock_dsl();
    inject_run_input("q\n"); // Just quit
    
    CU_ASSERT_FALSE(run_dsl_game(dsl));
    cleanup_run_test();
}

static void test_run_move_undo(void) {
    const char *dsl = create_mock_dsl();
    inject_run_input("m A 1 B\nu\nq\n"); // Move, Undo, Quit
    
    CU_ASSERT_FALSE(run_dsl_game(dsl));
    cleanup_run_test();
}

static void test_run_save_load(void) {
    const char *dsl = create_mock_dsl();
    inject_run_input("s test_run.save\nl test_run.save\nq\n"); // Save, Load, Quit
    
    CU_ASSERT_FALSE(run_dsl_game(dsl));
    cleanup_run_test();
}

static void test_run_invalid_commands(void) {
    const char *dsl = create_mock_dsl();
    inject_run_input("z\n?\nq\n"); // Unknown command, Help command, Quit
    
    CU_ASSERT_FALSE(run_dsl_game(dsl));
    cleanup_run_test();
}

// ==================== REGISTRATION & MAIN ====================

typedef struct {
    const char *name;
    CU_TestFunc func;
} test_case_t;

static void register_tests(CU_pSuite suite) {
    test_case_t tests[] = {
        {"test_run_invalid_file", test_run_invalid_file},
        {"test_run_immediate_quit", test_run_immediate_quit},
        {"test_run_move_undo", test_run_move_undo},
        {"test_run_save_load", test_run_save_load},
        {"test_run_invalid_commands", test_run_invalid_commands}
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
    
    CU_pSuite suite = CU_add_suite("run_suite", NULL, NULL);
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