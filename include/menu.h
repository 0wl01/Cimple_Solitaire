#ifndef MENU_H
#define MENU_H
#include <stdbool.h>
#define NUM_GAMES (sizeof(games) / sizeof(games[0]))

/**
 * @brief Structure to map a games name to its initialization function.
 */
typedef struct {
    const char *name;
    bool (*init_game_fun)(void); // pointer to game init
} GameOption;

/**
 * @brief Starts the interactive main menu for the Cimple Solitaire.
 * * Clears the screen and keeps the user in a continuous loop until 
 * they explicitly choose to exit the application.
 */
void show_main_menu(void);

#endif
