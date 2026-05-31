// Some bits of code shared by all games

#ifndef GAME_H
#define GAME_H
#include "card.h"
#include "command.h"
#include <stddef.h>

/**
 * @brief Signals used to control the main game loop state.
 */
typedef enum { LOOP_CONTINUE = 0, LOOP_RESTART, LOOP_QUIT } LoopSignal;

/**
 * @brief Function pointer type for handling game commands.
 */
typedef LoopSignal (*CommandHandler)(void *restrict game_state, const Command cmd);

/**
 * @brief Maps a CommandType to its corresponding handler function.
 */
typedef struct {
    CommandType type;
    CommandHandler handler;
} CommandDispatch;

typedef bool (*CanPlayFn)(void *state);
typedef void (*RenderFn)(void *state);
typedef void (*PostTurnFn)(void *state);
typedef bool (*HasWonFn)(void *state);

typedef struct {
    const CommandDispatch *dispatch_table;
    size_t                 dispatch_size;
    CanPlayFn              can_play;
    RenderFn               render;
    PostTurnFn             post_turn;
    HasWonFn               has_won;
} GameRunner;


/** @brief Default handler to quit the game. */
LoopSignal default_handle_quit(void *state, const Command cmd);

/** @brief Default handler to restart the game. */
LoopSignal default_handle_restart(void *state, const Command cmd);

/** @brief Default handler for unknown commands. */
LoopSignal default_handle_unknown(void *state, const Command cmd);

/** @brief Default handler to show the help menu. */
LoopSignal default_handle_help(void *state, const Command cmd);

/** @brief Default handler to show the help menu. */
LoopSignal default_handle_hint(void *state, const Command cmd);

/**
 * @brief Routes a command to the appropriate handler based on the dispatch table.
 * @param table Array of CommandDispatch mappings.
 * @param table_size Number of elements in the dispatch table.
 * @param state Pointer to the current game state.
 * @param cmd The command to dispatch.
 * @return The LoopSignal returned by the executed handler.
 */
LoopSignal dispatch(const CommandDispatch *table, const size_t table_size, void *state, Command cmd);

/**
 * @brief Runs the game loop, dispatching commands and handling game state.
 * @param state Pointer to the current game state.
 * @param runner The GameRunner configuration.
 * @return The final LoopSignal after the game loop ends.
 */
LoopSignal run_game(void *state, const GameRunner *runner);


#endif
