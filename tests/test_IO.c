#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "input.h"
#include "render.h"
#include "card_engine.h"
#include "command.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// ==================== TEST HELPERS ====================

// Injects string into stdin by redirecting a temporary file
static void inject_input(const char *str) {
    FILE *f = fopen("test_in.tmp", "w");
    if (f) {
        fputs(str, f);
        fclose(f);
        freopen("test_in.tmp", "r", stdin);
    }
}

static void cleanup_io_test(void) {
    remove("test_in.tmp");
}

// ==================== TEST CASES ====================

static void test_menu_input(void) {
    inject_input("y\n");
    CU_ASSERT_EQUAL(menu_get_input(), 'y');
    
    inject_input("n\n");
    CU_ASSERT_EQUAL(menu_get_input(), 'n');
}

static void test_game_input_move(void) {
    inject_input("m A 5 B\n");
    Command cmd = game_get_input();
    CU_ASSERT_EQUAL(cmd.type, CMD_MOV);
    CU_ASSERT_EQUAL(cmd.src_col, 'A');
    CU_ASSERT_EQUAL(cmd.index, 5);
    CU_ASSERT_EQUAL(cmd.dest_col, 'B');
    
    inject_input("m X\n"); // Testing incomplete/invalid move parsing
    Command inv = game_get_input();
    CU_ASSERT_EQUAL(inv.type, CMD_UNK);
}

static void test_game_input_file(void) {
    inject_input("s my_game.save\n");
    Command s_cmd = game_get_input();
    CU_ASSERT_EQUAL(s_cmd.type, CMD_SAV);
    CU_ASSERT_STRING_EQUAL(s_cmd.args, "my_game.save");
    
    inject_input("l old_game.save\n");
    Command l_cmd = game_get_input();
    CU_ASSERT_EQUAL(l_cmd.type, CMD_LOD);
    CU_ASSERT_STRING_EQUAL(l_cmd.args, "old_game.save");
}

static void test_game_input_single(void) {
    inject_input("?\n");
    CU_ASSERT_EQUAL(game_get_input().type, CMD_HLP);
    
    inject_input("u\n");
    CU_ASSERT_EQUAL(game_get_input().type, CMD_UND);
    
    inject_input("z\n"); // Unmapped key should return unknown
    CU_ASSERT_EQUAL(game_get_input().type, CMD_UNK);
}

static void test_render_smoke(void) {
    // Smoke tests ensure no crashes occur when rendering components.
    // They will print a small amount of text to the console during tests.
    card c_up = make_card(10, 1, 0); 
    card c_down = make_card(5, 2, 1);
    
    print_card(c_up);
    print_card(c_down);
    CU_ASSERT_TRUE(true);
}

// ==================== REGISTRATION & MAIN ====================

typedef struct {
    const char *name;
    CU_TestFunc func;
} test_case_t;

static void register_tests(CU_pSuite suite) {
    test_case_t tests[] = {
        {"test_menu_input", test_menu_input},
        {"test_game_input_move", test_game_input_move},
        {"test_game_input_file", test_game_input_file},
        {"test_game_input_single", test_game_input_single},
        {"test_render_smoke", test_render_smoke}
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
    
    CU_pSuite suite = CU_add_suite("io_suite", NULL, NULL);
    if (!suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    
    register_tests(suite);
    
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    cleanup_io_test();
    
    return CU_get_error();
}