/****************************************************************************
*
*    The MIT License (MIT)
*
*    Copyright (c) 2014 - 2022 Vivante Corporation
*
*    Permission is hereby granted, free of charge, to any person obtaining a
*    copy of this software and associated documentation files (the "Software"),
*    to deal in the Software without restriction, including without limitation
*    the rights to use, copy, modify, merge, publish, distribute, sublicense,
*    and/or sell copies of the Software, and to permit persons to whom the
*    Software is furnished to do so, subject to the following conditions:
*
*    The above copyright notice and this permission notice shall be included in
*    all copies or substantial portions of the Software.
*
*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
*    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
*    DEALINGS IN THE SOFTWARE.
*
*****************************************************************************
*
*    The GPL License (GPL)
*
*    Copyright (C) 2014 - 2022 Vivante Corporation
*
*    This program is free software; you can redistribute it and/or
*    modify it under the terms of the GNU General Public License
*    as published by the Free Software Foundation; either version 2
*    of the License, or (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*****************************************************************************
*
*    Note: This software is released under dual MIT and GPL licenses. A
*    recipient may use this file under the terms of either the MIT license or
*    GPL License. If you wish to use only one license not the other, you can
*    indicate your decision by deleting one of the above license notices in your
*    version of this file.
*
*****************************************************************************/

#ifndef VG_LITE_KERNEL_OPTION_H
#define VG_LITE_KERNEL_OPTION_H

#include "../../../lv_conf_internal.h"
#if LV_USE_VG_LITE_DRIVER

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Set gcdVG_ENABLE_WRITEBUFFER to 1 for Linux Write Combined memory access
 * Set gcdVG_ENABLE_WRITEBUFFER to 0 for Linux Uncacheable memory access
*/
#ifndef gcdVG_ENABLE_WRITEBUFFER
#define gcdVG_ENABLE_WRITEBUFFER                     1
#endif

/*
 * Backup state command, only support Linux and RTOS
*/
#ifndef gcdVG_ENABLE_BACKUP_COMMAND
#define gcdVG_ENABLE_BACKUP_COMMAND                  0
#endif

/*
 * Power management, only support Linux and RTOS
*/
#ifndef gcdVG_ENABLE_POWER_MANAGEMENT
#define gcdVG_ENABLE_POWER_MANAGEMENT                0
#endif

/*
 * when set to 1, vg_lite_hal_trace can use to print message
*/
#ifndef gcdVG_ENABLE_DEBUG
#define gcdVG_ENABLE_DEBUG                           1
#endif

/*
 * when set to 1, dump last submit command from kernel
*/
#ifndef gcdVG_ENABLE_DUMP_COMMAND
#define gcdVG_ENABLE_DUMP_COMMAND                    0
#endif

/*
 * when gpu hang, set 1 to open gpu reset function
*/
#ifndef gcdVG_ENABLE_GPU_RESET
#define gcdVG_ENABLE_GPU_RESET                       0
#endif

/*
 * Set 1 to open gpu auto clock gating feature
*/
#ifndef gcdVG_ENABLE_AUTO_CLOCK_GATING
#define gcdVG_ENABLE_AUTO_CLOCK_GATING               0
#endif

/*
 * Set 1 to open dump debug register
*/
#ifndef gcdVG_DUMP_DEBUG_REGISTER
#define gcdVG_DUMP_DEBUG_REGISTER                    0
#endif

/*
 * For zephyr system
 * Set gcdVG_ENABLE_DELAY_RESUME to 1 to open delay resume feature.
*/
#ifndef gcdVG_ENABLE_DELAY_RESUME
#define gcdVG_ENABLE_DELAY_RESUME                    0
#endif

/*
 * Set 1 to record GPU hardware running time.
*/
#ifndef gcdVG_RECORD_HARDWARE_RUNNING_TIME
#define gcdVG_RECORD_HARDWARE_RUNNING_TIME           0
#endif

#ifdef __cplusplus
}
#endif

#endif /* LV_USE_VG_LITE_DRIVER */

#endif /* VG_LITE_KERNEL_OPTION_H */
