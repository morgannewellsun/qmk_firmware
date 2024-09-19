#include "superkeys.h"

uint16_t superkey_currently_pressed = KC_NO;

static void superkey_pressed_task(superkey_state_t* superkey_state, superkey_config_t* superkey_config) {
    // Call user-specfied down function
    if (superkey_config->down != NULL) {
        superkey_config->down(superkey_state);
    }
    // Reset stuff
    superkey_state->tap_start_time = timer_read32();
    superkey_state->interrupt_result = NO_INTERRUPT;
    superkey_state->timeout_result = NO_TIMEOUT;
    superkey_state->interrupt_is_on = true;
    superkey_state->timeout_is_on = true;
}

static void superkey_released_task(superkey_state_t* superkey_state, superkey_config_t* superkey_config) {
    // Multitap logic
    if (superkey_state->interrupt_result == NO_INTERRUPT && superkey_state->timeout_result == NO_TIMEOUT) {
        // The current event is a multitap if the previous event was a tap/multitap and occured in the last multitap_term milliseconds
        if (superkey_state->prev_was_tap && timer_elapsed32(superkey_state->multitap_start_time) < superkey_config->multitap_term) {
            superkey_state->multitap_result++;
        } else {
            superkey_state->multitap_result = NO_MULTITAP;
        }
        // Prepare to check if the next tap is a multitap
        superkey_state->multitap_start_time = timer_read32();
        superkey_state->prev_was_tap = true;
    } else {
        // Neither this event nor the next event can be a multitap
        superkey_state->multitap_result = NO_MULTITAP;
        superkey_state->prev_was_tap = false;
    }
    // Call user-specfied up function and reset stuff
    if (superkey_config->up != NULL) {
        superkey_config->up(superkey_state);
    }
    superkey_state->interrupt_is_on = false;
    superkey_state->timeout_is_on = false;
}

bool superkey_process_record_task(uint16_t keycode, bool pressed) {
    bool continue_processing = true;
    for (size_t i = 0; i < n_superkeys; i++) {
        superkey_state_t* superkey_state = &(superkeys[i].state);
        superkey_config_t* superkey_config = &(superkeys[i].config);

        // Handle superkey presses and releases
        if (keycode == superkey_config->keycode) {

            if (pressed) {

                // Override previously pressed superkey
                if (superkey_currently_pressed != KC_NO) {
                    superkey_state_t* other_superkey_state = &(superkeys[SK_INDEX(superkey_currently_pressed)].state);
                    superkey_config_t* other_superkey_config = &(superkeys[SK_INDEX(superkey_currently_pressed)].config);
                    superkey_released_task(other_superkey_state, other_superkey_config);
                }

                // Press superkey
                superkey_currently_pressed = keycode;
                superkey_pressed_task(superkey_state, superkey_config);

            } else {
                
                // Release currently pressed superkey
                if (superkey_currently_pressed == keycode) {
                    superkey_currently_pressed = KC_NO;
                    superkey_released_task(superkey_state, superkey_config);
                }

            }

            // Superkeys can't be processed normally
            continue_processing = false;

            // Superkeys can't interrupt other superkeys, so there's no point in continuing to iterate
            break;

        // Handle a potential interrupt
        } else if (superkey_state->interrupt_is_on && pressed) {

            // Annotate interrupt information
            superkey_state->interrupting_keycode = keycode;
            superkey_state->interrupt_result = (superkey_state->timeout_result == NO_TIMEOUT) ? INTERRUPT_BEFORE_TIMEOUT : INTERRUPT_AFTER_TIMEOUT;
            superkey_state->interrupt_is_on = false;
                
        }
    }
    return continue_processing;
}

void superkey_matrix_scan_task(void) {
    for (size_t i = 0; i < n_superkeys; i++) {
        superkey_state_t* superkey_state = &(superkeys[i].state);
        superkey_config_t* superkey_config = &(superkeys[i].config);

        // Handle a timeout
        if (superkey_state->timeout_is_on && timer_elapsed32(superkey_state->tap_start_time) > superkey_config->tap_term) {
            if (superkey_config->timeout != NULL) {
                superkey_config->timeout(superkey_state);
            }
            superkey_state->timeout_result = superkey_state->interrupt_result == NO_INTERRUPT ? TIMEOUT_BEFORE_INTERRUPT : TIMEOUT_AFTER_INTERRUPT;
            superkey_state->timeout_is_on = false;
        }
    }
}

void superkey_inject_interrupt(uint16_t superkey_keycode, uint16_t interrupting_keycode) {
    superkey_state_t* superkey_state = &(superkeys[SK_INDEX(superkey_keycode)].state);
    if (!superkey_state->interrupt_is_on) {
        return;
    }
    superkey_state->interrupting_keycode = interrupting_keycode;
    superkey_state->interrupt_result = (superkey_state->timeout_result == NO_TIMEOUT) ? INTERRUPT_BEFORE_TIMEOUT : INTERRUPT_AFTER_TIMEOUT;
    superkey_state->interrupt_is_on = false;
    return;
}
