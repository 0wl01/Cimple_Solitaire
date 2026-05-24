#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stddef.h>
#include <stdlib.h>
#include "card.h"

/* SUITE 1: STANDARD (Normal Deck Manipulation) */
static Deck *std_d1 = NULL;
static Deck *std_d2 = NULL;

int init_suite_std(void) {
    std_d1 = create_deck(52);
    std_d2 = create_deck(52);
    return (!std_d1 || !std_d2) ? -1 : 0;
}

int clean_suite_std(void) {
    eliminate_deck(&std_d1);
    eliminate_deck(&std_d2);
    return 0;
}

/**
 * @brief Tests the allocation and basic initialization of a Deck.
 * * Verifies if create_deck() returns a non-null pointer, sets the correct 
 * size, and starts with an empty stack (top = 0).
 * * @see create_deck()
 * @see eliminate_deck()
 */
void test_create_and_eliminate_deck() {
    Deck *d = create_deck(52);
    CU_ASSERT_PTR_NOT_NULL(d);
    CU_ASSERT_EQUAL(d->size, 52);
    CU_ASSERT_EQUAL(d->top, 0);
    CU_ASSERT_TRUE(IS_EMPTY(d));

    eliminate_deck(&d);

    CU_ASSERT_PTR_NULL(d);
}

/**
 * @brief Tests a standard push operation on a deck with available space.
 * * Ensures that cards are correctly added to the stack and that the 
 * 'top' index increments as expected.
 * * @see push()
 */
void test_push_normal() {
    std_d1->top = 0;
    card c1 = make_card(1, 10);

    // Test successful pushes
    CU_ASSERT_TRUE(push(std_d1, c1));
    CU_ASSERT_EQUAL(std_d1->top, 1);
    CU_ASSERT_FALSE(IS_EMPTY(std_d1));

    card c2 = make_card(2,10);

    CU_ASSERT_TRUE(push(std_d1, c2));
    CU_ASSERT_EQUAL(std_d1->top, 2);
}

/**
 * @brief Tests the Last-In, First-Out (LIFO) behavior of the pop function.
 * * Ensures that the last card pushed is the first one retrieved and 
 * that the 'top' index decrements correctly.
 * * @see pop()
 */
void test_pop(void) {
    std_d1->top = 0; // deck reset
    card c1 = make_card(1, 9);
    card c2 = make_card(1, 10);

    push(std_d1, c1);
    push(std_d1, c2);

    // Test pop (should pop c2 first, LIFO)
    card popped = pop(std_d1);
    CU_ASSERT_EQUAL(popped, c2);
    CU_ASSERT_EQUAL(std_d1->top, 1);
}

/**
 * @brief Validates the deck population logic and card value offsets.
 * * Checks if the deck contains 52 cards and verifies specific card 
 * properties (suit, color, value) at different positions to ensure 
 * the nested loops work correctly.
 * * @see populate_deck()
 */
void test_populate_deck(void) {
    std_d1->top = 0;
    populate_deck(std_d1);

    CU_ASSERT_EQUAL(std_d1->top, 52);

    // Test first card (Spades (0), Value 3)
    CU_ASSERT_EQUAL(card_suit(std_d1->cards[0]), 0);
    CU_ASSERT_EQUAL(card_value(std_d1->cards[0]), 3);
    CU_ASSERT_FALSE(card_flipped(std_d1->cards[0]));

    // Test a middle card (Hearts (1), Value 12) -> index 22
    CU_ASSERT_EQUAL(card_suit(std_d1->cards[22]), 1);
    CU_ASSERT_EQUAL(card_value(std_d1->cards[22]), 12);

    // Test last card (Clubs (3), Value 15) -> index 51
    CU_ASSERT_EQUAL(card_suit(std_d1->cards[51]), 3);
    CU_ASSERT_EQUAL(card_value(std_d1->cards[51]), 15);
}

/**
 * @brief Tests a standard card transfer between two decks.
 * * Verifies that the correct number of cards is moved and that the 
 * LIFO order is maintained during the transfer.
 * * @see deal()
 */
void test_deal_normal(void) {
    std_d1->top = 0;
    std_d2->top = 0;
    populate_deck(std_d1);

    // Deal 3 cards from d1 to d2
    deal(std_d1, std_d2, 3, false);

    CU_ASSERT_EQUAL(std_d1->top, 49);
    CU_ASSERT_EQUAL(std_d2->top, 3);

    // The top card of d1 (index 9) should now be the bottom card of d2 (index
    CU_ASSERT_EQUAL(card_suit(std_d2->cards[0]), 3);
    CU_ASSERT_EQUAL(card_value(std_d2->cards[0]), 15);
}

/**
 * @brief Tests the safety of the top_card function on an empty deck.
 * * Confirms that calling top_card() on a deck with no elements returns 
 * a null-initialized Card structure instead of crashing.
 * * @see top_card()
 */
void test_top_card_empty(void) {
    std_d1->top = 0;

    // Test empty deck returns {0} safely
    card empty_top = top_card(std_d1);
    CU_ASSERT_EQUAL(empty_top, 0);
}

/**
 * @brief Tests retrieving the top card without removing it.
 * * Verifies that top_card() returns the correct data and that the 
 * deck's 'top' index remains unchanged.
 * * @see top_card()
 */
void test_top_card_normal(void) {
    std_d1->top = 0;

    // Test normal top card
    card c1 = make_card(0, 7);
    push(std_d1, c1);

    card top = top_card(std_d1);
    CU_ASSERT_EQUAL(top, c1);
    CU_ASSERT_EQUAL(std_d1->top, 1); // Ensure top_card doesn't pop it!
}

/**
 * @brief Tests the bulk flipping of all cards in a deck.
 * * Ensures that the flip bit is toggled for every card in the stack 
 * and can be toggled back.
 * * @see flip_all()
 */
void test_flip_all(void) {
    std_d1->top = 0;
    push(std_d1, make_card(0, 3));
    push(std_d1, make_card(0, 4));
    push(std_d1, make_card(0, 5));

    CU_ASSERT_FALSE(card_flipped(std_d1->cards[0]));
    CU_ASSERT_FALSE(card_flipped(std_d1->cards[1]));
    CU_ASSERT_FALSE(card_flipped(std_d1->cards[2]));

    flip_all(std_d1);

    CU_ASSERT_TRUE(card_flipped(std_d1->cards[0]));
    CU_ASSERT_TRUE(card_flipped(std_d1->cards[1]));
    CU_ASSERT_TRUE(card_flipped(std_d1->cards[2]));

    // Test toggle off
    flip_all(std_d1);
    CU_ASSERT_FALSE(card_flipped(std_d1->cards[0]));
    CU_ASSERT_FALSE(card_flipped(std_d1->cards[1]));
    CU_ASSERT_FALSE(card_flipped(std_d1->cards[2]));
}

/**
 * @brief Tests the individual card flipping logic.
 * * Verifies that the XOR operation on the flip bit correctly toggles 
 * the card's visibility state.
 * * @see flip_card()
 */
void test_flip_card(void) {
    card c = make_card(1, 10);
    c = flip_card(c);
    CU_ASSERT_TRUE(card_flipped(c)); // card is now face down
    c = flip_card(c);
    CU_ASSERT_FALSE(card_flipped(c)); // card return to initial state
}

/**
 * @brief Tests the combined functionality of dealing and flipping.
 * * Validates that cards are moved between decks and their 'flip' 
 * state is inverted in a single operation.
 * * @see flip_deal()
 */
void test_flip_deal(void) {
    std_d1->top = 0;
    std_d2->top = 0;
    card c = make_card(1, 10);

    // cards inserted in deck 1
    push(std_d1, c);
    push(std_d1, c);

    deal(std_d1, std_d2, 2, true); // move to std_d2 while fliping

    CU_ASSERT_EQUAL(std_d1->top, 0);
    CU_ASSERT_EQUAL(std_d2->top, 2); // d2 now has 2 cards and they should be flipped
    CU_ASSERT_TRUE(card_flipped(std_d2->cards[0]));
    CU_ASSERT_TRUE(card_flipped(std_d2->cards[1]));
}

/**
 * @brief Verifies the randomness and integrity of the shuffle algorithm.
 * * Ensures that after a shuffle, the deck still contains 52 cards 
 * but in a different order than the initial state.
 * * @see shuffle_deck()
 */
void test_shuffle_deck(void) {
    std_d1->top = 0;
    populate_deck(std_d1); // Fills deck in order

    card first_before = std_d1->cards[0];
    card last_before = std_d1->cards[51];

    shuffle_deck(std_d1);

    CU_ASSERT_EQUAL(std_d1->top, 52); // Must keep all cards

    // Checks if the first or last card changed
    // False Positive very unlikely
    CU_ASSERT_TRUE(std_d1->cards[0] != first_before || std_d1->cards[51] != last_before);
}

/**
 * @brief Tests the attempt to split a deck at an invalid position.
 * * @details Verifies that the `split_deck` function returns false and prevents 
 * the split when the provided index is greater than the current number 
 * of cards in the deck (above the `top`).
 */
void test_split_deck_invalid_position(void) {
    int i;

    std_d1->top = 0;
    std_d2->top = 0;

    // Insert exactly 5 cards (values 3, 4, 5, 6, 7)
    for(i = 3; i <= 7; i++) {
        push(std_d1, make_card(0, i));
    }

    // split attempt above top card
    CU_ASSERT_FALSE(split_deck(std_d1, std_d2, 6));
}

/**
 * @brief Tests the successful scenario of splitting a deck.
 * * @details Verifies that, given a valid index, the function correctly divides 
 * the cards between the two decks and updates their respective tops (`top`) 
 * to the expected sizes.
 */
void test_split_deck_valid(void) {
    int i;

    std_d1->top = 0;
    std_d2->top = 0;

    for(i = 3; i <= 7; i++) {
        push(std_d1, make_card(0, i));
    }

    CU_ASSERT_TRUE(split_deck(std_d1, std_d2, 2));
    CU_ASSERT_EQUAL(std_d1->top, 2);
    CU_ASSERT_EQUAL(std_d2->top, 3);
}

// TODO: DOC
void test_unflip_all(void) {
    std_d1->top = 0;
    
    card c_up = make_card(0, 3);
    card c_down = make_card(0, 4);
    c_down = flip_card(c_down); // face down card

    push(std_d1, c_up);
    push(std_d1, c_down);

    CU_ASSERT_FALSE(card_flipped(std_d1->cards[0]));
    CU_ASSERT_TRUE(card_flipped(std_d1->cards[1]));

    unflip_all(std_d1);

    CU_ASSERT_FALSE(card_flipped(std_d1->cards[0]));
    CU_ASSERT_FALSE(card_flipped(std_d1->cards[1]));
}

// TODO: DOC
void test_peek(void) {
    std_d1->top = 0;
    push(std_d1, make_card(2, 5));

    card c1 = peek(std_d1, 0);
    CU_ASSERT_EQUAL(c1, make_card(2, 5));

    /* Tentar peek acima do top (top = 1) */
    card c2_invalid = peek(std_d1, 1);
    CU_ASSERT_EQUAL(c2_invalid, 0); 
}

// TODO: DOC
void test_clone_deck(void) {
    Deck *cloned;
    int i;
    std_d1->top = 0;
    populate_deck(std_d1);
    
    cloned = clone_deck(std_d1);
    
    CU_ASSERT_PTR_NOT_NULL(cloned);
    CU_ASSERT_EQUAL(cloned->top, std_d1->top);
    CU_ASSERT_EQUAL(cloned->size, std_d1->size);

    // Check if cloned is 1 for 1 copy of std_d1
    for (i = 0; i < std_d1->top; i++) {
        CU_ASSERT_EQUAL(cloned->cards[i], std_d1->cards[i]);
    }
    
    eliminate_deck(&cloned);
}

// TODO: DOC
void test_sequences(void) {
    std_d1->top = 0;
    
    /* Vamos colocar 4 cartas:
       0: Espadas(0), Valor 5 (Preto)
       1: Espadas(0), Valor 4 (Preto)
       2: Copas(1),   Valor 3 (Vermelho)
       3: Copas(1),   Valor 4 (Vermelho)
    */
    push(std_d1, make_card(0, 5));
    push(std_d1, make_card(0, 4));
    push(std_d1, make_card(1, 3));
    push(std_d1, make_card(1, 4));

    /* Teste de Naipes iguais (0 a 1 é True, 0 a 2 é False) */
    CU_ASSERT_TRUE(sequence_same_suit(std_d1, 0, 1));
    CU_ASSERT_FALSE(sequence_same_suit(std_d1, 0, 2));

    /* Teste decrescente (5 -> 4 -> 3) */
    CU_ASSERT_TRUE(sequence_is_decreasing(std_d1, 0, 2));
    CU_ASSERT_FALSE(sequence_is_decreasing(std_d1, 0, 3));

    /* Teste crescente (3 -> 4) */
    CU_ASSERT_TRUE(sequence_is_increasing(std_d1, 2, 3));

    /* Decrescente + Mesmo Naipe */
    CU_ASSERT_TRUE(sequence_is_decreasing_hierarchy(std_d1, 0, 1));
    CU_ASSERT_FALSE(sequence_is_decreasing_hierarchy(std_d1, 0, 2)); /* Naipe muda */

    /* Mesma cor (0 e 1 são pretas, 2 e 3 são vermelhas) */
    CU_ASSERT_TRUE(sequence_same_color(std_d1, 0, 1));
    CU_ASSERT_TRUE(sequence_same_color(std_d1, 2, 3));
    CU_ASSERT_FALSE(sequence_same_color(std_d1, 1, 2));

    /* Cor e Naipe Alternado (Preto -> Vermelho) */
    CU_ASSERT_TRUE(sequence_alternating_color(std_d1, 1, 2));
    CU_ASSERT_TRUE(sequence_alternating_suit(std_d1, 1, 2));
}

/* Helper function: Transforms an macro into an actual function so we can use it as a pointer */
static bool test_pred_is_one_less(const card a, const card b) {
    return cards_is_one_less(a, b);
}
// TODO: DOC
void test_sequence_length(void) {
    card_count len;
    
    std_d1->top = 0;
    push(std_d1, make_card(0, 5));
    push(std_d1, make_card(0, 4));
    
    /* 2 cartas em ordem decrescente = comprimento 2 */
    len = sequence_length(std_d1, 0, test_pred_is_one_less);
    CU_ASSERT_EQUAL(len, 2);
}

/* SUITE 2: OVERFLOW & LIMITS (Small Decks)*/

static Deck *lim_d1 = NULL;
static Deck *lim_d2 = NULL;

int init_suite_limits(void) {
    lim_d1 = create_deck(5);
    lim_d2 = create_deck(2);
    return (!lim_d1 || !lim_d2) ? -1 : 0;
}

int clean_suite_limits(void) {
    eliminate_deck(&lim_d1);
    eliminate_deck(&lim_d2);
    return 0;
}

/**
 * @brief Tests pushing a card to a deck that has reached its maximum capacity.
 * * Asserts that the function returns (uint8_t)-1 and that the deck's 
 * top index remains unchanged.
 * * @see push()
 */
void test_push_full(void) {
    lim_d2-> top = 2; // max capacity
    push(lim_d2, make_card(1, 10));
    push(lim_d2, make_card(0, 11)); //deck is now full

    CU_ASSERT_FALSE(push(lim_d2, make_card(2, 3))); // push has to fail
}

/**
 * @brief Tests the deal function's behavior when the destination deck is full.
 * * Ensures that the function only transfers the amount of cards that fit 
 * in the destination, preventing memory corruption.
 * * @see deal()
 */
void test_deal_overflow(void) {
    int i;

    lim_d1->top = 0;
    lim_d2->top = 0;

    for(i = 0; i < 5; i++) {
        push(lim_d1, make_card(0, i+3));
    }

    // Attempt to deal 5 cards (but d2 only has 2 spaces)
    deal(lim_d1, lim_d2, 5, false);
    CU_ASSERT_EQUAL(lim_d1->top, 3); // Only 2 cards should be removed
    CU_ASSERT_EQUAL(lim_d2->top, 2); // d2 should be completely full
}

/**
 * @brief Tests the overflow protection on the destination deck.
 * * @details Verifies that the function fails safely when the number of cards 
 * to be moved exceeds the maximum allocated capacity of the destination deck.
 */
void test_split_deck_overflow(void) {
    int i;

    lim_d1->top = 0;
    lim_d2->top = 0;

    for(i = 3; i <= 7; i++) {
        push(lim_d1, make_card(0, i));
    }

    // too many cards to d2
    CU_ASSERT_FALSE(split_deck(lim_d1, lim_d2, 2));
}

/* SUITE 3: ARRAYS (test_get_bigger_deck) */
static Deck *arr_d[3] = {NULL};

int init_suite_arrays(void) {
    int i;
    for (i = 0; i < 3; i++) {
        arr_d[i] = create_deck(5);
        if (!arr_d[i]) return -1;
    }
    return 0;
}

int clean_suite_arrays(void) {
    int i;
    for (i = 0; i < 3; i++) {
        eliminate_deck(&arr_d[i]);
    }
    return 0;
}

/**
 * @brief Tests the logic for identifying the largest deck in a collection.
 * * Compares multiple decks with different card counts to ensure the 
 * function returns the pointer to the one with the highest occupancy.
 * * @see get_bigger_deck()
 */
void test_get_bigger_deck(void) {
    int i, j;
    for(i = 0; i < 3; i++) {
        arr_d[i]->top = 0;
    }

    // fills arr_d[0] with 1 card, arr_d[1] with 2 cards and arr_d[2] with 3 cards
    for(i = 0; i < 3; i++) {
        for(j = 0; j <= i; j++) {
            push(arr_d[i], make_card(0, 3));
        }
    }

    CU_ASSERT_PTR_EQUAL(get_bigger_deck(arr_d, 3), arr_d[2]);
    CU_ASSERT_EQUAL(get_bigger_deck(arr_d, 3)->top, 3);
}

// TODO: DOC
void test_clone_deck_array(void) {
    Deck **cloned_arr;
    int i, j;
    
    /* Prepara os 3 baralhos com quantidades diferentes de cartas */
    for (i = 0; i < 3; i++) {
        arr_d[i]->top = 0;
        for (j = 0; j <= i; j++) {
            push(arr_d[i], make_card(1, 10 + j));
        }
    }
    
    cloned_arr = clone_deckArray(arr_d, 3);
    CU_ASSERT_PTR_NOT_NULL(cloned_arr);
    
    /* Verifica array a array, e carta a carta! */
    for (i = 0; i < 3; i++) {
        CU_ASSERT_PTR_NOT_NULL(cloned_arr[i]);
        CU_ASSERT_EQUAL(cloned_arr[i]->top, arr_d[i]->top);
        
        for (j = 0; j < arr_d[i]->top; j++) {
            CU_ASSERT_EQUAL(cloned_arr[i]->cards[j], arr_d[i]->cards[j]);
        }
    }
    
    /* Limpeza de Memória */
    for (i = 0; i < 3; i++) {
        eliminate_deck(&cloned_arr[i]);
    }
    free(cloned_arr);
}

/* MAIN RUNNER & REGISTRY */
typedef struct {
    const char *name;
    CU_TestFunc fn;
} T;

static int add_tests(CU_pSuite suite, T *tests, size_t count) {
    for (int i = 0; i < count; i++)
        if (!CU_add_test(suite, tests[i].name, tests[i].fn))
            return 0;
    return 1;
}

int main(void) {
    if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();

    // SUITE 1: Standard
    CU_pSuite s_std = CU_add_suite("Card_Standard_Suite", init_suite_std, clean_suite_std);
    T t_std[] = {
        {"test of create/eliminate deck", test_create_and_eliminate_deck},
        {"test of a normal push", test_push_normal},
        {"test of push and pop", test_pop},
        {"test of populate_deck", test_populate_deck},
        {"test of a normal deal", test_deal_normal},
        {"test of top_card on empty deck", test_top_card_empty},
        {"test of top_card", test_top_card_normal},
        {"test of flip_all", test_flip_all},
        {"test of flip_card", test_flip_card},
        {"test of flip_deal", test_flip_deal},
        {"test of shuffle_deck", test_shuffle_deck},
        {"test of an invalid deck split", test_split_deck_invalid_position},
        {"test of an valid deck split", test_split_deck_valid},
        {"test of unflip_all", test_unflip_all},
        {"test of peek", test_peek},
        {"test of clone_deck", test_clone_deck},
        {"test of sequences", test_sequences},
        {"test of sequence_length", test_sequence_length}
    };
    if (!s_std || !add_tests(s_std, t_std, sizeof(t_std) / sizeof(t_std[0]))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // SUITE 2: Limits
    CU_pSuite s_lim = CU_add_suite("Card_Limits_Suite", init_suite_limits, clean_suite_limits);
    T t_lim[] = {
        {"test of push on a full deck", test_push_full},
        {"test of a overflow deal", test_deal_overflow},
        {"test of overflow protection", test_split_deck_overflow}
    };
    if (!s_lim || !add_tests(s_lim, t_lim, sizeof(t_lim) / sizeof(t_lim[0]))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    // SUITE 3: Arrays
    CU_pSuite s_arr = CU_add_suite("Card_Arrays_Suite", init_suite_arrays, clean_suite_arrays);
    T t_arr[] = {
        {"test of get_bigger_deck", test_get_bigger_deck},
        {"test of clone_deck_array", test_clone_deck_array}
    };
    if (!s_arr || !add_tests(s_arr, t_arr, sizeof(t_arr) / sizeof(t_arr[0]))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    int fails = CU_get_number_of_failures();
    CU_cleanup_registry();

    return fails > 0 ? 1 : 0;
}