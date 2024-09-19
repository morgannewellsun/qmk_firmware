#pragma once

#include "include/rgb_engine.h"
#include "quantum.h"

// ============================================================================
// QMK: KEYMAP AND KEYCODES
// ============================================================================

enum keymap_layers {
    LAYER_BASE_WORKMAN = 0,
    LAYER_BASE_QWERTY,
    LAYER_BASE_GAMING,
    LAYER_SYMBOLS,
    LAYER_ARROWS,
    LAYER_UTILITIES,
    LAYER_FUNCTION,
};

enum custom_keycodes {
    SK_BASE = SAFE_RANGE,
    SK_SYMB,
    SK_ALT,
    SK_CTRL,
    SK_GUI,
    CK_SALT,            // selective alt modifier
    CK_SCTL,            // selective ctrl modifier
    CK_SSFT,            // selective shift modifier
    CK_DWL,             // delete word left (ctrl-backspace)
    CK_DWR,             // delete word right (ctrl-delete)
    CK_DLN,             // delete line
    CK_SFTU,            // shift-up
    CK_SFTD,            // shift-down
    CK_ALTL,            // alt-left
    CK_ALTR,            // alt-right
    CK_UNDO,            // undo
    CK_REDO,            // redo
    CK_AB,              // toggle undo/redo
    CK_MOUSE_INTERRUPT, // placeholder for interrupts caused by mouse activity
};

// ============================================================================
// QMK: KEY OVERRIDES
// ============================================================================

const key_override_t *key_overrides[2];

// ============================================================================
// RGB ANIMATIONS
// ============================================================================

enum rgb_animation_names {
    RGB_ANIMATION_OFF = 0,
    RGB_ANIMATION_BASE_QWERTY_FROM_WORKMAN,
    RGB_ANIMATION_BASE_GAMING_FROM_WORKMAN,
    RGB_ANIMATION_BASE_QWERTY_TO_WORKMAN,
    RGB_ANIMATION_BASE_GAMING_TO_WORKMAN,
    RGB_ANIMATION_ONESHOT_ALT_FIRST_ON,
    RGB_ANIMATION_ONESHOT_CTRL_FIRST_ON,
    RGB_ANIMATION_ONESHOT_GUI_FIRST_ON,
    RGB_ANIMATION_ONESHOT_ALT_EXTRA_ON,
    RGB_ANIMATION_ONESHOT_CTRL_EXTRA_ON,
    RGB_ANIMATION_ONESHOT_GUI_EXTRA_ON,
    RGB_ANIMATION_ONESHOT_ALT_OFF_BASE_WORKMAN,
    RGB_ANIMATION_ONESHOT_CTRL_OFF_BASE_WORKMAN,
    RGB_ANIMATION_ONESHOT_GUI_OFF_BASE_WORKMAN,
    RGB_ANIMATION_ONESHOT_MULTIPLE_OFF_BASE_WORKMAN,
    RGB_ANIMATION_ONESHOT_ALT_OFF_BASE_OTHER,
    RGB_ANIMATION_ONESHOT_CTRL_OFF_BASE_OTHER,
    RGB_ANIMATION_ONESHOT_GUI_OFF_BASE_OTHER,
    RGB_ANIMATION_ONESHOT_MULTIPLE_OFF_BASE_OTHER,
};

// ============================================================================
// ONESHOT MODIFIERS
// ============================================================================

enum oneshots {
    ONESHOT_ALT = 0,
    ONESHOT_CTRL,
    ONESHOT_GUI,
    N_ONESHOTS,
};

// ============================================================================
// HELD MODIFIERS
// ============================================================================

enum held_modifiers {
    HELD_MODIFIER_ALT = 0,
    HELD_MODIFIER_CTRL,
    HELD_MODIFIER_GUI,
    N_HELD_MODIFIERS,
};

// ============================================================================
// MOUSE PASSTHROUGH
// ============================================================================

enum mouse_triggerable_modifiers {
    MOUSE_TRIGGERABLE_MODIFIER_ALT = 0,
    MOUSE_TRIGGERABLE_MODIFIER_CTRL,
    MOUSE_TRIGGERABLE_MODIFIER_GUI,
    N_MOUSE_TRIGGERABLE_MODIFERS,
};

// ============================================================================
// ALT ONESHOT AND ARROW LAYER
// ============================================================================

enum arrow_modifiers {
    ARROW_MODIFIER_ALT = 0,
    ARROW_MODIFIER_CTRL,
    ARROW_MODIFIER_SHIFT,
    ARROW_MODIFIER_SELECTIVE_ALT,
    ARROW_MODIFIER_SELECTIVE_CTRL,
    ARROW_MODIFIER_SELECTIVE_SHIFT,
    N_ARROW_MODIFIERS,
};

enum arrow_states {
    ARROW_STATE_CENTER = 0,
    ARROW_STATE_UP,
    ARROW_STATE_DOWN,
    ARROW_STATE_LEFT,
    ARROW_STATE_RIGHT,
};

// ============================================================================
// INTERCEPTS
// ============================================================================

enum intercepts {
    INTERCEPT_ARROWS = 0,
    INTERCEPT_CTRL,
    INTERCEPT_UTILITIES_ONESHOT,
    INTERCEPT_ONESHOTS,
};

// ============================================================================
// CTRL ONESHOT AND UTILITIES LAYER
// ============================================================================

enum utilities_oneshot_states {
    UTILITIES_ONESHOT_STATE_OFF = 0,
    UTILITIES_ONESHOT_STATE_WAITING_FOR_FIRST_KEY,
    UTILITIES_ONESHOT_STATE_HOLDING_FIRST_KEY,
    UTILITIES_ONESHOT_STATE_WAITING_FOR_SECOND_KEY,
    UTILITIES_ONESHOT_STATE_PERSISTENT,
};

// ============================================================================
// TIMEOUTS
// ============================================================================

enum timeouts {
    TIMEOUT_UTILITIES_ONESHOT = 0,
};

// ============================================================================
// KEYBOARD STATE
// ============================================================================

typedef struct keyboard_state_t {

    // rgb engine
    rgb_state_t rgb_state;

    // base layer switching
    uint8_t current_base_layer;
    bool base_is_locked;

    // oneshots
    uint8_t n_oneshots_active;
    uint8_t last_oneshot_active;
    bool oneshot_is_active[N_ONESHOTS];

    // held modifiers
    uint8_t n_held_modifiers_registered;
    bool held_modifier_is_registered[N_HELD_MODIFIERS];

    // mouse passthrough
    bool mouse_is_active;
    uint8_t n_mouse_triggerable_modifiers_active;
    bool mouse_triggerable_modifier_is_active[N_MOUSE_TRIGGERABLE_MODIFERS];

    // arrows layer
    bool arrow_modifier_is_active[N_ARROW_MODIFIERS];
    bool arrow_delete_word_left_is_registered;
    bool arrow_delete_word_right_is_registered;
    uint16_t arrow_horizontal_last_keycode_registered;
    uint8_t arrow_horizontal_state;
    uint16_t arrow_vertical_last_keycode_registered;
    uint8_t arrow_vertical_state;

    // utilities layer
    bool utilities_momentary_mode_is_on;
    uint8_t utilities_oneshot_state;
    bool utilities_ab_undo_is_next;
    bool utilities_ab_undo_is_registered;
    bool utilities_ab_redo_is_registered;

} keyboard_state_t;