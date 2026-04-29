#include "simon.h"
#include "card.h"
#include "cli.h"
#include "game.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Seeks for the index of the bottom of a sequence(of one suit).
 */
static size sequence_start_pos(const Deck *restrict column) {
    if (IS_EMPTY(column))
        return 0;
    size i = column->top - 1;
    while (i > 0 && one_less_same_suit(column->cards[i - 1], column->cards[i]))
        i--;
    return i;
}

/**
 * @brief Checks if there is at least one possible move.
 * Selects a collumn, picks,from top to bottom ,the lowest index of a sequence of same suit and then
 * cheks if the value of the bottom card of the sequence is one unit smaller than the top card of every other collumn.
 * @return If this is true at least one time, it will return true
 * 
 * @see bottom_of_sequence
 */
static bool has_play_left(const simon_state *restrict table) {
    bool hope = false;
    for (size i = 0; i < SIMON_COLUMNS && !hope; ++i) {
        if (table->columns[i]->top > 0) {
            uint8_t bottom_val = table->columns[i]->cards[sequence_start_pos(table->columns[i])].values.value;
            uint8_t top_val = top_card(table->columns[i]).values.value;
            for (size j = 0; j < SIMON_COLUMNS && !hope; ++j) {
                uint8_t needed_val = top_card(table->columns[j]).values.value - 1;
                hope = j != i && (IS_EMPTY(table->columns[j]) || (needed_val >= top_val && needed_val <= bottom_val));
            }
        }
    }
    return hope;
}

/**
 * @brief Checks if all the 52 cards were stored, and so the playing table is empty.
 */
static bool has_won(simon_state *restrict table) {
    bool win = true;
    for (size i = 0; i < SIMON_FOUNDATIONS; ++i)
        win = win && IS_FULL(table->foundations[i]);
    return win;
}

/**
 * @brief Scans all columns for a complete sequence (King to Ace of the same suit).
 * If found, moves the entire 13-card sequence to an empty foundation.
 * @param table Pointer to the current Simple Simon game state.
 */
static void check_and_move_completed_suits(simon_state *restrict table) {
    for (size i = 0; i < SIMON_COLUMNS; i++) {
        Deck *col = table->columns[i];

        // A complete suit requires exactly 13 cards
        if (col->top >= 13) {
            size start_idx = col->top - 13;

            // Checks if the last 13 cards are a perfect sequence of the same suit
            if (sequence_is_decreasing_hierarchy(col, start_idx, col->top - 1)) {
                bool moved = false;
                // Finds the first empty foundation to store the completed suit
                for (size f = 0; f < SIMON_FOUNDATIONS && !moved; f++) {
                    if (IS_EMPTY(table->foundations[f])) {
                        split_deck(col, table->foundations[f], start_idx);
                        moved = true; // Updates the flag to naturally stop the loop
                    }
                }
            }
        }
    }
}

/**
 * @brief Makes moves certain decks, or single cards, determined by a comand 
 * 
 * If all the right conditions are met, them being, having 2 diferent decks, both must have a corrent input(A-Z characters),
 * from the index, which is a position in the source deck(the one getting its cards taken from), to the its top must be a sequence and the top most card of the 
 * destination deck(the one getting the cards after the split deck function) should have a higher value than the one at the index position at the source.
 * 
 * @param state Pointer to the active simon_state.
 * @param cmd command inputted by the player.
 * 
 * @see sequence_is_decreasing_hierarchy
 * @see split_deck
 */
static LoopSignal simon_handle_move(void *restrict state, const Command cmd) {
    simon_state *table = state;
    int src, dest;
    if (cmd.src_col < 'A' || cmd.src_col > 'J')
        return 0;
    if (cmd.dest_col < 'A' || cmd.dest_col > 'J')
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

/**
 * @brief Prints the help menu specific to the Simple Simon game.
 * @param table Pointer to the game state (unused).
 * @param cmd The command context (unused).
 * @return Always returns LOOP_CONTINUE.
 */
static LoopSignal simon_handle_help(void *restrict table, const Command cmd) {
    (void)table;
    (void)cmd;
    print_simon_help();
    return LOOP_CONTINUE;
}

/**
 * @brief Dispatch table mapping CommandTypes to their Simon-specific handler functions.
 */
static const CommandDispatch simon_dispatch[] = {
    {CMD_MOV, simon_handle_move},      {CMD_HNT, default_handle_hint}, {CMD_HLP, simon_handle_help},
    {CMD_RST, default_handle_restart}, {CMD_QUT, default_handle_quit},
};

/**
 * @brief Main game loop for the Simple Simon game.
 * Handles table rendering, user input dispatching, and checks for game over states.
 * * @param table Pointer to the active simon_state.
 * @return LOOP_RESTART if the user chooses to play again, LOOP_QUIT otherwise.
 */
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
        check_and_move_completed_suits(table);
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
    for (size i = 0; i < SIMON_FOUNDATIONS; i++) {
        table->foundations[i] = create_deck(SIMON_FOUNDATION_SIZE);
    }
}

/**
 * @brief Fill all 52 cards(shuffled) in all the 10 columns of the table.
 * Using a temporary deck, to create and shuffle the cards, moves, with split_deck, 8 cards to the first 3 columns and then reduces the
 * the number of cards by one every collumn remaing.
 * 
 * @see split_deck
 */
static void setup_columns(simon_state *restrict table) {
    Deck *temp_deck = create_deck(DEFAULT_DECK_SIZE);
    populate_deck(temp_deck);
    shuffle_deck(temp_deck);

    for (size i = 0, cards_to_deal = 8; i < SIMON_COLUMNS; ++i, cards_to_deal = cards_to_deal - (i > 2)) {
        table->columns[i] = create_deck(SIMON_COLUMN_SIZE);
        split_deck(temp_deck, table->columns[i], temp_deck->top - cards_to_deal);
    }

    eliminate_deck(&temp_deck);
}

/**
 * @brief Frees the memory at the end of the game.
 */
static void clean_simon_table(simon_state *restrict table) {
    for (size i = 0; i < SIMON_COLUMNS; ++i)
        eliminate_deck(&table->columns[i]);
    for (size i = 0; i < SIMON_FOUNDATIONS; ++i)
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
