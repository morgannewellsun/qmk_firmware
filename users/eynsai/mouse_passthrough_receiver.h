#pragma once

#include "mouse_passthrough_common.h"

void mouse_passthrough_reciever_matrix_scan_task(void);
bool mouse_passthrough_reciever_raw_hid_receive_task(uint8_t* data);

bool is_mouse_passthrough_connected(void);
void mouse_passthrough_block_buttons_on(void);
void mouse_passthrough_block_buttons_off(void);
void mouse_passthrough_block_pointer_on(void);
void mouse_passthrough_block_pointer_off(void);
void mouse_passthrough_block_wheel_on(void);
void mouse_passthrough_block_wheel_off(void);
void mouse_passthrough_send_buttons_on(void);
void mouse_passthrough_send_buttons_off(void);
void mouse_passthrough_send_pointer_on(void);
void mouse_passthrough_send_pointer_off(void);
void mouse_passthrough_send_wheel_on(void);
void mouse_passthrough_send_wheel_off(void);
void mouse_passthrough_send_reset(void);