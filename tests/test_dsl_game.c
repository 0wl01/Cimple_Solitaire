#include <CUnit/Basic.h>
#include "game_engine.h"
#include "dsl.h"

// TODO: docs
void test_flag_star_always_true(void) {
    Deck *src = create_deck(10);
    Deck *dest = create_deck(10);
    /* A flag '*' deve retornar sempre true, independentemente do deck */
    CU_ASSERT_TRUE(flag_star(src, dest, 0));
    eliminate_deck(&src);
    eliminate_deck(&dest);
}