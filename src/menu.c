#include "menu.h"
#include "cli.h"
#include "golf.h"
#include "simon.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Array containing all available games.
 */
static const GameOption games[] = {{"Golf", init_golf}, {"Simple Simon", init_simple_simon}};

static bool process_choice(char choice) {
    if (choice == 'q') {
        printf("Closing Cimple-Solitaire! Goodbye!\n");
        return false;
    }
    if (choice > 0 && (size_t)choice <= NUM_GAMES) {
        printf("Loading %s...\n", games[choice - 1].name);
        games[choice - 1].init_game_fun();
    }

    // Invalid input
    else {
        printf("Invalid option! Try again.\n");
    }
    return true;
}

void show_main_menu() {
    char choice;
    do {
        print_menu(NUM_GAMES, games);
        choice = menu_get_input();
    } while (process_choice(choice));
}
