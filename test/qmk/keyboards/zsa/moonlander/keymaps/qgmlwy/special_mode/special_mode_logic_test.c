/** @file
 * Tests of the double whitespace rule of the special mode, with no QMK code.
 */

#include <stdbool.h>

#include "special_mode/special_mode_logic.h"
#include "unit_test.h"

/** Number of pushes in a list of pushes */
#define PUSH_COUNT(pushes) (sizeof(pushes) / sizeof((pushes)[0]))

/**
 * Starts a mode, then gives it each push of a list.
 *
 * @param pushes Keys that the operator pushes, in order.
 * @param count  Number of pushes.
 * @return True if the last push ends the mode.
 */
static bool last_push_ends_mode(const enum special_mode_key *pushes, size_t count) {
    special_mode_t mode;
    special_mode_start(&mode);
    bool ends_mode = false;
    for (size_t index = 0; index < count; index++) {
        assert_false(ends_mode);
        ends_mode = special_mode_push_ends_mode(&mode, pushes[index]);
    }
    return ends_mode;
}

static void special_mode_logic_r3_one_whitespace_push_keeps_mode(void **state) {
    (void)state;
    static const enum special_mode_key pushes[] = {SPECIAL_MODE_SPACE};
    assert_false(last_push_ends_mode(pushes, PUSH_COUNT(pushes)));
}

static void special_mode_logic_r4_space_twice_ends_mode(void **state) {
    (void)state;
    static const enum special_mode_key pushes[] = {
        SPECIAL_MODE_SPACE,
        SPECIAL_MODE_SPACE,
    };
    assert_true(last_push_ends_mode(pushes, PUSH_COUNT(pushes)));
}

static void special_mode_logic_r4_enter_twice_ends_mode(void **state) {
    (void)state;
    static const enum special_mode_key pushes[] = {
        SPECIAL_MODE_ENTER,
        SPECIAL_MODE_ENTER,
    };
    assert_true(last_push_ends_mode(pushes, PUSH_COUNT(pushes)));
}

static void special_mode_logic_r4_tab_twice_ends_mode(void **state) {
    (void)state;
    static const enum special_mode_key pushes[] = {
        SPECIAL_MODE_TAB,
        SPECIAL_MODE_TAB,
    };
    assert_true(last_push_ends_mode(pushes, PUSH_COUNT(pushes)));
}

static void special_mode_logic_r4_other_push_between_keeps_mode(void **state) {
    (void)state;
    static const enum special_mode_key pushes[] = {
        SPECIAL_MODE_SPACE,
        SPECIAL_MODE_OTHER_KEY,
        SPECIAL_MODE_SPACE,
    };
    assert_false(last_push_ends_mode(pushes, PUSH_COUNT(pushes)));
}

static void special_mode_logic_r4_other_keys_keep_mode(void **state) {
    (void)state;
    static const enum special_mode_key pushes[] = {
        SPECIAL_MODE_OTHER_KEY,
        SPECIAL_MODE_OTHER_KEY,
    };
    assert_false(last_push_ends_mode(pushes, PUSH_COUNT(pushes)));
}

static void special_mode_logic_r4_start_clears_the_count(void **state) {
    (void)state;
    special_mode_t mode;
    special_mode_start(&mode);
    assert_false(special_mode_push_ends_mode(&mode, SPECIAL_MODE_SPACE));
    special_mode_start(&mode);
    assert_false(special_mode_push_ends_mode(&mode, SPECIAL_MODE_SPACE));
}

static void special_mode_logic_r5_different_whitespace_keys_keep_mode(void **state) {
    (void)state;
    static const enum special_mode_key pushes[] = {
        SPECIAL_MODE_SPACE,
        SPECIAL_MODE_ENTER,
        SPECIAL_MODE_TAB,
        SPECIAL_MODE_SPACE,
    };
    assert_false(last_push_ends_mode(pushes, PUSH_COUNT(pushes)));
}

/** Cases of this program */
static const struct CMUnitTest special_mode_logic_cases[] = {
    cmocka_unit_test(special_mode_logic_r3_one_whitespace_push_keeps_mode),
    cmocka_unit_test(special_mode_logic_r4_space_twice_ends_mode),
    cmocka_unit_test(special_mode_logic_r4_enter_twice_ends_mode),
    cmocka_unit_test(special_mode_logic_r4_tab_twice_ends_mode),
    cmocka_unit_test(special_mode_logic_r4_other_push_between_keeps_mode),
    cmocka_unit_test(special_mode_logic_r4_other_keys_keep_mode),
    cmocka_unit_test(special_mode_logic_r4_start_clears_the_count),
    cmocka_unit_test(special_mode_logic_r5_different_whitespace_keys_keep_mode),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(special_mode_logic_cases, NULL, NULL);
}
