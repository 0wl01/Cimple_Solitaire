#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_engine.h"
#include "dsl.h"
#include "registry.h"
#include "card.h"

/* ==================================================
   HELPERS
   ================================================== */

/**
 * @brief Builds a minimal deck_entry with a given name, flags and cards.
 * @details The deck pointer is heap-allocated; call eliminate_deck on
 * entry->deck and never free `entry` itself (it lives on the stack).
 */
static void make_entry(deck_entry *e, const char *name, const char *flags,
                        card_count cap) {
    strncpy(e->name,  name,  sizeof(e->name)  - 1);
    strncpy(e->flags, flags, sizeof(e->flags) - 1);
    e->deck = create_deck(cap);
}

/**
 * @brief Builds a minimal move_rules struct.
 */
static move_rules make_rule(const char *src, const char *dst,
                             const char *flags) {
    move_rules r;
    memset(&r, 0, sizeof(r));
    strncpy(r.deck_src, src,   sizeof(r.deck_src)  - 1);
    strncpy(r.deck_dst, dst,   sizeof(r.deck_dst)  - 1);
    strncpy(r.flags,    flags, sizeof(r.flags)     - 1);
    return r;
}

/* ==================================================
   SUITE 1: can_move_rule
   ================================================== */

/**
 * @brief Caminho feliz: mover uma única carta com flag '*' (sempre verdadeiro).
 * @details Verifica que can_move_rule aprova um movimento quando a regra
 * corresponde aos nomes dos decks, a fonte não está vazia e a flag '*' passa.
 */
void test_can_move_rule_happy_path(void) {
    deck_entry src, dst;
    make_entry(&src, "col", "",  10);
    make_entry(&dst, "fund", "", 10);

    push(src.deck, make_card(SPADES, 5));

    move_rules rule = make_rule("col", "fund", "*");

    /* top - 1 == 0, que é o índice da carta */
    CU_ASSERT_TRUE(can_move_rule(&rule, &src, &dst, 0));

    eliminate_deck(&src.deck);
    eliminate_deck(&dst.deck);
}

/**
 * @brief can_move_rule deve retornar false quando a fonte está vazia.
 */
void test_can_move_rule_empty_src(void) {
    deck_entry src, dst;
    make_entry(&src, "col",  "", 10);
    make_entry(&dst, "fund", "", 10);

    move_rules rule = make_rule("col", "fund", "*");

    CU_ASSERT_FALSE(can_move_rule(&rule, &src, &dst, 0));

    eliminate_deck(&src.deck);
    eliminate_deck(&dst.deck);
}

/**
 * @brief can_move_rule deve retornar false quando os nomes não batem com a regra.
 */
void test_can_move_rule_name_mismatch(void) {
    deck_entry src, dst;
    make_entry(&src, "outro", "", 10);
    make_entry(&dst, "fund",  "", 10);

    push(src.deck, make_card(SPADES, 5));

    move_rules rule = make_rule("col", "fund", "*");

    CU_ASSERT_FALSE(can_move_rule(&rule, &src, &dst, 0));

    eliminate_deck(&src.deck);
    eliminate_deck(&dst.deck);
}

/**
 * @brief Flag '<': a carta de src deve ter valor um abaixo do topo de dst.
 * @details Caminho feliz: src tem 5, dst tem 6 → válido.
 */
void test_can_move_rule_flag_less_valid(void) {
    deck_entry src, dst;
    make_entry(&src, "col",  "", 10);
    make_entry(&dst, "fund", "", 10);

    push(src.deck, make_card(SPADES,  5));
    push(dst.deck, make_card(HEARTS,  6));

    move_rules rule = make_rule("col", "fund", "<");

    CU_ASSERT_TRUE(can_move_rule(&rule, &src, &dst, 0));

    eliminate_deck(&src.deck);
    eliminate_deck(&dst.deck);
}

/**
 * @brief Flag '<': src com valor 7 e dst com topo 6 → inválido.
 */
void test_can_move_rule_flag_less_invalid(void) {
    deck_entry src, dst;
    make_entry(&src, "col",  "", 10);
    make_entry(&dst, "fund", "", 10);

    push(src.deck, make_card(SPADES,  7));
    push(dst.deck, make_card(HEARTS,  6));

    move_rules rule = make_rule("col", "fund", "<");

    CU_ASSERT_FALSE(can_move_rule(&rule, &src, &dst, 0));

    eliminate_deck(&src.deck);
    eliminate_deck(&dst.deck);
}

/**
 * @brief Flag 'V' (destino tem flag '1'): só é válido se dest estiver vazio.
 * @details Se dest já tem cartas, o movimento deve ser recusado.
 */
void test_can_move_rule_flag_dest_must_be_empty_invalid(void) {
    deck_entry src, dst;
    make_entry(&src, "col",  "",  10);
    make_entry(&dst, "fund", "1", 10); /* flag '1' exige destino vazio */

    push(src.deck, make_card(SPADES, 5));
    push(dst.deck, make_card(HEARTS, 6)); /* dest já tem carta → deve falhar */

    move_rules rule = make_rule("col", "fund", "*");

    CU_ASSERT_FALSE(can_move_rule(&rule, &src, &dst, 0));

    eliminate_deck(&src.deck);
    eliminate_deck(&dst.deck);
}

/**
 * @brief Flag '+': permite mover uma sequência (índice != top-1 é válido).
 */
void test_can_move_rule_flag_plus_allows_sequence(void) {
    deck_entry src, dst;
    make_entry(&src, "col",  "", 10);
    make_entry(&dst, "fund", "", 10);

    push(src.deck, make_card(SPADES, 5));
    push(src.deck, make_card(SPADES, 4));
    push(src.deck, make_card(SPADES, 3));

    move_rules rule = make_rule("col", "fund", "+*");

    /* índice 0 não é top-1 (que é 2), mas '+' deve permitir */
    CU_ASSERT_TRUE(can_move_rule(&rule, &src, &dst, 0));

    eliminate_deck(&src.deck);
    eliminate_deck(&dst.deck);
}

/**
 * @brief Sem '+': tentar mover do meio da pilha (índice != top-1) é inválido.
 */
void test_can_move_rule_no_plus_rejects_sequence(void) {
    deck_entry src, dst;
    make_entry(&src, "col",  "", 10);
    make_entry(&dst, "fund", "", 10);

    push(src.deck, make_card(SPADES, 5));
    push(src.deck, make_card(SPADES, 4));
    push(src.deck, make_card(SPADES, 3));

    move_rules rule = make_rule("col", "fund", "*");

    /* índice 0 não é top-1 sem flag '+' → deve falhar */
    CU_ASSERT_FALSE(can_move_rule(&rule, &src, &dst, 0));

    eliminate_deck(&src.deck);
    eliminate_deck(&dst.deck);
}

/* ==================================================
   SUITE 2: dsl_has_won
   ================================================== */

/**
 * @brief Caminho feliz: todas as condições de vitória satisfeitas.
 */
void test_dsl_has_won_true(void) {
    /* Monta cfg com 1 condição: "fund" deve ter 13 cartas */
    game_cfg cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.n_winconds = 1;
    win_condition wc;
    strncpy(wc.deck_name, "fund", sizeof(wc.deck_name) - 1);
    wc.n = 13;
    cfg.conditions = &wc;

    /* Monta registry com 1 entrada "fund" com 13 cartas */
    deck_registry reg;
    deck_entry entry;
    make_entry(&entry, "fund", "", 13);
    for (card_count i = 3; i <= 15; ++i)
        push(entry.deck, make_card(SPADES, i));

    reg.entries  = &entry;
    reg.n_entries = 1;

    engine_state s = {.reg = &reg, .cfg = &cfg};
    CU_ASSERT_TRUE(dsl_has_won(&s));

    eliminate_deck(&entry.deck);
}

/**
 * @brief Condição de vitória não satisfeita: contagem errada.
 */
void test_dsl_has_won_false_wrong_count(void) {
    game_cfg cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.n_winconds = 1;
    win_condition wc;
    strncpy(wc.deck_name, "fund", sizeof(wc.deck_name) - 1);
    wc.n = 13;
    cfg.conditions = &wc;

    deck_registry reg;
    deck_entry entry;
    make_entry(&entry, "fund", "", 13);
    push(entry.deck, make_card(SPADES, 5)); /* só 1 carta, precisa de 13 */

    reg.entries   = &entry;
    reg.n_entries = 1;

    engine_state s = {.reg = &reg, .cfg = &cfg};
    CU_ASSERT_FALSE(dsl_has_won(&s));

    eliminate_deck(&entry.deck);
}

/**
 * @brief Condição de vitória não satisfeita: deck com nome diferente.
 * @details Nenhuma entrada bate com o nome da condição → seen == 0 → false.
 */
void test_dsl_has_won_false_name_not_found(void) {
    game_cfg cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.n_winconds = 1;
    win_condition wc;
    strncpy(wc.deck_name, "fund", sizeof(wc.deck_name) - 1);
    wc.n = 0;
    cfg.conditions = &wc;

    deck_registry reg;
    deck_entry entry;
    make_entry(&entry, "outro", "", 10); /* nome diferente */

    reg.entries   = &entry;
    reg.n_entries = 1;

    engine_state s = {.reg = &reg, .cfg = &cfg};
    CU_ASSERT_FALSE(dsl_has_won(&s));

    eliminate_deck(&entry.deck);
}

/* ==================================================
   SUITE 3: dsl_can_play
   ================================================== */

/**
 * @brief Caminho feliz: existe pelo menos uma jogada legal disponível.
 */
void test_dsl_can_play_true(void) {
    game_cfg cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.n_move_rules = 1;
    move_rules rule = make_rule("col", "fund", "*");
    cfg.mov_rules = &rule;

    deck_entry entries[2];
    make_entry(&entries[0], "col",  "", 10);
    make_entry(&entries[1], "fund", "", 10);
    push(entries[0].deck, make_card(SPADES, 5));

    deck_registry reg = {.entries = entries, .n_entries = 2};
    engine_state  s   = {.reg = &reg, .cfg = &cfg};

    CU_ASSERT_TRUE(dsl_can_play(&s));

    eliminate_deck(&entries[0].deck);
    eliminate_deck(&entries[1].deck);
}

/**
 * @brief Sem jogadas possíveis: todas as fontes estão vazias.
 */
void test_dsl_can_play_false_all_empty(void) {
    game_cfg cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.n_move_rules = 1;
    move_rules rule = make_rule("col", "fund", "*");
    cfg.mov_rules = &rule;

    deck_entry entries[2];
    make_entry(&entries[0], "col",  "", 10); /* vazio */
    make_entry(&entries[1], "fund", "", 10);

    deck_registry reg = {.entries = entries, .n_entries = 2};
    engine_state  s   = {.reg = &reg, .cfg = &cfg};

    CU_ASSERT_FALSE(dsl_can_play(&s));

    eliminate_deck(&entries[0].deck);
    eliminate_deck(&entries[1].deck);
}

/**
 * @brief Sem jogadas possíveis: nenhuma regra de movimento definida.
 */
void test_dsl_can_play_false_no_rules(void) {
    game_cfg cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.n_move_rules = 0;
    cfg.mov_rules = NULL;

    deck_entry entries[1];
    make_entry(&entries[0], "col", "", 10);
    push(entries[0].deck, make_card(SPADES, 5));

    deck_registry reg = {.entries = entries, .n_entries = 1};
    engine_state  s   = {.reg = &reg, .cfg = &cfg};

    CU_ASSERT_FALSE(dsl_can_play(&s));

    eliminate_deck(&entries[0].deck);
}

/* ==================================================
   SUITE 4: dsl_handle_move
   ================================================== */

/**
 * @brief Caminho feliz: carta movida com sucesso via comando de movimento.
 */
void test_dsl_handle_move_valid(void) {
    game_cfg cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.n_move_rules = 1;
    move_rules rule = make_rule("col", "fund", "*");
    cfg.mov_rules = &rule;

    deck_entry entries[2];
    make_entry(&entries[0], "col",  "", 10);
    make_entry(&entries[1], "fund", "", 10);
    push(entries[0].deck, make_card(SPADES, 5));

    deck_registry reg = {.entries = entries, .n_entries = 2};
    engine_state  s   = {.reg = &reg, .cfg = &cfg};

    /* 'a' → índice 0 (col), 'b' → índice 1 (fund) */
    Command cmd = {.type = CMD_MOV, .src_col = 'a', .index = 0, .dest_col = 'b'};
    LoopSignal sig = dsl_handle_move(&s, cmd);

    CU_ASSERT_EQUAL(sig, LOOP_CONTINUE);
    CU_ASSERT_EQUAL(entries[0].deck->top, 0);
    CU_ASSERT_EQUAL(entries[1].deck->top, 1);

    eliminate_deck(&entries[0].deck);
    eliminate_deck(&entries[1].deck);
}

/**
 * @brief Coluna de origem inválida (fora dos limites do registry).
 * @details Deve retornar LOOP_CONTINUE sem crash nem alteração nos decks.
 */
void test_dsl_handle_move_invalid_col(void) {
    game_cfg cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.n_move_rules = 0;
    cfg.mov_rules = NULL;

    deck_entry entries[1];
    make_entry(&entries[0], "col", "", 10);
    push(entries[0].deck, make_card(SPADES, 5));

    deck_registry reg = {.entries = entries, .n_entries = 1};
    engine_state  s   = {.reg = &reg, .cfg = &cfg};

    /* 'z' está fora do registry que só tem 1 entrada */
    Command cmd = {.type = CMD_MOV, .src_col = 'z', .index = 0, .dest_col = 'a'};
    LoopSignal sig = dsl_handle_move(&s, cmd);

    CU_ASSERT_EQUAL(sig, LOOP_CONTINUE);
    CU_ASSERT_EQUAL(entries[0].deck->top, 1); /* intocado */

    eliminate_deck(&entries[0].deck);
}

/**
 * @brief Carácter não-letra deve resultar em índice -1 e retornar LOOP_CONTINUE.
 */
void test_dsl_handle_move_non_alpha_col(void) {
    game_cfg cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.n_move_rules = 0;
    cfg.mov_rules = NULL;

    deck_entry entries[1];
    make_entry(&entries[0], "col", "", 10);

    deck_registry reg = {.entries = entries, .n_entries = 1};
    engine_state  s   = {.reg = &reg, .cfg = &cfg};

    Command cmd = {.type = CMD_MOV, .src_col = '1', .index = 0, .dest_col = '2'};
    LoopSignal sig = dsl_handle_move(&s, cmd);

    CU_ASSERT_EQUAL(sig, LOOP_CONTINUE);

    eliminate_deck(&entries[0].deck);
}

/* ==================================================
   SUITE 5: dsl_post_turn (auto-rules)
   ================================================== */

/**
 * @brief Caminho feliz: a auto-rule desloca uma carta automaticamente.
 * @details Uma auto-rule com flag '*' deve mover o topo de "col" para "fund"
 * sem qualquer intervenção do utilizador.
 */
void test_dsl_post_turn_triggers_auto(void) {
    game_cfg cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.n_auto_rules = 1;
    auto_rule ar = make_rule("col", "fund", "*");
    cfg.auto_rules = &ar;
    cfg.n_move_rules = 0;
    cfg.mov_rules = NULL;

    deck_entry entries[2];
    make_entry(&entries[0], "col",  "", 10);
    make_entry(&entries[1], "fund", "", 10);
    push(entries[0].deck, make_card(SPADES, 5));

    deck_registry reg = {.entries = entries, .n_entries = 2};
    engine_state  s   = {.reg = &reg, .cfg = &cfg};

    dsl_post_turn(&s);

    CU_ASSERT_EQUAL(entries[0].deck->top, 0);
    CU_ASSERT_EQUAL(entries[1].deck->top, 1);

    eliminate_deck(&entries[0].deck);
    eliminate_deck(&entries[1].deck);
}

/**
 * @brief Auto-rule não deve disparar se a fonte está vazia.
 */
void test_dsl_post_turn_no_trigger_empty_src(void) {
    game_cfg cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.n_auto_rules = 1;
    auto_rule ar = make_rule("col", "fund", "*");
    cfg.auto_rules = &ar;
    cfg.n_move_rules = 0;
    cfg.mov_rules = NULL;

    deck_entry entries[2];
    make_entry(&entries[0], "col",  "", 10); /* vazio */
    make_entry(&entries[1], "fund", "", 10);

    deck_registry reg = {.entries = entries, .n_entries = 2};
    engine_state  s   = {.reg = &reg, .cfg = &cfg};

    dsl_post_turn(&s);

    CU_ASSERT_EQUAL(entries[0].deck->top, 0);
    CU_ASSERT_EQUAL(entries[1].deck->top, 0);

    eliminate_deck(&entries[0].deck);
    eliminate_deck(&entries[1].deck);
}

/* ==================================================
   MAIN RUNNER
   ================================================== */
typedef struct {
    const char *name;
    CU_TestFunc fn;
} T;

static int add_tests(CU_pSuite suite, T *tests, size_t count) {
    for (size_t i = 0; i < count; i++)
        if (!CU_add_test(suite, tests[i].name, tests[i].fn))
            return 0;
    return 1;
}

int main(void) {
    if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();

    /* Suite 1: can_move_rule */
    CU_pSuite s1 = CU_add_suite("DSL_Engine_CanMoveRule", NULL, NULL);
    T t1[] = {
        {"happy path: flag '*'",               test_can_move_rule_happy_path},
        {"empty source deck",                  test_can_move_rule_empty_src},
        {"rule name mismatch",                 test_can_move_rule_name_mismatch},
        {"flag '<' valid (src=5, dst=6)",      test_can_move_rule_flag_less_valid},
        {"flag '<' invalid (src=7, dst=6)",    test_can_move_rule_flag_less_invalid},
        {"flag '1': dest not empty → false",   test_can_move_rule_flag_dest_must_be_empty_invalid},
        {"flag '+': sequence allowed",         test_can_move_rule_flag_plus_allows_sequence},
        {"no '+': sequence rejected",          test_can_move_rule_no_plus_rejects_sequence},
    };

    /* Suite 2: dsl_has_won */
    CU_pSuite s2 = CU_add_suite("DSL_Engine_HasWon", NULL, NULL);
    T t2[] = {
        {"all win conditions met",             test_dsl_has_won_true},
        {"wrong card count",                   test_dsl_has_won_false_wrong_count},
        {"deck name not found",                test_dsl_has_won_false_name_not_found},
    };

    /* Suite 3: dsl_can_play */
    CU_pSuite s3 = CU_add_suite("DSL_Engine_CanPlay", NULL, NULL);
    T t3[] = {
        {"move available",                     test_dsl_can_play_true},
        {"all sources empty",                  test_dsl_can_play_false_all_empty},
        {"no rules defined",                   test_dsl_can_play_false_no_rules},
    };

    /* Suite 4: dsl_handle_move */
    CU_pSuite s4 = CU_add_suite("DSL_Engine_HandleMove", NULL, NULL);
    T t4[] = {
        {"valid move transfers card",          test_dsl_handle_move_valid},
        {"column index out of bounds",         test_dsl_handle_move_invalid_col},
        {"non-alphabetic column character",    test_dsl_handle_move_non_alpha_col},
    };

    /* Suite 5: dsl_post_turn */
    CU_pSuite s5 = CU_add_suite("DSL_Engine_PostTurn", NULL, NULL);
    T t5[] = {
        {"auto-rule fires and moves card",     test_dsl_post_turn_triggers_auto},
        {"auto-rule does not fire if empty",   test_dsl_post_turn_no_trigger_empty_src},
    };

    if (!s1 || !s2 || !s3 || !s4 || !s5
        || !add_tests(s1, t1, sizeof(t1) / sizeof(t1[0]))
        || !add_tests(s2, t2, sizeof(t2) / sizeof(t2[0]))
        || !add_tests(s3, t3, sizeof(t3) / sizeof(t3[0]))
        || !add_tests(s4, t4, sizeof(t4) / sizeof(t4[0]))
        || !add_tests(s5, t5, sizeof(t5) / sizeof(t5[0]))) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();

    int fails = CU_get_number_of_failures();
    CU_cleanup_registry();
    return fails > 0 ? 1 : 0;
}