#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

/* Estrutura simplificada apenas para o teste */
typedef struct {
    uint8_t flip;
    uint8_t color;
    uint8_t suit;
    uint8_t value;
} CardValues;

typedef struct {
    CardValues values;
} Card;

/* Arrays para desenho */
const char *const SUIT[] = {"♠", "♥", "♦", "♣"};
const char *const VAL_STR[] = {
    "A ", "2 ", "3 ", "4 ", "5 ", "6 ", "7 ", "8 ", "9 ", "10", "J ", "Q ", "K "
};

void test_print_card(Card c, bool stacked) {
    if (c.values.value < 3) return;

    const char *top_border = stacked ? "├───┤" : "┌───┐";

    if (c.values.flip) {
        /* Verso em Ciano ANSI (\033[36m) */
        printf("\033[36m%s\n│###│\n└───┘\033[0m\n", top_border);
    } else {
        const uint8_t idx = c.values.value - 3;
        
        /* Frente: Vermelho ANSI (\033[31m) se color == 1, senão texto normal */
        if (c.values.color) {
            printf("\033[31m%s\n│%s%s│\n└───┘\033[0m\n", top_border, VAL_STR[idx], SUIT[c.values.suit]);
        } else {
            printf("%s\n│%s%s│\n└───┘\n", top_border, VAL_STR[idx], SUIT[c.values.suit]);
        }
    }
}

int main(void) {
    /* Criar algumas cartas de teste à mão */
    Card as_copas   = {.values = {.suit = 1, .value = 3,  .color = 1, .flip = 0}};
    Card rei_espadas = {.values = {.suit = 0, .value = 15, .color = 0, .flip = 0}};
    Card verso       = {.values = {.suit = 0, .value = 3,  .color = 0, .flip = 1}};

    printf("1. Carta Isolada (Ás de Copas Vermelho):\n");
    test_print_card(as_copas, false);

    printf("\n2. Carta Empilhada (Rei de Espadas com topo de encaixe):\n");
    test_print_card(rei_espadas, true);

    printf("\n3. Verso da Carta (Ciano):\n");
    test_print_card(verso, false);

    return 0;
}