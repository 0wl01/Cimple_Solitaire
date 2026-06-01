/**
 * @file menu.h
 * @brief Header file for the menu module, responsible for displaying the main menu.
 */

#pragma once
#include <stdbool.h>
#include <stdint.h>

#define MAX_GAMES 255
#define SCRIPT_EXTENSION ".paciencia"

/**
 * @brief Starts the interactive main menu for the Solitaire games.
 * Scans the specified folder for .paciencia files and allows the user to choose.
 * * @param folder The folder path to scan for game scripts.
 */
void show_main_menu(const char *folder);
