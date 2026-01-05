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

#include "../../../lv_conf_internal.h"
#if LV_USE_VG_LITE_DRIVER

#include "../lv_vg_lite_hal/vg_lite_platform.h"
#include "vg_lite_kernel.h"
#include "vg_lite_hal.h"
#include "vg_lite_hw.h"
#if defined(__linux__) && !defined(EMULATOR)
    #include <linux/sched.h>
    /*#include <asm/uaccess.h>*/
    #include <linux/uaccess.h>
    #include <linux/version.h>
    #include <linux/kernel.h>
    #include <linux/pm.h>
    #include <linux/suspend.h>
#endif

#if gcdVG_RECORD_HARDWARE_RUNNING_TIME

    #ifdef __linux__
        #include <linux/jiffies.h>
        unsigned long start_time, end_time;
        unsigned long period_time, total_time = 0;

    #else
        #include <sys/time.h>
        struct timeval start_time, end_time;
        unsigned long period_time, total_time = 0;
    #endif

#endif

#define FLEXA_TIMEOUT_STATE                 BIT(21)
#define FLEXA_HANDSHEKE_FAIL_STATE          BIT(22)
#define MIN_TS_SIZE                         (8 << 10)

#if gcdVG_ENABLE_BACKUP_COMMAND
# define STATE_COMMAND(address) (0x30010000 | address)
# define END_COMMAND(interrupt) (0x00000000 | interrupt)
# define SEMAPHORE_COMMAND(id)  (0x10000000 | id)
# define STALL_COMMAND(id)      (0x20000000 | id)

static vg_lite_kernel_context_t global_power_context = {0};
static uint32_t * power_context_klogical = NULL;
static uint32_t state_map_table[4096];
static uint32_t backup_command_buffer_physical;
static void * backup_command_buffer_klogical;
static uint32_t backup_command_buffer_size;
uint32_t init_buffer[12];
uint32_t is_init;
size_t physical_address;
#endif

static int s_reference = 0;

#if gcdVG_ENABLE_DELAY_RESUME
    static int delay_resume = 0;
#endif

#if gcdVG_ENABLE_GPU_RESET
    static uint32_t gpu_reset_count = 0;
#endif

static vg_lite_error_t do_terminate(vg_lite_kernel_terminate_t * data);
static vg_lite_error_t vg_lite_kernel_vidmem_allocate(uint32_t * bytes, uint32_t flags, vg_lite_vidmem_pool_t pool,
                                                      void ** memory, void ** kmemory, uint32_t * memory_gpu, void ** memory_handle);
static vg_lite_error_t vg_lite_kernel_vidmem_free(void * handle);
static void soft_reset(void);
static vg_lite_error_t do_wait(vg_lite_kernel_wait_t * data);

#if gcdVG_ENABLE_BACKUP_COMMAND
static vg_lite_error_t restore_gpu_state(void);

static vg_lite_error_t restore_init_command(uint32_t physical, uint32_t size)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_uint32_t total_suspend_time = 0;
    vg_lite_uint32_t suspend_time_limit = 1000;

    /* flush cache. */
    vg_lite_hal_barrier();

    vg_lite_hal_poke(VG_LITE_HW_CMDBUF_ADDRESS, physical);
    vg_lite_hal_poke(VG_LITE_HW_CMDBUF_SIZE, (size + 7) / 8);

    while(!vg_lite_hal_peek(VG_LITE_INTR_STATUS)) {
        vg_lite_hal_delay(2);
        if(total_suspend_time < suspend_time_limit) {
            total_suspend_time += 2;
        }
        else {
            error = VG_LITE_TIMEOUT;
            break;
        }
    }
    vg_lite_hal_delay(2);

    return error;
}
#if gcdVG_ENABLE_GPU_RESET && gcdVG_ENABLE_BACKUP_COMMAND
static vg_lite_error_t execute_command(uint32_t physical, uint32_t size, vg_lite_gpu_reset_type_t reset_type)
{
    vg_lite_kernel_wait_t wait;
    vg_lite_error_t error = VG_LITE_SUCCESS;

    wait.timeout_ms = 1000;
    wait.event_mask = (uint32_t)~0;
    wait.reset_type = reset_type;

    /* flush cache. */
    vg_lite_hal_barrier();

    vg_lite_hal_poke(VG_LITE_HW_CMDBUF_ADDRESS, physical);
    vg_lite_hal_poke(VG_LITE_HW_CMDBUF_SIZE, (size + 7) / 8);

    error = do_wait(&wait);

    return error;
}
#endif

static uint32_t push_command(uint32_t command, uint32_t data, uint32_t index)
{
    uint32_t address = 0;

    if((command & 0xFFFF0000) == 0x30010000) {
        address = command & 0x0000FFFF;
        state_map_table[address] = index;
    }

    if(NULL == power_context_klogical)
        power_context_klogical = global_power_context.power_context_klogical;

    power_context_klogical[index++] = command;
    power_context_klogical[index++] = data;

    return index;
}

static vg_lite_error_t backup_power_context_buffer(uint32_t * command_buffer_klogical, uint32_t size)
{
    int index              = 0;
    uint32_t command       = 0;
    uint32_t address       = 0;
    uint32_t context_index = 0;
    uint32_t data          = 0;

    if(NULL == command_buffer_klogical) {
        return VG_LITE_INVALID_ARGUMENT;
    }

    for(index = 0; index < size; index++) {
        command = command_buffer_klogical[index];

        if(((command & 0xFFFF0000) == 0x30010000) && ((index % 2) == 0)) {
            data = command_buffer_klogical[index + 1];
            address = command & 0x0000FFFF;
            context_index = state_map_table[address];
            if((address < 0) || (address > 4095)) {
                vg_lite_kernel_print("Index out of bounds, wrong address and data 0x%08X 0x%08X\n", command, data);
                return VG_LITE_INVALID_ARGUMENT;
            }
            if(-1 != context_index) {
                power_context_klogical[context_index + 1] = data;
            }
            else {
                power_context_klogical[global_power_context.power_context_size / 4 + 0] = command;
                power_context_klogical[global_power_context.power_context_size / 4 + 1] = data;
                state_map_table[address] = global_power_context.power_context_size / 4;
                global_power_context.power_context_size += 8;
            }
        }
    }

    return VG_LITE_SUCCESS;
}
#endif

static void gpu(int enable)
{
    vg_lite_hw_clock_control_t value;
    uint32_t          reset_timer = 2;
    const uint32_t    reset_timer_limit = 1000;
#if gcdVG_ENABLE_AUTO_CLOCK_GATING
    uint32_t          data;
#endif

    if(enable) {
        /* Enable clock gating. */
        value.data = vg_lite_hal_peek(VG_LITE_HW_CLOCK_CONTROL);
        value.control.clock_gate = 0;
        vg_lite_hal_poke(VG_LITE_HW_CLOCK_CONTROL, value.data);
        vg_lite_hal_delay(1);

        /* Set clock speed. */
        value.control.scale = 64;
        value.control.scale_load = 1;
        vg_lite_hal_poke(VG_LITE_HW_CLOCK_CONTROL, value.data);
        vg_lite_hal_delay(1);
        value.control.scale_load = 0;
        vg_lite_hal_poke(VG_LITE_HW_CLOCK_CONTROL, value.data);
        vg_lite_hal_delay(5);

#if gcdVG_DUMP_DEBUG_REGISTER
        value.control.debug_registers = 0;
        vg_lite_hal_poke(VG_LITE_HW_CLOCK_CONTROL, value.data);
#endif

        /* Perform a soft reset. */
        soft_reset();
        do {
            vg_lite_hal_delay(reset_timer);
            reset_timer *= 2;   // If reset failed, try again with a longer wait. Need to check why if dead lopp happens here.
        } while(!VG_LITE_KERNEL_IS_GPU_IDLE());

#if gcdVG_ENABLE_AUTO_CLOCK_GATING
        /* Enable Module Clock gating */
        data = vg_lite_hal_peek(VG_LITE_POWER_CONTROL);
        data |= 0x1;
        vg_lite_hal_poke(VG_LITE_POWER_CONTROL, data);
        vg_lite_hal_delay(1);

#if !gcFEATURE_VG_CLOCK_GATING_TS_MODULE
        data = vg_lite_hal_peek(VG_LITE_POWER_MODULE_CONTROL);
        data |= 0x800;
        vg_lite_hal_poke(VG_LITE_POWER_MODULE_CONTROL, data);
        vg_lite_hal_delay(1);
#endif

#if !gcFEATURE_VG_CLOCK_GATING_VG_MODULE
        data = vg_lite_hal_peek(VG_LITE_POWER_MODULE_CONTROL);
        data |= 0x100;
        vg_lite_hal_poke(VG_LITE_POWER_MODULE_CONTROL, data);
        vg_lite_hal_delay(1);
#endif

#endif
    }
    else {
        while(!VG_LITE_KERNEL_IS_GPU_IDLE() &&
              (reset_timer < reset_timer_limit)   // Force shutdown if timeout.
             ) {
            vg_lite_hal_delay(reset_timer);
            reset_timer *= 2;
        }

        /* Set idle speed. */
        value.data = vg_lite_hal_peek(VG_LITE_HW_CLOCK_CONTROL);
        value.control.scale = 1;
        value.control.scale_load = 1;
        vg_lite_hal_poke(VG_LITE_HW_CLOCK_CONTROL, value.data);
        vg_lite_hal_delay(1);
        value.control.scale_load = 0;
        vg_lite_hal_poke(VG_LITE_HW_CLOCK_CONTROL, value.data);
        vg_lite_hal_delay(5);

        /* Disable clock gating. */
        value.control.clock_gate = 1;
        vg_lite_hal_poke(VG_LITE_HW_CLOCK_CONTROL, value.data);
        vg_lite_hal_delay(1);
    }
}

/* Initialize some customized modeuls [DDRLess]. */
static vg_lite_error_t init_3rd(vg_lite_kernel_initialize_t * data)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    /* TODO: Init the YUV<->RGB converters. Reserved for SOC. */
    /* vg_lite_hal_poke(0x00514, data->yuv_pre);
       vg_lite_hal_poke(0x00518, data->yuv_post);
     */
    return error;
}

static vg_lite_error_t init_vglite(vg_lite_kernel_initialize_t * data)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_kernel_context_t * context;
    vg_lite_uint32_t flags = 0, i;
#if gcdVG_ENABLE_BACKUP_COMMAND
    vg_lite_uint32_t index;
#endif

#if defined(__linux__) && !defined(EMULATOR)
    vg_lite_kernel_context_t __user * context_usr;
    vg_lite_kernel_context_t mycontext = {
        .command_buffer = { 0 },
        .command_buffer_logical = { 0 },
        .command_buffer_klogical = { 0 },
        .command_buffer_physical = { 0 },
    };

    // Construct the context.
    context_usr = (vg_lite_kernel_context_t  __user *) data->context;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0)
    if(!access_ok(VERIFY_READ, context_usr, sizeof(*context_usr)) ||
       !access_ok(VERIFY_WRITE, context_usr, sizeof(*context_usr))) {
#else
    if(!access_ok(context_usr, sizeof(*context_usr)) ||
       !access_ok(context_usr, sizeof(*context_usr))) {
#endif
        /* Out of memory. */
        return VG_LITE_OUT_OF_MEMORY;
    }
    context = &mycontext;
#else
    // Construct the context.
    context = data->context;
    if(context == NULL) {
        /* Out of memory. */
        return VG_LITE_OUT_OF_MEMORY;
    }
#endif

    /* Zero out all pointers. */
    for(i = 0; i < CMDBUF_COUNT; i++) {
        context->command_buffer[i]          = NULL;
        context->command_buffer_logical[i]  = NULL;
        context->command_buffer_physical[i] = 0;
    }
    context->tess_buffer            = NULL;
    context->tessbuf_logical    = NULL;
    context->tessbuf_physical   = 0;
#if gcdVG_ENABLE_BACKUP_COMMAND
    global_power_context.power_context_logical = NULL;
    global_power_context.power_context_klogical = NULL;
    global_power_context.power_context_physical = 0;
    global_power_context.power_context = NULL;
    global_power_context.power_context_capacity = 32 << 10;
    global_power_context.power_context_size = 0;
#endif
    /* Increment reference counter. */
    if(s_reference++ == 0) {
        /* Initialize the SOC. */
        vg_lite_hal_initialize();

        /* Enable the GPU. */
        gpu(1);
    }

    /* Fill in hardware capabilities. */
    data->capabilities.data = 0;

    /* Allocate the command buffer. */
    if(data->command_buffer_size) {
        for(i = 0; i < CMDBUF_COUNT; i ++) {
            /* Allocate the memory. */
            error = vg_lite_kernel_vidmem_allocate(&data->command_buffer_size,
                                                   flags,
                                                   data->command_buffer_pool,
                                                   &context->command_buffer_logical[i],
                                                   &context->command_buffer_klogical[i],
                                                   &context->command_buffer_physical[i],
                                                   &context->command_buffer[i]);
            if(error != VG_LITE_SUCCESS) {
                /* Free any allocated memory. */
                vg_lite_kernel_terminate_t terminate = { context };
                do_terminate(&terminate);

                /* Out of memory. */
                ONERROR(error);
            }

            /* Return command buffer logical pointer and GPU address. */
            data->command_buffer[i] = context->command_buffer_logical[i];
            data->command_buffer_gpu[i] = context->command_buffer_physical[i];
        }
    }

#if gcdVG_ENABLE_BACKUP_COMMAND
    if(global_power_context.power_context_capacity) {
        /*  Allocate the backup buffer. */
        error = vg_lite_kernel_vidmem_allocate(&global_power_context.power_context_capacity,
                                               flags,
                                               VG_LITE_POOL_RESERVED_MEMORY1,
                                               &global_power_context.power_context_logical,
                                               &global_power_context.power_context_klogical,
                                               &global_power_context.power_context_physical,
                                               &global_power_context.power_context);
        if(error != VG_LITE_SUCCESS) {
            /* Free any allocated memory. */
            vg_lite_kernel_terminate_t terminate = { &global_power_context };
            do_terminate(&terminate);

            /* Out of memory. */
            ONERROR(error);
        }

        /* Initialize power context buffer */
        for(i = 0; i < sizeof(state_map_table) / sizeof(state_map_table[0]); i++)
            state_map_table[i] = -1;
#if (CHIPID==0x355 || CHIPID==0x255)
        index = push_command(STATE_COMMAND(0x0A30), 0x00000000, 0);
        index = push_command(STATE_COMMAND(0x0A31), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0A32), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0A33), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0A35), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0A36), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0A37), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0A38), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0A3A), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0A3D), 0x00000000, index);
#else
        index = push_command(STATE_COMMAND(0x0A35), 0x00000000, 0);
        index = push_command(STATE_COMMAND(0x0AC8), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0ACB), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0ACC), 0x00000000, index);
#endif
        index = push_command(STATE_COMMAND(0x0A90), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0A91), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0A92), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0A93), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0A94), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0A95), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0A96), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0A97), 0x00000000, index);

        index = push_command(STATE_COMMAND(0x0A10), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0AC8), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0AC8), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0A5C), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0A5D), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0A11), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0A12), 0x00000000, index);
        index = push_command(STATE_COMMAND(0x0A13), 0x00000000, index);

        global_power_context.power_context_size = index * 4;
    }
#endif
    /* Allocate the tessellation buffer. */
    if((data->tess_width > 0) && (data->tess_height > 0)) {
        int width = data->tess_width;
        int height = 0;
        int vg_countbuffer_size = 0, total_size = 0, ts_buffer_size = 0;

        height = VG_LITE_ALIGN(data->tess_height, 16);

#if (CHIPID==0x355 || CHIPID==0x255)
        {
            unsigned long stride, buffer_size, l1_size, l2_size;
#if (CHIPID==0x355)
            data->capabilities.cap.l2_cache = 1;
            width = VG_LITE_ALIGN(width, 128);
#endif
            /* Check if we can used tiled tessellation (128x16). */
            if(((width & 127) == 0) && ((height & 15) == 0)) {
                data->capabilities.cap.tiled = 0x3;
            }
            else {
                data->capabilities.cap.tiled = 0x2;
            }

            /* Compute tessellation buffer size. */
            stride = VG_LITE_ALIGN(width * 8, 64);
            buffer_size = VG_LITE_ALIGN(stride * height, 64);
            /* Each bit in the L1 cache represents 64 bytes of tessellation data. */
            l1_size = VG_LITE_ALIGN(VG_LITE_ALIGN(buffer_size / 64, 64) / 8, 64);
#if (CHIPID==0x355)
            /* Each bit in the L2 cache represents 32 bytes of L1 data. */
            l2_size = VG_LITE_ALIGN(VG_LITE_ALIGN(l1_size / 32, 64) / 8, 64);
#else
            l2_size = 0;
#endif
            total_size = buffer_size + l1_size + l2_size;
            ts_buffer_size = buffer_size;
        }
#else /* (CHIPID==0x355 || CHIPID==0x255) */
        {
            /* Check if we can used tiled tessellation (128x16). */
            if(((width & 127) == 0) && ((height & 15) == 0)) {
                data->capabilities.cap.tiled = 0x3;
            }
            else {
                data->capabilities.cap.tiled = 0x2;
            }

            vg_countbuffer_size = (height + 13) / 14;
            vg_countbuffer_size = vg_countbuffer_size * 64;
            total_size = height * 128 + vg_countbuffer_size;
            if(total_size < MIN_TS_SIZE)
                total_size = MIN_TS_SIZE;
            ts_buffer_size = total_size - vg_countbuffer_size;
        }
#endif /* (CHIPID==0x355 || CHIPID==0x255) */

        /* Allocate the memory. */
        error = vg_lite_kernel_vidmem_allocate((uint32_t *)&total_size,
                                               flags,
                                               data->tess_buffer_pool,
                                               &context->tessbuf_logical,
                                               &context->tessbuf_klogical,
                                               &context->tessbuf_physical,
                                               &context->tess_buffer);
        if(error != VG_LITE_SUCCESS) {
            /* Free any allocated memory. */
            vg_lite_kernel_terminate_t terminate = { context };
            do_terminate(&terminate);

            /* Out of memory. */
            ONERROR(error);
        }

        /* Return the tessellation buffer pointers and GPU addresses. */
        data->physical_addr = context->tessbuf_physical;
        data->logical_addr = (uint8_t *)context->tessbuf_logical;
        data->tessbuf_size = ts_buffer_size;
        data->countbuf_size = vg_countbuffer_size;
        data->tess_w_h = width | (height << 16);
    }

#if gcdVG_ENABLE_GPU_RESET
    gpu_reset_count = 0;
#endif
    vg_lite_set_gpu_execute_state(VG_LITE_GPU_STOP);
    /* Enable all interrupts. */
    vg_lite_hal_poke(VG_LITE_INTR_ENABLE, 0xFFFFFFFF);

#if defined(__linux__) && !defined(EMULATOR)
    if(copy_to_user(context_usr, context, sizeof(vg_lite_kernel_context_t)) != 0) {
        // Free any allocated memory.
        vg_lite_kernel_terminate_t terminate = { context };
        do_terminate(&terminate);

        return VG_LITE_NO_CONTEXT;
    }
#endif

on_error:
    return error;
}

static vg_lite_error_t do_initialize(vg_lite_kernel_initialize_t * data)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    /* Free any allocated memory for the context. */
    do {
        error = init_vglite(data);
        if(error != VG_LITE_SUCCESS)
            break;

        error = init_3rd(data);
        if(error != VG_LITE_SUCCESS)
            break;
    } while(0);

    return error;
}

static vg_lite_error_t terminate_vglite(vg_lite_kernel_terminate_t * data)
{
    vg_lite_kernel_context_t * context = NULL;
#if defined(__linux__) && !defined(EMULATOR)
    vg_lite_kernel_context_t mycontext = {
        .command_buffer = { 0 },
        .command_buffer_logical = { 0 },
        .command_buffer_klogical = { 0 },
        .command_buffer_physical = { 0 },
    };
    if(copy_from_user(&mycontext, data->context, sizeof(vg_lite_kernel_context_t)) != 0) {
        return VG_LITE_NO_CONTEXT;
    }
    context = &mycontext;
#else
    context = data->context;
#endif

    /* Free any allocated memory for the context. */
    if(context->command_buffer[0]) {
        /* Free the command buffer. */
        vg_lite_kernel_vidmem_free(context->command_buffer[0]);
        context->command_buffer[0] = NULL;
    }

#if !gcFEATURE_VG_SINGLE_COMMAND_BUFFER
    if(context->command_buffer[1]) {
        /* Free the command buffer. */
        vg_lite_kernel_vidmem_free(context->command_buffer[1]);
        context->command_buffer[1] = NULL;
    }
#endif

#if gcdVG_ENABLE_BACKUP_COMMAND
    if(global_power_context.power_context) {
        /* Free the power context. */
        vg_lite_kernel_vidmem_free(global_power_context.power_context);
        global_power_context.power_context = NULL;
    }
#endif
    if(context->tess_buffer) {
        /* Free the tessellation buffer. */
        vg_lite_kernel_vidmem_free(context->tess_buffer);
        context->tess_buffer = NULL;
    }
    vg_lite_hal_free_os_heap();
    /* Decrement reference counter. */
    if(--s_reference == 0) {
        /* Disable the GPU. */
        gpu(0);

        /* De-initialize the SOC. */
        vg_lite_hal_deinitialize();
    }

#if defined(__linux__) && !defined(EMULATOR)
    if(copy_to_user((vg_lite_kernel_context_t  __user *) data->context,
                    &mycontext, sizeof(vg_lite_kernel_context_t)) != 0) {
        return VG_LITE_NO_CONTEXT;
    }
#endif
    return VG_LITE_SUCCESS;
}

static vg_lite_error_t terminate_3rd(vg_lite_kernel_terminate_t * data)
{
    /* TODO: Terminate the converters. */

    return VG_LITE_SUCCESS;
}

static vg_lite_error_t do_terminate(vg_lite_kernel_terminate_t * data)
{
    terminate_vglite(data);
    terminate_3rd(data);

    return VG_LITE_SUCCESS;
}

static vg_lite_error_t vg_lite_kernel_vidmem_allocate(uint32_t * bytes, uint32_t flags, vg_lite_vidmem_pool_t pool,
                                                      void ** memory, void ** kmemory, uint32_t * memory_gpu, void ** memory_handle)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    error = vg_lite_hal_allocate_contiguous(*bytes, pool, memory, kmemory, memory_gpu, memory_handle);
    if(VG_IS_ERROR(error)) {
        ONERROR(error);
    }

    return error;
on_error:
    return error;
}

static vg_lite_error_t vg_lite_kernel_vidmem_free(void * handle)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    vg_lite_hal_free_contiguous(handle);

    return error;
}

static vg_lite_error_t do_allocate(vg_lite_kernel_allocate_t * data)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    error = vg_lite_kernel_vidmem_allocate(&data->bytes, data->flags, data->pool, &data->memory, &data->kmemory,
                                           &data->memory_gpu, &data->memory_handle);

    return error;
}

static vg_lite_error_t do_free(vg_lite_kernel_free_t * data)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    error = vg_lite_kernel_vidmem_free(data->memory_handle);

    return error;
}

static vg_lite_error_t do_submit(vg_lite_kernel_submit_t * data)
{
    uint32_t offset;
    vg_lite_kernel_context_t * context = NULL;
    uint32_t physical = data->context->command_buffer_physical[data->command_id];

#if defined(__linux__) && !defined(EMULATOR)
    vg_lite_kernel_context_t mycontext = {
        .command_buffer = { 0 },
        .command_buffer_logical = { 0 },
        .command_buffer_klogical = { 0 },
        .command_buffer_physical = { 0 },
    };

    if(copy_from_user(&mycontext, data->context, sizeof(vg_lite_kernel_context_t)) != 0) {
        return VG_LITE_NO_CONTEXT;
    }
    context = &mycontext;
    physical = context->command_buffer_physical[data->command_id];
#else
    context = data->context;
    if(context == NULL) {
        return VG_LITE_NO_CONTEXT;
    }
#endif
    /* Perform a memory barrier. */
    vg_lite_hal_barrier();

    offset = (uint8_t *) data->commands - (uint8_t *)context->command_buffer_logical[data->command_id];

#if gcdVG_ENABLE_BACKUP_COMMAND
    backup_power_context_buffer((uint32_t *)((uint8_t *)context->command_buffer_klogical[data->command_id] + offset),
                                (data->command_size + 3) / 4);
    backup_command_buffer_physical = physical + offset;
    backup_command_buffer_klogical = (uint32_t *)((uint8_t *)context->command_buffer_klogical[data->command_id] + offset);
    backup_command_buffer_size = data->command_size;
#endif

#if gcdVG_RECORD_HARDWARE_RUNNING_TIME
#ifdef __linux__
    start_time = jiffies;
#else
    gettimeofday(&start_time, NULL);
#endif
#endif

    /* set gpu to busy state  */
    vg_lite_set_gpu_execute_state(VG_LITE_GPU_RUN);

    /* Write the registers to kick off the command execution (CMDBUF_SIZE). */
    vg_lite_hal_poke(VG_LITE_HW_CMDBUF_ADDRESS, physical + offset);
    vg_lite_hal_poke(VG_LITE_HW_CMDBUF_SIZE, (data->command_size + 7) / 8);

    return VG_LITE_SUCCESS;
}

#if gcdVG_ENABLE_DUMP_COMMAND && gcdVG_ENABLE_BACKUP_COMMAND
static void dump_last_frame(void)
{
    uint32_t * ptr = backup_command_buffer_klogical;
    uint32_t size = backup_command_buffer_size;
    uint32_t i = 0;
    uint32_t data = 0;

    vg_lite_kernel_print("This is init command buffer:\n");
    vg_lite_kernel_print("@[%s 0x%08X 0x00000088\n", "command", physical_address);
    vg_lite_kernel_print("  0x30010A35 0x%08X 0x30010AC8 0x%08X\n", init_buffer[0], init_buffer[1]);
    vg_lite_kernel_print("  0x30010ACB 0x%08X 0x30010ACC 0x%08X\n", init_buffer[2], init_buffer[3]);
    vg_lite_kernel_print("  0x30010A90 0x%08X 0x30010A91 0x%08X\n", init_buffer[4], init_buffer[5]);
    vg_lite_kernel_print("  0x30010A92 0x%08X 0x30010A93 0x%08X\n", init_buffer[6], init_buffer[7]);
    vg_lite_kernel_print("  0x30010A94 0x%08X 0x30010A95 0x%08X\n", init_buffer[8], init_buffer[9]);
    vg_lite_kernel_print("  0x30010A96 0x%08X 0x30010A97 0x%08X\n", init_buffer[10], init_buffer[11]);
    vg_lite_kernel_print("  0x30010A00 0x00000001 0x30010A1B 0x00000011\n");
    vg_lite_kernel_print("  0x10000007 0x00000000 0x20000007 0x00000000\n");
    vg_lite_kernel_print("  0x00000000 0x00000000\n");
    vg_lite_kernel_print("] -- %s\n", "command");

    if(is_init == 1) {
        vg_lite_kernel_print("the last submit command is init command.\n");
    }
    else {
        vg_lite_kernel_print("the last submit command before hang:\n");
        vg_lite_kernel_print("@[%s 0x%08X 0x%08X\n", "command", backup_command_buffer_klogical, size);
        for(i = 0; i < size; i += 4) {
            vg_lite_kernel_print("  0x%08X 0x%08X 0x%08X 0x%08X\n", ptr[i], ptr[i + 1], ptr[i + 2], ptr[i + 3]);
        }
        if(size % 16) {
            int j = size % 16 / 4;
            switch(j) {
                case 1:
                    vg_lite_kernel_print("  0x%08X\n", ptr[(size - size % 16) / 4]);
                    break;
                case 2:
                    vg_lite_kernel_print("  0x%08X 0x%08X\n", ptr[(size - size % 16) / 4], ptr[(size - size % 16) / 4 + 1]);
                    break;
                case 3:
                    vg_lite_kernel_print("  0x%08X 0x%08X 0x%08X\n", ptr[(size - size % 16) / 4], ptr[(size - size % 16) / 4 + 1],
                                         ptr[(size - size % 16) / 4 + 2]);
                    break;
                default:
                    break;
            }
        }
    }
    vg_lite_kernel_print("] -- %s\n", "command");

    data = vg_lite_hal_peek(VG_LITE_HW_IDLE);
    vg_lite_kernel_print("vgidle reg = 0x%08X\n", data);
}
#endif

static vg_lite_error_t do_wait(vg_lite_kernel_wait_t * data)
{
#if gcdVG_ENABLE_GPU_RESET && gcdVG_ENABLE_BACKUP_COMMAND
    vg_lite_error_t error = VG_LITE_SUCCESS;
#endif
    /* Wait for interrupt. */
#if gcdVG_DUMP_DEBUG_REGISTER
    if(!vg_lite_hal_wait_interrupt(5000, data->event_mask, &data->event_got)) {
        /* Timeout. */
        unsigned int debug;
        unsigned int iter;

        debug = vg_lite_hal_peek(VG_LITE_HW_IDLE);
        vg_lite_kernel_print("idle = 0x%x\n", debug);

        debug = vg_lite_hal_peek(VG_LITE_HW_CLOCK_CONTROL);
        vg_lite_kernel_print("QAHiClockControl = 0x%x\n", debug);

        for(iter = 0; iter < 16 ; iter ++) {
            vg_lite_hal_poke(0x470, iter);
            debug = vg_lite_hal_peek(0x448);
            vg_lite_kernel_print("0x448[%d] = 0x%x\n", iter, debug);
        }
        for(iter = 0; iter < 16 ; iter ++) {
            vg_lite_hal_poke(0x470, iter << 8);
            debug = vg_lite_hal_peek(0x44C);
            vg_lite_kernel_print("0x44c[%d] = 0x%x\n", iter, debug);
        }
        for(iter = 0; iter < 28 ; iter ++) {
            vg_lite_hal_poke(0x470, iter << 16);
            debug = vg_lite_hal_peek(0x450);
            vg_lite_kernel_print("0x450[%d] = 0x%x\n", iter, debug);
        }
        for(iter = 0; iter < 31; iter++) {
            vg_lite_hal_poke(0x470, iter << 24);
            debug = vg_lite_hal_peek(0x454);
            vg_lite_kernel_print("0x454[%d] = 0x%x\n", iter, debug);
        }
        for(iter = 128; iter < 133; iter++) {
            vg_lite_hal_poke(0x470, iter << 24);
            debug = vg_lite_hal_peek(0x454);
            vg_lite_kernel_print("0x454[%d] = 0x%x\n", iter, debug);
        }
        for(iter = 0; iter < 21; iter++) {
            vg_lite_hal_poke(0x474, iter);
            debug = vg_lite_hal_peek(0x458);
            vg_lite_kernel_print("0x458[%d] = 0x%x\n", iter, debug);
        }
        for(iter = 0; iter < 62; iter++) {
            vg_lite_hal_poke(0x474, iter << 8);
            debug = vg_lite_hal_peek(0x45C);
            vg_lite_kernel_print("0x45C[%d] = 0x%x\n", iter, debug);
        }
        for(iter = 0; iter < 16; iter++) {
            vg_lite_hal_poke(0x474, iter << 16);
            debug = vg_lite_hal_peek(0x460);
            vg_lite_kernel_print("0x460[%d] = 0x%x\n", iter, debug);
        }
        for(iter = 0x40; iter <= 0x60; iter += 4) {
            debug = vg_lite_hal_peek(iter);
            vg_lite_kernel_print("0x%x = 0x%x\n", iter, debug);
        }

        debug = vg_lite_hal_peek(0x438);
        vg_lite_kernel_print("0x%x = 0x%x\n", 0x438, debug);
        debug = vg_lite_hal_peek(0x43C);
        vg_lite_kernel_print("0x%x = 0x%x\n", 0x43C, debug);
        debug = vg_lite_hal_peek(0x440);
        vg_lite_kernel_print("0x%x = 0x%x\n", 0x440, debug);
        debug = vg_lite_hal_peek(0x444);
        vg_lite_kernel_print("0x%x = 0x%x\n", 0x444, debug);
        debug = vg_lite_hal_peek(0x500);
        vg_lite_kernel_print("0x%x = 0x%x\n", 0x500, debug);
        debug = vg_lite_hal_peek(0x504);
        vg_lite_kernel_print("0x%x = 0x%x\n", 0x504, debug);
        debug = vg_lite_hal_peek(0x508);
        vg_lite_kernel_print("0x%x = 0x%x\n", 0x508, debug);
        debug = vg_lite_hal_peek(0x10);
        vg_lite_kernel_print("0x%x = 0x%x\n", 0x10, debug);

        for(iter = 0x14; iter <= 0x34; iter += 4) {
            debug = vg_lite_hal_peek(iter);
            vg_lite_kernel_print("0x%x = 0x%08x\n", iter, debug);
        }

        debug = vg_lite_hal_peek(0x98);
        vg_lite_kernel_print("0x%x = 0x%08x\n", 0x98, debug);
        debug = vg_lite_hal_peek(0xA4);
        vg_lite_kernel_print("0x%x = 0x%08x\n", 0xA4, debug);
        debug = vg_lite_hal_peek(0xA8);
        vg_lite_kernel_print("0x%x = 0x%08x\n", 0xA8, debug);
        debug = vg_lite_hal_peek(0xE8);
        vg_lite_kernel_print("0x%x = 0x%08x\n", 0xE8, debug);
#if gcdVG_ENABLE_DUMP_COMMAND && gcdVG_ENABLE_BACKUP_COMMAND
        dump_last_frame();
#endif
        return VG_LITE_TIMEOUT;
    }
#else
    if(!vg_lite_hal_wait_interrupt(data->timeout_ms, data->event_mask, &data->event_got)) {
        /* Timeout. */
        unsigned int debug;
        debug = vg_lite_hal_peek(VG_LITE_HW_IDLE);
        if(!VG_LITE_KERNEL_IS_GPU_IDLE()) {
            vg_lite_kernel_print("GPU hang.\n");
            vg_lite_kernel_print("GPU idle register = 0x%x\n", debug);
        }

#if gcdVG_ENABLE_DUMP_COMMAND && gcdVG_ENABLE_BACKUP_COMMAND
        dump_last_frame();
#endif

#if gcdVG_ENABLE_GPU_RESET && gcdVG_ENABLE_BACKUP_COMMAND
        gpu_reset_count++;
        if(gpu_reset_count <= 1) {
            if(data->reset_type == RESTORE_INIT_COMMAND) {
                error = VG_LITE_SUCCESS;
            }
            else if(data->reset_type == RESTORE_LAST_COMMAND) {
                error = VG_LITE_SUCCESS;
            }
            else if(data->reset_type == RESTORE_ALL_COMMAND) {
                /* reset and enable the GPU interrupt */
                gpu(1);
                vg_lite_hal_poke(VG_LITE_INTR_ENABLE, 0xFFFFFFFF);
                /* restore gpu state */
                error = execute_command(global_power_context.power_context_physical, global_power_context.power_context_size + 32,
                                        RESTORE_INIT_COMMAND);
                error = execute_command(backup_command_buffer_physical, backup_command_buffer_size, RESTORE_LAST_COMMAND);
            }
            else {
                error = VG_LITE_TIMEOUT;
            }
            gpu_reset_count = 0;
            return error;
        }
        vg_lite_kernel_print("GPU reset fail!\n");
#endif
        return VG_LITE_TIMEOUT;
    }
#endif

#if gcFEATURE_VG_FLEXA
    if(data->event_got & FLEXA_TIMEOUT_STATE)
        return VG_LITE_FLEXA_TIME_OUT;

    if(data->event_got & FLEXA_HANDSHEKE_FAIL_STATE)
        return VG_LITE_FLEXA_HANDSHAKE_FAIL;
#endif

    /* set gpu to idle state  */
    vg_lite_set_gpu_execute_state(VG_LITE_GPU_STOP);

    return VG_LITE_SUCCESS;
}

#if gcdVG_ENABLE_BACKUP_COMMAND
static vg_lite_error_t restore_gpu_state(void)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    int i = 0;
    uint32_t total_size = 0;

    power_context_klogical[global_power_context.power_context_size / 4 + 0] = 0x30010A1B;
    power_context_klogical[global_power_context.power_context_size / 4 + 1] = 0x00000001;
    power_context_klogical[global_power_context.power_context_size / 4 + 2] = 0x10000007;
    power_context_klogical[global_power_context.power_context_size / 4 + 3] = 0x00000000;
    power_context_klogical[global_power_context.power_context_size / 4 + 4] = 0x20000007;
    power_context_klogical[global_power_context.power_context_size / 4 + 5] = 0x00000000;
    power_context_klogical[global_power_context.power_context_size / 4 + 6] = 0x00000000;
    power_context_klogical[global_power_context.power_context_size / 4 + 7] = 0x00000000;
    total_size = global_power_context.power_context_size + 32;

    vg_lite_kernel_print("after resume and the power_context is:\n");
    for(i = 0; i < total_size / 4; i += 4) {
        vg_lite_kernel_print("0x%08X 0x%08X",
                             power_context_klogical[i], power_context_klogical[i + 1]);
        if((i + 2) <= (total_size / 4 - 1))
#if defined(__linux__)
            vg_lite_kernel_print(KERN_CONT " 0x%08X 0x%08X\n",
                                 power_context_klogical[i + 2], power_context_klogical[i + 3]);
#else
            vg_lite_kernel_print(" 0x%08X 0x%08X\n",
                                 power_context_klogical[i + 2], power_context_klogical[i + 3]);
#endif
    }
    vg_lite_kernel_print("global_power_context size = %d\n", total_size);

    /* submit the backup power context */
    error = restore_init_command(global_power_context.power_context_physical, total_size);
    if(error == VG_LITE_SUCCESS)
        vg_lite_kernel_print("Initialize the GPU state success!\n");

    /* submit last frame before suspend */
    /*error = restore_init_command(backup_command_buffer_physical, backup_command_buffer_size);
    if (error == VG_LITE_SUCCESS)
        vg_lite_kernel_print("Initialize the GPU state success!\n");*/

    return error;
}
#endif

static vg_lite_error_t do_reset(vg_lite_kernel_reset_t * data)
{
#if gcdVG_ENABLE_DELAY_RESUME
    if(data->delay_resume_flag == 1) {
        /* If delay resume is enabled, power and clock should be turned on first.*/
        vg_lite_hal_initialize();
    }
#endif
    /* reset and enable the GPU interrupt */
    gpu(1);

#if gcdVG_ENABLE_BACKUP_COMMAND
    restore_gpu_state();
#endif

    vg_lite_hal_poke(VG_LITE_INTR_ENABLE, 0xFFFFFFFF);

    return VG_LITE_SUCCESS;
}

static vg_lite_error_t do_gpu_close(void)
{
    gpu(0);

    vg_lite_kernel_hintmsg("gpu is shutdown!\n");

    return VG_LITE_SUCCESS;
}

static vg_lite_error_t do_debug(void)
{
    return VG_LITE_SUCCESS;
}

static vg_lite_error_t do_map(vg_lite_kernel_map_t * data)
{
    data->memory_handle = vg_lite_hal_map(data->flags, data->bytes, data->logical, data->physical, data->dma_buf_fd,
                                          &data->memory_gpu);
    if(data->memory_handle == NULL)
        return VG_LITE_OUT_OF_RESOURCES;
    else if((long)data->memory_handle == (long) -1)
        return VG_LITE_NOT_SUPPORT;
    else
        return VG_LITE_SUCCESS;
}

static vg_lite_error_t do_unmap(vg_lite_kernel_unmap_t * data)
{
    vg_lite_hal_unmap(data->memory_handle);

    return VG_LITE_SUCCESS;
}

static vg_lite_error_t do_peek(vg_lite_kernel_info_t * data)
{
    data->reg = vg_lite_hal_peek(data->addr);

    return VG_LITE_SUCCESS;
}

#if gcFEATURE_VG_FLEXA
static vg_lite_error_t do_flexa_enable(vg_lite_kernel_flexa_info_t * data)
{
    /* reset all flexa states */
    vg_lite_hal_poke(0x03600, 0x0);
    /* set sync mode */
    vg_lite_hal_poke(0x03604, data->segment_address);

    vg_lite_hal_poke(0x03608, data->segment_count);

    vg_lite_hal_poke(0x0360C, data->segment_size);

    vg_lite_hal_poke(0x0520, data->sync_mode);

    vg_lite_hal_poke(0x03610, data->stream_id | data->sbi_mode | data->start_flag | data->stop_flag | data->reset_flag);

    return VG_LITE_SUCCESS;
}

static vg_lite_error_t do_flexa_set_background_address(vg_lite_kernel_flexa_info_t * data)
{
    vg_lite_hal_poke(0x03604, data->segment_address);

    vg_lite_hal_poke(0x03608, data->segment_count);

    vg_lite_hal_poke(0x0360C, data->segment_size);

    vg_lite_hal_poke(0x03610, data->stream_id | data->sbi_mode | data->start_flag | data->stop_flag | data->reset_flag);

    return VG_LITE_SUCCESS;
}

static vg_lite_error_t do_flexa_disable(vg_lite_kernel_flexa_info_t * data)
{

    vg_lite_hal_poke(0x0520, data->sync_mode);

    vg_lite_hal_poke(0x03610, data->stream_id | data->sbi_mode);

    /* reset all flexa states */
    vg_lite_hal_poke(0x03600, 0x0);

    return VG_LITE_SUCCESS;
}

static vg_lite_error_t do_flexa_stop_frame(vg_lite_kernel_flexa_info_t * data)
{
    vg_lite_hal_poke(0x03610, data->stream_id | data->sbi_mode | data->start_flag | data->stop_flag | data->reset_flag);

    return VG_LITE_SUCCESS;
}
#endif

static vg_lite_error_t do_query_mem(vg_lite_kernel_mem_t * data)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    error = vg_lite_hal_query_mem(data);

    return error;
}

#if gcdVG_ENABLE_DELAY_RESUME
static vg_lite_error_t set_delay_resume(vg_lite_kernel_delay_resume_t * data)
{
    delay_resume = data->set_delay_resume;

    return VG_LITE_SUCCESS;
}

static int do_query_delay_resume(void)
{
    if(delay_resume == 1) {
        /* Reset delay resume to 0 after query*/
        delay_resume = 0;
        return 1;
    }
    else
        return 0;
}

static int set_gpu_clock_state(vg_lite_kernel_gpu_clock_state_t * gpu_state)
{
#ifdef __ZEPHYR__
    vg_lite_gpu_execute_state_t state = gpu_state->state;
    vg_lite_set_gpu_clock_state(state);
#endif
    return VG_LITE_SUCCESS;
}
#endif

static vg_lite_error_t do_map_memory(vg_lite_kernel_map_memory_t * data)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    error = vg_lite_hal_map_memory(data);

    return error;
}

static vg_lite_error_t do_unmap_memory(vg_lite_kernel_unmap_memory_t * data)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    error = vg_lite_hal_unmap_memory(data);

    return error;
}

static vg_lite_error_t do_cache(vg_lite_kernel_cache_t * data)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    error = vg_lite_hal_operation_cache(data->memory_handle, data->cache_op);

    return error;
}

static vg_lite_error_t do_export_memory(vg_lite_kernel_export_memory_t * data)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    error = vg_lite_hal_memory_export(&data->fd);

    return error;
}

static vg_lite_error_t do_get_running_time(vg_lite_kernel_hardware_running_time_t * data)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
#if gcdVG_RECORD_HARDWARE_RUNNING_TIME
#ifdef __linux__
    data->run_time = total_time;
    data->hertz = HZ;
#else
    data->run_time = total_time;
    data->hertz = 1e6;
#endif
#endif
    return error;
}

vg_lite_error_t record_running_time(void)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

#if gcdVG_RECORD_HARDWARE_RUNNING_TIME

#ifdef __linux__
    end_time = jiffies;
    period_time = end_time - start_time;
    total_time += period_time;

#else
    gettimeofday(&end_time, NULL);
    period_time = (end_time.tv_sec - start_time.tv_sec) * 1e6 + end_time.tv_usec - start_time.tv_usec;
    total_time += period_time;
    //printk("GPU hardware running period time: %f s\n", (float)period_time/1e-6);
    //printk("GPU hardware running total time: %f s\n", (float)total_time/1e-6);
#endif

#endif

    return error;
}

static void soft_reset(void)
{
    vg_lite_hw_clock_control_t value;
    value.data = vg_lite_hal_peek(VG_LITE_HW_CLOCK_CONTROL);

    /* Perform a soft reset. */
    value.control.isolate = 1;
    vg_lite_hal_poke(VG_LITE_HW_CLOCK_CONTROL, value.data);
    value.control.soft_reset = 1;
    vg_lite_hal_poke(VG_LITE_HW_CLOCK_CONTROL, value.data);
    vg_lite_hal_delay(5);
    value.control.soft_reset = 0;
    vg_lite_hal_poke(VG_LITE_HW_CLOCK_CONTROL, value.data);
    value.control.isolate = 0;
    vg_lite_hal_poke(VG_LITE_HW_CLOCK_CONTROL, value.data);
}

vg_lite_error_t vg_lite_kernel(vg_lite_kernel_command_t command, void * data)
{
    /* Dispatch on command. */
    switch(command) {
        case VG_LITE_INITIALIZE:
            /* Initialize the context. */
            return do_initialize(data);

        case VG_LITE_TERMINATE:
            /* Terminate the context. */
            return do_terminate(data);

        case VG_LITE_ALLOCATE:
            /* Allocate contiguous memory. */
            return do_allocate(data);

        case VG_LITE_FREE:
            /* Free contiguous memory. */
            return do_free(data);

        case VG_LITE_SUBMIT:
            /* Submit a command buffer. */
            return do_submit(data);

        case VG_LITE_WAIT:
            /* Wait for the GPU. */
            return do_wait(data);

        case VG_LITE_RESET:
            /* Reset the GPU. */
            return do_reset(data);

        case VG_LITE_DEBUG:
            /* Perform debugging features. */
            return do_debug();

        case VG_LITE_MAP:
            /* Map some memory. */
            return do_map(data);

        case VG_LITE_UNMAP:
            /* Unmap some memory. */
            return do_unmap(data);

        /* Get register info. */
        case VG_LITE_CHECK:
            /* Get register value. */
            return do_peek(data);

#if gcFEATURE_VG_FLEXA
        case VG_LITE_FLEXA_DISABLE:
            /* Write register value. */
            return do_flexa_disable(data);

        case VG_LITE_FLEXA_ENABLE:
            /* Write register value. */
            return do_flexa_enable(data);

        case VG_LITE_FLEXA_STOP_FRAME:
            /* Write register value. */
            return do_flexa_stop_frame(data);

        case VG_LITE_FLEXA_SET_BACKGROUND_ADDRESS:
            /* Write register value. */
            return do_flexa_set_background_address(data);
#endif

        case VG_LITE_QUERY_MEM:
            return do_query_mem(data);

        case VG_LITE_MAP_MEMORY:
            /* Map memory to user */
            return do_map_memory(data);

        case VG_LITE_UNMAP_MEMORY:
            /* Unmap memory to user */
            return do_unmap_memory(data);

        case VG_LITE_CLOSE:
            return do_gpu_close();

        case VG_LITE_CACHE:
            return do_cache(data);

        case VG_LITE_EXPORT_MEMORY:
            return do_export_memory(data);

        case VG_LITE_RECORD_RUNNING_TIME:
            return do_get_running_time(data);

#if gcdVG_ENABLE_DELAY_RESUME
        case VG_LITE_SET_DELAY_RESUME:
            return set_delay_resume(data);

        case VG_LITE_QUERY_DELAY_RESUME:
            return do_query_delay_resume();

        case VG_LITE_SET_GPU_CLOCK_STATE:
            return set_gpu_clock_state(data);
#endif
        default:
            break;
    }

    /* Invalid command. */
    return VG_LITE_INVALID_ARGUMENT;
}

#endif /* LV_USE_VG_LITE_DRIVER */

