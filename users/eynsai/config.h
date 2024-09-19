#pragma once

// ============================================================================
// QMK: DISABLE FEATURES
// ============================================================================

#define NO_ACTION_TAPPING
#define NO_ACTION_ONESHOT

// ============================================================================
// QMK: POINTING DEVICE
// ============================================================================

#define MOUSE_EXTENDED_REPORT
#define WHEEL_EXTENDED_REPORT
#define POINTING_DEVICE_HIRES_SCROLL_ENABLE
#define POINTING_DEVICE_HIRES_SCROLL_MULTIPLIER 120
#define POINTING_DEVICE_HIRES_SCROLL_EXPONENT 1
#define POINTING_DEVICE_DRAGSCROLL_ENABLE

// ============================================================================
// TAPPING TERMS
// ============================================================================

#define DEFAULT_TAP_TERM 175
#define DEFAULT_MULTITAP_TERM 175

// ============================================================================
// RGB ANIMATIONS
// ============================================================================

#define RGB_MAX_ANIMATION_SIZE 5
#define RGB_UPDATE_INTERVAL 20
#define RGB_MAX_VAL 100

#define RGB_BREATHING_SPEED 2000
#define RGB_BREATHING_HUE_AMPLITUDE 10

#define RGB_STATIC_DUMMY_DURATION 1000

#define RGB_BASE_FROM_WORKMAN_START_DURATION 50
#define RGB_BASE_FROM_WORKMAN_HOLD_DURATION 50
#define RGB_BASE_FROM_WORKMAN_FADE_DURATION 500
#define RGB_BASE_TO_WORKMAN_START_DURATION 50
#define RGB_BASE_TO_WORKMAN_FADE_DURATION 100

#define RGB_ONESHOT_ON_FIRST_START_DURATION 50
#define RGB_ONESHOT_ON_FIRST_HOLD_DURATION 50
#define RGB_ONESHOT_ON_FIRST_FADE_DURATION 500
#define RGB_ONESHOT_ON_EXTRA_START_DURATION 200
#define RGB_ONESHOT_ON_EXTRA_HOLD_DURATION 300
#define RGB_ONESHOT_ON_EXTRA_FADE_DURATION 500
#define RGB_ONESHOT_OFF_START_DURATION 50
#define RGB_ONESHOT_OFF_FADE_DURATION 100
