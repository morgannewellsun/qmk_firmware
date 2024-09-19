#pragma once

#define INTERCEPT_DEFINE(_index, _function) [_index] = { .function = _function, .active = false }

struct keyboard_state_t;

typedef struct intercept_t {   
    bool (*function)(struct keyboard_state_t*, uint16_t, bool);  // returns whether or not to interrupt
    bool auto_off;
    bool active;
} intercept_t;

extern intercept_t intercepts[];
extern const size_t n_intercepts;

bool intercept_process_record_task(struct keyboard_state_t* keyboard_state, uint16_t keycode, bool pressed);
void intercept_on(size_t index);
void intercept_off(size_t index);

