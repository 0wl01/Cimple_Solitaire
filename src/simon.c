#include "simon.h"
#include "card.h"
#include "cli.h"
#include "game.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

static size_t sequence_start_pos(const Deck *restrict column) {
    if (IS_EMPTY(column))
        return 0;
    size_t i = column->top - 1;
    while (i > 0 && one_less_same_suit(column->cards[i - 1], column->cards[i]))
        i--;
    return i;
}

static bool has_play_left(const simon_state *restrict table) {
    bool hope = false;
    for (size_t i = 0; i < SIMON_COLUMNS && !hope; i++) {
        if (table->columns[i]->top > 0) {
            size_t bottom = sequence_start_pos(table->columns[i]);
            Card moving = table->columns[i]->cards[bottom];
            for (size_t j = 0; j < SIMON_COLUMNS && !hope; j++)
                hope = j != i && is_one_less(moving, top_card(table->columns[j]));
        }
    }
    return hope;
}

static bool has_won(simon_state *restrict table) {
    bool win = false;
    for (size_t i = 0; i < SIMON_FOUNDATIONS; ++i)
        win = win && IS_FULL(table->foundations[i]);
    return win;
}
// TODO: docs
static LoopSignal simon_handle_move(void *restrict state, const Command cmd) {
    simon_state *table = state;
    int src, dest;
    if (cmd.src_col < 'A' || cmd.src_col > 'Z')
        return 0;
    if (cmd.dest_col < 'A' || cmd.dest_col > 'Z')
        return 0;
    if (cmd.src_col == cmd.dest_col)
        return 0;
    src = cmd.src_col - 'A';
    dest = cmd.dest_col - 'A';
    if (cmd.index >= table->columns[src]->top)
        return 0;
    if (!sequence_is_decreasing_hierarchy(table->columns[src], cmd.index, table->columns[src]->top - 1))
        return 0;
    if (!is_one_less(top_card(table->columns[dest]), table->columns[src]->cards[cmd.index]))
        return 0;
    split_deck(table->columns[src], table->columns[dest], cmd.index);
    return 0;
}

// TODO: docs
static LoopSignal simon_handle_help(void *restrict table, const Command cmd) {
    (void)table;
    (void)cmd;
    print_simon_help();
    return LOOP_CONTINUE;
}

// TODO: docs
static const CommandDispatch simon_dispatch[] = {
    {CMD_MOV, simon_handle_move},      {CMD_HNT, default_handle_hint}, {CMD_HLP, simon_handle_help},
    {CMD_RST, default_handle_restart}, {CMD_QUT, default_handle_quit},
};

static LoopSignal run_simon(simon_state *restrict table) {
    LoopSignal sig = LOOP_CONTINUE;
    while (sig == LOOP_CONTINUE && has_play_left(table)) {
        print_table(&(TableLayout){.columns = table->columns,
                                   .foundations = table->foundations,
                                   .stock = NULL,
                                   .waste = NULL,
                                   .n_columns = SIMON_COLUMNS,
                                   .n_foundations = SIMON_FOUNDATIONS});
        print_prompt();
        sig = dispatch(simon_dispatch, sizeof(simon_dispatch) / sizeof(simon_dispatch[0]), table, game_get_input());
    }
    if (!has_play_left(table)) {
        print_end(has_won(table));
        print_prompt();
        if (game_get_input().type == CMD_YES)
            sig = LOOP_RESTART;
    }
    return sig;
}

/**
 * @brief Initializes the 4 foundation decks for Simple Simon.
 */
static void setup_foundations(simon_state *restrict table) {
    for (size_t i = 0; i < SIMON_FOUNDATIONS; i++) {
        table->foundations[i] = create_deck(SIMON_FOUNDATION_SIZE);
    }
}

// TODO: needs docs
static void setup_columns(simon_state *restrict table) {
    Deck *temp_deck = create_deck(DEFAULT_DECK_SIZE);
    populate_deck(temp_deck);
    shuffle_deck(temp_deck);

    for (size_t i = 0, cards_to_deal = 8; i < SIMON_COLUMNS; ++i, cards_to_deal = cards_to_deal - (i > 2)) {
        table->columns[i] = create_deck(SIMON_COLUMN_SIZE);
        split_deck(temp_deck, table->columns[i], temp_deck->top - cards_to_deal);
    }

    eliminate_deck(&temp_deck);
}

// TODO

// TODO: docs
static void clean_simon_table(simon_state *restrict table) {
    for (size_t i = 0; i < SIMON_COLUMNS; ++i)
        eliminate_deck(&table->columns[i]);
    for (size_t i = 0; i < SIMON_FOUNDATIONS; ++i)
        eliminate_deck(&table->foundations[i]);
}

/**
 * @brief Orchestrates the complete setup of a Simple Simon game.
 */
bool init_simple_simon() {

    simon_state table;
    setup_foundations(&table);
    setup_columns(&table);
    LoopSignal exit_sig = run_simon(&table);
    clean_simon_table(&table);
    return exit_sig == LOOP_RESTART;
}
