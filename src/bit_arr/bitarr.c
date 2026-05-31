#include "bitarr.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "macros.h"

// size in bits
bit_arr_t *create_bit_arr(const size_t size) {
    assert(size > 0);
    bit_arr_t *arr = aligned_alloc(8, sizeof(bit_arr_t) + ((size + 7) >> 3));
    if (likely(arr)) {
        arr->size = size;
        memset(arr->arr, 0, (size + 7) >> 3);
    }
    return arr;
}

void destroy_bit_arr(bit_arr_t **arr) {
    assert(arr != NULL);
    free(*arr);
    *arr = NULL;
}

// returns the value of the bit at index i, or -1 if out of bounds
int8_t access_bit_arr(const size_t i, const bit_arr_t *restrict const arr) {
    assert(arr != NULL);
    return i >= arr->size ? -1 : arr->arr[i >> 3] >> (i & 7) & 1;
}

bool change_bit(bit_arr_t *restrict const arr, const size_t i, const bool val) {
    assert(arr != NULL);
    const bool result_code = i < arr->size;
    if (likely(result_code)) {
        const uint8_t byte_to_access = arr->arr[i >> 3];
        arr->arr[i >> 3] = (byte_to_access & ~(1 << (i & 7))) | (val << (i & 7));
    }
    return result_code;
}

bool toggle_bit(bit_arr_t *restrict const arr, const size_t idx) {
    assert(arr != NULL);
    const bool result_code = idx < arr->size;
    if (likely(result_code))
        arr->arr[idx >> 3] ^= (1 << (idx & 7));
    return result_code;
}

void set_all_bits(bit_arr_t *restrict const arr, const bool set) {
    assert(arr != NULL);
    memset(arr->arr, set ? 0xFF : 0, (arr->size + 7) >> 3);
    if (set && (arr->size & 7))
        arr->arr[arr->size >> 3] &= (1 << (arr->size & 7)) - 1;
}

bit_arr_t *copy_bit_arr(const bit_arr_t *restrict const to_copy) {
    assert(to_copy != NULL);
    bit_arr_t *new = create_bit_arr(to_copy->size);
    if (new)
        memcpy(new->arr, to_copy->arr, bytelen(to_copy->size));
    return new;
}

bool xor_bit_arr(bit_arr_t *restrict const dest, const bit_arr_t *restrict const src) { return APPLY_BIT_OP(dest, src, ^); }
bool and_bit_arr(bit_arr_t *restrict const dest, const bit_arr_t *restrict const src) { return APPLY_BIT_OP(dest, src, &); }
bool or_bit_arr(bit_arr_t *restrict const dest, const bit_arr_t *restrict const src) { return APPLY_BIT_OP(dest, src, |); }

void not_bit_arr(bit_arr_t *restrict const arr) {
    assert(arr != NULL);
    const size_t total = bytelen(arr->size);
    const size_t chunks = total >> 3;
    uint64_t *d64 = (uint64_t *)arr->arr;
    for (size_t i = 0; i < chunks; ++i)
        d64[i] = ~d64[i];
    uint8_t *d8 = (uint8_t *)arr->arr + (chunks << 3);
    for (size_t i = 0; i < (total & 7); ++i)
        d8[i] = ~d8[i];
    if (arr->size & 7)
        d8[(total & 7) - 1] &= (1 << (arr->size & 7)) - 1;
}
