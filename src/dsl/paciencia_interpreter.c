// IGNORING ERROR CHECKING BECAUSE IT JUST WON'T HAPPEN

#include "paciencia_interpreter.h"
#include "bitarr.h"
#include "macros.h"
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline void remove_comment_from_line(char *restrict const line, const size_t length) {
    for (size_t i = 0; i < length; ++i) {
        if (line[i] == '#') {
            line[i] = '\0';
            return;
        }
    }
}

static inline void first_pass_count_args(paciencia_game_t *restrict const game, FILE *restrict const file) {
    char *line cleanup(mfree) = NULL;
    size_t line_len = 0;
    while (getline(&line, &line_len, file) != -1) {
        remove_comment_from_line(line, line_len);
        if (strncmp(line, "WIN ", 4) == 0)
            ++game->win_conditions_count;
        else if (strncmp(line, "MOV ", 4) == 0)
            ++game->move_rules_count;
        else if (strncmp(line, "AUTO ", 5) == 0)
            ++game->auto_move_rules_count;
        else if (strncmp(line, "INIT ", 5) == 0)
            ++game->decks_to_create;
    }
}

static inline void alloc_fixed_memory(paciencia_game_t *restrict const game) {
    game->deck_recipes = calloc(game->decks_to_create, sizeof(deck_recipe_t));
    game->win_conditions = calloc(game->win_conditions_count, sizeof(win_condition_t));
    game->move_rules = calloc(game->move_rules_count, sizeof(move_rule_t));
    game->auto_move_rules = calloc(game->auto_move_rules_count, sizeof(move_rule_t));
}

static inline bool alloc_game_memory(paciencia_game_t *restrict const game) {
    bool result_code = true;
    alloc_fixed_memory(game);
    if (game->deck_recipes && game->win_conditions && game->move_rules && game->auto_move_rules) {
        for (uint_fast16_t i = 0; i < game->move_rules_count; ++i)
            if (!(game->move_rules[i].flags = create_bit_arr(MOV_FLAG_COUNT)))
                return false;
        for (uint_fast16_t i = 0; i < game->auto_move_rules_count; ++i)
            if (!(game->auto_move_rules[i].flags = create_bit_arr(MOV_FLAG_COUNT)))
                return false;
    } else {
        result_code = false;
    }

    return result_code;
}

void free_game_resources(paciencia_game_t *restrict *restrict const game) {
    if (*game) {
        free((*game)->deck_recipes);
        for (uint_fast16_t i = 0; i < (*game)->move_rules_count && (*game)->move_rules; ++i) {
            destroy_bit_arr(&(*game)->move_rules[i].flags);
        }
        free((*game)->move_rules);
        for (uint_fast16_t i = 0; i < (*game)->auto_move_rules_count && (*game)->auto_move_rules; ++i) {
            destroy_bit_arr(&(*game)->auto_move_rules[i].flags);
        }
        free((*game)->auto_move_rules);
        free((*game)->win_conditions);
        free(*game);
        *game = NULL;
    }
}

// apply this metadata to every deck with that id
static inline void apply_deck_metadata(paciencia_game_t *restrict const game, const char *id, const char flags[5]) {
    for (uint_fast16_t i = 0; i < game->decks_to_create; ++i) {
        if (!strcmp(game->deck_recipes[i].id, id)) {
            strcpy(game->deck_recipes[i].metadata, flags);
        }
    }
}

// receives a string and converts it to a bit in a  bit_arr_t using the mov_flag_t enum
static inline void string_to_mov_flag(const char *flags, move_rule_t *restrict const rule) {
    static const struct {
        char flag;
        mov_flag_t bit;
    } flag_map[] = {
        {'*', MOV_ANY},        {'+', MOV_SEQUENCE},      {'[', MOV_SEQ_DEC},         {']', MOV_SEQ_INC},
        {'<', MOV_TOP_LESS},   {'>', MOV_TOP_GREATER},   {'~', MOV_TOP_ADJACENT},    {'m', MOV_SEQ_SUIT},
        {'M', MOV_DEST_SUIT},  {'x', MOV_SEQ_ALT_SUIT},  {'X', MOV_DEST_DIFF_SUIT},  {'c', MOV_SEQ_COLOR},
        {'C', MOV_DEST_COLOR}, {'d', MOV_SEQ_ALT_COLOR}, {'D', MOV_DEST_DIFF_COLOR}, {'V', MOV_DEST_EMPTY},
        {'a', MOV_TOP_ACE},    {'A', MOV_BOT_ACE},       {'k', MOV_TOP_KING},        {'K', MOV_BOT_KING},
    };

    constexpr size_t flag_map_size = sizeof(flag_map) / sizeof(flag_map[0]);
    for (size_t i = 0; i < flag_map_size; ++i) {
        if (strchr(flags, flag_map[i].flag)) {
            toggle_bit(rule->flags, flag_map[i].bit);
        }
    }
}

static inline void parse_move_rule(const char *line, move_rule_t *rule) {
    char flags[MOV_FLAG_COUNT + 1];
    sscanf(line, "%*s %255s %254s %20s", rule->src_id, rule->dest_id, flags);
    string_to_mov_flag(flags, rule);
}

static inline void parse_game_rules(const char *line, paciencia_game_t *restrict const game, uint_fast16_t *wi,
                                    uint_fast16_t *mi, uint_fast16_t *ai, uint_fast16_t *ii) {
    if (!strncmp(line, "WIN ", 4)) {
        sscanf(line, "WIN %255s %" SCNuFAST16, game->win_conditions[*wi].id, &game->win_conditions[*wi].card_count);
        ++(*wi);
    } else if (!strncmp(line, "MOV ", 4)) {
        parse_move_rule(line, &game->move_rules[*mi]);
        ++(*mi);
    } else if (!strncmp(line, "AUTO ", 5)) {
        parse_move_rule(line, &game->auto_move_rules[*ai]);
        game->auto_move_rules[*ai].is_auto = true;
        ++(*ai);
    } else if (!strncmp(line, "INIT ", 5)) {
        sscanf(line, "INIT %255s %" SCNuFAST16, game->deck_recipes[*ii].id, &game->deck_recipes[*ii].starting_cards);
        ++(*ii);
    }
}

static inline void parse_deck_flags(paciencia_game_t *restrict const game, FILE *restrict const file) {
    char *file_line cleanup(mfree) = NULL;
    size_t file_line_len = 0;
    while (getline(&file_line, &file_line_len, file) != -1) {
        remove_comment_from_line(file_line, file_line_len);
        if (!strncmp(file_line, "TIPO ", 5)) {
            char id[MAX_GAME_NAME_LEN], flags[5];
            sscanf(file_line, "TIPO %255s %4s", id, flags);
            apply_deck_metadata(game, id, flags);
        }
    }
}

static inline void second_pass_read_args(paciencia_game_t *restrict const game, FILE *restrict const file) {
    uint_fast16_t deck_recipe_index = 0, move_rule_index = 0, auto_move_rule_index = 0, win_condition_index = 0;
    char *file_line cleanup(mfree) = NULL;
    size_t file_line_len = 0;
    while (getline(&file_line, &file_line_len, file) != -1) {
        remove_comment_from_line(file_line, file_line_len);
        parse_game_rules(file_line, game, &win_condition_index, &move_rule_index, &auto_move_rule_index,
                         &deck_recipe_index);
    }
    rewind(file);
    parse_deck_flags(game, file);
}

paciencia_game_t *scan_paciencia_game_file(const char *restrict const filename) {
    paciencia_game_t *restrict game = calloc(1, sizeof(paciencia_game_t));
    FILE *file cleanup(close_file) = fopen(filename, "r");
    if (likely(game)) {
        if (likely(file)) {
            first_pass_count_args(game, file);
            rewind(file);
            if (likely(alloc_game_memory(game))) {
                second_pass_read_args(game, file);
            } else {
                free_game_resources(&game);
            }

        } else {
            perror("Failed to open paciencia file");
            free(game);
            game = NULL;
        }
    }
    return game;
}
