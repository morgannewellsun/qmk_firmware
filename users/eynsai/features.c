#include "quantum.h"
#include "raw_hid.h"
#include "host_driver.h"
#include "pointing_device.h"
#include "features.h"
#include "intercepts.h"
#include "mouse_passthrough_receiver.h"
#include "superkeys.h"
#include "timeouts.h"

keyboard_state_t keyboard_state = {0};  // TODO refactor out all of the pointless pointers!

// ============================================================================
// QMK KEY OVERRIDES
// ============================================================================

const key_override_t key_override_9 = ko_make_basic(MOD_MASK_SHIFT, KC_9, KC_PIPE);
const key_override_t key_override_0 = ko_make_basic(MOD_MASK_SHIFT, KC_0, KC_BSLS);

const key_override_t *key_overrides[] = {
	&key_override_9,
    &key_override_0,
};

// ============================================================================
// RGB ANIMATIONS
// ============================================================================

#define ONESHOT_BREATHING_A(RGB_HUE_ACCENT) (NUDGE_HUE(RGB_HUE_MAIN_ONESHOT, RGB_HUE_ACCENT, -1 * RGB_BREATHING_HUE_AMPLITUDE))
#define ONESHOT_BREATHING_B(RGB_HUE_ACCENT) (NUDGE_HUE(RGB_HUE_MAIN_ONESHOT, RGB_HUE_ACCENT, RGB_BREATHING_HUE_AMPLITUDE))

rgb_animation_t rgb_animations[] = {

    // initial state
    [RGB_ANIMATION_OFF] = {
        .start_duration = 0, 
        .durations      = {RGB_STATIC_DUMMY_DURATION}, 
        .colors         = {                {0, 0, 0}}, 
        .loop_start_idx = 0, 
        .loop_stop_idx  = 1},

    // base layer switching - from workman
    [RGB_ANIMATION_BASE_QWERTY_FROM_WORKMAN] = {
        .start_duration = RGB_BASE_FROM_WORKMAN_START_DURATION, 
        .durations      = {                        RGB_BASE_FROM_WORKMAN_HOLD_DURATION,                         RGB_BASE_FROM_WORKMAN_FADE_DURATION,                           RGB_STATIC_DUMMY_DURATION},
        .colors         = {{RGB_HUE_ACCENT_QWERTY, RGB_SAT_ACCENT_QWERTY, RGB_MAX_VAL}, {RGB_HUE_ACCENT_QWERTY, RGB_SAT_ACCENT_QWERTY, RGB_MAX_VAL}, {RGB_HUE_MAIN_BASE, RGB_SAT_MAIN_BASE, RGB_MAX_VAL}}, 
        .loop_start_idx = 2, 
        .loop_stop_idx  = 3},
    [RGB_ANIMATION_BASE_GAMING_FROM_WORKMAN] = {
        .start_duration = RGB_BASE_FROM_WORKMAN_START_DURATION, 
        .durations      = {                        RGB_BASE_FROM_WORKMAN_HOLD_DURATION,                         RGB_BASE_FROM_WORKMAN_FADE_DURATION,                           RGB_STATIC_DUMMY_DURATION},
        .colors         = {{RGB_HUE_ACCENT_GAMING, RGB_SAT_ACCENT_GAMING, RGB_MAX_VAL}, {RGB_HUE_ACCENT_GAMING, RGB_SAT_ACCENT_GAMING, RGB_MAX_VAL}, {RGB_HUE_MAIN_BASE, RGB_SAT_MAIN_BASE, RGB_MAX_VAL}}, 
        .loop_start_idx = 2, 
        .loop_stop_idx  = 3},

    // base layer switching - to workman
    [RGB_ANIMATION_BASE_QWERTY_TO_WORKMAN] = {
        .start_duration = RGB_BASE_TO_WORKMAN_START_DURATION, 
        .durations      = {                          RGB_BASE_TO_WORKMAN_FADE_DURATION,                         RGB_STATIC_DUMMY_DURATION},
        .colors         = {{RGB_HUE_ACCENT_QWERTY, RGB_SAT_ACCENT_QWERTY, RGB_MAX_VAL}, {RGB_HUE_ACCENT_QWERTY, RGB_SAT_ACCENT_QWERTY, 0}},
        .loop_start_idx = 1,
        .loop_stop_idx  = 2},
    [RGB_ANIMATION_BASE_GAMING_TO_WORKMAN] = {
        .start_duration = RGB_BASE_TO_WORKMAN_START_DURATION, 
        .durations      = {                          RGB_BASE_TO_WORKMAN_FADE_DURATION,                         RGB_STATIC_DUMMY_DURATION},
        .colors         = {{RGB_HUE_ACCENT_GAMING, RGB_SAT_ACCENT_GAMING, RGB_MAX_VAL}, {RGB_HUE_ACCENT_GAMING, RGB_SAT_ACCENT_GAMING, 0}},
        .loop_start_idx = 1,
        .loop_stop_idx  = 2},

    // oneshot modifiers - first oneshot on
    [RGB_ANIMATION_ONESHOT_ALT_FIRST_ON] = {
        .start_duration = RGB_ONESHOT_ON_FIRST_START_DURATION, 
        .durations      = {                    RGB_ONESHOT_ON_FIRST_HOLD_DURATION,                       RGB_ONESHOT_ON_FIRST_FADE_DURATION,                                       RGB_BREATHING_SPEED,                                                        2 * RGB_BREATHING_SPEED,                                                       2 * RGB_BREATHING_SPEED},
        .colors         = {  {RGB_HUE_ACCENT_ALT, RGB_SAT_ACCENT_ALT, RGB_MAX_VAL},   {RGB_HUE_ACCENT_ALT, RGB_SAT_ACCENT_ALT, RGB_MAX_VAL}, {RGB_HUE_MAIN_ONESHOT, RGB_SAT_MAIN_ONESHOT, RGB_MAX_VAL},   {ONESHOT_BREATHING_A(RGB_HUE_ACCENT_ALT), RGB_SAT_MAIN_ONESHOT, RGB_MAX_VAL},  {ONESHOT_BREATHING_B(RGB_HUE_ACCENT_ALT), RGB_SAT_MAIN_ONESHOT, RGB_MAX_VAL}},
        .loop_start_idx = 3,
        .loop_stop_idx  = 5},
    [RGB_ANIMATION_ONESHOT_CTRL_FIRST_ON] = {
        .start_duration = RGB_ONESHOT_ON_FIRST_START_DURATION, 
        .durations      = {                     RGB_ONESHOT_ON_FIRST_HOLD_DURATION,                      RGB_ONESHOT_ON_FIRST_FADE_DURATION,                                       RGB_BREATHING_SPEED,                                                       2 * RGB_BREATHING_SPEED,                                                       2 * RGB_BREATHING_SPEED},
        .colors         = {{RGB_HUE_ACCENT_CTRL, RGB_SAT_ACCENT_CTRL, RGB_MAX_VAL}, {RGB_HUE_ACCENT_CTRL, RGB_SAT_ACCENT_CTRL, RGB_MAX_VAL}, {RGB_HUE_MAIN_ONESHOT, RGB_SAT_MAIN_ONESHOT, RGB_MAX_VAL}, {ONESHOT_BREATHING_A(RGB_HUE_ACCENT_CTRL), RGB_SAT_MAIN_ONESHOT, RGB_MAX_VAL}, {ONESHOT_BREATHING_B(RGB_HUE_ACCENT_CTRL), RGB_SAT_MAIN_ONESHOT, RGB_MAX_VAL}},
        .loop_start_idx = 3,
        .loop_stop_idx  = 5},
    [RGB_ANIMATION_ONESHOT_GUI_FIRST_ON] = {
        .start_duration = RGB_ONESHOT_ON_FIRST_START_DURATION, 
        .durations      = {                     RGB_ONESHOT_ON_FIRST_HOLD_DURATION,                      RGB_ONESHOT_ON_FIRST_FADE_DURATION,                                       RGB_BREATHING_SPEED,                                                        2 * RGB_BREATHING_SPEED,                                                       2 * RGB_BREATHING_SPEED},
        .colors         = {  {RGB_HUE_ACCENT_GUI, RGB_SAT_ACCENT_GUI, RGB_MAX_VAL},   {RGB_HUE_ACCENT_GUI, RGB_SAT_ACCENT_GUI, RGB_MAX_VAL}, {RGB_HUE_MAIN_ONESHOT, RGB_SAT_MAIN_ONESHOT, RGB_MAX_VAL},   {ONESHOT_BREATHING_A(RGB_HUE_ACCENT_GUI), RGB_SAT_MAIN_ONESHOT, RGB_MAX_VAL},  {ONESHOT_BREATHING_B(RGB_HUE_ACCENT_GUI), RGB_SAT_MAIN_ONESHOT, RGB_MAX_VAL}},
        .loop_start_idx = 3,
        .loop_stop_idx  = 5},

    // oneshot modifiers - extra oneshots on
    [RGB_ANIMATION_ONESHOT_ALT_EXTRA_ON] = {
        .start_duration = RGB_ONESHOT_ON_EXTRA_START_DURATION, 
        .durations      = {                    RGB_ONESHOT_ON_EXTRA_HOLD_DURATION,                       RGB_ONESHOT_ON_EXTRA_FADE_DURATION,                                       RGB_BREATHING_SPEED,                                                        2 * RGB_BREATHING_SPEED,                                                       2 * RGB_BREATHING_SPEED},
        .colors         = {  {RGB_HUE_ACCENT_ALT, RGB_SAT_ACCENT_ALT, RGB_MAX_VAL},   {RGB_HUE_ACCENT_ALT, RGB_SAT_ACCENT_ALT, RGB_MAX_VAL}, {RGB_HUE_MAIN_ONESHOT, RGB_SAT_MAIN_ONESHOT, RGB_MAX_VAL},   {ONESHOT_BREATHING_A(RGB_HUE_ACCENT_ALT), RGB_SAT_MAIN_ONESHOT, RGB_MAX_VAL},  {ONESHOT_BREATHING_B(RGB_HUE_ACCENT_ALT), RGB_SAT_MAIN_ONESHOT, RGB_MAX_VAL}},
        .loop_start_idx = 3,
        .loop_stop_idx  = 5},
    [RGB_ANIMATION_ONESHOT_CTRL_EXTRA_ON] = {
        .start_duration = RGB_ONESHOT_ON_EXTRA_START_DURATION, 
        .durations      = {                     RGB_ONESHOT_ON_EXTRA_HOLD_DURATION,                      RGB_ONESHOT_ON_EXTRA_FADE_DURATION,                                       RGB_BREATHING_SPEED,                                                       2 * RGB_BREATHING_SPEED,                                                       2 * RGB_BREATHING_SPEED},
        .colors         = {{RGB_HUE_ACCENT_CTRL, RGB_SAT_ACCENT_CTRL, RGB_MAX_VAL}, {RGB_HUE_ACCENT_CTRL, RGB_SAT_ACCENT_CTRL, RGB_MAX_VAL}, {RGB_HUE_MAIN_ONESHOT, RGB_SAT_MAIN_ONESHOT, RGB_MAX_VAL}, {ONESHOT_BREATHING_A(RGB_HUE_ACCENT_CTRL), RGB_SAT_MAIN_ONESHOT, RGB_MAX_VAL}, {ONESHOT_BREATHING_B(RGB_HUE_ACCENT_CTRL), RGB_SAT_MAIN_ONESHOT, RGB_MAX_VAL}},
        .loop_start_idx = 3,
        .loop_stop_idx  = 5},
    [RGB_ANIMATION_ONESHOT_GUI_EXTRA_ON] = {
        .start_duration = RGB_ONESHOT_ON_EXTRA_START_DURATION, 
        .durations      = {                     RGB_ONESHOT_ON_EXTRA_HOLD_DURATION,                      RGB_ONESHOT_ON_EXTRA_FADE_DURATION,                                       RGB_BREATHING_SPEED,                                                        2 * RGB_BREATHING_SPEED,                                                       2 * RGB_BREATHING_SPEED},
        .colors         = {  {RGB_HUE_ACCENT_GUI, RGB_SAT_ACCENT_GUI, RGB_MAX_VAL},   {RGB_HUE_ACCENT_GUI, RGB_SAT_ACCENT_GUI, RGB_MAX_VAL}, {RGB_HUE_MAIN_ONESHOT, RGB_SAT_MAIN_ONESHOT, RGB_MAX_VAL},   {ONESHOT_BREATHING_A(RGB_HUE_ACCENT_GUI), RGB_SAT_MAIN_ONESHOT, RGB_MAX_VAL},  {ONESHOT_BREATHING_B(RGB_HUE_ACCENT_GUI), RGB_SAT_MAIN_ONESHOT, RGB_MAX_VAL}},
        .loop_start_idx = 3,
        .loop_stop_idx  = 5},

    // oneshot modifiers - workman base layer
    [RGB_ANIMATION_ONESHOT_ALT_OFF_BASE_WORKMAN] = {
        .start_duration = RGB_ONESHOT_OFF_START_DURATION, 
        .durations      = {                            RGB_ONESHOT_OFF_FADE_DURATION,                       RGB_STATIC_DUMMY_DURATION},
        .colors         = {    {RGB_HUE_ACCENT_ALT, RGB_SAT_ACCENT_ALT, RGB_MAX_VAL},     {RGB_HUE_ACCENT_ALT, RGB_SAT_ACCENT_ALT, 0}},
        .loop_start_idx = 1,
        .loop_stop_idx  = 2},
    [RGB_ANIMATION_ONESHOT_CTRL_OFF_BASE_WORKMAN] = {
        .start_duration = RGB_ONESHOT_OFF_START_DURATION, 
        .durations      = {                            RGB_ONESHOT_OFF_FADE_DURATION,                       RGB_STATIC_DUMMY_DURATION},
        .colors         = {  {RGB_HUE_ACCENT_CTRL, RGB_SAT_ACCENT_CTRL, RGB_MAX_VAL},   {RGB_HUE_ACCENT_CTRL, RGB_SAT_ACCENT_CTRL, 0}},
        .loop_start_idx = 1,
        .loop_stop_idx  = 2},
    [RGB_ANIMATION_ONESHOT_GUI_OFF_BASE_WORKMAN] = {
        .start_duration = RGB_ONESHOT_OFF_START_DURATION, 
        .durations      = {                            RGB_ONESHOT_OFF_FADE_DURATION,                       RGB_STATIC_DUMMY_DURATION},
        .colors         = {    {RGB_HUE_ACCENT_GUI, RGB_SAT_ACCENT_GUI, RGB_MAX_VAL},     {RGB_HUE_ACCENT_GUI, RGB_SAT_ACCENT_GUI, 0}},
        .loop_start_idx = 1,
        .loop_stop_idx  = 2},
    [RGB_ANIMATION_ONESHOT_MULTIPLE_OFF_BASE_WORKMAN] = {
        .start_duration = RGB_ONESHOT_OFF_START_DURATION, 
        .durations      = {                            RGB_ONESHOT_OFF_FADE_DURATION,                       RGB_STATIC_DUMMY_DURATION},
        .colors         = {{RGB_HUE_MAIN_ONESHOT, RGB_SAT_MAIN_ONESHOT, RGB_MAX_VAL}, {RGB_HUE_MAIN_ONESHOT, RGB_SAT_MAIN_ONESHOT, 0}},
        .loop_start_idx = 1,
        .loop_stop_idx  = 2},

    // oneshot modifiers - other base layer
    [RGB_ANIMATION_ONESHOT_ALT_OFF_BASE_OTHER] = {
        .start_duration = RGB_ONESHOT_OFF_START_DURATION, 
        .durations      = {                            RGB_ONESHOT_OFF_FADE_DURATION,                           RGB_STATIC_DUMMY_DURATION},
        .colors         = {    {RGB_HUE_ACCENT_ALT, RGB_SAT_ACCENT_ALT, RGB_MAX_VAL}, {RGB_HUE_MAIN_BASE, RGB_SAT_MAIN_BASE, RGB_MAX_VAL}},
        .loop_start_idx = 1,
        .loop_stop_idx  = 2},
    [RGB_ANIMATION_ONESHOT_CTRL_OFF_BASE_OTHER] = {
        .start_duration = RGB_ONESHOT_OFF_START_DURATION, 
        .durations      = {                            RGB_ONESHOT_OFF_FADE_DURATION,                           RGB_STATIC_DUMMY_DURATION},
        .colors         = {  {RGB_HUE_ACCENT_CTRL, RGB_SAT_ACCENT_CTRL, RGB_MAX_VAL}, {RGB_HUE_MAIN_BASE, RGB_SAT_MAIN_BASE, RGB_MAX_VAL}},
        .loop_start_idx = 1,
        .loop_stop_idx  = 2},
    [RGB_ANIMATION_ONESHOT_GUI_OFF_BASE_OTHER] = {
        .start_duration = RGB_ONESHOT_OFF_START_DURATION, 
        .durations      = {                            RGB_ONESHOT_OFF_FADE_DURATION,                           RGB_STATIC_DUMMY_DURATION},
        .colors         = {    {RGB_HUE_ACCENT_GUI, RGB_SAT_ACCENT_GUI, RGB_MAX_VAL}, {RGB_HUE_MAIN_BASE, RGB_SAT_MAIN_BASE, RGB_MAX_VAL}},
        .loop_start_idx = 1,
        .loop_stop_idx  = 2},
    [RGB_ANIMATION_ONESHOT_MULTIPLE_OFF_BASE_OTHER] = {
        .start_duration = RGB_ONESHOT_OFF_START_DURATION, 
        .durations      = {                            RGB_ONESHOT_OFF_FADE_DURATION,                           RGB_STATIC_DUMMY_DURATION},
        .colors         = {{RGB_HUE_MAIN_ONESHOT, RGB_SAT_MAIN_ONESHOT, RGB_MAX_VAL}, {RGB_HUE_MAIN_BASE, RGB_SAT_MAIN_BASE, RGB_MAX_VAL}},
        .loop_start_idx = 1,
        .loop_stop_idx  = 2},
};

void rgb_oneshot_on_task(size_t oneshot) {
    if (keyboard_state.n_oneshots_active == 0) {
        // first oneshot on
        switch (oneshot) {
            case ONESHOT_ALT:
                rgb_start_animation(&(keyboard_state.rgb_state), RGB_ANIMATION_ONESHOT_ALT_FIRST_ON);
                break;
            case ONESHOT_CTRL:
                rgb_start_animation(&(keyboard_state.rgb_state), RGB_ANIMATION_ONESHOT_CTRL_FIRST_ON);
                break;
            case ONESHOT_GUI:
                rgb_start_animation(&(keyboard_state.rgb_state), RGB_ANIMATION_ONESHOT_GUI_FIRST_ON);
                break;
        }
    } else {
        switch (oneshot) {
            case ONESHOT_ALT:
                rgb_start_animation(&(keyboard_state.rgb_state), RGB_ANIMATION_ONESHOT_ALT_EXTRA_ON);
                break;
            case ONESHOT_CTRL:
                rgb_start_animation(&(keyboard_state.rgb_state), RGB_ANIMATION_ONESHOT_CTRL_EXTRA_ON);
                break;
            case ONESHOT_GUI:
                rgb_start_animation(&(keyboard_state.rgb_state), RGB_ANIMATION_ONESHOT_GUI_EXTRA_ON);
                break;
        }
    }
}

void rgb_oneshots_off_task(void) {
    if (keyboard_state.n_oneshots_active == 1) {
        switch (keyboard_state.last_oneshot_active) {
            case ONESHOT_ALT:
                switch (keyboard_state.current_base_layer) {
                    case LAYER_BASE_WORKMAN:
                        rgb_start_animation(&(keyboard_state.rgb_state), RGB_ANIMATION_ONESHOT_ALT_OFF_BASE_WORKMAN);
                        break;
                    case LAYER_BASE_QWERTY:
                    case LAYER_BASE_GAMING:
                        rgb_start_animation(&(keyboard_state.rgb_state), RGB_ANIMATION_ONESHOT_ALT_OFF_BASE_OTHER);
                        break;
                }
                break;
            case ONESHOT_CTRL:
                switch (keyboard_state.current_base_layer) {
                    case LAYER_BASE_WORKMAN:
                        rgb_start_animation(&(keyboard_state.rgb_state), RGB_ANIMATION_ONESHOT_CTRL_OFF_BASE_WORKMAN);
                        break;
                    case LAYER_BASE_QWERTY:
                    case LAYER_BASE_GAMING:
                        rgb_start_animation(&(keyboard_state.rgb_state), RGB_ANIMATION_ONESHOT_CTRL_OFF_BASE_OTHER);
                        break;
                }
                break;
            case ONESHOT_GUI:
                switch (keyboard_state.current_base_layer) {
                    case LAYER_BASE_WORKMAN:
                        rgb_start_animation(&(keyboard_state.rgb_state), RGB_ANIMATION_ONESHOT_GUI_OFF_BASE_WORKMAN);
                        break;
                    case LAYER_BASE_QWERTY:
                    case LAYER_BASE_GAMING:
                        rgb_start_animation(&(keyboard_state.rgb_state), RGB_ANIMATION_ONESHOT_GUI_OFF_BASE_OTHER);
                        break;
                }
                break;
        }
    } else {
        switch (keyboard_state.current_base_layer) {
            case LAYER_BASE_WORKMAN:
                rgb_start_animation(&(keyboard_state.rgb_state), RGB_ANIMATION_ONESHOT_MULTIPLE_OFF_BASE_WORKMAN);
                break;
            case LAYER_BASE_QWERTY:
            case LAYER_BASE_GAMING:
                rgb_start_animation(&(keyboard_state.rgb_state), RGB_ANIMATION_ONESHOT_MULTIPLE_OFF_BASE_OTHER);
                break;
        }
    }
}

// ============================================================================
// BASE LAYER SWITCHING
// ============================================================================

void sk_base_down_cb(superkey_state_t* superkey_state) {
    if (keyboard_state.n_oneshots_active > 0) {
        keyboard_state.base_is_locked = true;
        return;
    }
    if (keyboard_state.current_base_layer == LAYER_BASE_WORKMAN) {
        keyboard_state.base_is_locked = false;
    } else if (keyboard_state.current_base_layer == LAYER_BASE_QWERTY) {
        rgb_start_animation(&(keyboard_state.rgb_state), RGB_ANIMATION_BASE_QWERTY_TO_WORKMAN);
        keyboard_state.current_base_layer = LAYER_BASE_WORKMAN;
        keyboard_state.base_is_locked = true;
        layer_off(LAYER_BASE_QWERTY);
    } else if (keyboard_state.current_base_layer == LAYER_BASE_GAMING) {
        rgb_start_animation(&(keyboard_state.rgb_state), RGB_ANIMATION_BASE_GAMING_TO_WORKMAN);
        keyboard_state.current_base_layer = LAYER_BASE_WORKMAN;
        keyboard_state.base_is_locked = true;
        layer_off(LAYER_BASE_GAMING);
    }
}

void sk_base_up_cb(superkey_state_t* superkey_state) {
    if (keyboard_state.n_oneshots_active > 0) {
        return;
    }
    if (keyboard_state.base_is_locked) {
        return;
    } else if (keyboard_state.current_base_layer == LAYER_BASE_WORKMAN) {
        if (superkey_state->timeout_result == NO_TIMEOUT) {
            rgb_start_animation(&(keyboard_state.rgb_state), RGB_ANIMATION_BASE_QWERTY_FROM_WORKMAN);
            keyboard_state.current_base_layer = LAYER_BASE_QWERTY;
            layer_on(LAYER_BASE_QWERTY);
        }
    }
}

void sk_base_timeout_cb(superkey_state_t* superkey_state) {
    if (keyboard_state.n_oneshots_active > 0) {
        return;
    }
    if (keyboard_state.base_is_locked) {
        return;
    } else if (keyboard_state.current_base_layer == LAYER_BASE_WORKMAN) {
        rgb_start_animation(&(keyboard_state.rgb_state), RGB_ANIMATION_BASE_GAMING_FROM_WORKMAN);
        keyboard_state.current_base_layer = LAYER_BASE_GAMING;
        keyboard_state.base_is_locked = true;
        layer_on(LAYER_BASE_GAMING);
    }
}

// ============================================================================
// EXTRA SYMBOLS
// ============================================================================

void sk_symb_down_cb(superkey_state_t* superkey_state) {
    layer_on(LAYER_SYMBOLS);
}

void sk_symb_up_cb(superkey_state_t* superkey_state) {
    layer_off(LAYER_SYMBOLS);
}

// ============================================================================
// ONESHOT MODIFIERS
// ============================================================================

void oneshot_on_task(size_t oneshot) {
    rgb_oneshot_on_task(oneshot);
    if (keyboard_state.n_oneshots_active == 0) {
        intercept_on(INTERCEPT_ONESHOTS);
    }
    keyboard_state.n_oneshots_active++;
    keyboard_state.last_oneshot_active = oneshot;
    keyboard_state.oneshot_is_active[oneshot] = true;
}

void oneshots_off_task(void) {
    rgb_oneshots_off_task();
    intercept_off(INTERCEPT_ONESHOTS);
    keyboard_state.n_oneshots_active = 0;
    memset(keyboard_state.oneshot_is_active, 0, sizeof(keyboard_state.oneshot_is_active));
}

bool intercept_oneshots_cb(uint16_t keycode, bool pressed) {

    // only apply oneshot to simple keycodes
    if (keycode < KC_A || keycode > KC_EXSEL || !pressed) {
        return true;
    }

    // collect and apply modifiers
    for (int i = 0; i < N_ONESHOTS; i++) {
        if (keyboard_state.oneshot_is_active[i]) {
            switch (i) {
                case ONESHOT_ALT:
                    keycode = A(keycode);
                    break;
                case ONESHOT_CTRL:
                    keycode = C(keycode);
                    break;
                case ONESHOT_GUI:
                    keycode = G(keycode);
                    break;
            }
        }
    }
    tap_code16(keycode);

    // turn oneshots off
    rgb_oneshots_off_task();
    intercept_off(INTERCEPT_ONESHOTS);
    keyboard_state.n_oneshots_active = 0;
    memset(keyboard_state.oneshot_is_active, 0, sizeof(keyboard_state.oneshot_is_active));
    return false;
}

// ============================================================================
// MOUSE TRIGGERABLE MODIFIERS
// ============================================================================

void mouse_triggerable_modifier_pointing_device_task(report_mouse_t* mouse_report) {
    if (keyboard_state.mouse_triggerable_modifier_interrupted_dragscroll) {
        // only allow wheel through when dragscroll is interrupted
        mouse_report->x = 0;
        mouse_report->y = 0;
    }
    if (keyboard_state.mouse_triggerable_modifier_is_active && !keyboard_state.mouse_triggerable_modifier_is_triggered && (mouse_report->buttons != 0 || mouse_report->v != 0 || mouse_report->h != 0)) {
        keyboard_state.mouse_triggerable_modifier_is_triggered = true;
        keyboard_state.mouse_is_delayed = true;
        keyboard_state.mouse_delay_start_time = timer_read32();
        keyboard_state.mouse_delay_start_buttons = mouse_report->buttons;
        switch (keyboard_state.active_mouse_triggerable_modifier) {
            case MOUSE_TRIGGERABLE_MODIFIER_ALT:
                register_code(KC_LALT);
                superkey_inject_interrupt(SK_ALT, CK_MOUSE_INTERRUPT);
                break;
            case MOUSE_TRIGGERABLE_MODIFIER_CTRL:
                register_code(KC_LCTL);
                superkey_inject_interrupt(SK_CTRL, CK_MOUSE_INTERRUPT);
                break;
            case MOUSE_TRIGGERABLE_MODIFIER_GUI:
                register_code(KC_LGUI);
                superkey_inject_interrupt(SK_GUI, CK_MOUSE_INTERRUPT);
                break;
        }
    }
    // in order to reliably mod-click, we have to give the modifier some time to reach the OS before we click
    if (keyboard_state.mouse_is_delayed) {
        if (timer_elapsed32(keyboard_state.mouse_delay_start_time) > MOUSE_DELAY_DURATION) {
            keyboard_state.mouse_is_delayed = false;
            mouse_report->buttons |= keyboard_state.delayed_mouse_buttons;
            mouse_report->v += keyboard_state.delayed_mouse_wheel_v;
            mouse_report->h += keyboard_state.delayed_mouse_wheel_h;
            keyboard_state.delayed_mouse_buttons = 0;
            keyboard_state.delayed_mouse_wheel_v = 0;
            keyboard_state.delayed_mouse_wheel_h = 0; 
        } else {
            keyboard_state.delayed_mouse_buttons |= mouse_report->buttons;
            keyboard_state.delayed_mouse_wheel_v += mouse_report->v;
            keyboard_state.delayed_mouse_wheel_h += mouse_report->h;
            mouse_report->buttons = keyboard_state.mouse_delay_start_buttons;
            mouse_report->v = 0;
            mouse_report->h = 0;
        }
    }
}

void mouse_triggerable_modifier_on(size_t mouse_triggerable_modifier) {
    if (keyboard_state.mouse_triggerable_modifier_is_active) {
        return;
    }
    keyboard_state.mouse_triggerable_modifier_is_active = true;
    keyboard_state.mouse_triggerable_modifier_is_triggered = false;
    keyboard_state.active_mouse_triggerable_modifier = mouse_triggerable_modifier;
    if (is_dragscroll_on()) {
        dragscroll_off();
        keyboard_state.mouse_triggerable_modifier_interrupted_dragscroll = true;
    }
    // mouse_passthrough_send_buttons_on();
    // mouse_passthrough_block_buttons_on();
    mouse_passthrough_send_wheel_on();
    mouse_passthrough_block_wheel_on();
}

void mouse_triggerable_modifier_off(void) {
    if (!keyboard_state.mouse_triggerable_modifier_is_active) {
        return;
    }
    if (keyboard_state.mouse_triggerable_modifier_is_triggered) {
        switch (keyboard_state.active_mouse_triggerable_modifier) {
            case MOUSE_TRIGGERABLE_MODIFIER_ALT:
                unregister_code(KC_LALT);
                break;
            case MOUSE_TRIGGERABLE_MODIFIER_CTRL:
                unregister_code(KC_LCTL);
                break;
            case MOUSE_TRIGGERABLE_MODIFIER_GUI:
                unregister_code(KC_LGUI);
                break;
        }
    }
    keyboard_state.mouse_triggerable_modifier_is_active = false;
    keyboard_state.mouse_triggerable_modifier_is_triggered = false;
    if (keyboard_state.mouse_triggerable_modifier_interrupted_dragscroll) {
        dragscroll_on();
        keyboard_state.mouse_triggerable_modifier_interrupted_dragscroll = false;
    }
    // mouse_passthrough_send_buttons_off();
    // mouse_passthrough_block_buttons_off();
    mouse_passthrough_send_wheel_off();
    mouse_passthrough_block_wheel_off();
}

void mouse_triggerable_modifier_process_record_user_task(uint16_t keycode) {
    if (!(keycode == KC_LSFT || keycode == KC_RSFT)) {
        mouse_triggerable_modifier_off();
    }
}

// ============================================================================
// WHEEL ADJUSTMENT
// ============================================================================

void wheel_adjustment_pointing_device_task(report_mouse_t* mouse_report) {
    if (!is_hires_scroll_on() || is_dragscroll_on()) {
        return;
    }
    mouse_report->h *= pointing_device_get_hires_scroll_resolution();
    mouse_report->v *= pointing_device_get_hires_scroll_resolution();
}

// ============================================================================
// CTRL ONESHOT AND UTILITIES LAYER
// ============================================================================

// when oneshots are active, the utilities layer is momentary, without support for custom keycodes
// when oneshots are inactive, the utilities layer is an oneshot that can become persistent

void utilities_oneshot_on_task(void) {
    // mouse_passthrough_send_buttons_on();
    // mouse_passthrough_block_buttons_on();
    mouse_passthrough_send_pointer_on();
    mouse_passthrough_block_pointer_on();
    mouse_passthrough_send_wheel_on();
    mouse_passthrough_block_wheel_on();
    dragscroll_on();
    layer_on(LAYER_UTILITIES);
    intercept_on(INTERCEPT_UTILITIES_ONESHOT);
    keyboard_state.utilities_oneshot_state = UTILITIES_ONESHOT_STATE_WAITING_FOR_FIRST_KEY;
    keyboard_state.utilities_ab_undo_is_next = true;
}

void utilities_oneshot_off_task(void) {
    // mouse_passthrough_send_buttons_off();
    // mouse_passthrough_block_buttons_off();
    mouse_passthrough_send_pointer_off();
    mouse_passthrough_block_pointer_off();
    mouse_passthrough_send_wheel_off();
    mouse_passthrough_block_wheel_off();
    dragscroll_off();
    layer_off(LAYER_UTILITIES);
    intercept_off(INTERCEPT_UTILITIES_ONESHOT);
    timeout_off(TIMEOUT_UTILITIES_ONESHOT);
    keyboard_state.utilities_oneshot_state = UTILITIES_ONESHOT_STATE_OFF;
    keyboard_state.utilities_ab_undo_is_registered = false;
    keyboard_state.utilities_ab_redo_is_registered = false;
    clear_keyboard();
}

void sk_ctrl_down_cb(superkey_state_t* superkey_state) {
    mouse_triggerable_modifier_on(MOUSE_TRIGGERABLE_MODIFIER_CTRL);
    if (keyboard_state.n_oneshots_active > 0) {
        keyboard_state.utilities_momentary_mode_is_on = true;
        layer_on(LAYER_UTILITIES);
    } else {
        keyboard_state.utilities_momentary_mode_is_on = false;
        intercept_on(INTERCEPT_CTRL);
    }
}

void sk_ctrl_up_cb(superkey_state_t* superkey_state) {
    mouse_triggerable_modifier_off();
    if (keyboard_state.utilities_momentary_mode_is_on) {
        layer_off(LAYER_UTILITIES);
    } else {
        intercept_off(INTERCEPT_CTRL);
        clear_keyboard();
    }
    if (superkey_state->interrupt_result == NO_INTERRUPT && superkey_state->timeout_result == NO_TIMEOUT) {
        if (keyboard_state.oneshot_is_active[ONESHOT_CTRL]) {
            oneshots_off_task();
            if (keyboard_state.utilities_oneshot_state != UTILITIES_ONESHOT_STATE_OFF) {
                utilities_oneshot_off_task();
            }
        } else {
            oneshot_on_task(ONESHOT_CTRL);
            if (keyboard_state.n_oneshots_active == 1) {
                utilities_oneshot_on_task();
            }
        }
    }
}

bool intercept_ctrl_cb(uint16_t keycode, bool pressed) {
    if (keycode < KC_A || keycode > KC_EXSEL) {
        return true;
    }
    if (pressed) {
        register_code16(C(keycode));
    } else {
        unregister_code16(C(keycode));
    }
    return false;
}

bool intercept_utilities_oneshot_cb(uint16_t keycode, bool pressed) {
    switch (keycode) {

        // exit utilities oneshot
        case KC_SPC:
        case KC_ENT:
        case KC_BSPC:
            oneshots_off_task();
            utilities_oneshot_off_task();
            if (pressed) {
                register_code(keycode);
            } else {
                unregister_code(keycode);
            }
            return false;

        // print screen
        case KC_PSCR:
            if (pressed) {
                tap_code(KC_PSCR);
            }
            break;

        // basic keycodes
        case KC_ESC:
        case KC_DEL:
        case KC_DOWN:
        case KC_UP:
            if (pressed) {
                register_code(keycode);
            } else {
                unregister_code(keycode);
            }
            break;

        // modified arrow keys
        case CK_SFTU:
            if (pressed) {
                register_code16(S(KC_UP));
            } else {
                unregister_code16(S(KC_UP));
            }
            break;
        case CK_SFTD:
            if (pressed) {
                register_code16(S(KC_DOWN));
            } else {
                unregister_code16(S(KC_DOWN));
            }
            break;
        case CK_ALTL:
            if (pressed) {
                register_code16(A(KC_LEFT));
            } else {
                unregister_code16(A(KC_LEFT));
            }
            break;
        case CK_ALTR:
            if (pressed) {
                register_code16(A(KC_RIGHT));
            } else {
                unregister_code16(A(KC_RIGHT));
            }
            break;

        // undo/redo
        case CK_UNDO:
            if (pressed && !keyboard_state.utilities_ab_redo_is_registered) {
                register_code16(C(KC_Z));
                keyboard_state.utilities_ab_undo_is_registered = true;
            }
            if (!pressed && keyboard_state.utilities_ab_undo_is_registered) {
                unregister_code16(C(KC_Z));
                keyboard_state.utilities_ab_undo_is_registered = false;
                keyboard_state.utilities_ab_undo_is_next = false;
            }
            break;
        case CK_REDO:
            if (pressed && !keyboard_state.utilities_ab_undo_is_registered) {
                register_code16(C(S(KC_Z)));
                keyboard_state.utilities_ab_redo_is_registered = true;
            }
            if (!pressed && keyboard_state.utilities_ab_redo_is_registered) {
                unregister_code16(C(S(KC_Z)));
                keyboard_state.utilities_ab_redo_is_registered = false;
                keyboard_state.utilities_ab_undo_is_next = true;
            }
            break;
        case CK_AB:
            if (pressed && !keyboard_state.utilities_ab_undo_is_registered && !keyboard_state.utilities_ab_redo_is_registered) {
                tap_code16(keyboard_state.utilities_ab_undo_is_next ? C(KC_Z) : C(S(KC_Z)));
                keyboard_state.utilities_ab_undo_is_next = !keyboard_state.utilities_ab_undo_is_next;
            }
            break;

        // default
        default:
            return true;
    }

    // make the utilities layer persistent if more than one keypress occurs, or if the first key is held
    switch (keyboard_state.utilities_oneshot_state) {
        case UTILITIES_ONESHOT_STATE_WAITING_FOR_FIRST_KEY:
            if (pressed) {
                keyboard_state.utilities_oneshot_state = UTILITIES_ONESHOT_STATE_HOLDING_FIRST_KEY;
                timeout_on(TIMEOUT_UTILITIES_ONESHOT);
            }
            break;
        case UTILITIES_ONESHOT_STATE_HOLDING_FIRST_KEY:
            if (pressed) {
                keyboard_state.utilities_oneshot_state = UTILITIES_ONESHOT_STATE_PERSISTENT;
                timeout_off(TIMEOUT_UTILITIES_ONESHOT);
            } else {
                keyboard_state.utilities_oneshot_state = UTILITIES_ONESHOT_STATE_WAITING_FOR_SECOND_KEY;
            }
            break;
        case UTILITIES_ONESHOT_STATE_WAITING_FOR_SECOND_KEY:
            if (pressed) {
                keyboard_state.utilities_oneshot_state = UTILITIES_ONESHOT_STATE_PERSISTENT;
                timeout_off(TIMEOUT_UTILITIES_ONESHOT);
            }
            break;
    }

    return false;
}

void timeout_utilities_oneshot_cb(void) {
    if (keyboard_state.utilities_oneshot_state != UTILITIES_ONESHOT_STATE_HOLDING_FIRST_KEY) {
        oneshots_off_task();
        utilities_oneshot_off_task();
    }
}

// ============================================================================
// ALT ONESHOT AND ARROW LAYER
// ============================================================================

// selective alt only applies to up/down
// selective ctrl only applies to left/right
// selective shift applies to everything

void sk_alt_down_cb(superkey_state_t* superkey_state) {
    mouse_triggerable_modifier_on(MOUSE_TRIGGERABLE_MODIFIER_ALT);
    layer_on(LAYER_ARROWS);
    intercept_on(INTERCEPT_ARROWS);
}

void sk_alt_up_cb(superkey_state_t* superkey_state) {
    mouse_triggerable_modifier_off();
    layer_off(LAYER_ARROWS);
    intercept_off(INTERCEPT_ARROWS);

    // make sure things are cleaned up
    keyboard_state.n_selective_modifiers_active = 0;
    memset(keyboard_state.selective_modifier_is_active, 0, sizeof(keyboard_state.selective_modifier_is_active));
    if (keyboard_state.arrow_delete_word_left_is_registered) {
        keyboard_state.arrow_delete_word_left_is_registered = false;
        unregister_code16(C(KC_BSPC));
    }
    if (keyboard_state.arrow_delete_word_right_is_registered) {
        keyboard_state.arrow_delete_word_right_is_registered = false;
        unregister_code16(C(KC_DEL));
    }
    unregister_code16(keyboard_state.arrow_horizontal_last_keycode_registered);
    keyboard_state.arrow_horizontal_last_keycode_registered = KC_NO;
    keyboard_state.arrow_horizontal_state = ARROW_STATE_CENTER;
    unregister_code16(keyboard_state.arrow_vertical_last_keycode_registered);
    keyboard_state.arrow_vertical_last_keycode_registered = KC_NO;
    keyboard_state.arrow_vertical_state = ARROW_STATE_CENTER;

    // activate/deactivate oneshot if the key was tapped
    if (superkey_state->interrupt_result == NO_INTERRUPT && superkey_state->timeout_result == NO_TIMEOUT) {
        if (keyboard_state.oneshot_is_active[ONESHOT_ALT]) {
            oneshots_off_task();
        } else {
            oneshot_on_task(ONESHOT_ALT);
            if (keyboard_state.utilities_oneshot_state != UTILITIES_ONESHOT_STATE_OFF) {
                utilities_oneshot_off_task();
            }
        }
    }
}

bool intercept_arrows_cb(uint16_t keycode, bool pressed) {
    bool update_horizontal = false;
    bool update_vertical = false;
    switch (keycode) {

        // home and end keys
        case KC_HOME:
        case KC_END:
            if (pressed) {
                if (keyboard_state.selective_modifier_is_active[ARROW_MODIFIER_SELECTIVE_SHIFT]) {
                    keycode = S(keycode);
                }
                tap_code16(keycode);
            }
            return false;

        // arrow keys
        case KC_RIGHT:
            if (keyboard_state.arrow_horizontal_state == ARROW_STATE_RIGHT && !pressed) {
                keyboard_state.arrow_horizontal_state = ARROW_STATE_CENTER;
                update_horizontal = true;
            } else if (keyboard_state.arrow_horizontal_state != ARROW_STATE_RIGHT && pressed) {
                keyboard_state.arrow_horizontal_state = ARROW_STATE_RIGHT;
                update_horizontal = true;
                if ((keyboard_state.n_selective_modifiers_active > 0) && (keyboard_state.arrow_vertical_state != ARROW_STATE_CENTER)) {
                    keyboard_state.arrow_vertical_state = ARROW_STATE_CENTER;
                    update_vertical = true;
                }
            }
            break;
        case KC_LEFT:
            if (keyboard_state.arrow_horizontal_state == ARROW_STATE_LEFT && !pressed) {
                keyboard_state.arrow_horizontal_state = ARROW_STATE_CENTER;
                update_horizontal = true;
            } else if (keyboard_state.arrow_horizontal_state != ARROW_STATE_LEFT && pressed) {
                keyboard_state.arrow_horizontal_state = ARROW_STATE_LEFT;
                update_horizontal = true;
                if ((keyboard_state.n_selective_modifiers_active > 0) && (keyboard_state.arrow_vertical_state != ARROW_STATE_CENTER)) {
                    keyboard_state.arrow_vertical_state = ARROW_STATE_CENTER;
                    update_vertical = true;
                }
            }
            break;
        case KC_DOWN:
            if (keyboard_state.arrow_vertical_state == ARROW_STATE_DOWN && !pressed) {
                keyboard_state.arrow_vertical_state = ARROW_STATE_CENTER;
                update_vertical = true;
            } else if (keyboard_state.arrow_vertical_state != ARROW_STATE_DOWN && pressed) {
                keyboard_state.arrow_vertical_state = ARROW_STATE_DOWN;
                update_vertical = true;
                if ((keyboard_state.n_selective_modifiers_active > 0) && (keyboard_state.arrow_horizontal_state != ARROW_STATE_CENTER)) {
                    keyboard_state.arrow_horizontal_state = ARROW_STATE_CENTER;
                    update_horizontal = true;
                }
            }
            break;
        case KC_UP:
            if (keyboard_state.arrow_vertical_state == ARROW_STATE_UP && !pressed) {
                keyboard_state.arrow_vertical_state = ARROW_STATE_CENTER;
                update_vertical = true;
            } else if (keyboard_state.arrow_vertical_state != ARROW_STATE_UP && pressed) {
                keyboard_state.arrow_vertical_state = ARROW_STATE_UP;
                update_vertical = true;
                if ((keyboard_state.n_selective_modifiers_active > 0) && (keyboard_state.arrow_horizontal_state != ARROW_STATE_CENTER)) {
                    keyboard_state.arrow_horizontal_state = ARROW_STATE_CENTER;
                    update_horizontal = true;
                }
            }
            break;

        // standard modifier keys
        case KC_LALT:
        case KC_LCTL:
        case KC_LSFT:
            if (pressed) {
                register_code(keycode);
            } else {
                unregister_code(keycode);
            }
            return false;

        // selective modifier keys
        case CK_SALT:
            keyboard_state.selective_modifier_is_active[ARROW_MODIFIER_SELECTIVE_ALT] = pressed;
            keyboard_state.n_selective_modifiers_active += pressed ? 1 : -1;
            break;
        case CK_SCTL:
            keyboard_state.selective_modifier_is_active[ARROW_MODIFIER_SELECTIVE_CTRL] = pressed;
            keyboard_state.n_selective_modifiers_active += pressed ? 1 : -1;
            break;
        case CK_SSFT:
            keyboard_state.selective_modifier_is_active[ARROW_MODIFIER_SELECTIVE_SHIFT] = pressed;
            keyboard_state.n_selective_modifiers_active += pressed ? 1 : -1;
            break;

        // delete-word keys
        case CK_DWL:
            if (pressed) {
                keyboard_state.arrow_delete_word_left_is_registered = true;
                register_code16(C(KC_BSPC));
            } else {
                keyboard_state.arrow_delete_word_left_is_registered = false;
                unregister_code16(C(KC_BSPC));
            }
            return false;
        case CK_DWR:
            if (pressed) {
                keyboard_state.arrow_delete_word_right_is_registered = true;
                register_code16(C(KC_DEL));
            } else {
                keyboard_state.arrow_delete_word_right_is_registered = false;
                unregister_code16(C(KC_DEL));
            }
            return false;

        // delete-word key
        case CK_DLN:
            if (pressed) {
                tap_code(KC_MINUS);
                tap_code(KC_END);
                tap_code(KC_END);
                tap_code16(S(KC_HOME));
                tap_code16(S(KC_HOME));
                tap_code16(S(KC_LEFT));
                tap_code(KC_BSPC);
            }
            return false;
        
        // default
        default:
            return true;
    }

    if (update_horizontal) {
        unregister_code16(keyboard_state.arrow_horizontal_last_keycode_registered);
        if (keyboard_state.arrow_horizontal_state == ARROW_STATE_CENTER) {
            keyboard_state.arrow_horizontal_last_keycode_registered = KC_NO;
        } else {
            if (keyboard_state.arrow_horizontal_state == ARROW_STATE_LEFT) {
                keycode = KC_LEFT;
            } else {
                keycode = KC_RIGHT;
            }
            if (keyboard_state.selective_modifier_is_active[ARROW_MODIFIER_SELECTIVE_CTRL]) {
                keycode = C(keycode);
            }
            if (keyboard_state.selective_modifier_is_active[ARROW_MODIFIER_SELECTIVE_SHIFT]) {
                keycode = S(keycode);
            }
            register_code16(keycode);
            keyboard_state.arrow_horizontal_last_keycode_registered = keycode;
        }
    }
    if (update_vertical) {
        unregister_code16(keyboard_state.arrow_vertical_last_keycode_registered);
        if (keyboard_state.arrow_vertical_state == ARROW_STATE_CENTER) {
            keyboard_state.arrow_vertical_last_keycode_registered = KC_NO;
        } else {
            if (keyboard_state.arrow_vertical_state == ARROW_STATE_UP) {
                keycode = KC_UP;
            } else {
                keycode = KC_DOWN;
            }
            if (keyboard_state.selective_modifier_is_active[ARROW_MODIFIER_SELECTIVE_ALT]) {
                keycode = A(keycode);
            }
            if (keyboard_state.selective_modifier_is_active[ARROW_MODIFIER_SELECTIVE_SHIFT]) {
                keycode = S(keycode);
            }
            register_code16(keycode);
            keyboard_state.arrow_vertical_last_keycode_registered = keycode;
        }
    }
    
    return false;
}

// ============================================================================
// GUI ONESHOT AND FUNCTION LAYER
// ============================================================================

void sk_gui_down_cb(superkey_state_t* superkey_state) {
    mouse_triggerable_modifier_on(MOUSE_TRIGGERABLE_MODIFIER_GUI);
    layer_on(LAYER_FUNCTION);
}

void sk_gui_up_cb(superkey_state_t* superkey_state) {
    mouse_triggerable_modifier_off();
    layer_off(LAYER_FUNCTION);

    // activate/deactivate oneshot if the key was tapped
    if (superkey_state->multitap_result == DOUBLE_TAP) {
        timeout_on(TIMEOUT_RESET_KEYBOARD);
    } else if (superkey_state->multitap_result == TRIPLE_TAP) {
        timeout_off(TIMEOUT_RESET_KEYBOARD);
        mouse_passthrough_send_reset();
    } else if (superkey_state->interrupt_result == NO_INTERRUPT && superkey_state->timeout_result == NO_TIMEOUT) {
        if (keyboard_state.oneshot_is_active[ONESHOT_GUI]) {
            oneshots_off_task();
        } else {
            oneshot_on_task(ONESHOT_GUI);
            if (keyboard_state.utilities_oneshot_state != UTILITIES_ONESHOT_STATE_OFF) {
                utilities_oneshot_off_task();
            }
        }
    }
}

void timeout_reset_keyboard_cb(void) {
    reset_keyboard();
}

// ============================================================================
// INTERCEPTS
// ============================================================================

intercept_t intercepts[] = {
    //               IDENTIFIER,                    FUNCTION,
    INTERCEPT_DEFINE(INTERCEPT_ARROWS,              &intercept_arrows_cb),
    INTERCEPT_DEFINE(INTERCEPT_CTRL,                &intercept_ctrl_cb),
    INTERCEPT_DEFINE(INTERCEPT_UTILITIES_ONESHOT,   &intercept_utilities_oneshot_cb),
    INTERCEPT_DEFINE(INTERCEPT_ONESHOTS,            &intercept_oneshots_cb),
};
const size_t n_intercepts = sizeof(intercepts) / sizeof(intercept_t);

// ============================================================================
// SUPERKEYS
// ============================================================================

superkey_t superkeys[] = {
    //        KEYCODE,  TAP_TERM,           MULTITAP_TERM,          DOWN_FN,            UP_FN,          TIMEOUT_FN
    SK_DEFINE(SK_BASE,  DEFAULT_TAP_TERM,   DEFAULT_MULTITAP_TERM,  &sk_base_down_cb,   &sk_base_up_cb, &sk_base_timeout_cb),
    SK_DEFINE(SK_SYMB,  DEFAULT_TAP_TERM,   DEFAULT_MULTITAP_TERM,  &sk_symb_down_cb,   &sk_symb_up_cb, NULL),
    SK_DEFINE(SK_ALT,   DEFAULT_TAP_TERM,   DEFAULT_MULTITAP_TERM,  &sk_alt_down_cb,    &sk_alt_up_cb,  NULL),
    SK_DEFINE(SK_CTRL,  DEFAULT_TAP_TERM,   DEFAULT_MULTITAP_TERM,  &sk_ctrl_down_cb,   &sk_ctrl_up_cb, NULL),
    SK_DEFINE(SK_GUI,   DEFAULT_TAP_TERM,   DEFAULT_MULTITAP_TERM,  &sk_gui_down_cb,    &sk_gui_up_cb,  NULL),
};
const size_t n_superkeys = sizeof(superkeys) / sizeof(superkey_t);

// ============================================================================
// TIMEOUTS
// ============================================================================

timeout_t timeouts[] = {
    //             IDENTIFIER,                  FUNCTION,                       DURATION
    TIMEOUT_DEFINE(TIMEOUT_UTILITIES_ONESHOT,   &timeout_utilities_oneshot_cb,  DEFAULT_MULTITAP_TERM),
    TIMEOUT_DEFINE(TIMEOUT_RESET_KEYBOARD,      &timeout_reset_keyboard_cb,     DEFAULT_MULTITAP_TERM),
};
const size_t n_timeouts = sizeof(timeouts) / sizeof(timeout_t);

// ============================================================================
// QMK USER CODE HOOKS
// ============================================================================

void keyboard_post_init_user(void) {
    rgb_init_task(&(keyboard_state.rgb_state));
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (is_mouse_passthrough_connected()) {
        mouse_passthrough_send_buttons_on();
        mouse_passthrough_block_buttons_on();
    } else {
        mouse_passthrough_send_buttons_off();
        mouse_passthrough_block_buttons_off();
    }
    mouse_triggerable_modifier_process_record_user_task(keycode);
    // earlier intercepts can prevent later intercepts from being called
    // earlier superkey interrupts can't prevent later superkey interrupts from being called
    // intercepts can't prevent superkey interrupts from being called
    bool continue_processing = true;
    if (!intercept_process_record_task(keycode, record->event.pressed)) {
        continue_processing = false;
    }
    if (!superkey_process_record_task(keycode, record->event.pressed)) {
        continue_processing = false;
    }
    return continue_processing;
}

void matrix_scan_user(void) {
    rgb_matrix_scan_task(&(keyboard_state.rgb_state));
    superkey_matrix_scan_task();
    timeout_matrix_scan_task();
    mouse_passthrough_reciever_matrix_scan_task();
}

void raw_hid_receive(uint8_t* data, uint8_t length) {
    mouse_passthrough_reciever_raw_hid_receive_task(data);
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    report_mouse_t* mouse_report_ptr = &mouse_report;
    if (!is_mouse_passthrough_connected()) {
        memset(mouse_report_ptr, 0, sizeof(mouse_report));
        return mouse_report;
    }
    mouse_triggerable_modifier_pointing_device_task(mouse_report_ptr);
    wheel_adjustment_pointing_device_task(mouse_report_ptr);
    return mouse_report;
}
