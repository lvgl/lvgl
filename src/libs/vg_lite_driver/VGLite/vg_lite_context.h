/****************************************************************************
*
*    Copyright 2012 - 2023 Vivante Corporation, Santa Clara, California.
*    All Rights Reserved.
*
*    Permission is hereby granted, free of charge, to any person obtaining
*    a copy of this software and associated documentation files (the
*    'Software'), to deal in the Software without restriction, including
*    without limitation the rights to use, copy, modify, merge, publish,
*    distribute, sub license, and/or sell copies of the Software, and to
*    permit persons to whom the Software is furnished to do so, subject
*    to the following conditions:
*
*    The above copyright notice and this permission notice (including the
*    next paragraph) shall be included in all copies or substantial
*    portions of the Software.
*
*    THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
*    IN NO EVENT SHALL VIVANTE AND/OR ITS SUPPLIERS BE LIABLE FOR ANY
*    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
*    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
*    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*****************************************************************************/

#include "../../../lv_conf_internal.h"
#if LV_USE_VG_LITE_DRIVER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include "../inc/vg_lite.h"
#include "../VGLiteKernel/vg_lite_kernel.h"
#include "../VGLiteKernel/vg_lite_option.h"

#define DUMP_CAPTURE                            0
#define DUMP_COMMAND_CAPTURE                    0
#define DUMP_INIT_COMMAND                       0
#define DUMP_API                                0
#define DUMP_LAST_CAPTURE                       0

#if DUMP_API
    #include "dumpAPI.h"
#endif

#define VGLITE_LOG    printf

/*** Global Context Access ***/
#define GET_CONTEXT()               &s_context

/*** Default command buffer size is 32KB. Double command buffer is used.
     App can call vg_lite_set_command_buffer_size(size) before vg_lite_init()
     to overwrite the default command buffer size.
***/
#define VG_LITE_COMMAND_BUFFER_SIZE (32 << 10)
#define VG_LITE_SINGLE_COMMAND_BUFFER_SIZE (64 << 10) /* For only using one command buffer. */

#define CMDBUF_BUFFER(context)      (context).command_buffer[(context).command_buffer_current]
#define CMDBUF_INDEX(context)       (context).command_buffer_current
#define CMDBUF_SIZE(context)        (context).command_buffer_size
#define CMDBUF_OFFSET(context)      (context).command_offset[(context).command_buffer_current]
#define CMDBUF_SWAP(context)        (context).command_buffer_current = \
                                                                       ((context).command_buffer_current + 1) % CMDBUF_COUNT

/*** Command macros ***/
#define VG_LITE_END(interrupt)      (0x00000000 | interrupt)
#define VG_LITE_SEMAPHORE(id)       (0x10000000 | id)
#define VG_LITE_STALL(id)           (0x20000000 | id)
#define VG_LITE_STATE(address)      (0x30010000 | address)
#define VG_LITE_STATES(count, address)  (0x30000000 | ((count) << 16) | address)
#define VG_LITE_DATA(count)         (0x40000000 | count)
#define VG_LITE_CALL(count)         (0x60000000 | count)
#define VG_LITE_RETURN()            (0x70000000)
#define VG_LITE_NOP()               (0x80000000)

#define FC_BURST_BYTES              64
#define FC_BIT_TO_BYTES             64

#define STATES_COUNT                208
#define MIN_TS_SIZE                 8 << 10

#define VG_LITE_RETURN_ERROR(func) \
    if ((error = func) != VG_LITE_SUCCESS) \
        return error

#define VG_LITE_BREAK_ERROR(func) \
    if ((error = func) != VG_LITE_SUCCESS) \
        break

#define VG_LITE_ERROR_HANDLER(func) \
    if ((error = func) != VG_LITE_SUCCESS) \
        goto ErrorHandler

/*** Shortcuts. ***/
#define A(color)                    (color) >> 24
#define R(color)                    ((color) & 0x00ff0000) >> 16
#define G(color)                    ((color) & 0x0000ff00) >> 8
#define B(color)                    ((color) & 0xff)
#define ARGB(a, r, g, b)            ((a) << 24) | ((r) << 16) | ((g) << 8 ) | (b)
#define ARGB4(a, r, g, b)           (((a) & 0xf0) << 8) | (((r) & 0xf0) << 4) | (((g) & 0xf0)) | ((b) >> 4)

#define MIN(a, b)                   (a) > (b) ? (b) : (a)
#define MAX(a, b)                   (a) > (b) ? (a) : (b)

#define LERP(v1, v2, w)             ((v1) * (w) + (v2) * (1.0f - (w)))
#define CLAMP(x, min, max)          (((x) < (min)) ? (min) : ((x) > (max)) ? (max) : (x))

#define COLOR_FROM_RAMP(ColorRamp)  (((vg_lite_float_t *) ColorRamp) + 1)

#define MATRIX_ROWS                 3
#define GET_MATRIX_VALUES(Pointer)  ((float *) (Pointer))
#define MAT(Matrix, Row, Column)    (GET_MATRIX_VALUES(Matrix)[Row * MATRIX_ROWS + Column])
#define PI                          3.141592653589793238462643383279502f

#if !gcFEATURE_VG_MATH_PRECISION_FIX && (CHIPID == 0x555)
    #define VG_SW_BLIT_PRECISION_OPT 1
#else
    #define VG_SW_BLIT_PRECISION_OPT 0
#endif

/* Driver implementation internal structures.
*/
typedef struct vg_lite_states {
    uint32_t                    state;
    uint8_t                     init;
} vg_lite_states_t;

typedef struct vg_lite_hardware {
    vg_lite_states_t            hw_states[STATES_COUNT];
} vg_lite_hardware_t;

/* Tessellation buffer information. */
typedef struct vg_lite_tess_buffer {
    vg_lite_uint32_t            physical_addr;         /*! Physical address for tessellation buffer. */
    vg_lite_uint8_t      *      logical_addr;          /*! Logical address for tessellation buffer. */
    vg_lite_uint32_t            tessbuf_size;          /*! Buffer size for tessellation buffer */
    vg_lite_uint32_t            countbuf_size;         /*! Buffer size for VG count buffer */
    vg_lite_uint32_t            tess_w_h;              /*! Combination of buffer width and height. */
    /* gc355 Specific fields below */
    vg_lite_uint32_t            L1_phyaddr;            /*! L1 physical address. */
    vg_lite_uint32_t            L2_phyaddr;            /*! L2 physical address. */
    vg_lite_uint8_t      *      L1_logical;            /*! L1 Logical address. */
    vg_lite_uint8_t      *      L2_logical;            /*! L2 Logical address. */
    vg_lite_uint32_t            L1_size;               /*! L1 size for tessellation buffer */
    vg_lite_uint32_t            L2_size;               /*! L2 size for tessellation buffer */
    vg_lite_uint32_t            tess_stride;           /*! Stride for tessellation buffer */
} vg_lite_tess_buffer_t;

typedef struct vg_lite_context {
    vg_lite_kernel_context_t    context;
    vg_lite_hardware_t          hw;
    vg_lite_capabilities_t      capabilities;
    uint8_t          *          command_buffer[CMDBUF_COUNT];
    uint32_t                    command_buffer_size;
    uint32_t                    command_offset[CMDBUF_COUNT];
    uint32_t                    command_buffer_current;
    vg_lite_memory_pool_t       command_buffer_pool;

    vg_lite_tess_buffer_t       tessbuf;
    vg_lite_memory_pool_t       tess_buffer_pool;

    vg_lite_buffer_t      *     rtbuffer;                   /* DDRLess: this is used as composing buffer. */
    vg_lite_memory_pool_t       render_buffer_pool;

    vg_lite_float_t             path_lastX;
    vg_lite_float_t             path_lastY;
    uint32_t                    scissor_set;
    uint32_t                    scissor_enable;
    uint32_t                    scissor_dirty;
    int32_t                     scissor[4];                 /* Scissor area: x, y, right, bottom. */
    vg_lite_buffer_t      *      scissor_layer;

    uint32_t                    src_alpha_mode;
    uint32_t                    src_alpha_value;
    uint32_t                    dst_alpha_mode;
    uint32_t                    dst_alpha_value;
    vg_lite_blend_t             blend_mode;

    uint32_t                    sbi_mode;
    uint32_t                    sync_mode;
    uint32_t                    flexa_mode;
    uint32_t                    stream_id;
    uint32_t                    segment_address;
    uint32_t                    segment_count;
    uint32_t                    segment_size;
    uint32_t                    stop_flag;
    uint8_t                     flexa_dirty;
    uint32_t                    start_flag;
    uint32_t                    reset_flag;
    uint8_t                     custom_cmdbuf;
    uint8_t                     custom_tessbuf;
    uint32_t                    enable_mask;
    uint32_t                    matrix_enable;
    uint32_t                    tess_width;
    uint32_t                    tess_height;
    uint32_t                    target_width;
    uint32_t                    target_height;
    uint8_t                     enable_scissor;
    uint32_t                    mirror_orient;
    uint32_t                    mirror_dirty;
    uint32_t                    gamma_value;
    uint32_t                    gamma_dirty;
    uint32_t                    gamma_src;
    uint32_t                    gamma_dst;
    uint32_t                    gamma_stencil;
    uint32_t                    color_transform;
    uint32_t                    path_counter;
    vg_lite_filter_t            filter;
    void            *           last_command_buffer_logical;
    size_t                      Physical;
    uint32_t                    last_command_size;
    vg_lite_frame_flag_t        frame_flag;

} vg_lite_context_t;

typedef struct vg_lite_ftable {
    uint32_t                    ftable[gcFEATURE_COUNT];
} vg_lite_ftable_t;

extern vg_lite_context_t        s_context;
extern vg_lite_ftable_t         s_ftable;

extern vg_lite_error_t set_render_target(vg_lite_buffer_t * target);
extern vg_lite_error_t push_state(vg_lite_context_t * context, uint32_t address, uint32_t data);
extern vg_lite_error_t push_state_ptr(vg_lite_context_t * context, uint32_t address, void * data_ptr);
extern vg_lite_error_t push_call(vg_lite_context_t * context, uint32_t address, uint32_t bytes);
extern vg_lite_error_t push_data(vg_lite_context_t * context, uint32_t size, void * data);
extern vg_lite_error_t push_clut(vg_lite_context_t * context, uint32_t address, uint32_t count, uint32_t * data);
extern vg_lite_error_t push_stall(vg_lite_context_t * context, uint32_t module);

extern void * vg_lite_os_malloc(size_t size);
extern void  vg_lite_os_free(void * memory);

extern vg_lite_void set_gamma_dest_only(vg_lite_buffer_t * target, vg_lite_int32_t stencil);
extern vg_lite_void save_st_gamma_src_dest(vg_lite_buffer_t * source, vg_lite_buffer_t * target);
extern vg_lite_void get_st_gamma_src_dest(vg_lite_buffer_t * source, vg_lite_buffer_t * target);

extern vg_lite_void setup_lvgl_image(vg_lite_buffer_t * dst, vg_lite_buffer_t * src, vg_lite_buffer_t * temp,
                                     vg_lite_blend_t operation);

#if defined(__ZEPHYR__)
    extern void * vg_lite_os_fopen(const char * __restrict path, const char * __restrict mode);
    extern int vg_lite_os_fclose(void * fp);
    extern size_t vg_lite_os_fread(void * __restrict ptr, size_t size, size_t nmemb, void * __restrict fp);
    extern size_t vg_lite_os_fwrite(const void * __restrict ptr, size_t size, size_t nmemb, void * fp);
    extern int vg_lite_os_fseek(void * fp, long offset, int whence);
    extern int vg_lite_os_fflush(void * fp);
    extern int vg_lite_os_fprintf(void * __restrict fp, const char * __restrict format, ...);
    extern int vg_lite_os_getpid(void);
#else
    extern int   vg_lite_os_fseek(FILE * Stream, long Offset, int Origin);
    extern FILE * vg_lite_os_fopen(char const * FileName, char const * Mode);
    extern long  vg_lite_os_ftell(FILE * Stream);
    extern size_t vg_lite_os_fread(void * Buffer, size_t ElementSize, size_t ElementCount, FILE * Stream);
    extern size_t vg_lite_os_fwrite(void const * Buffer, size_t ElementSize, size_t ElementCount, FILE * Stream);
    extern int    vg_lite_os_close(FILE * Stream);
    extern int    vg_lite_os_fflush(FILE * fp);
#endif

/**************************** Dump command, image ********************************************/

#define DUMP_COMMAND                            0
#define DUMP_IMAGE                              0

/* Enable FC buffer dump if SOC supports fast clear */
#define VG_TARGET_FC_DUMP                       0

#if DUMP_COMMAND || DUMP_IMAGE
    #ifdef __linux__
        #include <unistd.h>
    #endif
    FILE * fp;
    char filename[30];
#endif

/**************************** Dump Capture ****************************************************/

#ifndef vgliteDUMP_PATH
    #define vgliteDUMP_PATH                      "./"
#endif

#ifndef vgliteDUMP_KEY
    #define vgliteDUMP_KEY                       "process"
#endif

#if DUMP_LAST_CAPTURE
    void _SetDumpFileInfo();
    vg_lite_error_t vglitefDumpBuffer_single(char * Tag, size_t Physical, void * Logical, size_t Offset, size_t Bytes);
    #define vglitemDUMP_single                             vglitefDump
    #define vglitemDUMP_BUFFER_single                     vglitefDumpBuffer_single
#endif
#if DUMP_CAPTURE
void _SetDumpFileInfo();
vg_lite_error_t vglitefDump(char * String, ...);
vg_lite_error_t vglitefDumpBuffer(char * Tag, size_t Physical, void * Logical, size_t Offset, size_t Bytes);
#define vglitemDUMP                             vglitefDump
#define vglitemDUMP_BUFFER                      vglitefDumpBuffer
#else
static inline void __dummy_dump(char * Message, ...) {}
static inline void __dummy_dump_buffer(char * Tag, size_t Physical, void * Logical, size_t Offset, size_t Bytes) {}
#define vglitemDUMP                             __dummy_dump
#define vglitemDUMP_BUFFER                      __dummy_dump_buffer
#endif

/**********************************************************************************************/

#endif /* LV_USE_VG_LITE_DRIVER */

