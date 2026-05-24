#pragma once
#include <stdint.h>

#define STR(x) #x
#define XSTR(x) STR(x)
#define cleanup(F) __attribute__((__cleanup__(F)))

constexpr uint8_t max_game_name_size = 0xFF;
constexpr uint8_t max_deck_t_flags_size = 5;
constexpr uint8_t max_mov_t_flags_size = 21;

typedef char name_t[max_game_name_size];

typedef struct {
    name_t name;
    char flags[max_deck_t_flags_size];
} deck_t;

typedef struct {
    name_t deck_name;
    uint16_t n;
} win_condition;

typedef struct {
    name_t deck_dst;
    name_t deck_src;
    char flags[max_mov_t_flags_size];
} move_rules;

typedef move_rules auto_rule;

typedef struct {
    name_t deck_t;
    uint16_t n_cards;
} deck_instance;

typedef struct {
    name_t game_name;
    uint8_t bar;
    uint8_t n_decks_types;
    uint8_t n_instances;
    uint8_t n_winconds;
    uint8_t n_move_rules;
    uint8_t n_auto_rules;
    deck_t *deck_types;
    deck_instance *instances;
    move_rules *mov_rules;
    auto_rule *auto_rules;
    win_condition *conditions;
} game_cfg;

void free_game_cfg(game_cfg **Game);

game_cfg* scan_game_file(const char *filename);
