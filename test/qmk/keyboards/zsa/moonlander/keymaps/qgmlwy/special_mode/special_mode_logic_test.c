/** @file
 * Tests of the one-shot and the double whitespace rule of the special mode, with no
 * QMK code.
 */

// #region Includes

#include <stdbool.h>

#include "special_mode/special_mode_logic.h"
#include "unit_test.h"

// #endregion

// #region Macros

/** Number of pushes in a list of pushes */
#define PUSH_COUNT(pushes) (sizeof(pushes) / sizeof((pushes)[0]))

// #endregion

// #region Static functions

/**
 * Starts a locked mode, then gives it each push of a list.
 *
 * @param pushes Keys that the operator pushes, in order.
 * @param count  Number of pushes.
 * @return True if the last push ends the mode.
 */
static bool last_push_ends_mode(const enum special_mode_key *pushes, size_t count) {
    special_mode_t mode;
    special_mode_start(&mode, SPECIAL_MODE_LOCKED);
    enum special_mode_push_result result = SPECIAL_MODE_STAYS_ON;
    for (size_t index = 0; index < count; index++) {
        assert_int_equal(result, SPECIAL_MODE_STAYS_ON);
        result = special_mode_push(&mode, pushes[index]);
    }
    // Only the one-shot sends the key that ends it
    assert_int_not_equal(result, SPECIAL_MODE_ENDS_AFTER_KEY);
    return result == SPECIAL_MODE_ENDS_WITH_NO_KEY;
}

/**
 * Starts a one-shot, then gives it one push.
 *
 * @param key Key class of the push.
 * @return True if the push sends its key and ends the one-shot.
 */
static bool one_shot_ends_after(enum special_mode_key key) {
    special_mode_t mode;
    special_mode_start(&mode, SPECIAL_MODE_ONE_SHOT);
    return special_mode_push(&mode, key) == SPECIAL_MODE_ENDS_AFTER_KEY;
}

/**
 * Gives one push to a mode.
 *
 * @param mode Mode state.
 * @param key  Key class of the push.
 * @return True if the push sends its key and the mode stays on.
 */
static bool push_keeps_mode(special_mode_t *mode, enum special_mode_key key) {
    return special_mode_push(mode, key) == SPECIAL_MODE_STAYS_ON;
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
    special_mode_start(&mode, SPECIAL_MODE_LOCKED);
    assert_true(push_keeps_mode(&mode, SPECIAL_MODE_SPACE));
    special_mode_start(&mode, SPECIAL_MODE_LOCKED);
    assert_true(push_keeps_mode(&mode, SPECIAL_MODE_SPACE));
}

static void special_mode_logic_r1_one_shot_ends_after_other_key(void **state) {
    (void)state;
    assert_true(one_shot_ends_after(SPECIAL_MODE_OTHER_KEY));
}

static void special_mode_logic_r1_one_shot_ends_after_whitespace_key(void **state) {
    (void)state;
    assert_true(one_shot_ends_after(SPECIAL_MODE_SPACE));
    assert_true(one_shot_ends_after(SPECIAL_MODE_ENTER));
    assert_true(one_shot_ends_after(SPECIAL_MODE_TAB));
}

static void special_mode_logic_r1_lock_after_one_shot_clears_the_count(void **state) {
    (void)state;
    special_mode_t mode;
    special_mode_start(&mode, SPECIAL_MODE_ONE_SHOT);
    special_mode_start(&mode, SPECIAL_MODE_LOCKED);
    assert_true(push_keeps_mode(&mode, SPECIAL_MODE_SPACE));
    assert_true(push_keeps_mode(&mode, SPECIAL_MODE_ENTER));
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

// #endregion

// #region Test runner

/** Cases of this program */
static const struct CMUnitTest special_mode_logic_cases[] = {
    cmocka_unit_test(special_mode_logic_r1_one_shot_ends_after_other_key),
    cmocka_unit_test(special_mode_logic_r1_one_shot_ends_after_whitespace_key),
    cmocka_unit_test(special_mode_logic_r1_lock_after_one_shot_clears_the_count),
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

// #endregion
