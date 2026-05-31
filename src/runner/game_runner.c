#include "game_runner.h"
#include "bitarr.h"
#include <stdlib.h>
#include <string.h>

// ==================== ALLOCATION & CLEANUP ====================

static void destroy_deck(deck_t *deck) {
    if (deck) {
        free(deck->id);
        destroy_bit_arr(&deck->flags);
        free(deck);
    }
}

static void deal_initial_cards(const game_state_t *state, deck_t *master) {
    for (size_t i = 0; i < state->deck_count; ++i) {
        size_t to_deal = state->rules->deck_recipes[i].starting_cards;
        for (size_t j = 0; j < to_deal; ++j) {
            deal(master, state->decks[i]);
        }
    }
}

static size_t count_required_cards(const paciencia_game_t *rules) {
    size_t total = 0;
    for (size_t i = 0; i < rules->decks_to_create; ++i) {
        total += rules->deck_recipes[i].starting_cards;
    }
    return total;
}

static void setup_master_and_deal(const game_state_t *state) {
    size_t req = count_required_cards(state->rules);
    size_t pack_size = req < COMMON_DECK_SIZE ? COMMON_DECK_SIZE : req;
    deck_t *master = create_deck("master", "", pack_size);
    if (master) {
        fill_deck_with_cards(&master, pack_size);
        shuffle_deck(master);
        deal_initial_cards(state, master);
        destroy_deck(master);
    }
}

static bool create_state_decks(const game_state_t *state) {
    bool success = true;
    for (size_t i = 0; i < state->deck_count; ++i) {
        deck_recipe_t *r = &state->rules->deck_recipes[i];
        state->decks[i] = create_deck(r->id, r->metadata, 52);
        if (!state->decks[i]) {
            success = false;
        }
    }
    return success;
}

game_state_t *init_game_state(const paciencia_game_t *rules) {
    game_state_t *state = calloc(1, sizeof(game_state_t));
    if (state && rules) {
        state->rules = rules;
        state->deck_count = rules->decks_to_create;
        state->decks = calloc(state->deck_count, sizeof(deck_t*));
        if (state->decks && create_state_decks(state)) {
            setup_master_and_deal(state);
        }
    }
    return state;
}

void free_game_state(game_state_t **state) {
    if (state && *state) {
        if ((*state)->decks) {
            for (size_t i = 0; i < (*state)->deck_count; ++i) {
                destroy_deck((*state)->decks[i]);
            }
            free((*state)->decks);
        }
        free(*state);
        *state = NULL;
    }
}

// ==================== RULE VALIDATION HELPERS ====================

static bool req_empty(const move_rule_t *rule, bool is_empty) {
    return access_bit_arr(MOV_DEST_EMPTY, rule->flags) ? is_empty : !is_empty;
}

static bool req_less(const move_rule_t *rule, card b, card d) {
    return access_bit_arr(MOV_TOP_LESS, rule->flags) ? cards_is_one_less(b, d) : true;
}

static bool req_greater(const move_rule_t *rule, card b, card d) {
    return access_bit_arr(MOV_TOP_GREATER, rule->flags) ? cards_is_one_more(b, d) : true;
}

static bool req_suit(const move_rule_t *rule, card b, card d) {
    return access_bit_arr(MOV_DEST_SUIT, rule->flags) ? cards_same_suit(b, d) : true;
}

static bool req_diff_suit(const move_rule_t *rule, card b, card d) {
    return access_bit_arr(MOV_DEST_DIFF_SUIT, rule->flags) ? cards_different_suit(b, d) : true;
}

static bool validate_dest_full(const move_rule_t *rule, card b, card d) {
    bool ok = true;
    if (ok && !req_less(rule, b, d)) ok = false;
    if (ok && !req_greater(rule, b, d)) ok = false;
    if (ok && !req_suit(rule, b, d)) ok = false;
    if (ok && !req_diff_suit(rule, b, d)) ok = false;
    if (access_bit_arr(MOV_TOP_ADJACENT, rule->flags)) {
        if (ok && !cards_are_adjacent(b, d)) ok = false;
    }
    return ok;
}

static bool validate_card_props(const move_rule_t *rule, card b, card t) {
    bool ok = true;
    if (ok && access_bit_arr(MOV_BOT_ACE, rule->flags)) ok = card_is_ace(b);
    if (ok && access_bit_arr(MOV_BOT_KING, rule->flags)) ok = card_is_king(b);
    if (ok && access_bit_arr(MOV_TOP_ACE, rule->flags)) ok = card_is_ace(t);
    if (ok && access_bit_arr(MOV_TOP_KING, rule->flags)) ok = card_is_king(t);
    return ok;
}

// ==================== SEQUENCE VALIDATORS ====================

typedef bool (*seq_check_fn)(card, card);

static bool validate_sequence(deck_t *src, size_t amount, seq_check_fn checker) {
    bool valid = true;
    for (size_t i = 0; i < amount - 1; ++i) {
        card upper = src->cards[src->top - amount + i];
        card lower = src->cards[src->top - amount + i + 1];
        if (valid && !checker(upper, lower)) {
            valid = false;
        }
    }
    return valid;
}

static bool check_seq_pair_dec(card u, card l) { return card_value(u) == card_value(l) + 1; }
static bool check_seq_pair_inc(card u, card l) { return card_value(u) + 1 == card_value(l); }

static bool validate_seq_order(move_rule_t *r, deck_t *src, size_t amt, bool ok) {
    if (ok && access_bit_arr(MOV_SEQ_DEC, r->flags))
        ok = validate_sequence(src, amt, check_seq_pair_dec);
    if (ok && access_bit_arr(MOV_SEQ_INC, r->flags))
        ok = validate_sequence(src, amt, check_seq_pair_inc);
    return ok;
}

static bool validate_seq_suits(move_rule_t *r, deck_t *src, size_t amt, bool ok) {
    if (ok && access_bit_arr(MOV_SEQ_SUIT, r->flags))
        ok = validate_sequence(src, amt, cards_same_suit);
    if (ok && access_bit_arr(MOV_SEQ_ALT_SUIT, r->flags))
        ok = validate_sequence(src, amt, cards_different_suit);
    return ok;
}

static bool validate_seq_colors(move_rule_t *r, deck_t *src, size_t amt, bool ok) {
    if (ok && access_bit_arr(MOV_SEQ_COLOR, r->flags))
        ok = validate_sequence(src, amt, cards_same_color);
    if (ok && access_bit_arr(MOV_SEQ_ALT_COLOR, r->flags))
        ok = validate_sequence(src, amt, cards_different_color);
    return ok;
}

static bool validate_seq_props(move_rule_t *r, deck_t *src, size_t amt) {
    bool ok = validate_seq_order(r, src, amt, true);
    ok = validate_seq_suits(r, src, amt, ok);
    return validate_seq_colors(r, src, amt, ok);
}

static bool validate_amount(move_rule_t *r, size_t amt) {
    return (amt > 1) ? access_bit_arr(MOV_SEQUENCE, r->flags) : true;
}

// ==================== CORE RULE PROCESSOR ====================

static bool is_rule_valid(game_state_t *st, move_rule_t *r, size_t s, size_t d, size_t amt) {
    deck_t *src = st->decks[s];
    deck_t *dest = st->decks[d];
    if (amt == 0 || src->top < amt || !validate_amount(r, amt)) return false;
    if (access_bit_arr(MOV_ANY, r->flags)) return true;

    bool dest_empty = is_deck_ptr_empty(dest);
    if (!req_empty(r, dest_empty)) return false;

    card b_card = src->cards[src->top - amt];
    card t_card = src->cards[src->top - 1];
    if (!validate_card_props(r, b_card, t_card)) return false;

    if (!dest_empty && !validate_dest_full(r, b_card, dest->cards[dest->top - 1])) {
        return false;
    }
    return amt > 1 ? validate_seq_props(r, src, amt) : true;
}

static bool match_rule_ids(game_state_t *st, move_rule_t *r, size_t s, size_t d) {
    bool src_match = strcmp(st->decks[s]->id, r->src_id) == 0;
    bool dest_match = strcmp(st->decks[d]->id, r->dest_id) == 0;
    return src_match && dest_match;
}

bool is_move_valid(const game_state_t *state, size_t src_idx, size_t dest_idx, size_t amount) {
    bool valid = false;
    for (size_t i = 0; i < state->rules->move_rules_count; ++i) {
        if (!valid && match_rule_ids(state, &state->rules->move_rules[i], src_idx, dest_idx)) {
            if (is_rule_valid(state, &state->rules->move_rules[i], src_idx, dest_idx, amount)) {
                valid = true;
            }
        }
    }
    return valid;
}

bool execute_move(game_state_t *state, size_t src_idx, size_t dest_idx, size_t amount) {
    bool valid = is_move_valid(state, src_idx, dest_idx, amount);
    if (valid) {
        size_t pos = state->decks[src_idx]->top - amount;
        split_deck(pos, state->decks[src_idx], &state->decks[dest_idx]);
    }
    return valid;
}

// ==================== AUTO MOVES & WIN CONDITIONS ====================

static bool check_and_execute_auto(game_state_t *st, move_rule_t *r, size_t s, size_t d) {
    bool moved = false;
    if (match_rule_ids(st, r, s, d)) {
        size_t max_amt = st->decks[s]->top;
        for (size_t amt = max_amt; amt > 0; --amt) {
            if (!moved && is_rule_valid(st, r, s, d, amt)) {
                size_t pos = st->decks[s]->top - amt;
                split_deck(pos, st->decks[s], &st->decks[d]);
                moved = true;
            }
        }
    }
    return moved;
}

static bool try_auto_rule(game_state_t *state, move_rule_t *rule) {
    bool moved = false;
    for (size_t s = 0; s < state->deck_count; ++s) {
        for (size_t d = 0; d < state->deck_count; ++d) {
            if (!moved) {
                moved = check_and_execute_auto(state, rule, s, d);
            }
        }
    }
    return moved;
}

void execute_auto_moves(game_state_t *state) {
    bool moved = true;
    while (moved) {
        moved = false;
        for (size_t i = 0; i < state->rules->auto_move_rules_count; ++i) {
            if (!moved) {
                moved = try_auto_rule(state, &state->rules->auto_move_rules[i]);
            }
        }
    }
}

static bool check_win_condition(game_state_t *state, win_condition_t *cond) {
    bool condition_met = true;
    for (size_t i = 0; i < state->deck_count; ++i) {
        if (strcmp(state->decks[i]->id, cond->id) == 0) {
            if (state->decks[i]->top != cond->card_count) {
                condition_met = false;
            }
        }
    }
    return condition_met;
}

bool check_win_condition_met(const game_state_t *state) {
    bool won = true;
    for (size_t i = 0; i < state->rules->win_conditions_count; ++i) {
        if (!check_win_condition(state, &state->rules->win_conditions[i])) {
            won = false;
        }
    }
    return won;
}
