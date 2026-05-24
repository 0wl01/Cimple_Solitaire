#include "dsl_game.h"
#include "card.h"
#include "cli.h"
#include "game.h"
#include <stdio.h>
#include <string.h>

static bool flag_sequence_decreasing(const Deck *src, card_count index) {
    return sequence_is_decreasing(src, index, src->top - 1);
}

static bool flag_sequence_increasing(const Deck *src, card_count index) {
    return sequence_is_increasing(src, index, src->top - 1);
}

static bool flag_sequence_same_suit(const Deck *src, card_count index) {
    return sequence_same_suit(src, index, src->top - 1);
}

static bool flag_sequence_alt_color(const Deck *src, card_count index) {
    return sequence_alternating_color(src, index, src->top - 1);
}

static bool flag_sequence_alt_suit(const Deck *src, card_count index) {
    return sequence_alternating_suit(src, index, src->top - 1);
}

static bool flag_sequence_same_color(const Deck *src, card_count index) {
    return sequence_same_color(src, index, src->top - 1);
}

static bool flag_less(const Deck *src, const Deck *dst) {
    return card_value(top_card(src)) == card_value(top_card(dst)) - 1;
}

static bool flag_greater(const Deck *src, const Deck *dst) {
    return card_value(top_card(src)) == card_value(top_card(dst)) + 1;
}

static bool flag_same_suit_dst(const Deck *src, const Deck *dst) {
    return cards_same_suit(top_card(src), top_card(dst));
}

static bool flag_diff_suit_dst(const Deck *src, const Deck *dst) {
    return !cards_same_suit(top_card(src), top_card(dst));
}

static bool flag_same_color_dst(const Deck *src, const Deck *dst) {
    return card_color(top_card(src)) == card_color(top_card(dst));
}

static bool flag_diff_color_dst(const Deck *src, const Deck *dst) {
    return card_color(top_card(src)) != card_color(top_card(dst));
}

static bool flag_dst_empty(const Deck *dst) { return is_deck_empty(dst); }

static bool flag_top_is_ace(const Deck *src) { return card_value(top_card(src)) == CARD_ACE; }

static bool flag_top_is_king(const Deck *src) { return card_value(top_card(src)) == CARD_KING; }

static bool flag_bottom_is_ace(const Deck *src, card_count index) { return card_value(src->cards[index]) == CARD_ACE; }

static bool flag_bottom_is_king(const Deck *src, card_count index) {
    return card_value(src->cards[index]) == CARD_KING;
}

static bool check_tipo_flags(const deck_entry *dst) {
    return strchr(dst->flags, '1') ? is_deck_empty(dst->deck) : true;
}

static bool check_single_flag(char f, const deck_entry *src, const deck_entry *dst, card_count index) {
    switch (f) {
    case '*':
        return true;
    case 'V':
        return flag_dst_empty(dst->deck);
    case '<':
        return flag_less(src->deck, dst->deck);
    case '>':
        return flag_greater(src->deck, dst->deck);
    case '~':
        return flag_less(src->deck, dst->deck) || flag_greater(src->deck, dst->deck);
    case 'm':
        return flag_sequence_same_suit(src->deck, index);
    case 'M':
        return flag_same_suit_dst(src->deck, dst->deck);
    case 'x':
        return flag_sequence_alt_suit(src->deck, index);
    case 'X':
        return flag_diff_suit_dst(src->deck, dst->deck);
    case 'c':
        return flag_sequence_same_color(src->deck, index);
    case 'C':
        return flag_same_color_dst(src->deck, dst->deck);
    case 'd':
        return flag_sequence_alt_color(src->deck, index);
    case 'D':
        return flag_diff_color_dst(src->deck, dst->deck);
    case 'a':
        return flag_top_is_ace(src->deck);
    case 'A':
        return flag_bottom_is_ace(src->deck, index);
    case 'k':
        return flag_top_is_king(src->deck);
    case 'K':
        return flag_bottom_is_king(src->deck, index);
    case '[':
        return flag_sequence_decreasing(src->deck, index);
    case ']':
        return flag_sequence_increasing(src->deck, index);
    case '+':
        return true; // just allows sequences, handled at move time
    default:
        return true;
    }
}

static bool check_mov_flags(const char *flags, const deck_entry *src, const deck_entry *dst, card_count index) {
    for (uint8_t i = 0; flags[i] != '\0'; ++i)
        if (!check_single_flag(flags[i], src, dst, index))
            return false;
    return true;
}

static bool move_is_valid(const move_rules *rule, const deck_entry *src, const deck_entry *dst, card_count index) {
    if (is_deck_empty(src->deck)) {
        return false;
    }
    if (!check_tipo_flags(dst)) {
        return false;
    }
    if (!strchr(rule->flags, '+') && index != src->deck->top - 1) {
        return false; // no sequence allowed, must be single card
    }
    return check_mov_flags(rule->flags, src, dst, index);
}

bool can_move_rule(const move_rules *rule, const deck_entry *src, const deck_entry *dst, card_count index) {
    return strcmp(src->name, rule->deck_src) == 0 && strcmp(dst->name, rule->deck_dst) == 0 &&
           move_is_valid(rule, src, dst, index);
}

static bool try_apply_move(const game_cfg *cfg, deck_entry *src,
                            deck_entry *dst, card_count index) {
    for (uint8_t i = 0; i < cfg->n_move_rules; ++i) {
        if (!can_move_rule(&cfg->mov_rules[i], src, dst, index))
            continue;
        bool moved = split_deck(src->deck, dst->deck, index);
        if (moved && strchr(dst->flags, '='))
            unflip_all(dst->deck);
        if (moved && strchr(src->flags, '^') && src->deck->top > 0)
            flip_card(src->deck->cards[src->deck->top - 1]);
        return moved;
    }
    return false;
}

static int8_t col_to_index(char c) {
    if (c >= 'a' && c <= 'z')
        return c - 'a';
    if (c >= 'A' && c <= 'Z')
        return (c - 'A') + 26;
    return -1;
}

LoopSignal dsl_handle_move(void *restrict state, Command cmd) {
    dsl_state *s = state;
    int8_t src_i = col_to_index(cmd.src_col);
    int8_t dst_i = col_to_index(cmd.dest_col);
    if (src_i < 0 || dst_i < 0 || src_i >= s->reg->n_entries || dst_i >= s->reg->n_entries)
        return LOOP_CONTINUE;
    deck_entry *src = &s->reg->entries[src_i];
    deck_entry *dst = &s->reg->entries[dst_i];
    try_apply_move(s->cfg, src, dst, cmd.index);
    return LOOP_CONTINUE;
}

static bool win_condition_met(const win_condition *cond, const deck_registry *reg) {
    uint8_t seen = 0;
    for (uint8_t i = 0; i < reg->n_entries; ++i) {
        if (strcmp(reg->entries[i].name, cond->deck_name) != 0)
            continue;
        if (deck_count(reg->entries[i].deck) != cond->n)
            return false;
        ++seen;
    }
    return seen > 0;
}

bool dsl_has_won(void *state) {
    dsl_state *s = state;
    for (uint8_t i = 0; i < s->cfg->n_winconds; ++i)
        if (!win_condition_met(&s->cfg->conditions[i], s->reg))
            return false;
    return true;
}

static bool try_auto_rule(const auto_rule *rule, deck_registry *reg) {
    bool triggered = false;
    for (uint8_t i = 0; i < reg->n_entries; ++i) {
        if (strcmp(reg->entries[i].name, rule->deck_src) != 0)
            continue;
        for (uint8_t j = 0; j < reg->n_entries; ++j) {
            if (strcmp(reg->entries[j].name, rule->deck_dst) != 0)
                continue;
            if (!move_is_valid((move_rules *)rule, &reg->entries[i], &reg->entries[j], reg->entries[i].deck->top - 1))
                continue;
            split_deck(reg->entries[i].deck, reg->entries[j].deck, reg->entries[i].deck->top - 1);
            triggered = true;
        }
    }
    return triggered;
}

static bool run_auto_rules_once(dsl_state *s) {
    bool triggered = false;
    for (uint8_t i = 0; i < s->cfg->n_auto_rules; ++i)
        triggered = try_auto_rule(&s->cfg->auto_rules[i], s->reg) || triggered;
    return triggered;
}

void dsl_post_turn(void *state) {
    dsl_state *s = state;
    uint8_t limit = 255; // max autos that can trigger per turn
    while (run_auto_rules_once(s) && --limit)
        ;
}

static bool rule_can_apply(const move_rules *rule, const deck_registry *reg) {
    for (uint8_t i = 0; i < reg->n_entries; ++i) {
        if (strcmp(reg->entries[i].name, rule->deck_src) != 0)
            continue;
        for (uint8_t j = 0; j < reg->n_entries; ++j) {
            if (strcmp(reg->entries[j].name, rule->deck_dst) != 0)
                continue;
            for (card_count k = 0; k < reg->entries[i].deck->top; ++k)
                if (move_is_valid(rule, &reg->entries[i], &reg->entries[j], k))
                    return true;
        }
    }
    return false;
}

bool dsl_can_play(void *state) {
    dsl_state *s = state;
    for (uint8_t i = 0; i < s->cfg->n_move_rules; ++i)
        if (rule_can_apply(&s->cfg->mov_rules[i], s->reg))
            return true;
    return false;
}

static const CommandDispatch dsl_dispatch[] = {
    {CMD_MOV, dsl_handle_move},        {CMD_HNT, default_handle_hint}, {CMD_HLP, default_handle_unknown},
    {CMD_RST, default_handle_restart}, {CMD_QUT, default_handle_quit},
};

bool run_dsl_game(const char *filename) {
    game_cfg *cfg cleanup(free_game_cfg) = scan_game_file(filename);
    if (!cfg)
        return false;
    deck_registry *reg cleanup(free_registry) = build_registry(cfg);
    if (!reg)
        return false;

    dsl_state state = {.reg = reg, .cfg = cfg};

    const GameRunner runner = {
        .dispatch_table = dsl_dispatch,
        .dispatch_size = sizeof(dsl_dispatch) / sizeof(*dsl_dispatch),
        .can_play = dsl_can_play,
        .render = dsl_render,
        .post_turn = dsl_post_turn,
        .has_won = dsl_has_won,
    };

    return run_game(&state, &runner) == LOOP_RESTART;
}
