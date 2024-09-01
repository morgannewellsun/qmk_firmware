/* Copyright 2022 Morgan Newell Sun
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "pointing_device_dragscroll.h"

/* implement a simple ring buffer for smoothing hires scrolling */

#ifdef POINTING_DEVICE_HIRES_SCROLL_ENABLE
typedef struct {
    float* items;
    float current_sum;
    size_t current_size;
    size_t maximum_size;
    size_t next_index;
} ring_buffer_t;

static void ring_buffer_reset(ring_buffer_t* rb) {
    rb->current_sum = 0;
    rb->current_size = 0;
    rb->next_index = 0;
}

static void ring_buffer_push(ring_buffer_t* rb, float item) {
    if (rb->current_size == rb->maximum_size) {
        rb->current_sum -= rb->items[rb->next_index];
    } else {
        rb->current_size++;
    }
    rb->items[rb->next_index] = item;
    rb->current_sum += item;
    rb->next_index = (rb->next_index + 1) % rb->maximum_size;
}

static float ring_buffer_mean(ring_buffer_t* rb) {
    return rb->current_size > 0 ? rb->current_sum / rb->current_size : 0;
}
#endif

/* stuff to keep track of dragscroll state */

enum dragscroll_axis_snapping_states {
    AXIS_SNAPPING_UNDECIDED = 0,
    AXIS_SNAPPING_HORIZONTAL,
    AXIS_SNAPPING_VERTICAL,
    AXIS_SNAPPING_OFF,
};

typedef struct {
    float accumulator_h;
    float accumulator_v;
    float rounding_error_h;
    float rounding_error_v;
    float axis_snapping_deviation;
#ifdef POINTING_DEVICE_HIRES_SCROLL_ENABLE
    ring_buffer_t smoothing_buffer_h;
    ring_buffer_t smoothing_buffer_v;
#endif
    uint32_t last_movement_time;
    size_t axis_snapping_state;
    bool active;
} dragscroll_state_t;

#ifdef POINTING_DEVICE_HIRES_SCROLL_ENABLE
float hires_scroll_effective_resolution_multiplier;
float hires_scroll_axis_snapping_threshold;
#endif

uint32_t last_scroll_time = 0;  // use a single shared timer for throttling

dragscroll_state_t dragscroll_state = {0};

#if defined(SPLIT_POINTING_ENABLE) && defined(POINTING_DEVICE_COMBINED)
dragscroll_state_t dragscroll_state_right = {0};
#endif

/* core implementation of drag scroll */

static void dragscroll_reset_task(dragscroll_state_t* d) {
    d->accumulator_h = 0;
    d->accumulator_v = 0;
    d->rounding_error_h = 0;
    d->rounding_error_v = 0;
    d->axis_snapping_deviation = 0;
#ifdef POINTING_DEVICE_HIRES_SCROLL_ENABLE
    ring_buffer_reset(&(d->smoothing_buffer_h));
    ring_buffer_reset(&(d->smoothing_buffer_v));
#endif
    d->last_movement_time = timer_read32();
    if (d->axis_snapping_state != AXIS_SNAPPING_OFF) {
        d->axis_snapping_state = AXIS_SNAPPING_UNDECIDED;
    }
}

static void dragscroll_accumulate_task(dragscroll_state_t* d, report_mouse_t* mouse_report) {
    if (!(d->active)) { return; }
    float delta_h;
    float delta_v;

    // reset drag scroll if the pointing device has been idle for too long    
    if (mouse_report->x == 0 && mouse_report->y == 0) {
        if (timer_elapsed32(d->last_movement_time) > DRAGSCROLL_TIMEOUT_MS) {
            dragscroll_reset_task(d);
        }
        return;
    }
    d->last_movement_time = timer_read32();

    // scale hires scrolling so that hires and normal scrolling have the same speed
#ifdef POINTING_DEVICE_HIRES_SCROLL_ENABLE
    if (is_hires_scroll_on()) {
        delta_h = ((float)(mouse_report->x)) * DRAGSCROLL_MULTIPLIER_H * hires_scroll_effective_resolution_multiplier;
        delta_v = ((float)(mouse_report->y)) * DRAGSCROLL_MULTIPLIER_V * hires_scroll_effective_resolution_multiplier;
    } else {
#endif
        delta_h = ((float)(mouse_report->x)) * DRAGSCROLL_MULTIPLIER_H;
        delta_v = ((float)(mouse_report->y)) * DRAGSCROLL_MULTIPLIER_V;
#ifdef POINTING_DEVICE_HIRES_SCROLL_ENABLE
    }
#endif

    // update accumulators
    d->accumulator_h += delta_h;
    d->accumulator_v += delta_v;

    // zero out the mouse report
    mouse_report->x = 0;
    mouse_report->y = 0;
}

static void dragscroll_scroll_task(dragscroll_state_t* d, report_mouse_t* mouse_report) {
    if (!(d->active)) { return; }
    float h;
    float v;
#ifdef POINTING_DEVICE_HIRES_SCROLL_ENABLE
    float current_axis_snapping_threshold;
#endif

    // apply smoothing to hires scrolling - don't apply it to normal scrolling since it makes normal scrolling feel unresponsive
#ifdef POINTING_DEVICE_HIRES_SCROLL_ENABLE
    if (is_hires_scroll_on()) {
        ring_buffer_push(&(d->smoothing_buffer_h), d->accumulator_h);
        ring_buffer_push(&(d->smoothing_buffer_v), d->accumulator_v);
        h = ring_buffer_mean(&(d->smoothing_buffer_h));
        v = ring_buffer_mean(&(d->smoothing_buffer_v));
    } else {
#endif
        h = d->accumulator_h;
        v = d->accumulator_v;
#ifdef POINTING_DEVICE_HIRES_SCROLL_ENABLE
    }
#endif

    // apply any previously recorded rounding errors
    h += d->rounding_error_h;
    v += d->rounding_error_v; 

    // zero out the accumulators
    d->accumulator_h = 0;
    d->accumulator_v = 0;

    // apply axis snapping
    switch (d->axis_snapping_state) {
        case AXIS_SNAPPING_OFF:
            break;
        case AXIS_SNAPPING_UNDECIDED:
            // we don't know which axis to snap since the user hasn't moved the pointing device
            if (abs(h) > abs(v)) {
                // snap to horizontal axis
                v = 0;
                d->axis_snapping_state = AXIS_SNAPPING_HORIZONTAL;
            } else if (abs(h) < abs(v)) {
                // snap to vertical axis
                h = 0;
                d->axis_snapping_state = AXIS_SNAPPING_VERTICAL;
            }
            break;
        case AXIS_SNAPPING_HORIZONTAL:
            d->axis_snapping_deviation += v;
            if (d->axis_snapping_deviation > 0) {
                d->axis_snapping_deviation -= abs(h) * DRAGSCROLL_AXIS_SNAPPING_RATIO;
                d->axis_snapping_deviation = d->axis_snapping_deviation < 0 ? 0 : d->axis_snapping_deviation;
            } else if (d->axis_snapping_deviation < 0) {
                d->axis_snapping_deviation += abs(h) * DRAGSCROLL_AXIS_SNAPPING_RATIO;
                d->axis_snapping_deviation = d->axis_snapping_deviation > 0 ? 0 : d->axis_snapping_deviation;
            }
#ifdef POINTING_DEVICE_HIRES_SCROLL_ENABLE
            // hires scrolling requires a larger axis snapping threshold than normal scrolling
            current_axis_snapping_threshold = is_hires_scroll_on() ? hires_scroll_axis_snapping_threshold : DRAGSCROLL_AXIS_SNAPPING_THRESHOLD;
            if (abs(d->axis_snapping_deviation) > current_axis_snapping_threshold) {
#else
            if (abs(d->axis_snapping_deviation) > DRAGSCROLL_AXIS_SNAPPING_THRESHOLD) {
#endif
                // switch to the vertical axis
                h = 0;
                d->rounding_error_h = 0;
                d->rounding_error_v = 0;
#ifdef POINTING_DEVICE_HIRES_SCROLL_ENABLE
                ring_buffer_reset(&(d->smoothing_buffer_h));
                ring_buffer_reset(&(d->smoothing_buffer_v));
#endif
                d->axis_snapping_state = AXIS_SNAPPING_VERTICAL;
                d->axis_snapping_deviation = 0;
            } else {
                v = 0;
            }
            break;
        case AXIS_SNAPPING_VERTICAL:
            d->axis_snapping_deviation += h;
            if (d->axis_snapping_deviation > 0) {
                d->axis_snapping_deviation -= abs(v) * DRAGSCROLL_AXIS_SNAPPING_RATIO;
                d->axis_snapping_deviation = d->axis_snapping_deviation < 0 ? 0 : d->axis_snapping_deviation;
            } else if (d->axis_snapping_deviation < 0) {
                d->axis_snapping_deviation += abs(v) * DRAGSCROLL_AXIS_SNAPPING_RATIO;
                d->axis_snapping_deviation = d->axis_snapping_deviation > 0 ? 0 : d->axis_snapping_deviation;
            }
#ifdef POINTING_DEVICE_HIRES_SCROLL_ENABLE
            // hires scrolling requires a larger axis snapping threshold than normal scrolling
            current_axis_snapping_threshold = is_hires_scroll_on() ? hires_scroll_axis_snapping_threshold : DRAGSCROLL_AXIS_SNAPPING_THRESHOLD;
            if (abs(d->axis_snapping_deviation) > current_axis_snapping_threshold) {
#else
            if (abs(d->axis_snapping_deviation) > DRAGSCROLL_AXIS_SNAPPING_THRESHOLD) {
#endif
                // switch to the horizontal axis
                v = 0;
                d->rounding_error_h = 0;
                d->rounding_error_v = 0;
#ifdef POINTING_DEVICE_HIRES_SCROLL_ENABLE
                ring_buffer_reset(&(d->smoothing_buffer_h));
                ring_buffer_reset(&(d->smoothing_buffer_v));
#endif
                d->axis_snapping_state = AXIS_SNAPPING_HORIZONTAL;
                d->axis_snapping_deviation = 0;
            } else {
                h = 0;
            }
            break;
    }

    // save rounding errors
    mouse_report->h = (mouse_hv_report_t)h;
    mouse_report->v = (mouse_hv_report_t)v;
    d->rounding_error_h = h - mouse_report->h;
    d->rounding_error_v = v - mouse_report->v;
}

static void dragscroll_on_task(dragscroll_state_t* d) {
    if (d->active) { return; }
    dragscroll_reset_task(d);
    d->active = true;
}

static void dragscroll_off_task(dragscroll_state_t* d) {
    d->active = false;
}

static void dragscroll_toggle_task(dragscroll_state_t* d) {
    if (d->active) {
        dragscroll_off_task(d);
    } else {
        dragscroll_on_task(d);
    }
}

static bool is_dragscroll_on_task(dragscroll_state_t* d) {
    return d->active;
}

static void dragscroll_axis_snapping_on_task(dragscroll_state_t* d) {
    if (d->axis_snapping_state != AXIS_SNAPPING_OFF) { return; }
    d->axis_snapping_deviation = 0;
    d->axis_snapping_state = AXIS_SNAPPING_UNDECIDED;
}

static void dragscroll_axis_snapping_off_task(dragscroll_state_t* d) {
    d->axis_snapping_state = AXIS_SNAPPING_OFF;
}

static void dragscroll_axis_snapping_toggle_task(dragscroll_state_t* d) {
    if (d->axis_snapping_state == AXIS_SNAPPING_OFF) {
        dragscroll_axis_snapping_on_task(d);
    } else {
        dragscroll_axis_snapping_off_task(d);
    }
}

static bool is_dragscroll_axis_snapping_on_task(dragscroll_state_t* d) {
    return d->axis_snapping_state != AXIS_SNAPPING_OFF;
}

/* functions used in pointing_device.c */

void dragscroll_init(void) {
#ifdef POINTING_DEVICE_HIRES_SCROLL_ENABLE
    hires_scroll_effective_resolution_multiplier = POINTING_DEVICE_HIRES_SCROLL_MULTIPLIER;
    for (int i = 0; i < POINTING_DEVICE_HIRES_SCROLL_EXPONENT; i++) {
        hires_scroll_effective_resolution_multiplier *= 10;
    }
    hires_scroll_axis_snapping_threshold = DRAGSCROLL_AXIS_SNAPPING_THRESHOLD * hires_scroll_effective_resolution_multiplier;
    float* items_h = (float*)malloc(DRAGSCROLL_SMOOTHING_H * sizeof(float));
    float* items_v = (float*)malloc(DRAGSCROLL_SMOOTHING_V * sizeof(float));
    dragscroll_state.smoothing_buffer_h.items = items_h;
    dragscroll_state.smoothing_buffer_v.items = items_v;
    dragscroll_state.smoothing_buffer_h.maximum_size = DRAGSCROLL_SMOOTHING_H;
    dragscroll_state.smoothing_buffer_v.maximum_size = DRAGSCROLL_SMOOTHING_V;
#    if defined(SPLIT_POINTING_ENABLE) && defined(POINTING_DEVICE_COMBINED)
    float* items_h_right = (float*)malloc(DRAGSCROLL_SMOOTHING_H * sizeof(float));
    float* items_v_right = (float*)malloc(DRAGSCROLL_SMOOTHING_V * sizeof(float));
    dragscroll_state_right.smoothing_buffer_h.items = items_h_right;
    dragscroll_state_right.smoothing_buffer_v.items = items_v_right;
    dragscroll_state_right.smoothing_buffer_h.maximum_size = DRAGSCROLL_SMOOTHING_H;
    dragscroll_state_right.smoothing_buffer_v.maximum_size = DRAGSCROLL_SMOOTHING_V;
#    endif
#endif  // POINTING_DEVICE_HIRES_SCROLL_ENABLE
}

void pointing_device_dragscroll(report_mouse_t* mouse_report) {
    // accumulate on every call, but only send a nonzero mouse report periodically
    dragscroll_accumulate_task(&dragscroll_state, mouse_report);
    if (timer_elapsed32(last_scroll_time) < DRAGSCROLL_THROTTLE_MS) {
        return;
    }
    last_scroll_time = timer_read32();
    dragscroll_scroll_task(&dragscroll_state, mouse_report);
}

#if defined(SPLIT_POINTING_ENABLE) && defined(POINTING_DEVICE_COMBINED)
void pointing_device_dragscroll_combined(report_mouse_t* mouse_report_left, report_mouse_t* mouse_report_right) {
    // accumulate on every call, but only send a nonzero mouse report periodically
    dragscroll_accumulate_task(&dragscroll_state, mouse_report_left);
    dragscroll_accumulate_task(&dragscroll_state_right, mouse_report_right);
    if (timer_elapsed32(last_scroll_time) < DRAGSCROLL_THROTTLE_MS) {
        return;
    }
    last_scroll_time = timer_read32();
    dragscroll_scroll_task(&dragscroll_state, mouse_report_left);
    dragscroll_scroll_task(&dragscroll_state_right, mouse_report_right);
}
#endif

/* functions to allow the user to control drag scroll */

void dragscroll_on(void) { dragscroll_on_task(&dragscroll_state); }
void dragscroll_off(void) { dragscroll_off_task(&dragscroll_state); }
void dragscroll_toggle(void) { dragscroll_toggle_task(&dragscroll_state); }
bool is_dragscroll_on(void) { return is_dragscroll_on_task(&dragscroll_state); }
void dragscroll_axis_snapping_on(void) { dragscroll_axis_snapping_on_task(&dragscroll_state); }
void dragscroll_axis_snapping_off(void) { dragscroll_axis_snapping_off_task(&dragscroll_state); }
void dragscroll_axis_snapping_toggle(void) { dragscroll_axis_snapping_toggle_task(&dragscroll_state); }
bool is_dragscroll_axis_snapping_on(void) { return is_dragscroll_axis_snapping_on_task(&dragscroll_state); }

#if defined(SPLIT_POINTING_ENABLE) && defined(POINTING_DEVICE_COMBINED)
void dragscroll_on_right(void) { dragscroll_on_task(&dragscroll_state_right); }
void dragscroll_off_right(void) { dragscroll_off_task(&dragscroll_state_right); }
void dragscroll_toggle_right(void) { dragscroll_toggle_task(&dragscroll_state_right); }
bool is_dragscroll_on_right(void) { return is_dragscroll_on_task(&dragscroll_state_right); }
void dragscroll_axis_snapping_on_right(void) { dragscroll_axis_snapping_on_task(&dragscroll_state_right); }
void dragscroll_axis_snapping_off_right(void) { dragscroll_axis_snapping_off_task(&dragscroll_state_right); }
void dragscroll_axis_snapping_toggle_right(void) { dragscroll_axis_snapping_toggle_task(&dragscroll_state_right); }
bool is_dragscroll_axis_snapping_on_right(void) { return is_dragscroll_axis_snapping_on_task(&dragscroll_state_right); }
#endif
