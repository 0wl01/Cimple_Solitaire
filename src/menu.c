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

static void print_menu(const size_t games_length, const GameOption *restrict games) {
    printf("\n==================================\n");
    printf("      C-litaire      \n");
    printf("==================================\n");

    for (size_t i = 0; i < games_length; ++i)
        printf("  [%zu] %s\n", i + 1, games[i].name);
    printf("  [q] Exit\n");
    printf("==================================\n");
    printf("Pick your game: \n");
}

/**
 * @brief Processes the user's choice from the main menu.
 * Launches the selected game or triggers the application exit sequence.
 * * @param choice The character inputted by the user.
 * @return false if the user chooses to quit ('q'), true otherwise.
 */
static bool process_choice(char choice) {
    if (choice == 'q') {
        printf("Closing C-litaire! Goodbye!\n");
        return false;
    }
    choice -= '0';
    if (choice > 0 && (size_t)choice <= NUM_GAMES) {
        printf("Loading %s...\n", games[choice - 1].name);
        while (games[choice - 1].init_game_fun())
            ;
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
