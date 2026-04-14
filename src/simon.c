#include <stdio.h>
#include <stdlib.h>

// Correct includes using quotes for local files
#include "../include/card.h"
#include "../include/simon.h"

/**
 * @brief Temporary dummy function to test the menu integration.
 * Acts as a placeholder while the rest of the team develops the real logic.
 */
void init_simple_simon(void) {
    // 1. Simulate the state creation internally
    simon_state table;
    
    // 2. Visual feedback for the menu test
    printf("\n**************************************\n");
    printf("* SIMPLE SIMON (UNDER CONSTRUCTION)  *\n");
    printf("**************************************\n");
    printf(">> O menu chamou o modulo Simon com sucesso!\n");
    printf(">> Os meus colegas estao a trabalhar nesta parte.\n\n");
    
    // 3. Pause so you can see the result before the menu clears/loops
    printf("Pressione ENTER para voltar ao menu principal...");
    
    // Clean whatever might be in the buffer, then wait for an ENTER
    int c;
    while ((c = getchar()) != '\n' && c != EOF); 
}