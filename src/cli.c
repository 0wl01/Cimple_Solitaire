#include "cli.h"
#include "card.h"
#include <assert.h>
#include <stdio.h>
#include <sys/types.h>
#include <inttypes.h>

// lookup tables for card symbols and suits
// Maybe if the red suits were index 1 and 3 i could use the 2^0 bit to check
// the color
const char *const SUIT[] = {"♠", "♥", "♣", "♦"};
const char *const CARDS[4][13] = {
    {"🂡", "🂢", "🂣", "🂤", "🂥", "🂦", "🂧", "🂨", "🂩", "🂪", "🂫", "🂭",
     "🂮"}, // 0: Spades
    {"🂱", "🂲", "🂳", "🂴", "🂵", "🂶", "🂷", "🂸", "🂹", "🂺", "🂻", "🂽",
     "🂾"}, // 1: Hearts
    {"🃑", "🃒", "🃓", "🃔", "🃕", "🃖", "🃗", "🃘", "🃙", "🃚", "🃛", "🃝",
     "🃞"}, // 2: Clubs
    {"🃁", "🃂", "🃃", "🃄", "🃅", "🃆", "🃇", "🃈", "🃉", "🃊", "🃋", "🃍",
     "🃎"}, // 3: Diamonds
};

// TODO: Implement way to paint the card red
// probably using ansi escape codes
void print_card(const card c) {
  if (card_flipped(c))
    printf("\U0001F0A0 ");
  else if (card_value(c) < 3)
    printf(" ");
  else {
    const uint8_t val_idx = card_value(c) - 3;
    printf("%s ", CARDS[card_suit(c)][val_idx]);
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

// The top card in the columns is not the top card of the stack. This function
// reverts the stack.
void print_decks_columns(Deck *restrict decks[], const uint8_t columns) {
  Deck *biggest = get_bigger_deck(decks, columns);
  for (card_count i = 0; i <= biggest->top; ++i) {
    for (card_count j = 0; j < columns; ++j) {
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

void print_golf_table(const card_count qnty_columns, Deck *restrict columns[],
                      Deck *restrict stock, Deck *restrict waste) {
  puts("A B C D E F G\n");
  print_decks_columns(columns, qnty_columns);
  print_card(top_card(stock));
  printf(" ");
  print_card(top_card(waste));
  printf("\n");
}

void print_prompt() { printf("(? for help) ~> "); }

void print_golf_help() {
  printf(" s - To deal card from the stock\n 1-7 - To deal cards from the "
         "columns\n q - To quit\n ? - For this "
         "screen\n");
}

void print_simon_help() {
  printf("m -> Columns should be in capital letters that range from A-J and "
         "positions range from 0 to the column's "
         "current top card position, no spaces in between.\n"
         "example: mA6B\n"
         "h -> Gives you an advice on a command.\n"
         "? -> Pops this screen.\n"
         "r -> Creates a new simon table from the beginning.\n"
         "q -> Quits");
}

void print_invalid_column() { printf("Invalid column!\n"); }

void print_unknown_command() { printf("Unknown command!\n"); }

char menu_get_input() {
  char buffer[3];
  return fgets(buffer, sizeof(buffer), stdin) ? buffer[0] : 'q';
}

/**
 * @brief Parses a move command string into a Command struct.
 * @param buffer The input string (e.g., "m A 5 B").
 * @return A populated Command struct, or a CMD_UNK command if parsing fails.
 */
static Command parse_move(const char *restrict buffer) {
  Command cmd = {
      .type = CMD_MOV, .src_col = 0, .index = SIZE_MAX, .dest_col = 0};
  int result_code =
      sscanf(buffer, "m %c %zu %c", &cmd.src_col, &cmd.index, &cmd.dest_col);
  return result_code < 1 ? (Command){.type = CMD_UNK} : cmd;
}

/**
 * @brief Turns character from an input into a command.
 *
 * @return If input is none of the ones listed in "map" then it returns a
 * unknown command, which does nothing.
 *
 * @see CommandType
 */
static CommandType char_to_command(const char c) {
  const struct {
    char key;
    CommandType cmd;
  } map[] = {{'h', CMD_HNT}, {'?', CMD_HLP}, {'r', CMD_RST},
             {'q', CMD_QUT}, {'y', CMD_YES}, {'n', CMD_NOT}};
  for (card_count i = 0; i < sizeof(map) / sizeof(*map); ++i)
    if (map[i].key == c)
      return map[i].cmd;
  return CMD_UNK;
}

Command game_get_input() {
  char buffer[32];
  if (!fgets(buffer, sizeof(buffer), stdin))
    return (Command){.type = CMD_QUT};
  return buffer[0] == 'm' ? parse_move(buffer)
                          : (Command){.type = char_to_command(buffer[0])};
}

/**
 * @brief Renders the alphabetical column headers (A, B, C...).
 * @param n_columns The number of headers to print.
 */
static void print_column_headers(const uint8_t n_columns) {
  printf("  ");
  for (uint8_t i = 0; i < n_columns; ++i)
    printf("%c ", 'A' + i);
  putchar('\n');
}

static char index_to_col(uint8_t i) {
    return i < 26 ? 'a' + i : 'A' + (i - 26);
}

static void render_headers(const deck_registry *reg) {
    printf("   ");
    for (uint8_t i = 0; i < reg->n_entries; ++i)
        printf("%c  ", index_to_col(i));
    putchar('\n');
}

static void render_row(const deck_registry *reg, card_count row) {
    printf("%"PRIuFAST16, row);
    for (uint8_t i = 0; i < reg->n_entries; ++i) {
        const deck_entry *e = &reg->entries[i];
        if (e->deck->top > row) print_card(e->deck->cards[row]);
        else                    printf("  ");
        printf(" ");
    }
    putchar('\n');
}

static Deck *get_tallest(const deck_registry *reg) {
    Deck *tallest = reg->entries[0].deck;
    for (uint8_t i = 1; i < reg->n_entries; ++i)
        if (reg->entries[i].deck->top > tallest->top)
            tallest = reg->entries[i].deck;
    return tallest;
}

void dsl_render(void *state) {
    dsl_state *s = state;
    render_headers(s->reg);
    Deck *tallest = get_tallest(s->reg);
    for (card_count i = 0; tallest && i < tallest->top; ++i)
        render_row(s->reg, i);
}
