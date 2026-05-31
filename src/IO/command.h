#pragma once
#include <stddef.h>

/**
 * @brief Every command type in the game.
 */
typedef enum {
    CMD_MOV,
    CMD_HNT,
    CMD_HLP,
    CMD_RST,
    CMD_QUT,
    CMD_UNK,
    CMD_YES,
    CMD_NOT,
    CMD_SAV,
    CMD_LOD,
    CMD_UND // New: Undo Command
} CommandType;

/**
 * @brief Organizes the information from an input.
 */
typedef struct {
    CommandType type;
    char src_col;
    size_t index;
    char dest_col;
    char args[256]; // For holding save/load filenames
} Command;
