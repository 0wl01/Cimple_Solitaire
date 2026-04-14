#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h> // Para podermos usar bool, true e false

// Usar aspas para ficheiros locais e apontar SEMPRE para os ficheiros .h
#include "menu.h"
#include "golf.h"
#include "simon.h"

/**
 * @brief Structure to map a games name to its initialization function.
 */
typedef struct {
    const char *name;
    void (*run_func)(void); // Pointer to game running function
} GameOption;

/**
 * @brief Array containing all available games.
 */
static const GameOption games[] = {
    {"Golf", init_golf},
    {"Simple Simon", init_simple_simon}
};

// Automatically defines NUM_games trough this MACRO
#define NUM_GAMES (sizeof(games) / sizeof(games[0]))

/**
 * @brief Handles the visual rendering of the main menu.
 */
static void menu_options(void) {
    printf("\n==================================\n");
    printf("   Cimple-Solitaire   \n");
    printf("==================================\n");

    for (size_t i = 0; i < NUM_GAMES; i++) {
        printf("  [%zu] %s\n", i + 1, games[i].name);
    }

    printf("  [0] Exit\n");
    printf("==================================\n");
    printf("Pick your game: \n");
}

/**
 * @brief Reads the user's input.
 * @return The select Intenger or a -1 if the input was invalid.
 */
static int read_user_choice(void) {
    int choice;
    if (scanf("%d", &choice) != 1) {
        // Clear the input buffer to prevent infinite loops
        while (getchar() != '\n');
        return -1;
    }
    return choice;
}

/**
 * @brief Process the chosen option and launches the respetive game.
 * @param choice The intenger chosen by the user.
 * @return true to keep the menu running, false to exit the application.
 */
static bool process_choice (int choice) {
    if (choice == 0) {
        printf("Closing Cimple-Solitaire! Goodbye!\n");
        return false;
    }
    
    // Valid game exec
    if (choice > 0 && (size_t)choice <= NUM_GAMES) {
        printf("Loading %s...\n", games[choice - 1].name);
        games[choice - 1].run_func();
    }

    // Invalid input
    else {
        printf("Invalid option! Try again.\n");
    }
    return true;
}

/**
 * @brief Main entry for the Menu System
 */
void show_main_menu(void) {
    bool is_running = true;

    // Main Menu Loop
    while (is_running) {
        menu_options();
        int choice = read_user_choice();
        is_running = process_choice(choice);
    }

}