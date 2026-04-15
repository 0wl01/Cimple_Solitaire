#include <card.h>
#include <simon.h>
/**
 * @brief Initializes the 4 foundation decks for Simple Simon.
 */
static void setup_foundations(simon_state *table) {
    for (uint8_t i = 0; i < 4; i++) {
        table->foundations[i] = create_deck(13);
    }
}

/**
 * @brief Orchestrates the complete setup of a Simple Simon game.
 */
void init_simple_simon(simon_state *table) {
    // 1. Setup factory deck
    Deck *temp_stock = create_deck(52);
    populate_deck(temp_stock);
    shuffle_deck(temp_stock);

    // 2. Delegate to helper modules
    setup_foundations(table);
    setup_columns(table, temp_stock);

    // 3. Clean up factory memory
    eliminate_deck(temp_stock);
}
