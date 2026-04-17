#include "cli.h"
#include <card.h>
#include <simon.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

static int simon_handle_move(simon_state *restrict table, const Command cmd) {
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

static int simon_handle_restart(simon_state *restrict table, const Command cmd) {
    (void)table;
    (void)cmd;
    return 1;
}

static int simon_handle_quit(simon_state *restrict table, const Command cmd) {
    (void)table;
    (void)cmd;
    return -1;
}

static int simon_handle_hint(simon_state *restrict table, const Command cmd) {
    (void)table;
    (void)cmd;
    return 0;
}

static int simon_handle_help(simon_state *restrict table, const Command cmd) {
    (void)table;
    (void)cmd;
    print_simon_help();
    return 0;
}

// TODO: docs
static const CommandDispatch simon_dispatch[] = {
    {CMD_MOV, simon_handle_move},    {CMD_HNT, simon_handle_hint}, {CMD_HLP, simon_handle_help},
    {CMD_RST, simon_handle_restart}, {CMD_QUT, simon_handle_quit},
};

// TODO: docs
static int dispatch(const CommandDispatch *table, const size_t table_size, simon_state *state, Command cmd) {
    for (size_t i = 0; i < table_size; ++i)
        if (table[i].type == cmd.type)
            return table[i].handler(state, cmd);
    print_unknown_command();
    return 0;
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
static bool can_play(simon_state *restrict table) {}

// TODO
static bool run_simon(simon_state *restrict table) {
    int result = 0;
    Command cmd;
    while (!result) {
        print_table(&(TableLayout){.columns = table->columns,
                                   .foundations = table->foundations,
                                   .waste = NULL,
                                   .stock = NULL,
                                   .n_columns = SIMON_COLUMNS,
                                   .n_foundations = SIMON_FOUNDATIONS});
        print_prompt();
        result = dispatch(simon_dispatch, sizeof(simon_dispatch) / sizeof(simon_dispatch[0]), table, game_get_input());
    }
    if (!can_play(table) || result == 2) {
        print_end(result == 2);
        print_prompt();
        cmd = game_get_input();
        result = cmd.type == CMD_YES;
    }
    return result == 1;
}

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

    run_simon(&table);
    clean_simon_table(&table);
    return false;
}
