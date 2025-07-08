/**
 * @file lv_vglite_utils.c
 *
 */

/**
 * Copyright 2022-2024 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_vglite_utils.h"
#include "lv_vglite_path.h"


#if LV_USE_DRAW_VGLITE
#include "lv_vglite_buf.h"

#include "../../core/lv_refr.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

#if LV_USE_VGLITE_DRAW_ASYNC
    static volatile bool _cmd_buf_flushed = false;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
#if LV_USE_VGLITE_DEBUG

const char * vglite_error_to_string(vg_lite_error_t error)
{
    switch(error) {
            ENUM_TO_STRING(VG_LITE_SUCCESS);
            ENUM_TO_STRING(VG_LITE_INVALID_ARGUMENT);
            ENUM_TO_STRING(VG_LITE_OUT_OF_MEMORY);
            ENUM_TO_STRING(VG_LITE_NO_CONTEXT);
            ENUM_TO_STRING(VG_LITE_TIMEOUT);
            ENUM_TO_STRING(VG_LITE_OUT_OF_RESOURCES);
            ENUM_TO_STRING(VG_LITE_GENERIC_IO);
            ENUM_TO_STRING(VG_LITE_NOT_SUPPORT);
            ENUM_TO_STRING(VG_LITE_ALREADY_EXISTS);
            ENUM_TO_STRING(VG_LITE_NOT_ALIGNED);
            ENUM_TO_STRING(VG_LITE_FLEXA_TIME_OUT);
            ENUM_TO_STRING(VG_LITE_FLEXA_HANDSHAKE_FAIL);
        default:
            break;
    }

    return "VG_LITE_UNKNOWN_ERROR";
}

void vglite_dump_info(void)
{
    char name[64];
    vg_lite_uint32_t chip_id;
    vg_lite_uint32_t chip_rev;
    vg_lite_uint32_t cid;
    vg_lite_get_product_info(name, &chip_id, &chip_rev);
    vg_lite_get_register(0x30, &cid);
    LV_LOG_USER("Product Info: %s"
                " | Chip ID: 0x%" LV_PRIx32
                " | Revision: 0x%" LV_PRIx32
                " | CID: 0x%" LV_PRIx32,
                name, (uint32_t)chip_id, (uint32_t)chip_rev, (uint32_t)cid);

    vg_lite_info_t info;
    vg_lite_get_info(&info);
    LV_LOG_USER("VGLite API version: 0x%" LV_PRIx32, (uint32_t)info.api_version);
    LV_LOG_USER("VGLite API header version: 0x%" LV_PRIx32, (uint32_t)info.header_version);
    LV_LOG_USER("VGLite release version: 0x%" LV_PRIx32, (uint32_t)info.release_version);

    for(int feature = 0; feature < gcFEATURE_COUNT; feature++) {
        vg_lite_uint32_t ret = vg_lite_query_feature((vg_lite_feature_t)feature);
        LV_UNUSED(ret);
        LV_LOG_USER("Feature-%d: %s\t - %s",
                    feature, vglite_feature_string((vg_lite_feature_t)feature),
                    ret ? "YES" : "NO");
    }

    vg_lite_uint32_t mem_avail = 0;
    vg_lite_get_mem_size(&mem_avail);
    LV_LOG_USER("Memory Available: %" LV_PRId32 " Bytes", (uint32_t)mem_avail);
}

const char * vglite_feature_string(vg_lite_feature_t feature)
{
    switch(feature) {
            FEATURE_ENUM_TO_STRING(IM_INDEX_FORMAT);
            FEATURE_ENUM_TO_STRING(SCISSOR);
            FEATURE_ENUM_TO_STRING(BORDER_CULLING);
            FEATURE_ENUM_TO_STRING(RGBA2_FORMAT);
            FEATURE_ENUM_TO_STRING(QUALITY_8X);
            FEATURE_ENUM_TO_STRING(IM_FASTCLAER);
            FEATURE_ENUM_TO_STRING(RADIAL_GRADIENT);
            FEATURE_ENUM_TO_STRING(GLOBAL_ALPHA);
            FEATURE_ENUM_TO_STRING(RGBA8_ETC2_EAC);
            FEATURE_ENUM_TO_STRING(COLOR_KEY);
            FEATURE_ENUM_TO_STRING(DOUBLE_IMAGE);
            FEATURE_ENUM_TO_STRING(YUV_OUTPUT);
            FEATURE_ENUM_TO_STRING(FLEXA);
            FEATURE_ENUM_TO_STRING(24BIT);
            FEATURE_ENUM_TO_STRING(DITHER);
            FEATURE_ENUM_TO_STRING(USE_DST);
            FEATURE_ENUM_TO_STRING(PE_CLEAR);
            FEATURE_ENUM_TO_STRING(IM_INPUT);
            FEATURE_ENUM_TO_STRING(DEC_COMPRESS);
            FEATURE_ENUM_TO_STRING(LINEAR_GRADIENT_EXT);
            FEATURE_ENUM_TO_STRING(MASK);
            FEATURE_ENUM_TO_STRING(MIRROR);
            FEATURE_ENUM_TO_STRING(GAMMA);
            FEATURE_ENUM_TO_STRING(NEW_BLEND_MODE);
            FEATURE_ENUM_TO_STRING(STENCIL);
            FEATURE_ENUM_TO_STRING(SRC_PREMULTIPLIED); /*! Valid only if FEATURE_ENUM_TO_STRING(HW_PREMULTIPLY is 0   */
            FEATURE_ENUM_TO_STRING(HW_PREMULTIPLY); /*! HW multiplier can accept either premultiplied or not */
            FEATURE_ENUM_TO_STRING(COLOR_TRANSFORMATION);
            FEATURE_ENUM_TO_STRING(LVGL_SUPPORT);
            FEATURE_ENUM_TO_STRING(INDEX_ENDIAN);
            FEATURE_ENUM_TO_STRING(24BIT_PLANAR);
            FEATURE_ENUM_TO_STRING(PIXEL_MATRIX);
            FEATURE_ENUM_TO_STRING(NEW_IMAGE_INDEX);
            FEATURE_ENUM_TO_STRING(PARALLEL_PATHS);
            FEATURE_ENUM_TO_STRING(STRIPE_MODE);
            FEATURE_ENUM_TO_STRING(IM_DEC_INPUT);
            FEATURE_ENUM_TO_STRING(GAUSSIAN_BLUR);
            FEATURE_ENUM_TO_STRING(RECTANGLE_TILED_OUT);
            FEATURE_ENUM_TO_STRING(TESSELLATION_TILED_OUT);
            FEATURE_ENUM_TO_STRING(IM_REPEAT_REFLECT);
            FEATURE_ENUM_TO_STRING(YUY2_INPUT);
            FEATURE_ENUM_TO_STRING(YUV_INPUT);
            FEATURE_ENUM_TO_STRING(YUV_TILED_INPUT);
            FEATURE_ENUM_TO_STRING(AYUV_INPUT);
            FEATURE_ENUM_TO_STRING(16PIXELS_ALIGN);
            FEATURE_ENUM_TO_STRING(DEC_COMPRESS_2_0);
        default:
            break;
    }
    return "UNKNOW_FEATURE";
}

const char * vglite_vlc_op_string(uint8_t vlc_op)
{
    switch(vlc_op) {
            VLC_OP_ENUM_TO_STRING(END);
            VLC_OP_ENUM_TO_STRING(CLOSE);
            VLC_OP_ENUM_TO_STRING(MOVE);
            VLC_OP_ENUM_TO_STRING(MOVE_REL);
            VLC_OP_ENUM_TO_STRING(LINE);
            VLC_OP_ENUM_TO_STRING(LINE_REL);
            VLC_OP_ENUM_TO_STRING(QUAD);
            VLC_OP_ENUM_TO_STRING(QUAD_REL);
            VLC_OP_ENUM_TO_STRING(CUBIC);
            VLC_OP_ENUM_TO_STRING(CUBIC_REL);

            VLC_OP_ENUM_TO_STRING(SCCWARC);
            VLC_OP_ENUM_TO_STRING(SCCWARC_REL);
            VLC_OP_ENUM_TO_STRING(SCWARC);
            VLC_OP_ENUM_TO_STRING(SCWARC_REL);
            VLC_OP_ENUM_TO_STRING(LCCWARC);
            VLC_OP_ENUM_TO_STRING(LCCWARC_REL);
            VLC_OP_ENUM_TO_STRING(LCWARC);
            VLC_OP_ENUM_TO_STRING(LCWARC_REL);
        default:
            break;
    }
    return "UNKNOW_VLC_OP";
}

static void path_data_print_cb(void * user_data, uint8_t op_code, const float * data, uint32_t len)
{
    LV_UNUSED(user_data);

    LV_LOG("%s, ", vglite_vlc_op_string(op_code));
    for(uint32_t i = 0; i < len; i++) {
        LV_LOG("%0.2f, ", data[i]);
    }
    LV_LOG("\n");
}

void vglite_path_dump_info(const vg_lite_path_t * path)
{
    LV_ASSERT(path != NULL);
    LV_ASSERT(path->path != NULL);
    uint8_t fmt_len = lv_vglite_path_format_len(path->format);
    size_t len = path->path_length / fmt_len;

    LV_ASSERT(len > 0);

    LV_LOG_USER("address: %p", (void *)path->path);
    LV_LOG_USER("length: %d", (int)len);
    LV_LOG_USER("bonding box: (%0.2f, %0.2f) - (%0.2f, %0.2f)",
                path->bounding_box[0], path->bounding_box[1],
                path->bounding_box[2], path->bounding_box[3]);
    LV_LOG_USER("format: %d", (int)path->format);
    LV_LOG_USER("quality: %d", (int)path->quality);
    LV_LOG_USER("path_changed: %d", (int)path->path_changed);
    LV_LOG_USER("pdata_internal: %d", (int)path->pdata_internal);
    LV_LOG_USER("type: %d", (int)path->path_type);
    LV_LOG_USER("add_end: %d", (int)path->add_end);

    lv_vglite_path_for_each_data(path, path_data_print_cb, NULL);

    if(path->stroke) {
        LV_LOG_USER("stroke_path: %p", (void *)path->stroke_path);
        LV_LOG_USER("stroke_size: %d", (int)path->stroke_size);
        LV_LOG_USER("stroke_color: 0x%X", (int)path->stroke_color);
        vglite_stroke_dump_info(path->stroke);
    }
}

void vglite_stroke_dump_info(const vg_lite_stroke_t * stroke)
{
    LV_ASSERT(stroke != NULL);
    LV_LOG_USER("stroke: %p", (void *)stroke);

    /* Stroke parameters */
    LV_LOG_USER("cap_style: 0x%X", (int)stroke->cap_style);
    LV_LOG_USER("join_style: 0x%X", (int)stroke->join_style);
    LV_LOG_USER("line_width: %f", stroke->line_width);
    LV_LOG_USER("miter_limit: %f", stroke->miter_limit);

    LV_LOG_USER("dash_pattern: %p", (void *)stroke->dash_pattern);
    LV_LOG_USER("pattern_count: %d", (int)stroke->pattern_count);
    if(stroke->dash_pattern) {
        for(int i = 0; i < (int)stroke->pattern_count; i++) {
            LV_LOG_USER("dash_pattern[%d]: %f", i, stroke->dash_pattern[i]);
        }
    }

    LV_LOG_USER("dash_phase: %f", stroke->dash_phase);
    LV_LOG_USER("dash_length: %f", stroke->dash_length);
    LV_LOG_USER("dash_index: %d", (int)stroke->dash_index);
    LV_LOG_USER("half_width: %f", stroke->half_width);

    /* Total length of stroke dash patterns. */
    LV_LOG_USER("pattern_length: %f", stroke->pattern_length);

    /* For fast checking. */
    LV_LOG_USER("miter_square: %f", stroke->miter_square);

    /* Temp storage of stroke subPath. */
    LV_LOG_USER("path_points: %p", (void *)stroke->path_points);
    LV_LOG_USER("path_end: %p", (void *)stroke->path_end);
    LV_LOG_USER("point_count: %d", (int)stroke->point_count);

    LV_LOG_USER("left_point: %p", (void *)stroke->left_point);
    LV_LOG_USER("right_point: %p", (void *)stroke->right_point);
    LV_LOG_USER("stroke_points: %p", (void *)stroke->stroke_points);
    LV_LOG_USER("stroke_end: %p", (void *)stroke->stroke_end);
    LV_LOG_USER("stroke_count: %d", (int)stroke->stroke_count);

    /* Divide stroke path according to move or move_rel for avoiding implicit closure. */
    LV_LOG_USER("path_list_divide: %p", (void *)stroke->path_list_divide);

    /* pointer to current divided path data. */
    LV_LOG_USER("cur_list: %p", (void *)stroke->cur_list);

    /* Flag that add end_path in driver. */
    LV_LOG_USER("add_end: %d", (int)stroke->add_end);
    LV_LOG_USER("dash_reset: %d", (int)stroke->dash_reset);

    /* Sub path list. */
    LV_LOG_USER("stroke_paths: %p", (void *)stroke->stroke_paths);

    /* Last sub path. */
    LV_LOG_USER("last_stroke: %p", (void *)stroke->last_stroke);

    /* Swing area handling. */
    LV_LOG_USER("swing_handling: %d", (int)stroke->swing_handling);
    LV_LOG_USER("swing_deltax: %f", stroke->swing_deltax);
    LV_LOG_USER("swing_deltay: %f", stroke->swing_deltay);
    LV_LOG_USER("swing_start: %p", (void *)stroke->swing_start);
    LV_LOG_USER("swing_stroke: %p", (void *)stroke->swing_stroke);
    LV_LOG_USER("swing_length: %f", stroke->swing_length);
    LV_LOG_USER("swing_centlen: %f", stroke->swing_centlen);
    LV_LOG_USER("swing_count: %d", (int)stroke->swing_count);
    LV_LOG_USER("need_swing: %d", (int)stroke->need_swing);
    LV_LOG_USER("swing_ccw: %d", (int)stroke->swing_ccw);

    LV_LOG_USER("stroke_length: %f", stroke->stroke_length);
    LV_LOG_USER("stroke_size: %d", (int)stroke->stroke_size);

    LV_LOG_USER("fattened: %d", (int)stroke->fattened);
    LV_LOG_USER("closed: %d", (int)stroke->closed);
}

void vglite_buffer_dump_info(const vg_lite_buffer_t * buffer)
{
    LV_LOG_USER("memory: %p", (buffer)->memory);
    LV_LOG_USER("address: 0x%08x", (int)(buffer)->address);
    LV_LOG_USER("size: W%d x H%d", (int)((buffer)->width), (int)((buffer)->height));
    LV_LOG_USER("stride: %d", (int)((buffer)->stride));
    LV_LOG_USER("tiled: %d", (int)((buffer)->tiled));
}

void vglite_matrix_dump_info(const vg_lite_matrix_t * matrix)
{
    for(int i = 0; i < 3; i++) {
        LV_LOG_USER("| %f, %f, %f |",
                    (matrix)->m[i][0], (matrix)->m[i][1], (matrix)->m[i][2]);
    }
}
#endif

#if LV_USE_VGLITE_DRAW_ASYNC
bool vglite_cmd_buf_is_flushed(void)
{
    return _cmd_buf_flushed;
}
#endif

void vglite_run(void)
{
#if LV_USE_VGLITE_DRAW_ASYNC
    vg_lite_uint32_t gpu_idle = 0;

    VGLITE_CHECK_ERROR(vg_lite_get_parameter(VG_LITE_GPU_IDLE_STATE, 1, (vg_lite_pointer)&gpu_idle));

    if(!gpu_idle) {
        _cmd_buf_flushed = false;

        return;
    }
#endif

    /*
     * If LV_USE_VGLITE_DRAW_ASYNC is enabled, simply flush the command buffer and the
     * vglite draw thread will signal asynchronous the dispatcher for completed tasks.
     * Without draw async, process the tasks and signal them as complete one by one.
     */
#if LV_USE_VGLITE_DRAW_ASYNC
    VGLITE_CHECK_ERROR(vg_lite_flush());
    _cmd_buf_flushed = true;
#else
    VGLITE_CHECK_ERROR(vg_lite_finish());
#endif
}

#if LV_USE_VGLITE_DRAW_ASYNC
void vglite_wait_for_finish(void)
{
    VGLITE_CHECK_ERROR(vg_lite_finish());
}
#endif

vg_lite_color_t vglite_get_color(lv_color32_t lv_col32, bool gradient)
{
    vg_lite_color_t vg_col32;

    /* Pre-multiply alpha */
    lv_col32.red = LV_UDIV255(lv_col32.red * lv_col32.alpha);
    lv_col32.green = LV_UDIV255(lv_col32.green * lv_col32.alpha);
    lv_col32.blue = LV_UDIV255(lv_col32.blue * lv_col32.alpha);

    if(!gradient)
        /* The color is in ABGR8888 format with red channel in the lower 8 bits. */
        vg_col32 = ((vg_lite_color_t)lv_col32.alpha << 24) | ((vg_lite_color_t)lv_col32.blue << 16) |
                   ((vg_lite_color_t)lv_col32.green << 8) | (vg_lite_color_t)lv_col32.red;
    else
        /* The gradient color is in ARGB8888 format with blue channel in the lower 8 bits. */
        vg_col32 = ((vg_lite_color_t)lv_col32.alpha << 24) | ((vg_lite_color_t)lv_col32.red << 16) |
                   ((vg_lite_color_t)lv_col32.green << 8) | (vg_lite_color_t)lv_col32.blue;

    return vg_col32;
}

vg_lite_blend_t vglite_get_blend_mode(lv_blend_mode_t lv_blend_mode)
{
    vg_lite_blend_t vg_blend_mode = VG_LITE_BLEND_NONE;

    if(vg_lite_query_feature(gcFEATURE_BIT_VG_LVGL_SUPPORT)) {
        switch(lv_blend_mode) {
            case LV_BLEND_MODE_NORMAL:
                vg_blend_mode = VG_LITE_BLEND_NORMAL_LVGL;
                break;
            case LV_BLEND_MODE_ADDITIVE:
                vg_blend_mode = VG_LITE_BLEND_ADDITIVE_LVGL;
                break;
            case LV_BLEND_MODE_SUBTRACTIVE:
                vg_blend_mode = VG_LITE_BLEND_SUBTRACT_LVGL;
                break;
            case LV_BLEND_MODE_MULTIPLY:
                vg_blend_mode = VG_LITE_BLEND_MULTIPLY_LVGL;
                break;
            default:
                VGLITE_ASSERT_MSG(false, "Unsupported blend mode.");
                break;
        }
    }
    else {
        switch(lv_blend_mode) {
            case LV_BLEND_MODE_NORMAL:
                vg_blend_mode = VG_LITE_BLEND_SRC_OVER;
                break;
            case LV_BLEND_MODE_ADDITIVE:
                vg_blend_mode = VG_LITE_BLEND_ADDITIVE;
                break;
            case LV_BLEND_MODE_SUBTRACTIVE:
                vg_blend_mode = VG_LITE_BLEND_SUBTRACT;
                break;
            case LV_BLEND_MODE_MULTIPLY:
                vg_blend_mode = VG_LITE_BLEND_MULTIPLY;
                break;
            default:
                VGLITE_ASSERT_MSG(false, "Unsupported blend mode.");
                break;
        }
    }

    return vg_blend_mode;
}

vg_lite_buffer_format_t vglite_get_buf_format(lv_color_format_t cf)
{
    vg_lite_buffer_format_t vg_buffer_format = VG_LITE_BGR565;

    switch(cf) {
        case LV_COLOR_FORMAT_L8:
            vg_buffer_format = VG_LITE_L8;
            break;
        case LV_COLOR_FORMAT_A4:
            vg_buffer_format = VG_LITE_A4;
            break;
        case LV_COLOR_FORMAT_A8:
            vg_buffer_format = VG_LITE_A8;
            break;
        case LV_COLOR_FORMAT_I1:
            vg_buffer_format = VG_LITE_INDEX_1;
            break;
        case LV_COLOR_FORMAT_I2:
            vg_buffer_format = VG_LITE_INDEX_2;
            break;
        case LV_COLOR_FORMAT_I4:
            vg_buffer_format = VG_LITE_INDEX_4;
            break;
        case LV_COLOR_FORMAT_I8:
            vg_buffer_format = VG_LITE_INDEX_8;
            break;
        case LV_COLOR_FORMAT_RGB565:
            vg_buffer_format = VG_LITE_BGR565;
            break;
        case LV_COLOR_FORMAT_ARGB2222:
            vg_buffer_format = VG_LITE_BGRA2222;
            break;
        case LV_COLOR_FORMAT_ARGB1555:
            vg_buffer_format =  VG_LITE_BGRA5551;
            break;
        case LV_COLOR_FORMAT_ARGB4444:
            vg_buffer_format =  VG_LITE_BGRA4444;
            break;
        case LV_COLOR_FORMAT_ARGB8565:
            vg_buffer_format = VG_LITE_BGRA5658;
            break;
        case LV_COLOR_FORMAT_RGB888:
            vg_buffer_format = VG_LITE_BGR888;
            break;
        case LV_COLOR_FORMAT_ARGB8888:
            vg_buffer_format = VG_LITE_BGRA8888;
            break;
        case LV_COLOR_FORMAT_XRGB8888:
            vg_buffer_format = VG_LITE_BGRX8888;
            break;

        default:
            VGLITE_ASSERT_MSG(false, "Unsupported color format.");
            break;
    }

    return vg_buffer_format;
}

uint8_t vglite_get_stride_alignment(lv_color_format_t cf)
{
    uint8_t align_bytes = LV_COLOR_DEPTH / 8 * 16; /*16 pixels*/

    switch(cf) {
        case LV_COLOR_FORMAT_I1:
        case LV_COLOR_FORMAT_I2:
        case LV_COLOR_FORMAT_I4:
        case LV_COLOR_FORMAT_A4:
            align_bytes = 8;
            break;
        case LV_COLOR_FORMAT_I8:
        case LV_COLOR_FORMAT_A8:
        case LV_COLOR_FORMAT_L8:
        case LV_COLOR_FORMAT_ARGB2222:
            align_bytes = 16;
            break;
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_ARGB1555:
        case LV_COLOR_FORMAT_ARGB4444:
            align_bytes = 32;
            break;
        case LV_COLOR_FORMAT_ARGB8565:
        case LV_COLOR_FORMAT_RGB888:
            align_bytes = 48;
            break;
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            align_bytes = 64;
            break;

        default:
            VGLITE_ASSERT_MSG(false, "Unsupported buffer format.");
            break;
    }

    return align_bytes;
}

bool vglite_src_buf_aligned(const void * buf, uint32_t stride, lv_color_format_t cf)
{
    /* No alignment requirement for destination buffer when using mode VG_LITE_LINEAR */

    /* Test for pointer alignment */
    if((uintptr_t)buf % LV_ATTRIBUTE_MEM_ALIGN_SIZE)
        return false;

    /* Test for stride alignment */
    if(stride == 0 || stride % vglite_get_stride_alignment(cf))
        return false;

    return true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VGLITE*/
