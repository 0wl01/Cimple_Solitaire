// Error checking and other stuff will be ignored for now as the tests made will be correct

#include <inttypes.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_reader.h"


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

// file_name must be a valid name ig
void scan_game_file(const char *file_name) {
    game Game = {0};

    FILE *f_ptr cleanup(close_file) = fopen(file_name, "r");
    // TODO check case of f_ptr being null or other errors
    if (!f_ptr) {
        perror(file_name);
        return;
    }

    char *line cleanup(mfree) = NULL;
    size_t line_length = 0;

    // first pass
    while (getline(&line, &line_length, f_ptr) != -1) {
        if (strncmp(line, "TIPO ", 5) == 0)
            ++Game.n_decks_types;
        else if (strncmp(line, "WIN ", 4) == 0)
            ++Game.n_winconds;
        else if (strncmp(line, "MOV ", 4) == 0)
            ++Game.n_move_rules;
        else if (strncmp(line, "AUTO ", 5) == 0)
            ++Game.n_auto_rules;
        else if (strncmp(line, "INIT ", 5) == 0)
            ++Game.n_instances;
    }

    Game.deck_types = malloc(sizeof(deck_t) * Game.n_decks_types);
    Game.conditions = malloc(sizeof(win_condition) * Game.n_winconds);
    Game.mov_rules = malloc(sizeof(move_rules) * Game.n_move_rules);
    Game.auto_rules = malloc(sizeof(auto_rule) * Game.n_auto_rules);
    Game.instances = malloc(sizeof(deck_instance) * Game.n_instances);

    rewind(f_ptr);

    //second pass
    uint16_t win_cond_i = 0, mov_rule_i = 0, auto_rule_i = 0, deck_type_i = 0, instance_i = 0;
    while (getline(&line, &line_length, f_ptr) != -1) {
        treat_line(line, strlen(line));
        sscanf(line, "JOGO %254s", Game.game_name);
        sscanf(line, "BARALHOS %" SCNu8, &Game.bar);
        if (strncmp(line, "TIPO ", 5) == 0) {
            sscanf(line, "TIPO %254s %4s", Game.deck_types[deck_type_i].name, Game.deck_types[deck_type_i].flags);
            ++deck_type_i;
        } else if (strncmp(line, "WIN ", 4) == 0) {
            sscanf(line, "WIN %254s %" SCNu16, Game.conditions[win_cond_i].deck_name, &Game.conditions[win_cond_i].n);
            ++win_cond_i;
        } else if (strncmp(line, "MOV ", 4) == 0) {
            sscanf(line, "MOV %254s %254s %20s", Game.mov_rules[mov_rule_i].deck_dst,
                   Game.mov_rules[mov_rule_i].deck_src, Game.mov_rules[mov_rule_i].flags);
            ++mov_rule_i;
        } else if (strncmp(line, "AUTO ", 5) == 0) {
            sscanf(line, "AUTO %254s %254s %20s", Game.auto_rules[auto_rule_i].deck_dst,
                   Game.auto_rules[auto_rule_i].deck_src, Game.auto_rules[auto_rule_i].flags);
            ++auto_rule_i;
        } else if (strncmp(line, "INIT ", 5) == 0) {
            sscanf(line, "INIT %254s %" SCNu16, Game.instances[instance_i].deck_t, &Game.instances[instance_i].n_cards);
            ++instance_i;
        }
    }
    printf("Game name: %s\n", Game.game_name);
    printf("Bar: %d\n", Game.bar);
    printf("Num decks types: %d\n", Game.n_decks_types);
    printf("Num win conditions: %d\n", Game.n_winconds);
    printf("Num move rules: %d\n", Game.n_move_rules);
    printf("Num auto rules: %d\n", Game.n_auto_rules);
    printf("Num instances: %d\n", Game.n_instances);
}
