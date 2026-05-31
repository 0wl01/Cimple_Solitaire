#pragma once

#include "paciencia_interpreter.h"
#include "card_engine.h"
#include <stdbool.h>
#include <stddef.h>

// ============ TYPEDEFS

// Represents an instantiated, playable game.
typedef struct {
    const paciencia_game_t *rules;
    deck_t **decks;
    size_t deck_count;
} game_state_t;

// ================== FUNCTION PROTOTYPES

/**
 * @brief Initializes a playable game state based on interpreted rules.
 *
 * @param rules The rules created by the paciencia interpreter.
 * @return A pointer to a newly allocated game state, or NULL on failure.
 */
game_state_t *init_game_state(const paciencia_game_t *rules);

/**
 * @brief Safely frees all memory allocated for a game state.
 *
 * @param state A double pointer to the game state.
 */
void free_game_state(game_state_t **state);

/**
 * @brief Validates if a move is legal according to the parsed DSL rules.
 *
 * @param state Current game state.
 * @param src_idx Index of the source deck.
 * @param dest_idx Index of the destination deck.
 * @param amount Amount of cards being moved.
 * @return true if the move is allowed, false otherwise.
 */
bool is_move_valid(const game_state_t *state, size_t src_idx, size_t dest_idx, size_t amount);

/**
 * @brief Executes a move if it is valid.
 *
 * @param state Current game state.
 * @param src_idx Index of the source deck.
 * @param dest_idx Index of the destination deck.
 * @param amount Amount of cards being moved.
 * @return true if move was executed successfully, false otherwise.
 */
bool execute_move(game_state_t *state, size_t src_idx, size_t dest_idx, size_t amount);

/**
 * @brief Chained execution of all applicable automatic moves.
 *
 * Runs continuously checking all rules until no more automatic moves can be performed.
 *
 * @param state Current game state.
 */
void execute_auto_moves(game_state_t *state);

/**
 * @brief Evaluates all victory conditions to determine if the game has been won.
 *
 * @param state Current game state.
 * @return true if all win conditions are met, false otherwise.
 */
bool check_win_condition_met(const game_state_t *state);
