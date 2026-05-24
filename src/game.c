#include "game.h"
#include "cli.h"
#include <stddef.h>
#include "macros.h"

LoopSignal default_handle_quit(void *UNUSED state,
                               const Command UNUSED cmd) {
  return LOOP_QUIT;
}
LoopSignal default_handle_restart(void *UNUSED state,
                                  const Command UNUSED cmd) {
  return LOOP_RESTART;
}
LoopSignal default_handle_unknown(void *UNUSED state,
                                  const Command UNUSED cmd) {
  print_unknown_command();
  return LOOP_CONTINUE;
}
LoopSignal default_handle_help(void *UNUSED state,
                               const Command UNUSED cmd) {
  // maybe add default stuff here
  return LOOP_CONTINUE;
}

LoopSignal default_handle_hint(void *UNUSED state,
                               const Command UNUSED cmd) {
  // maybe add default stuff here
  return LOOP_CONTINUE;
}

LoopSignal dispatch(const CommandDispatch *table, const size_t table_size,
                    void *state, Command cmd) {
  for (size_t i = 0; i < table_size; ++i)
    if (table[i].type == cmd.type)
      return table[i].handler(state, cmd);
  return default_handle_unknown(state, cmd);
}

static LoopSignal game_end(const GameRunner *runner, void *state) {
    print_end(runner->has_won(state));
    print_prompt();
    return game_get_input().type == CMD_YES ? LOOP_RESTART : LOOP_QUIT;
}

static LoopSignal game_tick(const GameRunner *runner, void *state) {
    runner->render(state);
    print_prompt();
    LoopSignal sig = dispatch(runner->dispatch_table,
                              runner->dispatch_size,
                              state,
                              game_get_input());
    if (runner->post_turn)
        runner->post_turn(state);
    return sig;
}

LoopSignal run_game(void *state, const GameRunner *runner) {
    LoopSignal sig = LOOP_CONTINUE;
    while (sig == LOOP_CONTINUE && runner->can_play(state))
        sig = game_tick(runner, state);
    return sig == LOOP_CONTINUE ? game_end(runner, state) : sig;
}
