#include "command.h"
#include "card.h"
#include <stdio.h>

/**
 *
 */
char menu_get_input() {
  char buffer[3];
  return fgets(buffer, sizeof(buffer), stdin) ? buffer[0] : 'q';
}

/**
 * @brief Parses a move command string into a Command struct.
 * @param buffer The input string (e.g., "m A 5 B").
 * @return A populated Command struct, or a CMD_UNK command if parsing fails.
 */
static Command parse_move(const char *restrict buffer) {
  Command cmd = {
      .type = CMD_MOV, .src_col = 0, .index = SIZE_MAX, .dest_col = 0};
  int result_code =
      sscanf(buffer, "m %c %zu %c", &cmd.src_col, &cmd.index, &cmd.dest_col);
  return result_code < 1 ? (Command){.type = CMD_UNK} : cmd;
}

/**
 * @brief Turns character from an input into a command.
 *
 * @return If input is none of the ones listed in "map" then it returns a
 * unknown command, which does nothing.
 *
 * @see CommandType
 */
static CommandType char_to_command(const char c) {
  const struct {
    char key;
    CommandType cmd;
  } map[] = {{'h', CMD_HNT}, {'?', CMD_HLP}, {'r', CMD_RST},
             {'q', CMD_QUT}, {'y', CMD_YES}, {'n', CMD_NOT},
             {'s', CMD_SAV}, {'l', CMD_LOD}};
  for (card_count i = 0; i < sizeof(map) / sizeof(*map); ++i)
    if (map[i].key == c)
      return map[i].cmd;
  return CMD_UNK;
}

Command game_get_input() {
  char buffer[32];
  if (!fgets(buffer, sizeof(buffer), stdin))
    return (Command){.type = CMD_QUT};
  return buffer[0] == 'm' ? parse_move(buffer)
                          : (Command){.type = char_to_command(buffer[0])};
}
