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

#ifndef VG_LITE_HW_H
#define VG_LITE_HW_H

#include "../../../lv_conf_internal.h"
#if LV_USE_VG_LITE_DRIVER

#define VG_LITE_HW_CLOCK_CONTROL     0x000
#define VG_LITE_HW_IDLE              0x004
#define VG_LITE_INTR_STATUS          0x010
#define VG_LITE_INTR_ENABLE          0x014
#define VG_LITE_HW_CHIP_ID           0x020
#define VG_LITE_HW_CMDBUF_ADDRESS    0x500
#define VG_LITE_HW_CMDBUF_SIZE       0x504
#define VG_LITE_POWER_CONTROL        0x100
#define VG_LITE_POWER_MODULE_CONTROL 0x104

#define VG_LITE_EXT_WORK_CONTROL     0x520
#define VG_LITE_EXT_VIDEO_SIZE       0x524
#define VG_LITE_EXT_CLEAR_VALUE      0x528

#define VG_LITE_EXT_VIDEO_CONTROL    0x51C

typedef struct clock_control {
    uint32_t reserved0 : 1;
    uint32_t clock_gate : 1;
    uint32_t scale : 7;
    uint32_t scale_load : 1;
    uint32_t ram_clock_gating : 1;
    uint32_t debug_registers : 1;
    uint32_t soft_reset : 1;
    uint32_t reserved13 : 6;
    uint32_t isolate : 1;
} clock_control_t;

typedef union vg_lite_hw_clock_control {
    clock_control_t control;
    uint32_t        data;
} vg_lite_hw_clock_control_t;

#define VG_LITE_HW_IDLE_STATE       0x0B05

#endif /* LV_USE_VG_LITE_DRIVER */

#endif /* VG_LITE_HW_H */
