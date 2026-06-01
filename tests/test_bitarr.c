#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "bitarr.h"
#include <stdbool.h>

// ==================== TEST CASES ====================

static void test_create_destroy(void) {
    bit_arr_t *arr = create_bit_arr(10);
    CU_ASSERT_PTR_NOT_NULL(arr);
    CU_ASSERT_EQUAL(arr->size, 10);
    destroy_bit_arr(&arr);
    CU_ASSERT_PTR_NULL(arr);
}

static void test_change_access(void) {
    bit_arr_t *arr = create_bit_arr(10);
    change_bit(arr, 3, true);
    CU_ASSERT_EQUAL(access_bit_arr(3, arr), 1);
    change_bit(arr, 3, false);
    CU_ASSERT_EQUAL(access_bit_arr(3, arr), 0);
    CU_ASSERT_EQUAL(access_bit_arr(1, arr), 0);
    destroy_bit_arr(&arr);
}

static void test_toggle(void) {
    bit_arr_t *arr = create_bit_arr(10);
    toggle_bit(arr, 5);
    CU_ASSERT_EQUAL(access_bit_arr(5, arr), 1);
    toggle_bit(arr, 5);
    CU_ASSERT_EQUAL(access_bit_arr(5, arr), 0);
    destroy_bit_arr(&arr);
}

static void test_set_all(void) {
    bit_arr_t *arr = create_bit_arr(10);
    set_all_bits(arr, true);
    CU_ASSERT_EQUAL(access_bit_arr(0, arr), 1);
    CU_ASSERT_EQUAL(access_bit_arr(9, arr), 1);
    set_all_bits(arr, false);
    CU_ASSERT_EQUAL(access_bit_arr(0, arr), 0);
    destroy_bit_arr(&arr);
}

static void test_copy(void) {
    bit_arr_t *arr = create_bit_arr(10);
    change_bit(arr, 2, true);
    bit_arr_t *cpy = copy_bit_arr(arr);
    CU_ASSERT_PTR_NOT_NULL(cpy);
    CU_ASSERT_EQUAL(cpy->size, 10);
    CU_ASSERT_EQUAL(access_bit_arr(2, cpy), 1);
    destroy_bit_arr(&arr);
    destroy_bit_arr(&cpy);
}

static void test_and(void) {
    bit_arr_t *a = create_bit_arr(10);
    bit_arr_t *b = create_bit_arr(10);
    change_bit(a, 1, true);
    change_bit(b, 1, true);
    change_bit(b, 2, true);
    and_bit_arr(a, b);
    CU_ASSERT_EQUAL(access_bit_arr(1, a), 1);
    CU_ASSERT_EQUAL(access_bit_arr(2, a), 0);
    destroy_bit_arr(&a);
    destroy_bit_arr(&b);
}

static void test_or_xor(void) {
    bit_arr_t *a = create_bit_arr(10);
    bit_arr_t *b = create_bit_arr(10);
    change_bit(a, 1, true);
    change_bit(b, 2, true);
    or_bit_arr(a, b);
    CU_ASSERT_EQUAL(access_bit_arr(1, a), 1);
    CU_ASSERT_EQUAL(access_bit_arr(2, a), 1);
    xor_bit_arr(a, b);
    CU_ASSERT_EQUAL(access_bit_arr(2, a), 0);
    destroy_bit_arr(&a);
    destroy_bit_arr(&b);
}

static void test_not(void) {
    bit_arr_t *arr = create_bit_arr(10);
    not_bit_arr(arr);
    CU_ASSERT_EQUAL(access_bit_arr(0, arr), 1);
    CU_ASSERT_EQUAL(access_bit_arr(9, arr), 1);
    not_bit_arr(arr);
    CU_ASSERT_EQUAL(access_bit_arr(0, arr), 0);
    destroy_bit_arr(&arr);
}

static void test_bounds(void) {
    bit_arr_t *arr = create_bit_arr(10);
    CU_ASSERT_FALSE(change_bit(arr, 10, true)); // Out of bounds
    CU_ASSERT_FALSE(toggle_bit(arr, 15));       // Out of bounds
    CU_ASSERT_EQUAL(access_bit_arr(20, arr), -1); // Access violation
    destroy_bit_arr(&arr);
}

// ==================== REGISTRATION & MAIN ====================

typedef struct {
    const char *name;
    CU_TestFunc func;
} test_case_t;

static void register_tests(CU_pSuite suite) {
    test_case_t tests[] = {
        {"test_create_destroy", test_create_destroy},
        {"test_change_access", test_change_access},
        {"test_toggle", test_toggle},
        {"test_set_all", test_set_all},
        {"test_copy", test_copy},
        {"test_and", test_and},
        {"test_or_xor", test_or_xor},
        {"test_not", test_not},
        {"test_bounds", test_bounds}
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

    CU_pSuite suite = CU_add_suite("bit_arr_suite", NULL, NULL);
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
