/* Copyright 2024 Morgan Newell Sun
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

#pragma once

#include <stdlib.h>
#include "pointing_device.h"
#include "report.h"

#ifndef POINTING_DEVICE_DRAGSCROLL_ENABLE
#    error "POINTING_DEVICE_DRAGSCROLL_ENABLE not defined! check config settings"
#endif

#ifndef DRAGSCROLL_THROTTLE_MS
#    define DRAGSCROLL_THROTTLE_MS 16
#endif

#ifndef DRAGSCROLL_TIMEOUT_MS
#    define DRAGSCROLL_TIMEOUT_MS 500
#endif

#ifndef DRAGSCROLL_MULTIPLIER_H
#    define DRAGSCROLL_MULTIPLIER_H 1.0
#endif

#ifndef DRAGSCROLL_MULTIPLIER_V
#    define DRAGSCROLL_MULTIPLIER_V 1.0
#endif

#ifndef DRAGSCROLL_SMOOTHING_H
#    define DRAGSCROLL_SMOOTHING_H 1
#else
#    if DRAGSCROLL_SMOOTHING_H < 1
#        error "DRAGSCROLL_SMOOTHING_H must be a positive integer!"
#    endif
#endif

#ifndef DRAGSCROLL_SMOOTHING_V
#    define DRAGSCROLL_SMOOTHING_V 1
#else
#    if DRAGSCROLL_SMOOTHING_V < 1
#        error "DRAGSCROLL_SMOOTHING_V must be a positive integer!"
#    endif
#endif

#ifndef DRAGSCROLL_AXIS_SNAPPING_THRESHOLD
#    define DRAGSCROLL_AXIS_SNAPPING_THRESHOLD 0.25
#endif

#ifndef DRAGSCROLL_AXIS_SNAPPING_RATIO
#    define DRAGSCROLL_AXIS_SNAPPING_RATIO 2.0
#endif

void dragscroll_init(void);
void pointing_device_dragscroll(report_mouse_t* mouse_report);
void dragscroll_on(void);
void dragscroll_off(void);
void dragscroll_toggle(void);
bool is_dragscroll_on(void);
void dragscroll_axis_snapping_on(void);
void dragscroll_axis_snapping_off(void);
void dragscroll_axis_snapping_toggle(void);
bool is_dragscroll_axis_snapping_on(void);

#if defined(SPLIT_POINTING_ENABLE) && defined(POINTING_DEVICE_COMBINED)
void pointing_device_dragscroll_combined(report_mouse_t* mouse_report_left, report_mouse_t* mouse_report_right);
void dragscroll_on_right(void);
void dragscroll_off_right(void);
void dragscroll_toggle_right(void);
bool is_dragscroll_on_right(void);
void dragscroll_axis_snapping_on_right(void);
void dragscroll_axis_snapping_off_right(void);
void dragscroll_axis_snapping_toggle_right(void);
bool is_dragscroll_axis_snapping_on_right(void);
#endif
