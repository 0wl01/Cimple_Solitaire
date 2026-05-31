#include "input.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static Command parse_move(const char *restrict buffer) {
    Command cmd = {.type = CMD_MOV, .src_col = 0, .index = SIZE_MAX, .dest_col = 0};
    int res = sscanf(buffer, "m %c %zu %c", &cmd.src_col, &cmd.index, &cmd.dest_col);
    if (res < 1) {
        cmd.type = CMD_UNK;
    }
    return cmd;
}

static Command parse_file_cmd(CommandType t, const char *restrict buffer) {
    Command cmd = {.type = t};
    sscanf(buffer, "%*c %255s", cmd.args);
    return cmd;
}

static CommandType char_to_command(const char c) {
    const struct {
        char key;
        CommandType cmd;
    } map[] = {
        {'h', CMD_HNT}, {'?', CMD_HLP}, {'r', CMD_RST}, {'q', CMD_QUT},
        {'y', CMD_YES}, {'n', CMD_NOT}, {'u', CMD_UND} // Map the 'u' key to Undo
    };
    for (size_t i = 0; i < sizeof(map) / sizeof(*map); ++i) {
        if (map[i].key == c)
            return map[i].cmd;
    }
    return CMD_UNK;
}

char menu_get_input(void) {
    char buffer[4];
    return fgets(buffer, sizeof(buffer), stdin) ? buffer[0] : 'q';
}

Command game_get_input(void) {
    char buffer[256];
    if (!fgets(buffer, sizeof(buffer), stdin))
        return (Command){.type = CMD_QUT};
    if (buffer[0] == 'm')
        return parse_move(buffer);
    if (buffer[0] == 's')
        return parse_file_cmd(CMD_SAV, buffer);
    if (buffer[0] == 'l')
        return parse_file_cmd(CMD_LOD, buffer);
    return (Command){.type = char_to_command(buffer[0])};
}
