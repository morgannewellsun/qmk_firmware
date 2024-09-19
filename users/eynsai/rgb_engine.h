#pragma once

#include "quantum.h"

#define HSV_EQUAL(a, b) ((a.h == b.h) && (a.s == b.s) && (a.v == b.v))
#define NUDGE_HUE(BASE_HUE, TARGET_HUE, MAGNITUDE) ((BASE_HUE + (((TARGET_HUE - BASE_HUE + 256) % 256) <= ((BASE_HUE - TARGET_HUE + 256) % 256) ? 1 : -1) * MAGNITUDE + 256) % 256)

typedef struct rgb_animation_t {
    uint32_t start_duration;  // start_duration is the time it takes to interpolate from the current color to colors[0]
    uint32_t durations[RGB_MAX_ANIMATION_SIZE];  // durations[i] is the time it takes to interpolate from colors[i] to colors[i+1]
    HSV colors[RGB_MAX_ANIMATION_SIZE];
    uint8_t loop_start_idx;  // inclusive
    uint8_t loop_stop_idx;  // exclusive
} rgb_animation_t;

typedef struct rgb_state_t {
    float lerp_duration_inv;
    HSV current_color;
    HSV lerp_final_color;
    HSV lerp_initial_color;
    uint32_t last_update_time;
    uint32_t lerp_duration;
    uint32_t lerp_initial_time;
    size_t lerp_final_idx;
    size_t lerp_initial_idx;
    size_t animation_idx;
} rgb_state_t;

extern rgb_animation_t rgb_animations[];

void rgb_init_task(rgb_state_t* rgb_state);
void rgb_matrix_scan_task(rgb_state_t* rgb_state);
void rgb_start_animation(rgb_state_t* rgb_state, size_t animation_idx);
