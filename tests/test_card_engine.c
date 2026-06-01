#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "card_engine.h"
#include <stdbool.h>
#include <stdlib.h>

// ==================== TEST HELPERS ====================

// Local helper to cleanly free decks since engine relies on runner for this
static void destroy_test_deck(deck_t *d) {
    if (d) {
        free(d->id);
        destroy_bit_arr(&d->flags);
        free(d);
    }
}

// ==================== TEST CASES ====================

static void test_card_macros(void) {
    card c = make_card(10, 2, 1);
    CU_ASSERT_EQUAL(card_value(c), 10);
    CU_ASSERT_EQUAL(card_suit(c), 2);
    CU_ASSERT_EQUAL(card_flip(c), 1);
    
    card unflip = flip_card(c);
    CU_ASSERT_EQUAL(card_flip(unflip), 0);
    
    card forced = flip_card_to(unflip, 1);
    CU_ASSERT_EQUAL(card_flip(forced), 1);
}

static void test_deck_lifecycle(void) {
    deck_t *d = create_deck("main", "1=", 10);
    CU_ASSERT_PTR_NOT_NULL(d);
    CU_ASSERT_STRING_EQUAL(d->id, "main");
    
    deck_t *cpy = clone_deck(d);
    CU_ASSERT_PTR_NOT_NULL(cpy);
    CU_ASSERT_EQUAL(cpy->size, d->size);
    
    destroy_test_deck(d);
    destroy_test_deck(cpy);
}

static void test_push_pop_peek(void) {
    deck_t *d = create_deck("t", "", 5);
    card c1 = make_card(5, 1, 0);
    
    CU_ASSERT_TRUE(push_card_in_deck(c1, d));
    CU_ASSERT_EQUAL(d->top, 1);
    CU_ASSERT_EQUAL(peek(d, 0), c1);
    
    card popped = pop_card_from_deck(d);
    CU_ASSERT_EQUAL(popped, c1);
    CU_ASSERT_EQUAL(d->top, 0);
    
    destroy_test_deck(d);
}

static void test_fill_and_deal(void) {
    deck_t *src = create_deck("src", "", 5);
    deck_t *dest = create_deck("dest", "", 5);
    
    fill_deck_with_cards(&src, 3);
    CU_ASSERT_EQUAL(src->top, 3);
    
    deal(src, dest);
    CU_ASSERT_EQUAL(src->top, 2);
    CU_ASSERT_EQUAL(dest->top, 1);
    
    destroy_test_deck(src);
    destroy_test_deck(dest);
}

static void test_split_resize(void) {
    deck_t *src = create_deck("src", "", 2);
    deck_t *dest = create_deck("dest", "", 2);
    
    fill_deck_with_cards(&src, 4); // Should auto-resize to fit 4
    CU_ASSERT_TRUE(src->size >= 4);
    
    split_deck(2, src, &dest); // Moves index 2 and 3
    CU_ASSERT_EQUAL(src->top, 2);
    CU_ASSERT_EQUAL(dest->top, 2);
    
    destroy_test_deck(src);
    destroy_test_deck(dest);
}

static void test_card_helpers(void) {
    card c_ace = make_card(ACE_VALUE, 0, 0);
    card c_two = make_card(4, 0, 0);
    card c_king = make_card(KING_VALUE, 1, 0);

    CU_ASSERT_TRUE(card_is_ace(c_ace));
    CU_ASSERT_TRUE(card_is_king(c_king));
    CU_ASSERT_TRUE(cards_is_one_less(c_ace, c_two));
    CU_ASSERT_TRUE(cards_is_one_more(c_two, c_ace));
    CU_ASSERT_TRUE(cards_different_suit(c_ace, c_king));
    CU_ASSERT_TRUE(cards_are_adjacent(c_ace, c_two));
}

// ==================== REGISTRATION & MAIN ====================

typedef struct {
    const char *name;
    CU_TestFunc func;
} test_case_t;

static void register_tests(CU_pSuite suite) {
    test_case_t tests[] = {
        {"test_card_macros", test_card_macros},
        {"test_deck_lifecycle", test_deck_lifecycle},
        {"test_push_pop_peek", test_push_pop_peek},
        {"test_fill_and_deal", test_fill_and_deal},
        {"test_split_resize", test_split_resize},
        {"test_card_helpers", test_card_helpers}
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
    
    CU_pSuite suite = CU_add_suite("card_engine_suite", NULL, NULL);
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