#include "menu.h"
#include "render.h"
#include "input.h"
#include "run.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

static bool has_script_extension(const char *filename) {
    size_t flen = strlen(filename);
    size_t elen = strlen(SCRIPT_EXTENSION);
    bool is_valid = false;
    if (flen >= elen) {
        is_valid = (strcmp(filename + flen - elen, SCRIPT_EXTENSION) == 0);
    }
    return is_valid;
}

static void process_dir_entry(struct dirent *entry, char names[MAX_GAMES][256], uint8_t *n) {
    if (*n < MAX_GAMES) {
        if (has_script_extension(entry->d_name)) {
            strncpy(names[*n], entry->d_name, 255);
            (*n)++;
        }
    }
}

static uint8_t load_files(const char *folder, char names[MAX_GAMES][256]) {
    DIR *dir = opendir(folder);
    uint8_t n = 0;
    if (dir) {
        struct dirent *entry = readdir(dir);
        while (entry) {
            process_dir_entry(entry, names, &n);
            entry = readdir(dir);
        }
        closedir(dir);
    } else {
        perror(folder);
    }
    return n;
}

static void print_menu_page(const char *folder, char names[MAX_GAMES][256], uint8_t n, uint8_t page) {
    uint8_t start = page * 8;
    uint8_t end = (start + 8 < n) ? start + 8 : n;
    printf("\n=== C-litaire (%s) ===\n", folder);
    for (uint8_t i = start; i < end; ++i) {
        printf("  [%d] %s\n", i - start + 1, names[i]);
    }
    if (page > 0) printf("  [0] Previous page\n");
    if (end < n) printf("  [9] Next page\n");
    printf("  [q] Exit\n");
}

static void launch_game(const char *folder, const char *name) {
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", folder, name);
    bool want_restart = true;
    while (want_restart) {
        want_restart = run_dsl_game(path);
    }
}

static void handle_menu_input(char c, const char *folder, char names[MAX_GAMES][256], uint8_t n, uint8_t *page) {
    if (c == '9' && (*page + 1) * 8 < n) {
        (*page)++;
    } else if (c == '0' && *page > 0) {
        (*page)--;
    } else if (c != 'q') {
        uint8_t idx = (*page) * 8 + (c - '1');
        if (c >= '1' && c <= '8' && idx < n) {
            launch_game(folder, names[idx]);
        } else {
            printf("Invalid choice.\n");
        }
    }
}

void show_main_menu(const char *folder) {
    char (*names)[256] = calloc(MAX_GAMES, 256);
    if (names) {
        uint8_t n = load_files(folder, names);
        uint8_t page = 0;
        char c = 0;
        while (c != 'q') {
            print_menu_page(folder, names, n, page);
            print_prompt();
            c = menu_get_input();
            handle_menu_input(c, folder, names, n, &page);
        }
        free(names);
    }
}
