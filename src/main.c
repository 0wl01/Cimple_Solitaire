#include "menu.h"
#include <dirent.h>
#include <stdio.h>


int main(int argc, char *argv[]) {
    const char *folder = argc > 1 ? argv[1] : "paciencias";
    DIR *dir = opendir(folder);
    if (!dir) {
        perror(folder);
        return 1;
    }
    closedir(dir);
    show_main_menu(folder);
    return 0;
}
