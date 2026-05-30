#include "game_engine.h"
#include "card.h"
#include "game.h"
#include "macros.h"
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include "registry.h"
#include "save.h"

Saveroom *init_saveroom(engine_state *firstTable){
    Saveroom *saveroom = malloc(sizeof(Saveroom));
    saveroom->currS[0] = firstTable;
    saveroom->currTop = 1;
    return saveroom;
}

void pop_undo(Saveroom *allsaves){
    free(allsaves->currS[allsaves->currTop-1]);
    allsaves->currTop -= 1;
}

void push_undo(Saveroom *allsaves UNUSED, engine_state *newsave){
    for(size_t i = 0; i <= newsave->reg->n_entries; ++i){
        allsaves->currS[allsaves->currTop]->reg->entries[i].deck = clone_deck(newsave->reg->entries[i].deck);
    }
    allsaves->currTop += 1;
}
static bool flag_seq_decreasing(const Deck *src, const Deck *dest UNUSED,
                                card_count index) {
  return sequence_is_decreasing(src, index, src->top - 1);
}

static bool flag_seq_increasing(const Deck *src, const Deck *dest UNUSED,
                                card_count index) {
  return sequence_is_increasing(src, index, src->top - 1);
}

static bool flag_seq_same_suit(const Deck *src, const Deck *dest UNUSED,
                               card_count index) {
  return sequence_same_suit(src, index, src->top - 1);
}

static bool flag_seq_alt_color(const Deck *src, const Deck *dest UNUSED,
                               card_count index) {
  return sequence_alternating_color(src, index, src->top - 1);
}

static bool flag_seq_alt_suit(const Deck *src, const Deck *dest UNUSED,
                              card_count index) {
  return sequence_alternating_suit(src, index, src->top - 1);
}

static bool flag_seq_same_color(const Deck *src, const Deck *dest UNUSED,
                                card_count index) {
  return sequence_same_color(src, index, src->top - 1);
}

static bool flag_less(const Deck *src, const Deck *dest,
                      card_count UNUSED index) {
  return card_value(top_card(src)) == card_value(top_card(dest)) - 1;
}

static bool flag_greater(const Deck *src, const Deck *dest,
                         card_count UNUSED index) {
  return card_value(top_card(src)) == card_value(top_card(dest)) + 1;
}

static bool flag_same_suit_dest(const Deck *src, const Deck *dest,
                                card_count UNUSED index) {
  return cards_same_suit(top_card(src), top_card(dest));
}

static bool flag_diff_suit_dest(const Deck *src, const Deck *dest,
                                card_count UNUSED index) {
  return !cards_same_suit(top_card(src), top_card(dest));
}

static bool flag_same_color_dest(const Deck *src, const Deck *dest,
                                 card_count UNUSED index) {
  return card_color(top_card(src)) == card_color(top_card(dest));
}

static bool flag_diff_color_dest(const Deck *src, const Deck *dest,
                                 card_count UNUSED index) {
  return card_color(top_card(src)) != card_color(top_card(dest));
}

static bool flag_dest_empty(const Deck *src UNUSED, const Deck *dest,
                            card_count UNUSED index) {
  return is_deck_empty(dest);
}

static bool flag_top_is_ace(const Deck *src, const Deck *dest UNUSED,
                            card_count UNUSED index) {
  return card_value(top_card(src)) == CARD_ACE;
}

static bool flag_top_is_king(const Deck *src, const Deck *dest UNUSED,
                             card_count UNUSED index) {
  return card_value(top_card(src)) == CARD_KING;
}

static bool flag_bottom_is_ace(const Deck *src, const Deck *dest UNUSED,
                               card_count index) {
  return card_value(src->cards[index]) == CARD_ACE;
}

static bool flag_bottom_is_king(const Deck *src, const Deck *dest UNUSED,
                                card_count index) {
  return card_value(src->cards[index]) == CARD_KING;
}

static bool flag_tilde(const Deck *src, const Deck *dest, card_count index) {
  return flag_less(src, dest, index) || flag_greater(src, dest, index);
}

static bool flag_star(const Deck *src UNUSED, const Deck *dest UNUSED,
                      card_count UNUSED index) {
  return true;
}

static bool check_tipo_flags(const deck_entry *dest) {
  return strchr(dest->flags, '1') ? is_deck_empty(dest->deck) : true;
}

constexpr uint8_t TABLE_SIZE = 20;
static const FlagDispatch flag_table[TABLE_SIZE] = {
    {'*', flag_star},
    {'V', flag_dest_empty},
    {'<', flag_less},
    {'>', flag_greater},
    {'~', flag_tilde},
    {'m', flag_seq_same_suit},
    {'M', flag_same_suit_dest},
    {'x', flag_seq_alt_suit},
    {'X', flag_diff_suit_dest},
    {'c', flag_seq_same_color},
    {'C', flag_same_color_dest},
    {'d', flag_seq_alt_color},
    {'D', flag_diff_color_dest},
    {'a', flag_top_is_ace},
    {'A', flag_bottom_is_ace},
    {'k', flag_top_is_king},
    {'K', flag_bottom_is_king},
    {'[', flag_seq_decreasing},
    {']', flag_seq_increasing},
    {'+', flag_star},
};

static bool check_single_flag(char flag, const Deck *src, const Deck *dest, card_count index) {
    for(size_t i = 0; i < TABLE_SIZE; ++i){
        if(flag_table[i].flag == flag) return(flag_table[i].check(src, dest, index));
    }
    return true;
}

static bool check_mov_flags(const char *flags, const deck_entry *src,
                            const deck_entry *dest, card_count index) {
  for (uint8_t i = 0; flags[i] != '\0'; ++i)
    if (!check_single_flag(flags[i], src->deck, dest->deck, index))
      return false;
  return true;
}

static bool move_is_valid(const move_rules *rule, const deck_entry *src,
                          const deck_entry *dest, card_count index) {
  if (is_deck_empty(src->deck)) {
    return false;
  }
  if (!check_tipo_flags(dest)) {
    return false;
  }
  if (!strchr(rule->flags, '+') && index != src->deck->top - 1) {
    return false; // no sequence allowed, must be single card
  }
  return check_mov_flags(rule->flags, src, dest, index);
}

bool can_move_rule(const move_rules *rule, const deck_entry *src,
                   const deck_entry *dest, card_count index) {
  return strcmp(src->name, rule->deck_src) == 0 &&
         strcmp(dest->name, rule->deck_dst) == 0 &&
         move_is_valid(rule, src, dest, index);
}

static bool try_apply_move(const game_cfg *cfg, deck_entry *src,
                           deck_entry *dest, card_count index) {
  for (uint8_t i = 0; i < cfg->n_move_rules; ++i) {
    if (!can_move_rule(&cfg->mov_rules[i], src, dest, index))
      continue;
    bool moved = split_deck(src->deck, dest->deck, index);
    if (moved && strchr(dest->flags, '='))
      unflip_all(dest->deck);
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
  engine_state *s = state;
  int8_t src_i = col_to_index(cmd.src_col);
  int8_t dest_i = col_to_index(cmd.dest_col);
  if (src_i < 0 || dest_i < 0 || src_i >= s->reg->n_entries ||
      dest_i >= s->reg->n_entries)
    return LOOP_CONTINUE;
  deck_entry *src = &s->reg->entries[src_i];
  deck_entry *dest = &s->reg->entries[dest_i];
  try_apply_move(s->cfg, src, dest, cmd.index);
  return LOOP_CONTINUE;
}

static bool win_condition_met(const win_condition *cond,
                              const deck_registry *reg) {
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
  engine_state *s = state;
  for (uint8_t i = 0; i < s->cfg->n_winconds; ++i)
    if (!win_condition_met(&s->cfg->conditions[i], s->reg))
      return false;
  return true;
}

static bool try_auto_src(deck_entry *src, const auto_rule *rule, deck_registry *reg) {
    bool triggered = false;
    for (uint8_t j = 0; j < reg->n_entries; ++j) {
        if (strcmp(reg->entries[j].name, rule->deck_dst) == 0
            && move_is_valid((move_rules *)rule, src, &reg->entries[j], src->deck->top - 1)) {
            split_deck(src->deck, reg->entries[j].deck, src->deck->top - 1);
            triggered = true;
        }
    }
    return triggered;
}

static bool try_auto_rule(const auto_rule *rule, deck_registry *reg) {
    bool triggered = false;
    for (uint8_t i = 0; i < reg->n_entries; ++i) {
        if (strcmp(reg->entries[i].name, rule->deck_src) == 0)
            triggered = try_auto_src(&reg->entries[i], rule, reg) || triggered;
    }
    return triggered;
}



static bool run_auto_rules_once(engine_state *s) {
  bool triggered = false;
  for (uint8_t i = 0; i < s->cfg->n_auto_rules; ++i)
    triggered = try_auto_rule(&s->cfg->auto_rules[i], s->reg) || triggered;
  return triggered;
}

void dsl_post_turn(void *state) {
    engine_state *s = state;
    uint8_t limit = 255; // max autos that can trigger per turn
    while (run_auto_rules_once(s) && --limit)
        ;
}

static bool entry_can_reach(const move_rules *rule, const deck_entry *src,
                             const deck_entry *dst) {
    card_count k = 0;
    while (k < src->deck->top && !move_is_valid(rule, src, dst, k))
        ++k;
    return k < src->deck->top;
}

static bool src_can_reach_dst(const move_rules *rule, const deck_entry *src,
                               const deck_registry *reg) {
    for (uint8_t j = 0; j < reg->n_entries; ++j)
        if (strcmp(reg->entries[j].name, rule->deck_dst) == 0
            && entry_can_reach(rule, src, &reg->entries[j]))
            return true;
    return false;
}

static bool rule_can_apply(const move_rules *rule, const deck_registry *reg) {
    for (uint8_t i = 0; i < reg->n_entries; ++i)
        if (strcmp(reg->entries[i].name, rule->deck_src) == 0
            && src_can_reach_dst(rule, &reg->entries[i], reg))
            return true;
    return false;
}

bool dsl_can_play(void *state) {
  engine_state *s = state;
  for (uint8_t i = 0; i < s->cfg->n_move_rules; ++i)
    if (rule_can_apply(&s->cfg->mov_rules[i], s->reg))
      return true;
  return false;
}

static LoopSignal handle_loadfile(void *state, Command cmd) {
    engine_state *s = state;
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", s->folder, cmd.filename);

    game_cfg *new_cfg = scan_game_file(path);
    if (!new_cfg) return LOOP_CONTINUE;

    deck_registry *new_reg = build_registry(new_cfg);
    if (!new_reg) { free_game_cfg(&new_cfg); return LOOP_CONTINUE; }

    free_registry(&s->reg);
    free_game_cfg(&s->cfg);

    s->reg = new_reg;
    s->cfg = new_cfg;
    strncpy(s->dsl_filename, cmd.filename, sizeof(s->dsl_filename) - 1);
    return LOOP_CONTINUE;
}

static LoopSignal handle_save(void *state, const Command UNUSED cmd) {
    engine_state *s = state;
    save_game(s->reg, s->cfg, s->dsl_filename);
    return LOOP_CONTINUE;
}

static LoopSignal handle_load(void *state, Command UNUSED cmd) {
    engine_state *s = state;
    load_game(&s->reg, s->cfg, "save.paciencia");
    return LOOP_CONTINUE;
}

static const CommandDispatch dsl_dispatch[] = {
    {CMD_MOV, dsl_handle_move},        {CMD_HNT, default_handle_hint},
    {CMD_HLP, default_handle_help}, {CMD_RST, default_handle_restart},
    {CMD_QUT, default_handle_quit}, {CMD_SAV, handle_save}, {CMD_LOD, handle_load},
    {CMD_LDF, handle_loadfile}
};

bool run_dsl_game(const char *filename, const char *folder) {
    game_cfg *cfg = scan_game_file(filename);   // sem cleanup
    if (!cfg) return false;
    deck_registry *reg = build_registry(cfg);   // sem cleanup
    if (!reg) { free_game_cfg(&cfg); return false; }
    // iniciamos o state sem o filename
    engine_state state = {
        .reg = reg, .cfg = cfg, .folder = folder
    };
    // e copiamos o nome do ficheiro
    strncpy(state.dsl_filename, filename, sizeof(state.dsl_filename) - 1);
    state.dsl_filename[sizeof(state.dsl_filename) - 1] = '\0';

    const GameRunner runner = {
        .dispatch_table = dsl_dispatch,
        .dispatch_size  = sizeof(dsl_dispatch) / sizeof(*dsl_dispatch),
        .can_play       = dsl_can_play,
        .render         = print_game_table,
        .post_turn      = dsl_post_turn,
        .has_won        = dsl_has_won,
    };

    LoopSignal sig = run_game(&state, &runner);

    free_registry(&state.reg);    // liberta o que estiver no state no fim
    free_game_cfg(&state.cfg);    // (pode ser o original ou um substituído pelo f)

    return sig == LOOP_RESTART;
}
