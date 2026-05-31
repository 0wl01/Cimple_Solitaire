#include "run.h"
#include "input.h"
#include "render.h"
#include "save.h"
#include "game_runner.h"
#include <string.h>

#define MAX_UNDO 50

static game_state_t *clone_game_state(const game_state_t *state) {
    if (!state) return NULL;
    game_state_t *new_state = calloc(1, sizeof(game_state_t));
    if (new_state) {
        new_state->rules = state->rules;
        new_state->deck_count = state->deck_count;
        new_state->decks = calloc(new_state->deck_count, sizeof(deck_t*));
        for (size_t i = 0; new_state->decks && i < state->deck_count; ++i) {
            new_state->decks[i] = clone_deck(state->decks[i]);
        }
    }
    return new_state;
}

typedef struct {
    game_state_t *states[MAX_UNDO];
    size_t head;
    size_t count;
} undo_stack_t;

static void push_undo(undo_stack_t *u, const game_state_t *s) {
    size_t idx = (u->head + u->count) % MAX_UNDO;
    if (u->count == MAX_UNDO) {
        free_game_state(&u->states[u->head]);
        u->head = (u->head + 1) % MAX_UNDO;
        u->count--;
    }
    u->states[idx] = clone_game_state(s);
    u->count++;
}

static game_state_t *pop_undo(undo_stack_t *u) {
    game_state_t *res = NULL;
    if (u->count > 0) {
        u->count--;
        size_t idx = (u->head + u->count) % MAX_UNDO;
        res = u->states[idx];
        u->states[idx] = NULL;
    }
    return res;
}

static void free_undo(undo_stack_t *u) {
    while (u->count > 0) {
        game_state_t *state = pop_undo(u);
        free_game_state(&state);
    }
}

static size_t char_to_idx(char c) {
    return (c >= 'a') ? c - 'a' + 26 : (c >= 'A' ? c - 'A' : 0);
}

static void handle_move(game_state_t **st, undo_stack_t *u, Command cmd) {
    size_t src = char_to_idx(cmd.src_col);
    size_t dest = char_to_idx(cmd.dest_col);
    if (is_move_valid(*st, src, dest, cmd.index)) {
        push_undo(u, *st);
        execute_move(*st, src, dest, cmd.index);
        execute_auto_moves(*st);
    } else {
        print_unknown_command();
    }
}

static bool process_cmd(game_state_t **st, undo_stack_t *u, Command cmd, const char *path) {
    bool playing = true;
    if (cmd.type == CMD_QUT) playing = false;
    else if (cmd.type == CMD_MOV) handle_move(st, u, cmd);
    else if (cmd.type == CMD_UND) {
        game_state_t *prev = pop_undo(u);
        if (prev) { free_game_state(st); *st = prev; }
    }
    else if (cmd.type == CMD_SAV) save_game_file(*st, cmd.args, path);
    else if (cmd.type == CMD_LOD) {
        game_state_t *loaded = load_game_file((*st)->rules, cmd.args);
        if (loaded) { push_undo(u, *st); free_game_state(st); *st = loaded; }
    }
    else if (cmd.type == CMD_HLP) print_game_help();
    else print_unknown_command();
    return playing;
}

static bool game_loop(game_state_t *state, const char *path) {
    undo_stack_t undo = {0};
    bool playing = true;
    execute_auto_moves(state);
    while (playing && !check_win_condition_met(state)) {
        print_game_table(state);
        print_prompt();
        Command cmd = game_get_input();
        playing = process_cmd(&state, &undo, cmd, path);
    }
    if (check_win_condition_met(state)) print_end(true);
    free_undo(&undo);
    free_game_state(&state);
    return false;
}

bool run_dsl_game(const char *path) {
    paciencia_game_t *rules = scan_paciencia_game_file(path);
    if (!rules) return false;
    game_state_t *state = init_game_state(rules);
    bool want_restart = state ? game_loop(state, path) : false;
    free_game_resources(&rules);
    return want_restart;
}
