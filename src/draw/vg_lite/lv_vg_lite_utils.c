/**
 * @file vg_lite_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_vg_lite_utils.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_vg_lite_decoder.h"
#include "lv_vg_lite_path.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/

#define ENUM_TO_STRING(e) \
    case (e):             \
    return #e

#define VG_LITE_ENUM_TO_STRING(e) \
    case (VG_LITE_##e):           \
    return #e

#define VLC_OP_ENUM_TO_STRING(e) \
    case (VLC_OP_##e):           \
    return #e

#define FEATURE_ENUM_TO_STRING(e) \
    case (gcFEATURE_BIT_VG_##e):  \
    return #e

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_vg_lite_dump_info(void)
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
                    feature, lv_vg_lite_feature_string((vg_lite_feature_t)feature),
                    ret ? "YES" : "NO");
    }

    vg_lite_uint32_t mem_avail = 0;
    vg_lite_get_mem_size(&mem_avail);
    LV_LOG_USER("Memory Avaliable: %" LV_PRId32 " Bytes", (uint32_t)mem_avail);
}

const char * lv_vg_lite_error_string(vg_lite_error_t error)
{
    switch(error) {
            VG_LITE_ENUM_TO_STRING(SUCCESS);
            VG_LITE_ENUM_TO_STRING(INVALID_ARGUMENT);
            VG_LITE_ENUM_TO_STRING(OUT_OF_MEMORY);
            VG_LITE_ENUM_TO_STRING(NO_CONTEXT);
            VG_LITE_ENUM_TO_STRING(TIMEOUT);
            VG_LITE_ENUM_TO_STRING(OUT_OF_RESOURCES);
            VG_LITE_ENUM_TO_STRING(GENERIC_IO);
            VG_LITE_ENUM_TO_STRING(NOT_SUPPORT);
            VG_LITE_ENUM_TO_STRING(ALREADY_EXISTS);
            VG_LITE_ENUM_TO_STRING(NOT_ALIGNED);
            VG_LITE_ENUM_TO_STRING(FLEXA_TIME_OUT);
            VG_LITE_ENUM_TO_STRING(FLEXA_HANDSHAKE_FAIL);
        default:
            break;
    }
    return "UNKNOW_ERROR";
}

const char * lv_vg_lite_feature_string(vg_lite_feature_t feature)
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
        default:
            break;
    }
    return "UNKNOW_FEATURE";
}

const char * lv_vg_lite_buffer_format_string(vg_lite_buffer_format_t format)
{
    switch(format) {
            VG_LITE_ENUM_TO_STRING(RGBA8888);
            VG_LITE_ENUM_TO_STRING(BGRA8888);
            VG_LITE_ENUM_TO_STRING(RGBX8888);
            VG_LITE_ENUM_TO_STRING(BGRX8888);
            VG_LITE_ENUM_TO_STRING(RGB565);
            VG_LITE_ENUM_TO_STRING(BGR565);
            VG_LITE_ENUM_TO_STRING(RGBA4444);
            VG_LITE_ENUM_TO_STRING(BGRA4444);
            VG_LITE_ENUM_TO_STRING(BGRA5551);
            VG_LITE_ENUM_TO_STRING(A4);
            VG_LITE_ENUM_TO_STRING(A8);
            VG_LITE_ENUM_TO_STRING(L8);
            VG_LITE_ENUM_TO_STRING(YUYV);
            VG_LITE_ENUM_TO_STRING(YUY2);
            VG_LITE_ENUM_TO_STRING(NV12);
            VG_LITE_ENUM_TO_STRING(ANV12);
            VG_LITE_ENUM_TO_STRING(AYUY2);
            VG_LITE_ENUM_TO_STRING(YV12);
            VG_LITE_ENUM_TO_STRING(YV24);
            VG_LITE_ENUM_TO_STRING(YV16);
            VG_LITE_ENUM_TO_STRING(NV16);
            VG_LITE_ENUM_TO_STRING(YUY2_TILED);
            VG_LITE_ENUM_TO_STRING(NV12_TILED);
            VG_LITE_ENUM_TO_STRING(ANV12_TILED);
            VG_LITE_ENUM_TO_STRING(AYUY2_TILED);
            VG_LITE_ENUM_TO_STRING(INDEX_1);
            VG_LITE_ENUM_TO_STRING(INDEX_2);
            VG_LITE_ENUM_TO_STRING(INDEX_4);
            VG_LITE_ENUM_TO_STRING(INDEX_8);
            VG_LITE_ENUM_TO_STRING(RGBA2222);
            VG_LITE_ENUM_TO_STRING(BGRA2222);
            VG_LITE_ENUM_TO_STRING(ABGR2222);
            VG_LITE_ENUM_TO_STRING(ARGB2222);
            VG_LITE_ENUM_TO_STRING(ABGR4444);
            VG_LITE_ENUM_TO_STRING(ARGB4444);
            VG_LITE_ENUM_TO_STRING(ABGR8888);
            VG_LITE_ENUM_TO_STRING(ARGB8888);
            VG_LITE_ENUM_TO_STRING(ABGR1555);
            VG_LITE_ENUM_TO_STRING(RGBA5551);
            VG_LITE_ENUM_TO_STRING(ARGB1555);
            VG_LITE_ENUM_TO_STRING(XBGR8888);
            VG_LITE_ENUM_TO_STRING(XRGB8888);
            VG_LITE_ENUM_TO_STRING(RGBA8888_ETC2_EAC);
            VG_LITE_ENUM_TO_STRING(RGB888);
            VG_LITE_ENUM_TO_STRING(BGR888);
            VG_LITE_ENUM_TO_STRING(ABGR8565);
            VG_LITE_ENUM_TO_STRING(BGRA5658);
            VG_LITE_ENUM_TO_STRING(ARGB8565);
            VG_LITE_ENUM_TO_STRING(RGBA5658);
        default:
            break;
    }
    return "UNKNOW_BUFFER_FORMAT";
}

const char * lv_vg_lite_vlc_op_string(uint8_t vlc_op)
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

    LV_LOG("%s, ", lv_vg_lite_vlc_op_string(op_code));
    for(uint32_t i = 0; i < len; i++) {
        LV_LOG("%0.2f, ", data[i]);
    }
    LV_LOG("\n");
}

void lv_vg_lite_path_dump_info(const vg_lite_path_t * path)
{
    LV_ASSERT(path != NULL);
    LV_ASSERT(path->path != NULL);
    uint8_t fmt_len = lv_vg_lite_path_format_len(path->format);
    size_t len = path->path_length / fmt_len;

    LV_ASSERT(len > 0);

    LV_LOG_USER("address: %p", path->path);
    LV_LOG_USER("length: %d", (int)len);
    LV_LOG_USER("bonding box: (%0.2f, %0.2f) - (%0.2f, %0.2f)",
                path->bounding_box[0], path->bounding_box[1],
                path->bounding_box[2], path->bounding_box[3]);
    LV_LOG_USER("format: %d", (int)path->format);
    LV_LOG_USER("quality: %d", (int)path->quality);

    lv_vg_lite_path_for_each_data(path, path_data_print_cb, NULL);
}

void lv_vg_lite_buffer_dump_info(const vg_lite_buffer_t * buffer)
{
    LV_LOG_USER("memory: %p", (buffer)->memory);
    LV_LOG_USER("address: 0x%08x", (int)(buffer)->address);
    LV_LOG_USER("size: W%d x H%d", (int)((buffer)->width), (int)((buffer)->height));
    LV_LOG_USER("stride: %d", (int)((buffer)->stride));
    LV_LOG_USER("format: %d (%s)",
                (int)((buffer)->format),
                lv_vg_lite_buffer_format_string((buffer)->format));
    LV_LOG_USER("tiled: %d", (int)((buffer)->tiled));
}

void lv_vg_lite_matrix_dump_info(const vg_lite_matrix_t * matrix)
{
    for(int i = 0; i < 3; i++) {
        LV_LOG_USER("| %0.2f, %0.2f, %0.2f |",
                    (matrix)->m[i][0], (matrix)->m[i][1], (matrix)->m[i][2]);
    }
}

bool lv_vg_lite_is_dest_cf_supported(lv_color_format_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_RGB565A8:
        case LV_COLOR_FORMAT_RGB888:
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            return true;
        default:
            break;
    }
    return false;
}

bool lv_vg_lite_is_src_cf_supported(lv_color_format_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_A4:
        case LV_COLOR_FORMAT_A8:
        case LV_COLOR_FORMAT_I8:
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_RGB565A8:
        case LV_COLOR_FORMAT_RGB888:
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
        case LV_COLOR_FORMAT_NV12:
            return true;
        default:
            break;
    }
    return false;
}

vg_lite_buffer_format_t lv_vg_lite_vg_fmt(lv_color_format_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_A4:
            return VG_LITE_A4;

        case LV_COLOR_FORMAT_A8:
            return VG_LITE_A8;

        case LV_COLOR_FORMAT_L8:
            return VG_LITE_L8;

        case LV_COLOR_FORMAT_I1:
            return VG_LITE_INDEX_1;

        case LV_COLOR_FORMAT_I2:
            return VG_LITE_INDEX_2;

        case LV_COLOR_FORMAT_I4:
            return VG_LITE_INDEX_4;

        case LV_COLOR_FORMAT_I8:
            return VG_LITE_INDEX_8;

        case LV_COLOR_FORMAT_RGB565:
            return VG_LITE_BGR565;

        case LV_COLOR_FORMAT_RGB565A8:
            return VG_LITE_BGRA5658;

        case LV_COLOR_FORMAT_RGB888:
            return VG_LITE_BGR888;

        case LV_COLOR_FORMAT_ARGB8888:
            return VG_LITE_BGRA8888;

        case LV_COLOR_FORMAT_XRGB8888:
            return VG_LITE_BGRX8888;

        case LV_COLOR_FORMAT_NV12:
            return VG_LITE_NV12;

        default:
            LV_LOG_ERROR("unsupport color format: %d", cf);
            break;
    }

    LV_ASSERT(false);
    return 0;
}

void lv_vg_lite_buffer_format_bytes(
    vg_lite_buffer_format_t format,
    uint32_t * mul,
    uint32_t * div,
    uint32_t * bytes_align)
{
    /* Get the bpp information of a color format. */
    *mul = *div = 1;
    *bytes_align = 4;
    switch(format) {
        case VG_LITE_L8:
        case VG_LITE_A8:
        case VG_LITE_RGBA8888_ETC2_EAC:
            break;
        case VG_LITE_A4:
            *div = 2;
            break;
        case VG_LITE_ABGR1555:
        case VG_LITE_ARGB1555:
        case VG_LITE_BGRA5551:
        case VG_LITE_RGBA5551:
        case VG_LITE_RGBA4444:
        case VG_LITE_BGRA4444:
        case VG_LITE_ABGR4444:
        case VG_LITE_ARGB4444:
        case VG_LITE_RGB565:
        case VG_LITE_BGR565:
        case VG_LITE_YUYV:
        case VG_LITE_YUY2:
        case VG_LITE_YUY2_TILED:
        /* AYUY2 buffer memory = YUY2 + alpha. */
        case VG_LITE_AYUY2:
        case VG_LITE_AYUY2_TILED:
            *mul = 2;
            break;
        case VG_LITE_RGBA8888:
        case VG_LITE_BGRA8888:
        case VG_LITE_ABGR8888:
        case VG_LITE_ARGB8888:
        case VG_LITE_RGBX8888:
        case VG_LITE_BGRX8888:
        case VG_LITE_XBGR8888:
        case VG_LITE_XRGB8888:
            *mul = 4;
            break;
        case VG_LITE_NV12:
        case VG_LITE_NV12_TILED:
            *mul = 1;
            break;
        case VG_LITE_ANV12:
        case VG_LITE_ANV12_TILED:
            *mul = 4;
            break;
        case VG_LITE_INDEX_1:
            *div = 8;
            *bytes_align = 8;
            break;
        case VG_LITE_INDEX_2:
            *div = 4;
            *bytes_align = 8;
            break;
        case VG_LITE_INDEX_4:
            *div = 2;
            *bytes_align = 8;
            break;
        case VG_LITE_INDEX_8:
            *bytes_align = 1;
            break;
        case VG_LITE_RGBA2222:
        case VG_LITE_BGRA2222:
        case VG_LITE_ABGR2222:
        case VG_LITE_ARGB2222:
            *mul = 1;
            break;
        case VG_LITE_RGB888:
        case VG_LITE_BGR888:
        case VG_LITE_ABGR8565:
        case VG_LITE_BGRA5658:
        case VG_LITE_ARGB8565:
        case VG_LITE_RGBA5658:
            *mul = 3;
            break;
        default:
            LV_LOG_ERROR("unsupport color format: 0x%" PRIx32, (uint32_t)format);
            LV_ASSERT(false);
            break;
    }
}

uint32_t lv_vg_lite_width_to_stride(uint32_t w, vg_lite_buffer_format_t color_format)
{
    w = lv_vg_lite_width_align(w);

    uint32_t mul, div, align;
    lv_vg_lite_buffer_format_bytes(color_format, &mul, &div, &align);
    return LV_VG_LITE_ALIGN((w * mul / div), align);
}

uint32_t lv_vg_lite_width_align(uint32_t w)
{
    if(lv_vg_lite_16px_align()) {
        w = LV_VG_LITE_ALIGN(w, 16);
    }

    return w;
}

void lv_vg_lite_buffer_init(
    vg_lite_buffer_t * buffer,
    const void * ptr,
    int32_t width,
    int32_t height,
    vg_lite_buffer_format_t format,
    bool tiled)
{
    uint32_t mul;
    uint32_t div;
    uint32_t align;
    LV_ASSERT_NULL(buffer);
    LV_ASSERT_NULL(ptr);

    lv_memzero(buffer, sizeof(vg_lite_buffer_t));

    buffer->format = format;
    if(tiled || format == VG_LITE_RGBA8888_ETC2_EAC) {
        buffer->tiled = VG_LITE_TILED;
    }
    else {
        buffer->tiled = VG_LITE_LINEAR;
    }
    buffer->image_mode = VG_LITE_NORMAL_IMAGE_MODE;
    buffer->transparency_mode = VG_LITE_IMAGE_OPAQUE;
    buffer->width = width;
    buffer->height = height;
    lv_vg_lite_buffer_format_bytes(buffer->format, &mul, &div, &align);
    buffer->stride = LV_VG_LITE_ALIGN((buffer->width * mul / div), align);

    if(format == VG_LITE_NV12) {
        lv_yuv_buf_t * frame_p = (lv_yuv_buf_t *)ptr;
        buffer->memory = (void *)frame_p->semi_planar.y.buf;
        buffer->address = (uintptr_t)frame_p->semi_planar.y.buf;
        buffer->yuv.swizzle = VG_LITE_SWIZZLE_UV;
        buffer->yuv.alpha_stride = buffer->stride;
        buffer->yuv.uv_height = buffer->height / 2;
        buffer->yuv.uv_memory = (void *)frame_p->semi_planar.uv.buf;
        buffer->yuv.uv_planar = (uint32_t)(uintptr_t)frame_p->semi_planar.uv.buf;
        buffer->yuv.uv_stride = frame_p->semi_planar.uv.stride;
    }
    else {
        buffer->memory = (void *)ptr;
        buffer->address = (uintptr_t)ptr;
    }
}

void lv_vg_lite_buffer_from_draw_buf(vg_lite_buffer_t * buffer, const lv_draw_buf_t * draw_buf)
{
    LV_ASSERT_NULL(buffer);
    LV_ASSERT_NULL(draw_buf);

    const uint8_t * ptr = draw_buf->data;
    int32_t width = draw_buf->header.w;
    int32_t height = draw_buf->header.h;
    vg_lite_buffer_format_t format = lv_vg_lite_vg_fmt(draw_buf->header.cf);

    if(LV_COLOR_FORMAT_IS_INDEXED(draw_buf->header.cf))
        ptr += LV_COLOR_INDEXED_PALETTE_SIZE(draw_buf->header.cf) * 4;

    width = lv_vg_lite_width_align(width);

    lv_vg_lite_buffer_init(buffer, ptr, width, height, format, false);

    /* Alpha image need to be multiplied by color */
    if(LV_COLOR_FORMAT_IS_ALPHA_ONLY(draw_buf->header.cf)) {
        buffer->image_mode = VG_LITE_MULTIPLY_IMAGE_MODE;
    }
}

void lv_vg_lite_image_matrix(vg_lite_matrix_t * matrix, int32_t x, int32_t y, const lv_draw_image_dsc_t * dsc)
{
    LV_ASSERT_NULL(matrix);
    LV_ASSERT_NULL(dsc);

    int32_t rotation = dsc->rotation;
    int32_t scale_x = dsc->scale_x;
    int32_t scale_y = dsc->scale_y;

    vg_lite_translate(x, y, matrix);

    if(rotation != 0 || scale_x != LV_SCALE_NONE || scale_y != LV_SCALE_NONE) {
        lv_point_t pivot = dsc->pivot;
        vg_lite_translate(pivot.x, pivot.y, matrix);

        if(rotation != 0) {
            vg_lite_rotate(rotation * 0.1f, matrix);
        }

        if(scale_x != LV_SCALE_NONE || scale_y != LV_SCALE_NONE) {
            vg_lite_scale(
                (vg_lite_float_t)scale_x / LV_SCALE_NONE,
                (vg_lite_float_t)scale_y / LV_SCALE_NONE,
                matrix);
        }

        vg_lite_translate(-pivot.x, -pivot.y, matrix);
    }
}

bool lv_vg_lite_buffer_open_image(vg_lite_buffer_t * buffer, lv_image_decoder_dsc_t * decoder_dsc, const void * src,
                                  bool no_cache)
{
    LV_ASSERT_NULL(buffer);
    LV_ASSERT_NULL(decoder_dsc);
    LV_ASSERT_NULL(src);

    lv_image_decoder_args_t args;
    lv_memzero(&args, sizeof(lv_image_decoder_args_t));
    args.premultiply = !lv_vg_lite_support_blend_normal();
    args.stride_align = true;
    args.use_indexed = true;
    args.no_cache = no_cache;

    lv_result_t res = lv_image_decoder_open(decoder_dsc, src, &args);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to open image");
        return false;
    }

    const lv_draw_buf_t * decoded = decoder_dsc->decoded;
    if(decoded == NULL || decoded->data == NULL) {
        lv_image_decoder_close(decoder_dsc);
        LV_LOG_ERROR("image data is NULL");
        return false;
    }

    if(!lv_vg_lite_is_src_cf_supported(decoded->header.cf)) {
        LV_LOG_ERROR("unsupported color format: %d", decoded->header.cf);
        lv_image_decoder_close(decoder_dsc);
        return false;
    }

    if(LV_COLOR_FORMAT_IS_INDEXED(decoded->header.cf)) {
        uint32_t palette_size = LV_COLOR_INDEXED_PALETTE_SIZE(decoded->header.cf);
        LV_VG_LITE_CHECK_ERROR(vg_lite_set_CLUT(palette_size, (vg_lite_uint32_t *)decoded->data));
    }

    lv_vg_lite_buffer_from_draw_buf(buffer, decoded);
    return true;
}

void lv_vg_lite_rect(vg_lite_rectangle_t * rect, const lv_area_t * area)
{
    rect->x = area->x1;
    rect->y = area->y1;
    rect->width = lv_area_get_width(area);
    rect->height = lv_area_get_height(area);
}

uint32_t lv_vg_lite_get_palette_size(vg_lite_buffer_format_t format)
{
    uint32_t size = 0;
    switch(format) {
        case VG_LITE_INDEX_1:
            size = 1 << 1;
            break;
        case VG_LITE_INDEX_2:
            size = 1 << 2;
            break;
        case VG_LITE_INDEX_4:
            size = 1 << 4;
            break;
        case VG_LITE_INDEX_8:
            size = 1 << 8;
            break;
        default:
            break;
    }
    return size;
}

vg_lite_color_t lv_vg_lite_color(lv_color_t color, lv_opa_t opa, bool pre_mul)
{
    if(pre_mul && opa < LV_OPA_MAX) {
        color.red = LV_UDIV255(color.red * opa);
        color.green = LV_UDIV255(color.green * opa);
        color.blue = LV_UDIV255(color.blue * opa);
    }
    return (uint32_t)opa << 24 | (uint32_t)color.blue << 16 | (uint32_t)color.green << 8 | color.red;
}

vg_lite_blend_t lv_vg_lite_blend_mode(lv_blend_mode_t blend_mode)
{
    if(lv_vg_lite_support_blend_normal()) {
        switch(blend_mode) {
            case LV_BLEND_MODE_NORMAL: /**< Simply mix according to the opacity value*/
                return VG_LITE_BLEND_NORMAL_LVGL;

            case LV_BLEND_MODE_ADDITIVE: /**< Add the respective color channels*/
                return VG_LITE_BLEND_ADDITIVE_LVGL;

            case LV_BLEND_MODE_SUBTRACTIVE: /**< Subtract the foreground from the background*/
                return VG_LITE_BLEND_SUBTRACT_LVGL;

            case LV_BLEND_MODE_MULTIPLY: /**< Multiply the foreground and background*/
                return VG_LITE_BLEND_MULTIPLY_LVGL;

            default:
                return VG_LITE_BLEND_NONE;
        }
    }

    switch(blend_mode) {
        case LV_BLEND_MODE_NORMAL: /**< Simply mix according to the opacity value*/
            return VG_LITE_BLEND_SRC_OVER;

        case LV_BLEND_MODE_ADDITIVE: /**< Add the respective color channels*/
            return VG_LITE_BLEND_ADDITIVE;

        case LV_BLEND_MODE_SUBTRACTIVE: /**< Subtract the foreground from the background*/
            return VG_LITE_BLEND_SUBTRACT;

        case LV_BLEND_MODE_MULTIPLY: /**< Multiply the foreground and background*/
            return VG_LITE_BLEND_MULTIPLY;

        default:
            return VG_LITE_BLEND_NONE;
    }
}

bool lv_vg_lite_buffer_check(const vg_lite_buffer_t * buffer, bool is_src)
{
    uint32_t mul;
    uint32_t div;
    uint32_t align;
    int32_t stride;

    if(!buffer) {
        LV_LOG_ERROR("buffer is NULL");
        return false;
    }

    if(buffer->width < 1) {
        LV_LOG_ERROR("buffer width(%d) < 1", (int)buffer->width);
        return false;
    }

    if(buffer->height < 1) {
        LV_LOG_ERROR("buffer height(%d) < 1", (int)buffer->height);
        return false;
    }

    if(buffer->stride < 1) {
        LV_LOG_ERROR("buffer stride(%d) < 1", (int)buffer->stride);
        return false;
    }

    if(!(buffer->tiled == VG_LITE_LINEAR || buffer->tiled == VG_LITE_TILED)) {
        LV_LOG_ERROR("buffer tiled(%d) is invalid", (int)buffer->tiled);
        return false;
    }

    if(buffer->memory == NULL) {
        LV_LOG_ERROR("buffer memory is NULL");
        return false;
    }

    if((uint32_t)(uintptr_t)buffer->memory != buffer->address) {
        LV_LOG_ERROR("buffer memory(%p) != address(%p)",
                     buffer->memory, (void *)(uintptr_t)buffer->address);
        return false;
    }

    if(is_src && buffer->width != (vg_lite_int32_t)lv_vg_lite_width_align(buffer->width)) {
        LV_LOG_ERROR("buffer width(%d) is not aligned", (int)buffer->width);
        return false;
    }

    if(!LV_VG_LITE_IS_ALIGNED(buffer->memory, LV_DRAW_BUF_ALIGN)) {
        LV_LOG_ERROR("buffer address(%p) is not aligned to %d", buffer->memory, LV_DRAW_BUF_ALIGN);
        return false;
    }

    lv_vg_lite_buffer_format_bytes(buffer->format, &mul, &div, &align);
    stride = LV_VG_LITE_ALIGN((buffer->width * mul / div), align);

    if(buffer->stride != stride) {
        LV_LOG_ERROR("buffer stride(%d) != %d", (int)buffer->stride, (int)stride);
        return false;
    }

    if(!(buffer->image_mode == VG_LITE_NORMAL_IMAGE_MODE
         || buffer->image_mode == VG_LITE_NONE_IMAGE_MODE
         || buffer->image_mode == VG_LITE_MULTIPLY_IMAGE_MODE)) {
        LV_LOG_ERROR("buffer image_mode(%d) is invalid", (int)buffer->image_mode);
        return false;
    }

    if(!(buffer->transparency_mode == VG_LITE_IMAGE_OPAQUE
         || buffer->transparency_mode == VG_LITE_IMAGE_TRANSPARENT)) {
        LV_LOG_ERROR("buffer transparency_mode(%d) is invalid",
                     (int)buffer->transparency_mode);
        return false;
    }

    return true;
}

bool lv_vg_lite_path_check(const vg_lite_path_t * path)
{
    if(path == NULL) {
        LV_LOG_ERROR("path is NULL");
        return false;
    }

    if(path->path == NULL) {
        LV_LOG_ERROR("path->path is NULL");
        return false;
    }

    uint8_t fmt_len = lv_vg_lite_path_format_len(path->format);
    if(!fmt_len) {
        LV_LOG_ERROR("path format(%d) is invalid", (int)path->format);
        return false;
    }

    size_t len = path->path_length / fmt_len;

    if(len < 1) {
        LV_LOG_ERROR("path length(%d) error", (int)path->path_length);
        return false;
    }

    const uint8_t * cur = path->path;
    const uint8_t * end = cur + path->path_length;

    while(cur < end) {
        /* get op code */
        uint8_t op_code = VLC_GET_OP_CODE(cur);

        /* get arguments length */
        uint8_t arg_len = lv_vg_lite_vlc_op_arg_len(op_code);

        /* get next op code */
        cur += (fmt_len * (1 + arg_len)) ;

        /* break if end */
        if(op_code == VLC_OP_END) {
            break;
        }
    }

    if(cur != end) {
        LV_LOG_ERROR("path length(%d) error", (int)path->path_length);
        return false;
    }

    uint8_t end_op_code = VLC_GET_OP_CODE(end - fmt_len);
    if(end_op_code != VLC_OP_END) {
        LV_LOG_ERROR("%d (%s) -> is NOT VLC_OP_END", end_op_code, lv_vg_lite_vlc_op_string(end_op_code));
        return false;
    }

    return true;
}

bool lv_vg_lite_support_blend_normal(void)
{
    return vg_lite_query_feature(gcFEATURE_BIT_VG_LVGL_SUPPORT);
}

bool lv_vg_lite_16px_align(void)
{
    return vg_lite_query_feature(gcFEATURE_BIT_VG_16PIXELS_ALIGN);
}

void lv_vg_lite_draw_linear_grad(
    vg_lite_buffer_t * buffer,
    vg_lite_path_t * path,
    const lv_area_t * area,
    const lv_grad_dsc_t * grad,
    const vg_lite_matrix_t * matrix,
    vg_lite_fill_t fill,
    vg_lite_blend_t blend)
{
    LV_ASSERT_NULL(buffer);
    LV_ASSERT_NULL(path);
    LV_ASSERT_NULL(area);
    LV_ASSERT_NULL(grad);

    LV_ASSERT(grad->dir != LV_GRAD_DIR_NONE);

    vg_lite_uint32_t colors[VLC_MAX_GRADIENT_STOPS];
    vg_lite_uint32_t stops[VLC_MAX_GRADIENT_STOPS];

    /* Gradient setup */
    uint8_t cnt = grad->stops_count;
    LV_ASSERT(cnt < VLC_MAX_GRADIENT_STOPS);
    for(uint8_t i = 0; i < cnt; i++) {
        stops[i] = grad->stops[i].frac;
        const lv_color_t * c = &grad->stops[i].color;
        lv_opa_t opa = grad->stops[i].opa;

        /* lvgl color -> gradient color */
        lv_color_t grad_color = lv_color_make(c->blue, c->green, c->red);
        colors[i] = lv_vg_lite_color(grad_color, opa, true);
    }

    vg_lite_linear_gradient_t gradient;
    lv_memzero(&gradient, sizeof(gradient));

    LV_VG_LITE_CHECK_ERROR(vg_lite_init_grad(&gradient));
    LV_VG_LITE_CHECK_ERROR(vg_lite_set_grad(&gradient, cnt, colors, stops));
    LV_VG_LITE_CHECK_ERROR(vg_lite_update_grad(&gradient));

    vg_lite_matrix_t * grad_matrix = vg_lite_get_grad_matrix(&gradient);
    vg_lite_identity(grad_matrix);
    vg_lite_translate(area->x1, area->y1, grad_matrix);

    if(grad->dir == LV_GRAD_DIR_VER) {
        vg_lite_scale(1, lv_area_get_height(area) / 256.0f, grad_matrix);
        vg_lite_rotate(90, grad_matrix);
    }
    else {   /*LV_GRAD_DIR_HOR*/
        vg_lite_scale(lv_area_get_width(area) / 256.0f, 1, grad_matrix);
    }

    LV_VG_LITE_CHECK_ERROR(vg_lite_draw_grad(
                               buffer,
                               path,
                               fill,
                               (vg_lite_matrix_t *)matrix,
                               &gradient,
                               blend));

    LV_VG_LITE_CHECK_ERROR(vg_lite_clear_grad(&gradient));
}

void lv_vg_lite_matrix_multiply(vg_lite_matrix_t * matrix, const vg_lite_matrix_t * mult)
{
    vg_lite_matrix_t temp;
    int row, column;

    /* Process all rows. */
    for(row = 0; row < 3; row++) {
        /* Process all columns. */
        for(column = 0; column < 3; column++) {
            /* Compute matrix entry. */
            temp.m[row][column] = (matrix->m[row][0] * mult->m[0][column])
                                  + (matrix->m[row][1] * mult->m[1][column])
                                  + (matrix->m[row][2] * mult->m[2][column]);
        }
    }

    /* Copy temporary matrix into result. */
    lv_memcpy(matrix, &temp, sizeof(temp));
}

void lv_vg_lite_matrix_flip_y(vg_lite_matrix_t * matrix)
{
    matrix->m[1][1] = -matrix->m[1][1];
}

bool lv_vg_lite_matrix_inverse(vg_lite_matrix_t * result, const vg_lite_matrix_t * matrix)
{
    vg_lite_float_t det00, det01, det02;
    vg_lite_float_t d;
    bool is_affine;

    /* Test for identity matrix. */
    if(matrix == NULL) {
        result->m[0][0] = 1.0f;
        result->m[0][1] = 0.0f;
        result->m[0][2] = 0.0f;
        result->m[1][0] = 0.0f;
        result->m[1][1] = 1.0f;
        result->m[1][2] = 0.0f;
        result->m[2][0] = 0.0f;
        result->m[2][1] = 0.0f;
        result->m[2][2] = 1.0f;

        /* Success. */
        return true;
    }

    det00 = (matrix->m[1][1] * matrix->m[2][2]) - (matrix->m[2][1] * matrix->m[1][2]);
    det01 = (matrix->m[2][0] * matrix->m[1][2]) - (matrix->m[1][0] * matrix->m[2][2]);
    det02 = (matrix->m[1][0] * matrix->m[2][1]) - (matrix->m[2][0] * matrix->m[1][1]);

    /* Compute determinant. */
    d = (matrix->m[0][0] * det00) + (matrix->m[0][1] * det01) + (matrix->m[0][2] * det02);

    /* Return 0 if there is no inverse matrix. */
    if(d == 0.0f)
        return false;

    /* Compute reciprocal. */
    d = 1.0f / d;

    /* Determine if the matrix is affine. */
    is_affine = (matrix->m[2][0] == 0.0f) && (matrix->m[2][1] == 0.0f) && (matrix->m[2][2] == 1.0f);

    result->m[0][0] = d * det00;
    result->m[0][1] = d * ((matrix->m[2][1] * matrix->m[0][2]) - (matrix->m[0][1] * matrix->m[2][2]));
    result->m[0][2] = d * ((matrix->m[0][1] * matrix->m[1][2]) - (matrix->m[1][1] * matrix->m[0][2]));
    result->m[1][0] = d * det01;
    result->m[1][1] = d * ((matrix->m[0][0] * matrix->m[2][2]) - (matrix->m[2][0] * matrix->m[0][2]));
    result->m[1][2] = d * ((matrix->m[1][0] * matrix->m[0][2]) - (matrix->m[0][0] * matrix->m[1][2]));
    result->m[2][0] = is_affine ? 0.0f : d * det02;
    result->m[2][1] = is_affine ? 0.0f : d * ((matrix->m[2][0] * matrix->m[0][1]) - (matrix->m[0][0] * matrix->m[2][1]));
    result->m[2][2] = is_affine ? 1.0f : d * ((matrix->m[0][0] * matrix->m[1][1]) - (matrix->m[1][0] * matrix->m[0][1]));

    /* Success. */
    return true;
}

lv_point_precise_t lv_vg_lite_matrix_transform_point(const vg_lite_matrix_t * matrix, const lv_point_precise_t * point)
{
    lv_point_precise_t p;
    p.x = (lv_value_precise_t)(point->x * matrix->m[0][0] + point->y * matrix->m[0][1] + matrix->m[0][2]);
    p.y = (lv_value_precise_t)(point->x * matrix->m[1][0] + point->y * matrix->m[1][1] + matrix->m[1][2]);
    return p;
}

void lv_vg_lite_set_scissor_area(const lv_area_t * area)
{
    vg_lite_enable_scissor();
    vg_lite_set_scissor(
        area->x1,
        area->y1,
        lv_area_get_width(area),
        lv_area_get_height(area));
}

void lv_vg_lite_disable_scissor(void)
{
    vg_lite_disable_scissor();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VG_LITE*/
