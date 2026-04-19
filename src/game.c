#include "game.h"
#include "cli.h"
#include <stddef.h>

LoopSignal default_handle_quit(void *state, const Command cmd) {
    // TODO remove silencers when logic is implemented
    (void)state; (void)cmd;
    return LOOP_QUIT; 
}
LoopSignal default_handle_restart(void *state, const Command cmd) {
    // TODO remove silencers when logic is implemented
    (void)state; (void)cmd;
    return LOOP_RESTART; 
}
LoopSignal default_handle_unknown(void *state, const Command cmd) {
    // TODO remove silencers when logic is implemented
    (void)state; (void)cmd;
    print_unknown_command();
    return LOOP_CONTINUE;
}
LoopSignal default_handle_help(void *state, const Command cmd) {
    // TODO remove silencers when logic is implemented
    (void)state; (void)cmd;
    // maybe add default stuff here
    return LOOP_CONTINUE;
}

LoopSignal default_handle_hint(void *state, const Command cmd) {
    // TODO remove silencers when logic is implemented
    (void)state; (void)cmd;
    // maybe add default stuff here
    return LOOP_CONTINUE;
}

LoopSignal dispatch(const CommandDispatch *table, const size_t table_size, void *state, Command cmd) {
    for (size_t i = 0; i < table_size; ++i)
        if (table[i].type == cmd.type)
            return table[i].handler(state, cmd);
    return default_handle_unknown(state, cmd);
}
