#pragma once

#include "quantum.h"

#define TIMEOUT_DEFINE(_index, _function, _duration) [_index] = { .function = _function, .start_time = 0, .duration = _duration, .active = false }

struct keyboard_state_t;

typedef struct timeout_t {   
    void (*function)(void);
    uint32_t start_time;
    uint32_t duration;
    bool active;
} timeout_t;

extern timeout_t timeouts[];
extern const size_t n_timeouts;

void timeout_matrix_scan_task(void);
void timeout_on(size_t index);
void timeout_off(size_t index);
