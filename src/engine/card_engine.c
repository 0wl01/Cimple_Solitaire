#include "card_engine.h"
#include "bitarr.h"
#include "macros.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline void *cleanup_on_create_fail(void *ret_pointer, bit_arr_t *flags) {
    free(flags);
    free(ret_pointer);
    return NULL;
}

static inline deck_t *init_deck_on_success(void *pointer, bit_arr_t *flags_arr, const card_count start_size,
                                           const char *deck_name) {
    deck_t *deck_ptr = pointer;
    deck_ptr->flags = flags_arr;
    deck_ptr->size = start_size;
    deck_ptr->top = 0;
    deck_ptr->id = strdup(deck_name);
    if (unlikely(deck_ptr->id == NULL))
        deck_ptr = cleanup_on_create_fail(deck_ptr, flags_arr);
    return deck_ptr;
}

// creates a deck_ptr with the given id and start size
void *create_deck(char const *restrict const deck_name, char const *restrict metadata, card_count start_size) {
    assert(deck_name != NULL && strlen(metadata) <= DECK_METADATA_SIZE);

    static const char METADATA_CHARS[DECK_METADATA_SIZE + 1] = "=1_^";
    void *ret_pointer = NULL;
    bit_arr_t *flags = create_bit_arr(DECK_METADATA_SIZE);

    if (likely(flags)) {
        for (size_t i = 0; i < DECK_METADATA_SIZE; ++i) {
            if (strchr(metadata, METADATA_CHARS[i]))
                toggle_bit(flags, i);
        }
        ret_pointer =
            calloc(1, sizeof(deck_t) +
                          (start_size = access_bit_arr(DECK_FLAG_FIXED_SIZE, flags) ? 1 : start_size) * CARD_SIZE);
    }

    return ret_pointer && flags ? init_deck_on_success(ret_pointer, flags, start_size, deck_name)
                                : cleanup_on_create_fail(ret_pointer, flags);
}

// peeks at a card in a deck
card peek(const deck_t *restrict const deck, const size_t index) { return index < deck->top ? deck->cards[index] : 0; }

// pushes a card onto the top of a deck
bool push_card_in_deck(const card card, deck_t *restrict const deck_ptr) {
    assert(deck_ptr != NULL);
    bool result_code = false;
    if (likely(deck_ptr->top < deck_ptr->size)) {
        deck_ptr->cards[deck_ptr->top++] = card;
        result_code = true;
    }
    return result_code;
}

// pops a card from the top of a deck
card pop_card_from_deck(deck_t *restrict const deck_ptr) {
    assert(deck_ptr != NULL);
    return is_deck_ptr_not_empty(deck_ptr) ? deck_ptr->cards[--deck_ptr->top] : 0;
}

// fills a deck with cards_to_fill cards and if the deck is full resizes it
bool fill_deck_with_cards(deck_t **restrict const deck_ptr, const card_count cards_to_fill) {
    assert(deck_ptr != NULL);
    assert(*deck_ptr != NULL);
    const card_count new_size = cards_to_fill + (*deck_ptr)->top;
    const bool result_code = (*deck_ptr)->size < new_size ? resize_deck(deck_ptr, new_size) : true;
    if (likely(result_code))
        for (card_count i = 0; i < cards_to_fill; ++i, ++(*deck_ptr)->top) {
            const card_count deck_top = (*deck_ptr)->top;
            (*deck_ptr)->cards[deck_top] =
                make_card(deck_top % CARD_VALUE_COUNT + 3, (deck_top / CARD_VALUE_COUNT) & 3, 0);
        }
    return result_code;
}

// shuffles a deck using the Fisher-Yates algorithm
void shuffle_deck(deck_t *restrict const deck_ptr) {
    assert(deck_ptr != NULL);
    for (card_count i = deck_ptr->top; i > 0; --i) {
        card_count j = arc4random_uniform(i);
        // using arc4random_uniform to get a random index
        // portability is not an issue as the game is only available for linux
        card temp = deck_ptr->cards[i - 1];
        deck_ptr->cards[i - 1] = deck_ptr->cards[j];
        deck_ptr->cards[j] = temp;
    }
}

// flips all cards in a deck
void flip_all_cards(deck_t *restrict const deck_ptr) {
    assert(deck_ptr != NULL);
    for (card_count i = 0; i < deck_ptr->top; ++i)
        deck_ptr->cards[i] = flip_card(deck_ptr->cards[i]);
}

void unflip_all(deck_t *restrict const deck) {
    assert(deck != NULL);
    for (card_count i = 0; i < deck->top; ++i) {
        const card temp_card = deck->cards[i];
        deck->cards[i] = is_flipped_card(temp_card) ? flip_card(temp_card) : temp_card;
    }
}

// gets the bigger deck from an array of deck_ptrs
deck_t *get_bigger_deck(const card_count size, deck_t *restrict const deck_ptr[size]) {
    assert(deck_ptr != NULL);
    assert(size > 0);
    assert(*deck_ptr != NULL);
    deck_t *bigger_deck_ptr = deck_ptr[0];
    for (card_count i = 0; i < size; ++i) {
        assert(deck_ptr[i] != NULL);
        bigger_deck_ptr = deck_ptr[i]->top > bigger_deck_ptr->top ? deck_ptr[i] : bigger_deck_ptr;
    }
    return bigger_deck_ptr;
}

// assumes positions starts at 0
// splits a deck into two decks at the given position
// any cards beyond the index (inclusive) are moved to the destination deck using
// the copying will fail if card type ever changes
bool split_deck(const card_count position, deck_t *restrict const deck_ptr_source,
                deck_t **restrict const deck_ptr_dest) {
    assert(deck_ptr_source != NULL);
    assert(deck_ptr_dest != NULL);
    assert(*deck_ptr_dest != NULL);
    assert(position < deck_ptr_source->top);
    const card_count cards_to_move = deck_ptr_source->top - position;
    const card_count new_top = cards_to_move + (*deck_ptr_dest)->top;
    bool result_code = false;
    if (unlikely(!(new_top > 1 && access_bit_arr(DECK_FLAG_FIXED_SIZE, (*deck_ptr_dest)->flags)))) {
        result_code = (*deck_ptr_dest)->size < new_top ? resize_deck(deck_ptr_dest, new_top) : true;
        if (result_code) {
            memcpy((*deck_ptr_dest)->cards + (*deck_ptr_dest)->top, deck_ptr_source->cards + position,
                   cards_to_move * CARD_SIZE);
            (*deck_ptr_dest)->top = new_top;
            deck_ptr_source->top = position;
        }
    }
    return result_code;
}

// if card ever changes this will need to be updated
bool resize_deck(deck_t **restrict deck_ptr, const card_count new_size) {
    assert(deck_ptr != NULL);
    assert(new_size > 0);
    bool result_code = true;
    deck_t *new_deck_ptr = realloc(*deck_ptr, new_size * CARD_SIZE + sizeof(deck_t));
    if (likely(result_code = new_deck_ptr != NULL)) {
        *deck_ptr = new_deck_ptr;
        (*deck_ptr)->size = new_size;
    }
    // if the new size is smaller than the top, set top to new size to avoid out of bounds access
    (*deck_ptr)->top = (*deck_ptr)->top < new_size ? (*deck_ptr)->top : new_size;
    return result_code;
}

deck_t *clone_deck(const deck_t *restrict const deck_ptr) {
    assert(deck_ptr != NULL);
    deck_t *new_deck_ptr = create_deck(deck_ptr->id, "", deck_ptr->size);
    if (likely(new_deck_ptr)) {
        memcpy(new_deck_ptr->cards, deck_ptr->cards, deck_ptr->top * CARD_SIZE);
        new_deck_ptr->top = deck_ptr->top;
        or_bit_arr(new_deck_ptr->flags, deck_ptr->flags);
    }

    return new_deck_ptr;
}

card deal(deck_t *restrict const deck_ptr_src, deck_t *restrict const deck_ptr_dest) {
    assert(deck_ptr_src != NULL);
    assert(deck_ptr_dest != NULL);

    card Card = 0;
    if (deck_ptr_dest->top < deck_ptr_dest->size && deck_ptr_src->top > 0) {
        Card = deck_ptr_src->cards[deck_ptr_src->top - 1];
        deck_ptr_dest->cards[deck_ptr_dest->top++] = Card;
        deck_ptr_src->top--;
    }
    return Card;
}
