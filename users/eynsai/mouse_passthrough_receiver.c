#include QMK_KEYBOARD_H
#include "raw_hid.h"
#include "report.h"
#include "mouse_passthrough_common.h"
#include "mouse_passthrough_receiver.h"

static uint8_t state = MOUSE_PASSTHROUGH_DISCONNECTED;
static uint8_t device_id_self;
static uint8_t device_id_others[MAX_REGISTERED_DEVICES - 1];
static uint8_t device_id_remote;
static uint32_t last_connection_attempt_time = 0;
static uint32_t last_connection_success_time = 0;
static uint8_t message_queue[QMK_RAW_HID_REPORT_SIZE * MAX_QUEUED_MESSAGES];
static uint8_t message_queue_next_empty_offset = 0;

static bool block_buttons_on = false;
static bool block_pointer_on = false;
static bool block_wheel_on = false;
static bool send_buttons_on = false;
static bool send_pointer_on = false;
static bool send_wheel_on = false;
static bool control_state_changed = false;

static report_mouse_t accumulated_mouse_report = {0};

void mouse_passthrough_reciever_matrix_scan_task(void) {

    // we can only send one raw hid message per matrix scan, anything after the first message gets garbled for some reason
    if (message_queue_next_empty_offset != 0) {
        message_queue_next_empty_offset -= QMK_RAW_HID_REPORT_SIZE;
        raw_hid_send(message_queue + message_queue_next_empty_offset, QMK_RAW_HID_REPORT_SIZE);
    } else if (control_state_changed) {
        // send control payload
        memset(message_queue, 0, QMK_RAW_HID_REPORT_SIZE);
        message_queue[REPORT_OFFSET_COMMAND_ID] = RAW_HID_HUB_COMMAND_ID;
        message_queue[REPORT_OFFSET_DEVICE_ID] = device_id_remote;
        message_queue[REPORT_OFFSET_CONTROL_BLOCK_BUTTONS] = block_buttons_on ? 1 : 0;
        message_queue[REPORT_OFFSET_CONTROL_BLOCK_POINTER] = block_pointer_on ? 1 : 0;
        message_queue[REPORT_OFFSET_CONTROL_BLOCK_WHEEL] = block_wheel_on ? 1 : 0;
        message_queue[REPORT_OFFSET_CONTROL_SEND_BUTTONS] = send_buttons_on ? 1 : 0;
        message_queue[REPORT_OFFSET_CONTROL_SEND_POINTER] = send_pointer_on ? 1 : 0;
        message_queue[REPORT_OFFSET_CONTROL_SEND_WHEEL] = send_wheel_on ? 1 : 0;
        raw_hid_send(message_queue, QMK_RAW_HID_REPORT_SIZE);
        control_state_changed = false;
    }

    if (timer_elapsed32(last_connection_success_time) > HUB_CONNECTION_EXPIRY_INTERVAL) {
        state = MOUSE_PASSTHROUGH_DISCONNECTED;
        memset(&accumulated_mouse_report, 0, sizeof(accumulated_mouse_report));
    }

    if (timer_elapsed32(last_connection_attempt_time) > HUB_CONNECTION_ATTEMPT_INTERVAL) {
        last_connection_attempt_time = timer_read32();
        // send a registration report
        if (message_queue_next_empty_offset < sizeof(message_queue)) {
            memset(message_queue + message_queue_next_empty_offset, 0, QMK_RAW_HID_REPORT_SIZE);
            message_queue[message_queue_next_empty_offset + REPORT_OFFSET_COMMAND_ID] = RAW_HID_HUB_COMMAND_ID;
            message_queue[message_queue_next_empty_offset + REPORT_OFFSET_DEVICE_ID] = DEVICE_ID_HUB;
            message_queue[message_queue_next_empty_offset + REPORT_OFFSET_REGISTRATION] = 0x01;
            message_queue_next_empty_offset += QMK_RAW_HID_REPORT_SIZE;
        }
    }
}

bool mouse_passthrough_reciever_raw_hid_receive_task(uint8_t* data) {

    if (data[REPORT_OFFSET_COMMAND_ID] != RAW_HID_HUB_COMMAND_ID) {
        return true;
    }

    last_connection_success_time = timer_read32();
    if (state == MOUSE_PASSTHROUGH_DISCONNECTED) {
        state = MOUSE_PASSTHROUGH_HUB_CONNECTED;
    }

    if (state == MOUSE_PASSTHROUGH_REMOTE_CONNECTED && data[REPORT_OFFSET_DEVICE_ID] == device_id_remote) {
        // unpack data payload
        accumulated_mouse_report.buttons = data[REPORT_OFFSET_DATA_BUTTONS];
        accumulated_mouse_report.x += ((uint16_t)data[REPORT_OFFSET_DATA_X_MSB] << 8) | ((uint16_t)data[REPORT_OFFSET_DATA_X_LSB]);
        accumulated_mouse_report.y += ((uint16_t)data[REPORT_OFFSET_DATA_Y_MSB] << 8) | ((uint16_t)data[REPORT_OFFSET_DATA_Y_LSB]);
        accumulated_mouse_report.v += ((uint16_t)data[REPORT_OFFSET_DATA_V_MSB] << 8) | ((uint16_t)data[REPORT_OFFSET_DATA_V_LSB]);
        accumulated_mouse_report.h += ((uint16_t)data[REPORT_OFFSET_DATA_H_MSB] << 8) | ((uint16_t)data[REPORT_OFFSET_DATA_H_LSB]);

    } else if (data[REPORT_OFFSET_DEVICE_ID] == DEVICE_ID_HUB) {
        if (data[REPORT_OFFSET_DEVICE_ID_SELF] == DEVICE_ID_UNASSIGNED) {
            // hub has shutdown
            state = MOUSE_PASSTHROUGH_DISCONNECTED;
            memset(&accumulated_mouse_report, 0, sizeof(accumulated_mouse_report));
        } else {
            device_id_self = data[REPORT_OFFSET_DEVICE_ID_SELF];
            memcpy(device_id_others, data + REPORT_OFFSET_DEVICE_ID_OTHERS, MAX_REGISTERED_DEVICES - 1);
            if (state == MOUSE_PASSTHROUGH_REMOTE_CONNECTED) {
                // see if we're still connected
                bool found = false;
                for (int i = REPORT_OFFSET_DEVICE_ID_OTHERS; i < QMK_RAW_HID_REPORT_SIZE; i++) {
                    if (data[i] == device_id_remote) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    state = MOUSE_PASSTHROUGH_HUB_CONNECTED;
                    memset(&accumulated_mouse_report, 0, sizeof(accumulated_mouse_report));
                }
            }
        }
        
    } else if (state == MOUSE_PASSTHROUGH_HUB_CONNECTED && data[REPORT_OFFSET_HANDSHAKE] == 13) {
        // handshake step 2/4: all capable keyboards respond to mouse
        if (message_queue_next_empty_offset < sizeof(message_queue)) {
            device_id_remote = data[REPORT_OFFSET_DEVICE_ID];
            memset(message_queue + message_queue_next_empty_offset, 0, QMK_RAW_HID_REPORT_SIZE);
            message_queue[message_queue_next_empty_offset + REPORT_OFFSET_COMMAND_ID] = RAW_HID_HUB_COMMAND_ID;
            message_queue[message_queue_next_empty_offset + REPORT_OFFSET_DEVICE_ID] = device_id_remote;
            message_queue[message_queue_next_empty_offset + REPORT_OFFSET_HANDSHAKE] = 26;
            message_queue_next_empty_offset += QMK_RAW_HID_REPORT_SIZE;
        }

    } else if (state == MOUSE_PASSTHROUGH_HUB_CONNECTED && data[REPORT_OFFSET_DEVICE_ID] == device_id_remote && data[REPORT_OFFSET_HANDSHAKE] == 39) {
        // handshake step 4/4: keyboard silently receives mouse response
        state = MOUSE_PASSTHROUGH_REMOTE_CONNECTED;
        device_id_remote = data[REPORT_OFFSET_DEVICE_ID];
    }

    return false;
}

report_mouse_t pointing_device_driver_get_report(report_mouse_t mouse_report) {
    memcpy(&mouse_report, &accumulated_mouse_report, sizeof(accumulated_mouse_report));
    memset(&accumulated_mouse_report, 0, sizeof(accumulated_mouse_report));
    accumulated_mouse_report.buttons = mouse_report.buttons;
    return mouse_report;
}

bool is_mouse_passthrough_connected(void) {
    return state == MOUSE_PASSTHROUGH_REMOTE_CONNECTED;
}

void mouse_passthrough_block_buttons_on(void) {
    if (!block_buttons_on) {
        block_buttons_on = true;
        control_state_changed = true;
    }
}

void mouse_passthrough_block_buttons_off(void) {
    if (block_buttons_on) {
        block_buttons_on = false;
        control_state_changed = true;
    }
}

void mouse_passthrough_block_pointer_on(void) {
    if (!block_pointer_on) {
        block_pointer_on = true;
        control_state_changed = true;
    }
}

void mouse_passthrough_block_pointer_off(void) {
    if (block_pointer_on) {
        block_pointer_on = false;
        control_state_changed = true;
    }
}

void mouse_passthrough_block_wheel_on(void) {
    if (!block_wheel_on) {
        block_wheel_on = true;
        control_state_changed = true;
    }
}

void mouse_passthrough_block_wheel_off(void) {
    if (block_wheel_on) {
        block_wheel_on = false;
        control_state_changed = true;
    }
}

void mouse_passthrough_send_buttons_on(void) {
    if (!send_buttons_on) {
        send_buttons_on = true;
        control_state_changed = true;
    }
}
void mouse_passthrough_send_buttons_off(void) {
    if (send_buttons_on) {
        send_buttons_on = false;
        control_state_changed = true;
    }
}
void mouse_passthrough_send_pointer_on(void) {
    if (!send_pointer_on) {
        send_pointer_on = true;
        control_state_changed = true;
    }
}
void mouse_passthrough_send_pointer_off(void) {
    if (send_pointer_on) {
        send_pointer_on = false;
        control_state_changed = true;
    }
}
void mouse_passthrough_send_wheel_on(void) {
    if (!send_wheel_on) {
        send_wheel_on = true;
        control_state_changed = true;
    }
}
void mouse_passthrough_send_wheel_off(void) {
    if (send_wheel_on) {
        send_wheel_on = false;
        control_state_changed = true;
    }
}

void mouse_passthrough_send_reset(void) {
    // send a registration report
    if (message_queue_next_empty_offset < sizeof(message_queue)) {
        memset(message_queue + message_queue_next_empty_offset, 0, QMK_RAW_HID_REPORT_SIZE);
        message_queue[message_queue_next_empty_offset + REPORT_OFFSET_COMMAND_ID] = RAW_HID_HUB_COMMAND_ID;
        message_queue[message_queue_next_empty_offset + REPORT_OFFSET_DEVICE_ID] = device_id_remote;
        message_queue[message_queue_next_empty_offset + REPORT_OFFSET_RESET] = 0x01;
        message_queue_next_empty_offset += QMK_RAW_HID_REPORT_SIZE;
    }
}
