#include "menu.h"
#include "render.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "input.h"
#include "macros.h"
#include "game_engine.h"

/**
 * @brief Array containing all available games.
 */

static bool has_script_extension(const char *filename) {
    size_t flen = strlen(filename);
    size_t elen = strlen(SCRIPT_EXTENSION);
    return flen >= elen && strcmp(filename + flen - elen, SCRIPT_EXTENSION) == 0;
}


static uint8_t load_files(const char *folder, char names[MAX_GAMES][256]) {
    DIR *dir = opendir(folder);
    if (!dir) { perror(folder); return 0; }
    uint8_t n = 0;
    struct dirent *entry;
    while (n < MAX_GAMES && (entry = readdir(dir)))
        if (has_script_extension(entry->d_name))
            strncpy(names[n++], entry->d_name, 255);
    closedir(dir);
    return n;
}



static void print_menu_page(const char *folder, char names[MAX_GAMES][256],
                             uint8_t n, uint8_t page) {
    uint8_t start = page * 8;
    uint8_t end   = start + 8 < n ? start + 8 : n;
    printf("\n=== C-litaire (%s) ===\n", folder);
    for (uint8_t i = start; i < end; ++i)
        printf("  [%d] %s\n", i - start + 1, names[i]);
    if (page > 0)  printf("  [0] Previous page\n");
    if (end < n)   printf("  [9] Next page\n");
    printf("  [q] Exit\n");
}

static void launch_game(const char *folder, const char *name) {
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", folder, name);
    while (run_dsl_game(path))
        ;
}

static void process_choice(char c, const char *folder,
                            char names[MAX_GAMES][256],
                            uint8_t n, uint8_t page) {
    uint8_t idx = page * 8 + (c - '1');
    if (c >= '1' && c <= '8' && idx < n)
        launch_game(folder, names[idx]);
    else
        printf("Invalid choice.\n");
}

void show_main_menu(const char *folder) {
    char c = 0, (*names)[256] = malloc(MAX_GAMES * sizeof(*names));
    if (!names) return;
    uint8_t n    = load_files(folder, names), page = 0;
    while (c != 'q') {
        print_menu_page(folder, names, n, page);
        print_prompt();
        c = menu_get_input();
        if      (c == '9' && (page + 1) * 8 < n) ++page;
        else if (c == '0' && page > 0)            --page;
        else if (c != 'q') process_choice(c, folder, names, n, page);
    }
    free(names);
}
