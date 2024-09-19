#pragma once

#include "report.h"
#include "mouse_passthrough_common.h"

void mouse_passthrough_sender_matrix_scan_task(void);
bool mouse_passthrough_sender_raw_hid_receive_task(uint8_t* data);
report_mouse_t mouse_passthrough_sender_pointing_device_task(report_mouse_t mouse);
bool is_mouse_passthrough_connected(void);
