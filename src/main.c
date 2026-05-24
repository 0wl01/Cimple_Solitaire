#include "menu.h"

int main(int argc, char *argv[]) {
    const char *folder = argc > 1 ? argv[1] : "paciencias";
    show_main_menu(folder);
    return 0;
}
