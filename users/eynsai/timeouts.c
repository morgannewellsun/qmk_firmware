#include "include/timeouts.h"
#include "keyboard.h"

void timeout_matrix_scan_task(struct keyboard_state_t* keyboard_state) {
    for (size_t i = 0; i < n_timeouts; i++) {
        if (timeouts[i].active && timer_elapsed32(timeouts[i].start_time) > timeouts[i].duration) {
            if (timeouts[i].function != NULL) {
                timeouts[i].function(keyboard_state);
            }
            timeouts[i].active = false;
        }
    }
}

void timeout_on(size_t index) {
    timeouts[index].active = true;
    timeouts[index].start_time = timer_read32();
}

void timeout_off(size_t index) {
    timeouts[index].active = false;
}
