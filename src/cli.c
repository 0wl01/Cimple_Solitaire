#include "cli.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

// lookup tables for card symbols and suits
// Maybe if the red suits were index 1 and 3 i could use the 2^0 bit to check
// the color
const char *const SUIT[] = {"♠", "♥", "♦", "♣"};
const char *const CARDS[4][13] = {
    {"🂡", "🂢", "🂣", "🂤", "🂥", "🂦", "🂧", "🂨", "🂩", "🂪", "🂫", "🂭", "🂮"}, // 0: Spades
    {"🂱", "🂲", "🂳", "🂴", "🂵", "🂶", "🂷", "🂸", "🂹", "🂺", "🂻", "🂽", "🂾"}, // 1: Hearts
    {"🃁", "🃂", "🃃", "🃄", "🃅", "🃆", "🃇", "🃈", "🃉", "🃊", "🃋", "🃍", "🃎"}, // 2: Diamonds
    {"🃑", "🃒", "🃓", "🃔", "🃕", "🃖", "🃗", "🃘", "🃙", "🃚", "🃛", "🃝", "🃞"}  // 3: Clubs
};

// TODO: Implement way to paint the card red
// probably using ansi escape codes
void print_card(const Card c) {
    if (c.values.flip)
        printf("\U0001F0A0");
    else if (c.values.value < 3)
        printf(" ");
    else {
        const uint8_t val_idx = c.values.value - 3;
        printf("%s", CARDS[c.values.suit][val_idx]);
    }
}

void print_deck(Deck const *deck) {
    for (int8_t i = deck->top - 1; i >= 0; --i) {
        // printf("(%d: ", i);
        print_card(deck->cards[i]);
        printf(", ");
        // printf("%d), ", deck->cards[i].values.value);
    }
    putchar('\n');
}

// The top card in the columns is not the top card of the stack. This function reverts the stack.
void print_decks_columns(Deck *restrict decks[], const uint8_t columns) {
    Deck *biggest = get_bigger_deck(decks, columns);
    for (size_t i = 0; i <= biggest->top; ++i) {
        for (size_t j = 0; j < columns; ++j) {
            if (decks[j]->top > i) {
                print_card(decks[j]->cards[i]);
            } else
                printf(" ");

            printf(" ");
        }
        printf("\n");
    }
}

void print_end(const bool win) {
    if (win)
        printf("You Win!\n");
    else
        printf("You Lose! HAHA\n");
    printf("Do you want to keep playing? (y/n)\n");
}

void print_golf_table(const size_t qnty_columns, Deck *restrict columns[], Deck *restrict stock, Deck *restrict waste) {
    puts("A B C D E F G\n");
    print_decks_columns(columns, qnty_columns);
    print_card(top_card(stock));
    printf(" ");
    print_card(top_card(waste));
    printf("\n");
}

void print_prompt() { printf("(? for help) ~> "); }

void print_golf_help() {
    printf(" s - To deal card from the stock\n 1-7 - To deal cards from the columns\n q - To quit\n ? - For this "
           "screen\n");
}

void print_simon_help() {
    printf(" s - To deal card from the stock\n 1-7 - To deal cards from the columns\n q - To quit\n ? - For this "
           "screen\n");
}

void print_invalid_column() { printf("Invalid column!\n"); }

void print_unknown_command() { printf("Unknown command!\n"); }

char menu_get_input() {
    char buffer[3];
    return fgets(buffer, sizeof(buffer), stdin) ? buffer[0] : 'q';
}

// TODO: docs
static Command parse_move(const char *restrict buffer) {
    Command cmd = {.type = CMD_MOV, .src_col = 0, .index = SIZE_MAX, .dest_col = 0};
    int result_code = sscanf(buffer, "m %c %zu %c", &cmd.src_col, &cmd.index, &cmd.dest_col);
    return result_code < 1 ? (Command){.type = CMD_UNK} : cmd;
}

// TODO: docs
static CommandType char_to_command(const char c) {
    const struct {
        char key;
        CommandType cmd;
    } map[] = {{'h', CMD_HNT}, {'?', CMD_HLP}, {'r', CMD_RST}, {'q', CMD_QUT}, {'y', CMD_YES}, {'n', CMD_NOT}};
    for (size_t i = 0; i < sizeof(map) / sizeof(*map); ++i)
        if (map[i].key == c)
            return map[i].cmd;
    return CMD_UNK;
}

Command game_get_input() {
    char buffer[32];
    if (!fgets(buffer, sizeof(buffer), stdin))
        return (Command){.type = CMD_QUT};
    return buffer[0] == 'm' ? parse_move(buffer) : (Command){.type = char_to_command(buffer[0])};
}

// TODO docs
static void print_top_row(const TableLayout *t) {
    if (t->stock) {
        print_card(top_card(t->stock));
        printf(" ");
    }
    if (t->waste) {
        print_card(top_card(t->waste));
        printf(" ");
    }
    for (uint8_t i = 0; i < t->n_foundations; ++i) {
        print_card(top_card(t->foundations[i]));
        printf(" ");
    }
    putchar('\n');
}

// TODO: docs
static void print_column_headers(const uint8_t n_columns) {
    for (uint8_t i = 0; i < n_columns; ++i)
        printf("%c ", 'A' + i);
    putchar('\n');
}

//TODO: docs
static void print_column_row(Deck *restrict *columns, const uint8_t n_columns, const size_t row) {
    for (uint8_t j = 0; j < n_columns; ++j) {
        if (columns[j] && columns[j]->top > row)
            print_card(columns[j]->cards[row]);
        else
            printf(" ");
        printf(" ");
    }
    putchar('\n');
}

void print_table(const TableLayout *restrict t) {
    assert(t != NULL);
    print_top_row(t);
    print_column_headers(t->n_columns);
    Deck *biggest = get_bigger_deck(t->columns, t->n_columns);
    for (size_t i = 0; biggest && i < biggest->top; ++i)
        print_column_row(t->columns, t->n_columns, i);
}
