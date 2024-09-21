#include QMK_KEYBOARD_H
#include "features.h"

// ============================================================================
// KEYMAPS
// ============================================================================

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[LAYER_BASE_WORKMAN] = LAYOUT(
    KC_1,           KC_2,           KC_3,           KC_4,           KC_5,           KC_6,                                           KC_7,           KC_8,           KC_9,           KC_0,           KC_MINS,        KC_EQL,
    KC_ENT,         KC_Q,           KC_D,           KC_R,           KC_W,           KC_B,                                           KC_J,           KC_F,           KC_U,           KC_P,           KC_SCLN,        KC_TAB,
    KC_BSPC,        KC_A,           KC_S,           KC_H,           KC_T,           KC_G,                                           KC_Y,           KC_N,           KC_E,           KC_O,           KC_I,           KC_QUOT,
    KC_LSFT,        KC_Z,           KC_X,           KC_M,           KC_C,           KC_V,           KC_NO,          KC_NO,          KC_K,           KC_L,           KC_COMM,        KC_DOT,         KC_SLSH,        KC_RSFT,
                                    KC_DOWN,        KC_UP,          SK_ALT,         KC_SPC,         SK_CTRL,        SK_BASE,        SK_SYMB,        SK_GUI,         KC_LEFT,        KC_RIGHT
),
[LAYER_BASE_QWERTY] = LAYOUT(
    KC_1,           KC_2,           KC_3,           KC_4,           KC_5,           KC_6,                                           KC_7,           KC_8,           KC_9,           KC_0,           KC_MINS,        KC_EQL,
    KC_ENT,         KC_Q,           KC_W,           KC_E,           KC_R,           KC_T,                                           KC_Y,           KC_U,           KC_I,           KC_O,           KC_P,           KC_TAB,
    KC_BSPC,        KC_A,           KC_S,           KC_D,           KC_F,           KC_G,                                           KC_H,           KC_J,           KC_K,           KC_L,           KC_SCLN,        KC_QUOT,
    KC_LSFT,        KC_Z,           KC_X,           KC_C,           KC_V,           KC_B,           KC_NO,          KC_NO,          KC_N,           KC_M,           KC_COMM,        KC_DOT,         KC_SLSH,        KC_RSFT,
                                    KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS
),
[LAYER_BASE_GAMING] = LAYOUT(
    KC_1,           KC_2,           KC_3,           KC_4,           KC_5,           KC_6,                                           KC_7,           KC_8,           KC_9,           KC_0,           KC_MINS,        KC_EQL,
    KC_ENT,         KC_Q,           KC_W,           KC_E,           KC_R,           KC_T,                                           KC_Y,           KC_U,           KC_I,           KC_O,           KC_P,           KC_TAB,
    KC_BSPC,        KC_A,           KC_S,           KC_D,           KC_F,           KC_G,                                           KC_H,           KC_J,           KC_K,           KC_L,           KC_SCLN,        KC_QUOT,
    KC_LSFT,        KC_Z,           KC_X,           KC_C,           KC_V,           KC_B,           KC_NO,          KC_NO,          KC_N,           KC_M,           KC_COMM,        KC_DOT,         KC_SLSH,        KC_RSFT,
                                    KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS
),
[LAYER_SYMBOLS] = LAYOUT(
    KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
    KC_NO,          KC_NO,          KC_NO,          KC_LCBR,        KC_RCBR,        KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
    KC_NO,          KC_TILD,        KC_GRV,         KC_LPRN,        KC_RPRN,        KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
    KC_NO,          KC_NO,          KC_NO,          KC_LBRC,        KC_RBRC,        KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
                                    KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS
),
[LAYER_ARROWS] = LAYOUT(
    KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
    KC_NO,          KC_NO,          KC_LALT,        KC_LCTL,        KC_LSFT,        KC_NO,                                          KC_NO,          KC_HOME,        KC_UP,          KC_END,         KC_NO,          KC_NO,
    KC_NO,          KC_NO,          CK_SALT,        CK_SCTL,        CK_SSFT,        KC_NO,                                          KC_NO,          KC_LEFT,        KC_DOWN,        KC_RGHT,        KC_NO,          KC_NO,
    KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          CK_DWL,         CK_DLN,         CK_DWR,         KC_NO,          KC_NO,
                                    KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS
),
[LAYER_FUNCTION] = LAYOUT(
    KC_F1,          KC_F2,          KC_F3,          KC_F4,          KC_F5,          KC_F6,                                          KC_F7,          KC_F8,          KC_F9,          KC_F10,         KC_F11,         KC_F12,
    KC_F13,         KC_F14,         KC_F15,         KC_F16,         KC_F17,         KC_F18,                                         KC_F19,         KC_F20,         KC_F21,         KC_F22,         KC_F23,         KC_F24,
    KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
    KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
                                    KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS
),
[LAYER_UTILITIES] = LAYOUT(
    KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
    KC_ENT,         KC_NO,          KC_UP,          CK_REDO,        CK_SFTU,        KC_ESC,                                         KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
    KC_BSPC,        KC_PSCR,        KC_DOWN,        CK_UNDO,        CK_SFTD,        KC_DEL,                                         KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
    KC_LSFT,        KC_NO,          CK_ALTL,        CK_AB,          CK_ALTR,        KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_RSFT,
                                    KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS
),
// [LAYER_ALL_TRNS] = LAYOUT(
//     KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,                                        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,
//     KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,                                        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,
//     KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,                                        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,
//     KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_NO,          KC_NO,          KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,
//                                     KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS
// ),
// [LAYER_ALL_NO] = LAYOUT(
//     KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
//     KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
//     KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
//     KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
//                                     KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS,        KC_TRNS
// ),
};
// clang-format on
