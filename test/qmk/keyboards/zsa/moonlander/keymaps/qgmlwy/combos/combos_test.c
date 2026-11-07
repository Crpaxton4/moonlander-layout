/** @file
 * Tests of the modes in which the combos operate, through the keymap's
 * housekeeping hook.
 *
 * QMK holds back no push of a combo key while its combos are off, so each push goes
 * on as the key of the active layer.
 */

// #region Includes

#include QMK_KEYBOARD_H

#include "layers/layers.h"
#include "recorded_calls.h"
#include "unit_test.h"

// #endregion

// #region Macros

/** Layer state with one layer on */
#define ONLY_LAYER(layer) ((layer_state_t)(1U << (layer)))

// #endregion

// #region Static functions

/**
 * Runs one housekeeping pass in a layer state, after the combos were on or off.
 *
 * @param combos_on True if the combos are on before the pass.
 * @param state     Layer state that QMK has.
 * @return True if the combos are on after the pass.
 */
static bool combos_on_after_pass(bool combos_on, layer_state_t state) {
    reset_fake_qmk();
    if (!combos_on) {
        combo_disable();
    }
    layer_state = state;
    housekeeping_task_user();
    return is_combo_enabled();
}

static void combos_on_at_power_up(void **state) {
    (void)state;
    reset_fake_qmk();
    assert_true(is_combo_enabled());
}

static void combos_operate_in_the_default_mode(void **state) {
    (void)state;
    assert_true(combos_on_after_pass(true, 0));
}

static void combos_go_off_in_the_nav_mode(void **state) {
    (void)state;
    assert_false(combos_on_after_pass(true, ONLY_LAYER(LAYER_NAV)));
}

static void combos_stay_off_in_the_nav_mode(void **state) {
    (void)state;
    assert_false(combos_on_after_pass(false, ONLY_LAYER(LAYER_NAV)));
}

static void combos_go_on_after_the_nav_mode(void **state) {
    (void)state;
    assert_true(combos_on_after_pass(false, 0));
}

static void combos_operate_in_the_plain_mode(void **state) {
    (void)state;
    // Plain-mode combo is the only way out of the plain mode
    assert_true(combos_on_after_pass(true, ONLY_LAYER(LAYER_PLAIN)));
}

static void combos_reach_the_special_mode(void **state) {
    (void)state;
    // Special mode takes each combo keycode as one push, which ends a one-shot
    assert_true(combos_on_after_pass(true, ONLY_LAYER(LAYER_SPECIAL)));
}

// #endregion

// #region Test runner

/** Cases of this program */
static const struct CMUnitTest combos_cases[] = {
    cmocka_unit_test(combos_on_at_power_up),
    cmocka_unit_test(combos_operate_in_the_default_mode),
    cmocka_unit_test(combos_go_off_in_the_nav_mode),
    cmocka_unit_test(combos_stay_off_in_the_nav_mode),
    cmocka_unit_test(combos_go_on_after_the_nav_mode),
    cmocka_unit_test(combos_operate_in_the_plain_mode),
    cmocka_unit_test(combos_reach_the_special_mode),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(combos_cases, NULL, NULL);
}

// #endregion
