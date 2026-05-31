#include "menu.h"
#include <dirent.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    // Defaults to "paciencias" folder if no command line arg is provided
    const char *folder = argc > 1 ? argv[1] : "paciencias";

    // Check if the directory actually exists
    DIR *dir = opendir(folder);
    if (!dir) {
        perror(folder);
        return 1;
    }
    closedir(dir);

    // Launch dynamic menu
    show_main_menu(folder);
    return 0;
}
