#include "game.h"
#include "cli.h"
#include <stddef.h>

LoopSignal default_handle_quit(void *__attribute__((unused)) state, const Command __attribute__((unused)) cmd) {
    return LOOP_QUIT;
}
LoopSignal default_handle_restart(void *__attribute__((unused)) state, const Command __attribute__((unused)) cmd) {
    return LOOP_RESTART;
}
LoopSignal default_handle_unknown(void *__attribute__((unused)) state, const Command __attribute__((unused)) cmd) {
    print_unknown_command();
    return LOOP_CONTINUE;
}
LoopSignal default_handle_help(void *__attribute__((unused)) state, const Command __attribute__((unused)) cmd) {
    // maybe add default stuff here
    return LOOP_CONTINUE;
}

LoopSignal default_handle_hint(void *__attribute__((unused)) state, const Command __attribute__((unused)) cmd) {
    // maybe add default stuff here
    return LOOP_CONTINUE;
}

LoopSignal dispatch(const CommandDispatch *table, const size_t table_size, void *state, Command cmd) {
    for (size_t i = 0; i < table_size; ++i)
        if (table[i].type == cmd.type)
            return table[i].handler(state, cmd);
    return default_handle_unknown(state, cmd);
}
