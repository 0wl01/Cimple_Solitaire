// Here lies all the possible commands used in this game

#ifndef COMMAND_H
#define COMMAND_H
#include <stddef.h>

typedef enum { CMD_MOV, CMD_HNT, CMD_HLP, CMD_RST, CMD_QUT, CMD_UNK, CMD_YES, CMD_NOT } CommandType;

typedef struct {
    CommandType type;
    char src_col;
    size_t index;
    char dest_col;
} Command;

#endif
