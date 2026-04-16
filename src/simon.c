#include <card.h>
#include <simon.h>
#include <stddef.h>

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
static void run_simon(simon_state *restrict table) {}

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
void init_simple_simon() {
    simon_state table;
    setup_foundations(&table);
    setup_columns(&table);

    run_simon(&table);
    clean_simon_table(&table);
}
