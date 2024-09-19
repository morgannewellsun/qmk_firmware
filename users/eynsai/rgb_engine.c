#include "include/rgb_engine.h"

static void rgb_simplify(rgb_state_t* rgb_state) {
    // Avoid unnecessary hue rotations
    if (rgb_state->lerp_initial_color.v == 0 || rgb_state->lerp_initial_color.s == 0) {
        rgb_state->lerp_initial_color.h = rgb_state->lerp_final_color.h;
    } else if (rgb_state->lerp_final_color.v == 0 || rgb_state->lerp_final_color.s == 0) {
        rgb_state->lerp_final_color.h = rgb_state->lerp_initial_color.h;
    }
    // Avoid unnecessasry interpolation to and from white
    if (rgb_state->lerp_initial_color.v == 0) {
        rgb_state->lerp_initial_color.s = rgb_state->lerp_final_color.s;
    } else if (rgb_state->lerp_final_color.v == 0) {
        rgb_state->lerp_final_color.s = rgb_state->lerp_initial_color.s;
    }
}

static HSV rgb_lerp(HSV initial_hsv, HSV final_hsv, float ratio) {
    HSV result;
    if (ratio < 0.0f) {
        ratio = 0.0f;
    }
    if (ratio > 1.0f) {
        ratio = 1.0f;
    }
    int h_difference = (int)final_hsv.h - (int)initial_hsv.h;
    if (initial_hsv.v == 0) {
        h_difference = 0;
    }
    if (h_difference > 127) {
        h_difference -= 256;
    } else if (h_difference < -127) {
        h_difference += 256;
    }
    int h_interpolated = (int)initial_hsv.h + (int)(h_difference * ratio);
    if (h_interpolated < 0) {
        h_interpolated += 256;
    } else if (h_interpolated > 255) {
        h_interpolated -= 256;
    }
    result.h = (uint8_t)h_interpolated;
    result.s = (uint8_t)(initial_hsv.s + (int)(((int)final_hsv.s - (int)initial_hsv.s) * ratio));
    result.v = (uint8_t)(initial_hsv.v + (int)(((int)final_hsv.v - (int)initial_hsv.v) * ratio));
    return result;
}

static void rgb_set(rgb_state_t* rgb_state, HSV color) {
    rgb_state->current_color = color;
    rgblight_sethsv_noeeprom(color.h, color.s, color.v <= RGB_MAX_VAL ? color.v : RGB_MAX_VAL);
}

void rgb_init_task(rgb_state_t* rgb_state) {
    // Make sure we're in the right animation mode
    rgblight_mode_noeeprom(0);
    // Default to the first available animation
    rgb_start_animation(rgb_state, 0);
}

void rgb_matrix_scan_task(rgb_state_t* rgb_state) {
    // Update every RGB_UPDATE_INTERVAL milliseconds
    if (timer_elapsed32(rgb_state->last_update_time) < RGB_UPDATE_INTERVAL) {
        return;
    }
    uint32_t time = timer_read32();
    rgb_state->last_update_time = time;
    // Advance to the current waypoint if necessary
    uint32_t time_elapsed = timer_elapsed32(rgb_state->lerp_initial_time);
    bool advanced = false;
    while (time_elapsed >= rgb_state->lerp_duration) {
        time_elapsed -= rgb_state->lerp_duration;
        rgb_state->lerp_initial_idx = rgb_state->lerp_final_idx;
        rgb_state->lerp_final_idx += 1;
        if (rgb_state->lerp_final_idx == rgb_animations[rgb_state->animation_idx].loop_stop_idx) {
            rgb_state->lerp_final_idx = rgb_animations[rgb_state->animation_idx].loop_start_idx;
        }
        rgb_state->lerp_duration = rgb_animations[rgb_state->animation_idx].durations[rgb_state->lerp_initial_idx];
        advanced = true;
    }
    if (advanced) {
        rgb_state->lerp_duration_inv = 1.0 / ((float) rgb_state->lerp_duration);
        rgb_state->lerp_initial_color = rgb_animations[rgb_state->animation_idx].colors[rgb_state->lerp_initial_idx];
        rgb_state->lerp_final_color = rgb_animations[rgb_state->animation_idx].colors[rgb_state->lerp_final_idx];
        rgb_state->lerp_initial_time = time;
        rgb_simplify(rgb_state);
    }
    // Interpolate from initial to final color
    if HSV_EQUAL(rgb_state->lerp_initial_color, rgb_state->lerp_final_color) {
        rgb_set(rgb_state, rgb_state->lerp_initial_color);
    } else {
        float ratio = time_elapsed * rgb_state->lerp_duration_inv;
        HSV color = rgb_lerp(rgb_state->lerp_initial_color, rgb_state->lerp_final_color, ratio);
        rgb_set(rgb_state, color);
    }
}

void rgb_start_animation(rgb_state_t* rgb_state, size_t animation_idx) {
    // Start by interpolating from current color to first animation color
    uint32_t time = timer_read32();
    rgb_state->lerp_duration_inv = 1.0 / ((float) rgb_animations[animation_idx].start_duration);
    rgb_state->lerp_final_color = rgb_animations[animation_idx].colors[0];
    rgb_state->lerp_initial_color = rgb_state->current_color;
    rgb_state->last_update_time = time;
    rgb_state->lerp_duration = rgb_animations[animation_idx].start_duration;
    rgb_state->lerp_initial_time = time;
    rgb_state->lerp_final_idx = 0;
    rgb_state->lerp_initial_idx = 0;
    rgb_state->animation_idx = animation_idx;
    rgb_simplify(rgb_state);
}
