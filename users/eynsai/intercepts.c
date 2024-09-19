#include "include/intercepts.h"

bool intercept_process_record_task(struct keyboard_state_t* keyboard_state, uint16_t keycode, bool pressed) {
    // earlier intercepts can prevent later intercepts from being called
    // earlier superkey interrupts can't prevent later superkey interrupts from being called
    // intercepts can't prevent superkey interrupts from being called
    for (size_t i = 0; i < n_intercepts; i++) {
        if (intercepts[i].active && !intercepts[i].function(keyboard_state, keycode, pressed)) {
            return false; 
        }
    }
    return true;
}

void intercept_on(size_t index) {
    intercepts[index].active = true;
}

void intercept_off(size_t index) {
    intercepts[index].active = false;
}
