#pragma once

// ================== INCLUDES
#include <stddef.h>
#include <stdint.h>

// ================== MACROS CONSTANTS

// ================== TYPEDEFS

typedef struct {
    size_t size;
    uint8_t arr[];
} bit_arr_t;

// ================== MACRO FUNCTIONS

#define bytelen(size_in_bits) (((size_in_bits) + 7) >> 3)

#define APPLY_BIT_OP(dest, src, op)                                                                                    \
    ({                                                                                                                 \
        int8_t _ret = false;                                                                                           \
        if (likely((dest) && (src) && (dest)->size == (src)->size)) {                                                  \
            const size_t _total = bytelen(dest->size);                                                                 \
            const size_t _chunks = _total >> 3;                                                                        \
            const size_t _tail = _total & 7;                                                                           \
            uint64_t *restrict _d64 = __builtin_assume_aligned((dest)->arr, 8);                                        \
            const uint64_t *restrict _s64 = __builtin_assume_aligned((src)->arr, 8);                                   \
            for (size_t _i = 0; _i < _chunks; ++_i)                                                                    \
                _d64[_i] op## = _s64[_i];                                                                              \
            uint8_t *restrict _d8 = (uint8_t *)(dest)->arr + (_chunks << 3);                                           \
            const uint8_t *restrict _s8 = (const uint8_t *)(src)->arr + (_chunks << 3);                                \
            for (size_t _i = 0; _i < _tail; ++_i)                                                                      \
                _d8[_i] op## = _s8[_i];                                                                                \
            _ret = true;                                                                                               \
        }                                                                                                              \
        _ret;                                                                                                          \
    })

// ================== FUNCTION PROTOTYPES

/**
 * @brief Creates a bit array of the specified size in bits.
 *
 * @param size_in_bits The size of the bit array in bits.
 * @return A pointer to the created bit array, or NULL if memory allocation fails.
 */
bit_arr_t *create_bit_arr(const size_t size_in_bits);

/**
 * @brief Destroys the bit array and frees the memory.
 *
 * @param arr A pointer to the bit array to destroy.
 */
void destroy_bit_arr(bit_arr_t **arr);

/**
 * @brief Accesses the bit at the specified index in the bit array.
 *
 * @param index The index of the bit to access.
 * @param arr The bit array to access.
 * @return The value of the bit at the specified index (0 or 1).
 */
int8_t access_bit_arr(const size_t index, const bit_arr_t *restrict const arr);

/**
 * @brief Changes the bit at the specified index in the bit array.
 *
 * @param index The index of the bit to change.
 * @param arr The bit array to change.
 * @param value The value to set the bit to (0 or 1).
 * @return false if the index is out of bounds, true otherwise.
 */
bool change_bit(bit_arr_t *restrict const arr, const size_t index, const bool value);

/**
 * @brief Toggles the bit at the specified index in the bit array.
 *
 * @param index The index of the bit to toggle.
 * @param arr The bit array to toggle.
 * @return false if the index is out of bounds, true otherwise.
 */
bool toggle_bit(bit_arr_t *restrict const arr, const size_t index);

/**
 * @brief Sets all bits in the bit array to 1 or 0.
 *
 * @param arr The bit array to set.
 * @param set The value to set the bits to (true for 1, false for 0).
 */
void set_all_bits(bit_arr_t *restrict const arr, const bool set);

/**
 * @brief Copies the bit array.
 *
 * @param to_copy The bit array to copy.
 * @return A pointer to the copied bit array, or NULL if memory allocation fails.
 */
bit_arr_t *copy_bit_arr(const bit_arr_t *restrict const to_copy);

/**
 * @brief XORs the bit array with another bit array.
 *
 * @param arr The bit array to XOR.
 * @param other The bit array to XOR with.
 * @return false if the bit arrays are not the same size, true otherwise.
 */
bool xor_bit_arr(bit_arr_t *restrict const dest, const bit_arr_t *restrict const src);

/**
 * @brief ANDs the bit array with another bit array.
 *
 * @param arr The bit array to AND.
 * @param other The bit array to AND with.
 * @return false if the bit arrays are not the same size, true otherwise.
 */
bool and_bit_arr(bit_arr_t *restrict const dest, const bit_arr_t *restrict const src);

/**
 * @brief ORs the bit array with another bit array.
 *
 * @param arr The bit array to OR.
 * @param other The bit array to OR with.
 * @return false if the bit arrays are not the same size, true otherwise.
 */
bool or_bit_arr(bit_arr_t *restrict const dest, const bit_arr_t *restrict const src);

/**
 * @brief NOTs the bit array.
 *
 * @param arr The bit array to NOT.
 */
void not_bit_arr(bit_arr_t *restrict const arr);
