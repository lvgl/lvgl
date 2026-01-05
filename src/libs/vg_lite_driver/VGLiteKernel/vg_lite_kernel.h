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

#ifndef VG_LITE_KERNEL_H
#define VG_LITE_KERNEL_H

#include "../../../lv_conf_internal.h"
#if LV_USE_VG_LITE_DRIVER

#include "../VGLite/vg_lite_options.h"
#include "vg_lite_option.h"

/* Interrupt IDs from GPU. */
#define EVENT_UNEXPECTED_MESH  0x80000000
#define EVENT_CMD_BAD_WRITE    0x40000000
#define EVENT_ERROR_RECOVER    0x20000000
#define EVENT_CMD_SWITCH       0x10000000
#define EVENT_MCU_BAD_WRITE    0x08000000
#define EVENT_END              0
#define EVENT_FRAME_END        1

#define MAX_CONTIGUOUS_SIZE 0x04000000

#define VG_LITE_INFINITE    0xFFFFFFFF

#if gcFEATURE_VG_SINGLE_COMMAND_BUFFER
    #define CMDBUF_COUNT        1
#else
    #define CMDBUF_COUNT        2
#endif

#define VG_LITE_ALIGN(number, alignment)    \
    (((number) + ((alignment) - 1)) & ~((alignment) - 1))

#ifndef  BIT
    #define  BIT(x)                 (1 << x)
#endif

#define VG_LITE_KERNEL_IS_GPU_IDLE() \
    ((vg_lite_hal_peek(VG_LITE_HW_IDLE) & VG_LITE_HW_IDLE_STATE) == VG_LITE_HW_IDLE_STATE)

/* Hardware chip Ids */
#define GPU_CHIP_ID_GCNANOLITEV         0x255
#define GPU_CHIP_ID_GC355               0x355
#define GPU_CHIP_ID_GCNANOULTRAV        0x265

/* vg_lite_kernel_map_t flag type */
#define VG_LITE_HAL_MAP_DMABUF          0x00000004
#define VG_LITE_HAL_MAP_USER_MEMORY     0x00000008
#define VG_LITE_HAL_ALLOC_4G            0x00000010

/* vg_lite_kernel_allocate_t flag type */
#define VG_LITE_RESERVED_ALLOCATOR      0x10000000
#define VG_LITE_GFP_ALLOCATOR           0x20000000
#define VG_LITE_DMA_ALLOCATOR           0x40000000
#define VG_LITE_MEMORY_ALLOCATOR_FLAG   0x70000000

#ifdef __cplusplus
extern "C" {
#endif

#ifndef VG_LITE_ERROR
#define VG_LITE_ERROR  1
/*!
    @abstract Error codes that the vg_lite functions can return.

    @discussion
    All API functions return a status code. On success, VG_LITE_SUCCESS will be returned when a function is
    successful. This value is set to zero, so if any function returns a non-zero value, an error has occurred.
    */
typedef enum vg_lite_error {
    VG_LITE_SUCCESS = 0,          /*! Success.                                         */
    VG_LITE_INVALID_ARGUMENT,     /*! An invalid argument was specified.               */
    VG_LITE_OUT_OF_MEMORY,        /*! Out of GPU memory                                */
    VG_LITE_NO_CONTEXT,           /*! No context or an unintialized context specified. */
    VG_LITE_TIMEOUT,              /*! A timeout has occurred during a wait.            */
    VG_LITE_OUT_OF_RESOURCES,     /*! Out of system resources.                         */
    VG_LITE_GENERIC_IO,           /*! Cannot communicate with the kernel driver.       */
    VG_LITE_NOT_SUPPORT,          /*! Function call not supported.                     */
    VG_LITE_ALREADY_EXISTS,       /*! Object already exists                            */
    VG_LITE_NOT_ALIGNED,          /*! Data alignment error                             */
    VG_LITE_FLEXA_TIME_OUT,       /*! VG timeout requesting for segment buffer         */
    VG_LITE_FLEXA_HANDSHAKE_FAIL, /*! VG and SBI synchronizer handshake failed         */
    VG_LITE_SYSTEM_CALL_FAIL,     /*! kernel api call fail                             */
}
vg_lite_error_t;
#endif

typedef enum vg_lite_kernel_counter {
    /* Dont't touch the counter. */
    VG_LITE_NONE,

    /* Turn the counter on. */
    VG_LITE_ON,

    /* Turn the counter off. */
    VG_LITE_OFF,

    /* Query the counter and reset its values. */
    VG_LITE_QUERY,
}
vg_lite_kernel_counter_t;

typedef enum vg_lite_kernel_command {
    /* Initialize the GPU. */
    VG_LITE_INITIALIZE,

    /* Terminate the GPU. */
    VG_LITE_TERMINATE,

    /* Allocate memory. */
    VG_LITE_ALLOCATE,

    /* Free memory. */
    VG_LITE_FREE,

    /* Submit a command buffer to the GPU. */
    VG_LITE_SUBMIT,

    /* Wait for the GPU to be completed. */
    VG_LITE_WAIT,

    /* Reset the GPU. */
    VG_LITE_RESET,

    /* Debug commands. */
    VG_LITE_DEBUG,

    /* Map memory. */
    VG_LITE_MAP,

    /* Unmap memory. */
    VG_LITE_UNMAP,

    /* Check info. */
    VG_LITE_CHECK,

    /* Query mem. */
    VG_LITE_QUERY_MEM,

    /* Flexa disable */
    VG_LITE_FLEXA_DISABLE,

    /* Flexa enable */
    VG_LITE_FLEXA_ENABLE,

    /* Flexa stop frame */
    VG_LITE_FLEXA_STOP_FRAME,

    /* Set background address */
    VG_LITE_FLEXA_SET_BACKGROUND_ADDRESS,

    /* Map memory to user */
    VG_LITE_MAP_MEMORY,

    /* Unmap memory to user */
    VG_LITE_UNMAP_MEMORY,

    /* Close gpu */
    VG_LITE_CLOSE,

    /* Operation cache */
    VG_LITE_CACHE,

    /* Export memory */
    VG_LITE_EXPORT_MEMORY,

    /* Record GPU hardware running time */
    VG_LITE_RECORD_RUNNING_TIME,

    /* Set delay resume state */
    VG_LITE_SET_DELAY_RESUME,

    /* Query delay resume state */
    VG_LITE_QUERY_DELAY_RESUME,

    /* Set GPU clock state */
    VG_LITE_SET_GPU_CLOCK_STATE,

}
vg_lite_kernel_command_t;

typedef enum vg_lite_cache_op {
    VG_LITE_CACHE_CLEAN,
    VG_LITE_CACHE_INVALIDATE,
    VG_LITE_CACHE_FLUSH,
}
vg_lite_cache_op_t;

typedef enum vg_lite_vidmem_pool {
    VG_LITE_POOL_RESERVED_MEMORY1 = 0,
    VG_LITE_POOL_RESERVED_MEMORY2 = 1,
}
vg_lite_vidmem_pool_t;

typedef enum vg_lite_gpu_execute_state {
    VG_LITE_GPU_STOP = 0,
    VG_LITE_GPU_RUN  = 1,
}
vg_lite_gpu_execute_state_t;

/* Context structure. */
typedef struct vg_lite_kernel_context {
    /* Command buffer. */
    void           *          command_buffer[CMDBUF_COUNT];
    void           *          command_buffer_logical[CMDBUF_COUNT];
    void           *          command_buffer_klogical[CMDBUF_COUNT];
    uint32_t                  command_buffer_physical[CMDBUF_COUNT];
    uint32_t                  end_of_frame;

    /* Tessellation buffer. */
    void           *          tess_buffer;
    void           *          tessbuf_logical;
    void           *          tessbuf_klogical;
    uint32_t                  tessbuf_physical;

    /* power context buffer  */
    void           *          power_context;
    void           *          power_context_logical;
    void           *          power_context_klogical;
    uint32_t                  power_context_physical;
    uint32_t                  power_context_size;
    uint32_t                  power_context_capacity;
}
vg_lite_kernel_context_t;

typedef struct capabilities {
    uint32_t tiled : 2;
    uint32_t l2_cache : 1;
}
capabilities_t;

typedef union vg_lite_capabilities {
    capabilities_t cap;
    uint32_t       data;
}
vg_lite_capabilities_t;

typedef struct vg_lite_kernel_initialize {
    /* INPUT */

    /* Command buffer size. */
    uint32_t command_buffer_size;

    /* Tessellation buffer width. */
    int32_t tess_width;

    /* Tessellation buffer height. */
    int32_t tess_height;

    /* Memory pool for command buffer. */
    vg_lite_vidmem_pool_t command_buffer_pool;

    /* Memory pool for tessellation buffer. */
    vg_lite_vidmem_pool_t tess_buffer_pool;

    /* OUTPUT */

    /* Context pointer. */
    vg_lite_kernel_context_t * context;

    /* Capabilities. */
    vg_lite_capabilities_t capabilities;

    /* Allocated command buffer. */
    void * command_buffer[CMDBUF_COUNT];

    /* GPU address for command buffer. */
    uint32_t command_buffer_gpu[CMDBUF_COUNT];

    /* GPU addresses for tesselation buffers. */
    uint32_t physical_addr;

    /* Logic addresses for tessellation buffers: used by SW Tessellator. */
    uint8_t * logical_addr;

    /* Size of each level of the tesselation buffer. */
    uint32_t tessbuf_size;

    /* Size of each level of the vg count buffer. */
    uint32_t countbuf_size;

    /* Width and height of tessellation buffer. */
    uint32_t tess_w_h;
}
vg_lite_kernel_initialize_t;

typedef struct vg_lite_kernel_terminate {
    /* Context to terminate. */
    vg_lite_kernel_context_t * context;
}
vg_lite_kernel_terminate_t;

typedef struct vg_lite_kernel_allocate {
    /* INPUT */

    /* Number of bytes to allocate. */
    uint32_t bytes;

    /* Flag to indicate whether the allocated memory is contiguous or not. */
    int32_t contiguous;

    /* Flag to indicate where to allocate memory  */
    uint32_t flags;

    /* select reserved memory pool */
    vg_lite_vidmem_pool_t pool;

    /* OUTPUT */

    /* Memory handle. */
    void * memory_handle;

    /* Allocated memory. */
    void * memory;

    /* kernel memory */
    void * kmemory;

    /* GPU address of allocated memory. */
    uint32_t memory_gpu;
}
vg_lite_kernel_allocate_t;

typedef struct vg_lite_kernel_free {
    /* Memory handle to free. */
    void * memory_handle;
}
vg_lite_kernel_free_t;

typedef struct vg_lite_kernel_submit {
    /* Context to submit to. */
    vg_lite_kernel_context_t * context;

    /* Pointer to command buffer. */
    void * commands;

    /* Number of bytes in command buffer. */
    uint32_t command_size;

    /* Command Buffer ID. */
    uint32_t command_id;
}
vg_lite_kernel_submit_t;

typedef enum vg_lite_gpu_reset_type {
    RESTORE_INIT_COMMAND = 0,
    RESTORE_LAST_COMMAND = 1,
    RESTORE_ALL_COMMAND  = 2,
    RESTORE_NONE         = 3,
}
vg_lite_gpu_reset_type_t;

typedef struct vg_lite_kernel_wait {
    /* Context to wait for. */
    vg_lite_kernel_context_t * context;

    /* Timeout in milliseconds. */
    uint32_t timeout_ms;

    /* The event to wait. */
    uint32_t event_mask;

    /* The event(s) got after waiting. */
    uint32_t event_got;

    /* After GPU reset, select submit command */
    vg_lite_gpu_reset_type_t reset_type;
}
vg_lite_kernel_wait_t;

typedef struct vg_lite_kernel_reset {
    /* Context to reset. */
    vg_lite_kernel_context_t * context;
    uint32_t delay_resume_flag;
}
vg_lite_kernel_reset_t;

typedef struct vg_lite_kernel_debug {
    /* Context to debug. */
    vg_lite_kernel_context_t * context;

    /* Bandwidth counter enabler. */
    vg_lite_kernel_counter_t bandwidth_counter;

    /* Pixel counter enabler. */
    vg_lite_kernel_counter_t pixel_counters;

    /* OUTPUT */

    /* Bandwidth counters:
     *  [0] - burst of 8.
     *  [1] - burst of 16.
     *  [2] - burst of 32.
     *  [3] - burst of 64.
     */
    uint32_t bandwidth[4];

    /* Pixel counters:.
     *  [0] - Number of tessellated pixels.
     *  [1] - Number of imaged pixels.
     *  [2] - Number of rendered pixels.
     */
    uint32_t pixels[3];
}
vg_lite_kernel_debug_t;

typedef struct vg_lite_kernel_map {
    /* INPUT */
    uint32_t flags;

    /* user memory */
    /* Number of bytes to map. */
    uint32_t bytes;

    /* Logical memory address or NULL. */
    void * logical;

    /* Physical memory address or 0. */
    uint32_t physical;

    /* dma_buf */
    /* dma_buf fd */
    int32_t dma_buf_fd;

    /* OUTPUT */
    /* Memory handle for mapped memory. */
    void * memory_handle;

    /* GPU address of mapped memory. */
    uint32_t memory_gpu;
}
vg_lite_kernel_map_t;

typedef struct vg_lite_kernel_unmap {
    /* Memory handle to unmap. */
    void * memory_handle;
}
vg_lite_kernel_unmap_t;

typedef struct vg_lite_kernel_cache {
    vg_lite_cache_op_t cache_op;

    /* Memory handle to operation. */
    void * memory_handle;
}
vg_lite_kernel_cache_t;

typedef struct vg_lite_kernel_info {
    /* Register's address. */
    uint32_t addr;

    /* Check register info. */
    uint32_t reg;
}
vg_lite_kernel_info_t;

typedef struct vg_lite_kernel_flexa_info {
    uint32_t                    sbi_mode;
    uint32_t                    sync_mode;
    uint32_t                    flexa_mode;
    uint32_t                    stream_id;
    uint32_t                    segment_address;
    uint32_t                    segment_count;
    uint32_t                    segment_size;
    uint32_t                    stop_flag;
    uint32_t                    start_flag;
    uint32_t                    reset_flag;
}
vg_lite_kernel_flexa_info_t;

typedef struct vg_lite_kernel_mem {
    uint32_t bytes;

    vg_lite_vidmem_pool_t pool;
}
vg_lite_kernel_mem_t;

typedef struct vg_lite_kernel_map_memory {
    /* Number of bytes to map. */
    uint32_t bytes;

    /* Physical memory address. */
    uint32_t physical;

    /* Logical memory address. */
    void * logical;
}
vg_lite_kernel_map_memory_t;

typedef struct vg_lite_kernel_unmap_memory {
    /* Number of bytes to map. */
    uint32_t bytes;

    /* Logical memory address. */
    void * logical;
}
vg_lite_kernel_unmap_memory_t;

typedef struct vg_lite_kernel_close {
    vg_lite_kernel_context_t * context;
}
vg_lite_kernel_close_t;

typedef struct vg_lite_kernel_export_memory {
    int32_t fd;
}
vg_lite_kernel_export_memory_t;

typedef struct vg_lite_kernel_hardware_running_time {
    unsigned long run_time;
    int32_t hertz;
}
vg_lite_kernel_hardware_running_time_t;

typedef struct vg_lite_kernel_delay_resume {
    uint32_t set_delay_resume;
    uint32_t query_delay_resume;
}
vg_lite_kernel_delay_resume_t;

typedef struct vg_lite_kernel_gpu_clock_state {
    uint32_t state;
}
vg_lite_kernel_gpu_clock_state_t;

vg_lite_error_t vg_lite_kernel(vg_lite_kernel_command_t command, void * data);

vg_lite_error_t record_running_time(void);

extern uint32_t init_buffer[12];
extern uint32_t is_init;
extern size_t physical_address;

#ifdef __cplusplus
}
#endif

#endif /* LV_USE_VG_LITE_DRIVER */

#endif /* VG_LITE_KERNEL_H */
