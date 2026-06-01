
/**
 * @file paciencia_interpreter.h
 * @brief Header file for the paciencia interpreter module, responsible for interpreting game commands.
 */

#pragma once
#include "bitarr.h"
#include "card_engine.h"
#include <stdint.h>

// ====================== MACROS CONSTANTS

#define MAX_GAME_NAME_LEN 256


// ====================== TYPEDEFS ======================

typedef enum {
    MOV_ANY = 0,         // *
    MOV_SEQUENCE,        // +
    MOV_SEQ_DEC,         // [
    MOV_SEQ_INC,         // ]
    MOV_TOP_LESS,        // <
    MOV_TOP_GREATER,     // >
    MOV_TOP_ADJACENT,    // ~
    MOV_SEQ_SUIT,        // m
    MOV_DEST_SUIT,       // M
    MOV_SEQ_ALT_SUIT,    // x
    MOV_DEST_DIFF_SUIT,  // X
    MOV_SEQ_COLOR,       // c
    MOV_DEST_COLOR,      // C
    MOV_SEQ_ALT_COLOR,   // d
    MOV_DEST_DIFF_COLOR, // D
    MOV_DEST_EMPTY,      // V
    MOV_TOP_ACE,         // a
    MOV_BOT_ACE,         // A
    MOV_TOP_KING,        // k
    MOV_BOT_KING,        // K
    MOV_FLAG_COUNT
} mov_flag_t;

typedef struct {
    card_count starting_cards;
    char id[MAX_GAME_NAME_LEN];
    char metadata[DECK_METADATA_SIZE + 1];
} deck_recipe_t;

typedef struct {
    char src_id[MAX_GAME_NAME_LEN];
    char dest_id[MAX_GAME_NAME_LEN];
    bit_arr_t *flags;
    bool is_auto;
} move_rule_t;

typedef struct {
    char id[MAX_GAME_NAME_LEN];
    uint_fast16_t card_count;
} win_condition_t;

typedef struct {
    uint_fast16_t decks_to_create;
    uint_fast16_t move_rules_count;
    uint_fast16_t auto_move_rules_count;
    uint_fast16_t win_conditions_count;
    deck_recipe_t *deck_recipes;
    move_rule_t *move_rules;
    move_rule_t *auto_move_rules;
    win_condition_t *win_conditions;
    char name[MAX_GAME_NAME_LEN];
} paciencia_game_t;


// ====================== FUNCTION PROTOTYPES

/**
 * @brief Scans a paciencia game file and returns a paciencia_game_t struct.
 * @param filename The path to the paciencia game file.
 * @return A paciencia_game_t struct, or NULL if the file could not be read.
 */
paciencia_game_t *scan_paciencia_game_file(const char *restrict const filename);

/**
 * @brief Frees the resources used by a paciencia_game_t struct.
 * @param game The paciencia_game_t struct to free.
 */
void free_game_resources(paciencia_game_t *restrict *restrict const game);
