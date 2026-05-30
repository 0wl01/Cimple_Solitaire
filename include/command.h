// Here lies all the possible commands used in this game

#ifndef COMMAND_H
#define COMMAND_H

/**
 * @file
 * @brief Here lies all possible commands used in this game.
 */

#include <stddef.h>

/**
 * @brief Every command type
 */
typedef enum { CMD_MOV, CMD_HNT, CMD_HLP, CMD_RST, CMD_QUT, CMD_UNK, CMD_YES, CMD_NOT, CMD_SAV, CMD_LOD, CMD_LDF} CommandType;

/**
 * @brief Organizes the information from an input.
 *
 * The char and size_t types are only used if CommandType is a move command.
 */
typedef struct {
    CommandType type;
    char src_col;
    size_t index;
    char dest_col;
    char filename[64];
} Command;

#endif
