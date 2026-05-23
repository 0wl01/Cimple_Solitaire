// Error checking and other stuff will be ignored for now as the tests made will be correct

#include "dsl.h"
#include <assert.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline void close_file(FILE **file) {
    if (*file)
        fclose(*file);
}

static inline void mfree(char **ptr) {
    if (*ptr)
        free(*ptr);
}

// func to remove comments from a line
static inline void treat_line(char *restrict line, const size_t length) {
    for (size_t i = 0; i < length; ++i) {
        if (line[i] == '#') {
            line[i] = '\0';
            return;
        }
    }
}

static inline void first_scan(char **line, size_t *line_length, FILE *f_ptr, game_cfg *Game) {
    while (getline(line, line_length, f_ptr) != -1) {
        if (strncmp(*line, "TIPO ", 5) == 0)
            ++Game->n_decks_types;
        else if (strncmp(*line, "WIN ", 4) == 0)
            ++Game->n_winconds;
        else if (strncmp(*line, "MOV ", 4) == 0)
            ++Game->n_move_rules;
        else if (strncmp(*line, "AUTO ", 5) == 0)
            ++Game->n_auto_rules;
        else if (strncmp(*line, "INIT ", 5) == 0)
            ++Game->n_instances;
    }
}

static inline void alloc_game_resources(game_cfg *Game) {
    Game->deck_types = malloc(sizeof(deck_t) * Game->n_decks_types);
    Game->conditions = malloc(sizeof(win_condition) * Game->n_winconds);
    Game->mov_rules = malloc(sizeof(move_rules) * Game->n_move_rules);
    Game->auto_rules = malloc(sizeof(auto_rule) * Game->n_auto_rules);
    Game->instances = malloc(sizeof(deck_instance) * Game->n_instances);
}

static inline void free_game_resources(game_cfg *Game) {
    free(Game->deck_types);
    free(Game->conditions);
    free(Game->mov_rules);
    free(Game->auto_rules);
    free(Game->instances);
}

void free_game_cfg(game_cfg **Game) {
    free_game_resources(*Game);
    free(*Game);
    *Game = nullptr;
}

static inline void parse_game_info(const char *line, game_cfg *G) {
    sscanf(line, "JOGO %254s", G->game_name);
    sscanf(line, "BARALHOS %" SCNu8, &G->bar);
}

static inline void parse_game_rules(const char *line, game_cfg *G, uint_fast16_t *di, uint_fast16_t *wi,
                                    uint_fast16_t *mi, uint_fast16_t *ai, uint_fast16_t *ii) {
    if (!strncmp(line, "TIPO ", 5))
        sscanf(line, "TIPO %254s %4s", G->deck_types[*di].name, G->deck_types[(*di)++].flags);
    else if (!strncmp(line, "WIN ", 4))
        sscanf(line, "WIN %254s %" SCNu16, G->conditions[*wi].deck_name, &G->conditions[(*wi)++].n);
    else if (!strncmp(line, "MOV ", 4))
        sscanf(line, "MOV %254s %254s %20s", G->mov_rules[*mi].deck_dst, G->mov_rules[*mi].deck_src,
               G->mov_rules[(*mi)++].flags);
    else if (!strncmp(line, "AUTO ", 5))
        sscanf(line, "AUTO %254s %254s %20s", G->auto_rules[*ai].deck_dst, G->auto_rules[*ai].deck_src,
               G->auto_rules[(*ai)++].flags);
    else if (!strncmp(line, "INIT ", 5))
        sscanf(line, "INIT %254s %" SCNu16, G->instances[*ii].deck_t, &G->instances[(*ii)++].n_cards);
}

static inline void second_scan(char **line, size_t *line_length, FILE *f_ptr, game_cfg *Game) {
    uint_fast16_t win_cond_i = 0, mov_rule_i = 0, auto_rule_i = 0, inst_i = 0, deck_type_i = 0;
    while (getline(line, line_length, f_ptr) != -1) {
        treat_line(*line, strlen(*line));
        parse_game_info(*line, Game);
        parse_game_rules(*line, Game, &deck_type_i, &win_cond_i, &mov_rule_i, &auto_rule_i, &inst_i);
    }
}

// file_name must be a valid name ig
game_cfg *scan_game_file(const char *file_name) {
    game_cfg *Game = calloc(1, sizeof(game_cfg));
    if (!Game)
        return nullptr;

    FILE *f_ptr cleanup(close_file) = fopen(file_name, "r");
    // TODO check case of f_ptr being null or other errors
    if (!f_ptr) {
        perror(file_name);
        free(Game);
        return nullptr;
    }

    char *line cleanup(mfree) = nullptr;
    size_t line_length = 0;

    first_scan(&line, &line_length, f_ptr, Game);
    alloc_game_resources(Game);

    rewind(f_ptr);

    second_scan(&line, &line_length, f_ptr, Game);
    return Game;
}
