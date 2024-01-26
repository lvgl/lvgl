/**
 * @file vg_lite_tvg.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"
#if LV_USE_DRAW_VG_LITE && LV_USE_VG_LITE_THORVG

#include "vg_lite.h"
#include "../../lvgl.h"
#include "../../libs/thorvg/thorvg.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <vector>

#if LV_VG_LITE_THORVG_YUV_SUPPORT
    #include <libyuv/convert_argb.h>
#endif

/*********************
 *      DEFINES
 *********************/

#ifndef LV_VG_LITE_THORVG_BUF_ADDR_ALIGN
    #define LV_VG_LITE_THORVG_BUF_ADDR_ALIGN 64
#endif

#define TVG_CANVAS_ENGINE CanvasEngine::Sw
#define TVG_COLOR(COLOR) B(COLOR), G(COLOR), R(COLOR), A(COLOR)
#define TVG_IS_VG_FMT_SUPPORT(fmt) ((fmt) == VG_LITE_BGRA8888 || (fmt) == VG_LITE_BGRX8888)

#define TVG_CHECK_RETURN_VG_ERROR(FUNC)                               \
    do {                                                              \
        Result res = FUNC;                                            \
        if (res != Result::Success) {                                 \
            LV_LOG_ERROR("Executed '" #FUNC "' error: %d", (int)res);                        \
            return vg_lite_error_conv(res);                           \
        }                                                             \
    } while (0)
#define TVG_CHECK_RETURN_RESULT(FUNC)                                 \
    do {                                                              \
        Result res = FUNC;                                            \
        if (res != Result::Success) {                                 \
            LV_LOG_ERROR("Executed '" #FUNC "' error: %d", (int)res);\
            return res;                                               \
        }                                                             \
    } while (0)

/* clang-format off */

#define IS_INDEX_FMT(fmt)           \
    ((fmt) == VG_LITE_INDEX_1       \
     || (fmt) == VG_LITE_INDEX_2 \
     || (fmt) == VG_LITE_INDEX_4 \
     || (fmt) == VG_LITE_INDEX_8)

#define VLC_GET_ARG(CUR, INDEX) vlc_get_arg((cur + (INDEX) * fmt_len), path->format);
#define VLC_GET_OP_CODE(ptr) (*((uint8_t*)ptr))
#define VLC_OP_ARG_LEN(OP, LEN) \
    case VLC_OP_##OP:           \
    return (LEN)

#define A(color) ((color) >> 24)
#define R(color) (((color) & 0x00ff0000) >> 16)
#define G(color) (((color) & 0x0000ff00) >> 8)
#define B(color) ((color) & 0xff)
#define ARGB(a, r, g, b) ((a) << 24) | ((r) << 16) | ((g) << 8) | (b)
#define MIN(a, b) (a) > (b) ? (b) : (a)
#define MAX(a, b) (a) > (b) ? (a) : (b)
#define UDIV255(x) (((x) * 0x8081U) >> 0x17)
#define LERP(v1, v2, w) ((v1) * (w) + (v2) * (1.0f - (w)))
#define CLAMP(x, min, max) (((x) < (min)) ? (min) : ((x) > (max)) ? (max) : (x))
#define COLOR_FROM_RAMP(ColorRamp) (((vg_lite_float_t*)ColorRamp) + 1)

#define VG_LITE_RETURN_ERROR(func)         \
    if ((error = func) != VG_LITE_SUCCESS) \
        return error

#define VG_LITE_ALIGN(number, align_bytes) \
    (((number) + ((align_bytes)-1)) & ~((align_bytes)-1))

#define VG_LITE_IS_ALIGNED(num, align) (((uintptr_t)(num) & ((align)-1)) == 0)

#define VG_LITE_IS_ALPHA_FORMAT(format) \
    ((format) == VG_LITE_A8 || (format) == VG_LITE_A4)

/* clang-format on */

/**********************
 *      TYPEDEFS
 **********************/

using namespace tvg;

#pragma pack(1)
typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} vg_color24_t;

typedef struct {
    uint16_t blue : 5;
    uint16_t green : 6;
    uint16_t red : 5;
} vg_color16_t;

typedef struct {
    vg_color16_t c;
    uint8_t alpha;
} vg_color16_alpha_t;

typedef struct {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    uint8_t alpha;
} vg_color32_t;

#pragma pack()

class vg_lite_ctx
{
    public:
        std::unique_ptr<SwCanvas> canvas;
        void * target_buffer;
        vg_lite_uint32_t target_px_size;
        vg_lite_buffer_format_t target_format;

    public:
        vg_lite_ctx()
            : target_buffer { nullptr }
            , target_px_size { 0 }
            , target_format { VG_LITE_BGRA8888 }
            , clut_2colors { 0 }
            , clut_4colors { 0 }
            , clut_16colors { 0 }
            , clut_256colors { 0 }
        {
            canvas = SwCanvas::gen();
        }

        vg_lite_uint32_t * get_image_buffer(vg_lite_uint32_t w, vg_lite_uint32_t h)
        {
            src_buffer.resize(w * h);
            return src_buffer.data();
        }

        vg_lite_uint32_t * get_temp_target_buffer(vg_lite_uint32_t w, vg_lite_uint32_t h)
        {
            vg_lite_uint32_t px_size = w * h;
            if(px_size > dest_buffer.size()) {
                dest_buffer.resize(w * h);
            }
            return dest_buffer.data();
        }

        vg_lite_uint32_t * get_temp_target_buffer()
        {
            return dest_buffer.data();
        }

        void set_CLUT(vg_lite_uint32_t count, const vg_lite_uint32_t * colors)
        {
            switch(count) {
                case 2:
                    memcpy(clut_2colors, colors, sizeof(clut_2colors));
                    break;
                case 4:
                    memcpy(clut_4colors, colors, sizeof(clut_4colors));
                    break;
                case 16:
                    memcpy(clut_16colors, colors, sizeof(clut_16colors));
                    break;
                case 256:
                    memcpy(clut_256colors, colors, sizeof(clut_256colors));
                    break;
                default:
                    LV_ASSERT(false);
                    break;
            }
        }

        const vg_lite_uint32_t * get_CLUT(vg_lite_buffer_format_t format)
        {
            switch(format) {
                case VG_LITE_INDEX_1:
                    return clut_2colors;

                case VG_LITE_INDEX_2:
                    return clut_2colors;

                case VG_LITE_INDEX_4:
                    return clut_4colors;

                case VG_LITE_INDEX_8:
                    return clut_256colors;

                default:
                    break;
            }

            LV_ASSERT(false);
            return nullptr;
        }

        static vg_lite_ctx * get_instance()
        {
            static vg_lite_ctx instance;
            return &instance;
        }

    private:
        /*  */
        std::vector<vg_lite_uint32_t> src_buffer;
        std::vector<vg_lite_uint32_t> dest_buffer;

        vg_lite_uint32_t clut_2colors[2];
        vg_lite_uint32_t clut_4colors[4];
        vg_lite_uint32_t clut_16colors[16];
        vg_lite_uint32_t clut_256colors[256];
};

template <typename DEST_TYPE, typename SRC_TYPE>
class vg_lite_converter
{
    public:
        typedef void (*converter_cb_t)(DEST_TYPE * dest, const SRC_TYPE * src, vg_lite_uint32_t px_size,
                                       vg_lite_uint32_t color);

    public:
        vg_lite_converter(converter_cb_t converter)
            : _converter_cb(converter)
        {
        }

        void convert(vg_lite_buffer_t * dest_buf, const vg_lite_buffer_t * src_buf, vg_lite_uint32_t color = 0)
        {
            LV_ASSERT(_converter_cb);
            uint8_t * dest = (uint8_t *)dest_buf->memory;
            const uint8_t * src = (const uint8_t *)src_buf->memory;
            vg_lite_uint32_t h = src_buf->height;

            while(h--) {
                _converter_cb((DEST_TYPE *)dest, (const SRC_TYPE *)src, src_buf->width, color);
                dest += dest_buf->stride;
                src += src_buf->stride;
            }
        }

    private:
        converter_cb_t _converter_cb;
};

typedef vg_lite_float_t FLOATVECTOR4[4];

/**********************
 *  STATIC PROTOTYPES
 **********************/

static vg_lite_error_t vg_lite_error_conv(Result result);
static Matrix matrix_conv(const vg_lite_matrix_t * matrix);
static FillRule fill_rule_conv(vg_lite_fill_t fill);
static BlendMethod blend_method_conv(vg_lite_blend_t blend);
static Result shape_append_path(std::unique_ptr<Shape> & shape, vg_lite_path_t * path, vg_lite_matrix_t * matrix);
static Result shape_append_rect(std::unique_ptr<Shape> & shape, const vg_lite_buffer_t * target,
                                const vg_lite_rectangle_t * rect);
static Result canvas_set_target(vg_lite_ctx * ctx, vg_lite_buffer_t * target);
static Result picture_load(vg_lite_ctx * ctx, std::unique_ptr<Picture> & picture, const vg_lite_buffer_t * source,
                           vg_lite_color_t color = 0);

static inline bool math_zero(float a)
{
    return (fabs(a) < FLT_EPSILON);
}

static inline bool math_equal(float a, float b)
{
    return math_zero(a - b);
}

static void ClampColor(FLOATVECTOR4 Source, FLOATVECTOR4 Target, uint8_t Premultiplied);
static uint8_t PackColorComponent(vg_lite_float_t value);
static void get_format_bytes(vg_lite_buffer_format_t format,
                             vg_lite_uint32_t * mul,
                             vg_lite_uint32_t * div,
                             vg_lite_uint32_t * bytes_align);

/**********************
 *  STATIC VARIABLES
 **********************/

/* color converters */

static vg_lite_converter<vg_color16_t, vg_color32_t> conv_bgra8888_to_bgr565(
    [](vg_color16_t * dest, const vg_color32_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t /* color */)
{
    while(px_size--) {
        dest->red = src->red >> 3;
        dest->green = src->green >> 2;
        dest->blue = src->blue >> 3;
        src++;
        dest++;
    }
});

static vg_lite_converter<vg_color16_alpha_t, vg_color32_t> conv_bgra8888_to_bgra5658(
    [](vg_color16_alpha_t * dest, const vg_color32_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t /* color */)
{
    while(px_size--) {
        dest->c.red = src->red >> 3;
        dest->c.green = src->green >> 2;
        dest->c.blue = src->blue >> 3;
        dest->alpha = src->alpha;
        src++;
        dest++;
    }
});

static vg_lite_converter<vg_color32_t, vg_color16_t> conv_bgr565_to_bgra8888(
    [](vg_color32_t * dest, const vg_color16_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t /* color */)
{
    while(px_size--) {
        dest->red = src->red << 3;
        dest->green = src->green << 2;
        dest->blue = src->blue << 3;
        dest->alpha = 0xFF;
        src++;
        dest++;
    }
});

static vg_lite_converter<vg_color32_t, vg_color16_alpha_t> conv_bgra5658_to_bgra8888(
    [](vg_color32_t * dest, const vg_color16_alpha_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t /* color */)
{
    while(px_size--) {
        dest->red = src->c.red << 3;
        dest->green = src->c.green << 2;
        dest->blue = src->c.blue << 3;
        dest->alpha = src->alpha;
        src++;
        dest++;
    }
});

static vg_lite_converter<vg_color32_t, vg_color32_t> conv_bgrx8888_to_bgra8888(
    [](vg_color32_t * dest, const vg_color32_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t /* color */)
{
    while(px_size--) {
        *dest = *src;
        dest->alpha = 0xFF;
        dest++;
        src++;
    }
});

static vg_lite_converter<vg_color32_t, vg_color24_t> conv_bgr888_to_bgra8888(
    [](vg_color32_t * dest, const vg_color24_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t /* color */)
{
    while(px_size--) {
        dest->red = src->red;
        dest->green = src->green;
        dest->blue = src->blue;
        dest->alpha = 0xFF;
        src++;
        dest++;
    }
});

static vg_lite_converter<vg_color32_t, uint8_t> conv_alpha8_to_bgra8888(
    [](vg_color32_t * dest, const uint8_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t color)
{
    while(px_size--) {
        uint8_t alpha = *src;
        dest->alpha = alpha;
        dest->red = UDIV255(B(color) * alpha);
        dest->green = UDIV255(G(color) * alpha);
        dest->blue = UDIV255(R(color) * alpha);
        dest++;
        src++;
    }
});

static vg_lite_converter<vg_color32_t, uint8_t> conv_alpha4_to_bgra8888(
    [](vg_color32_t * dest, const uint8_t * src, vg_lite_uint32_t px_size, vg_lite_uint32_t color)
{
    /* 1 byte -> 2 px */
    px_size /= 2;

    while(px_size--) {
        /* high 4bit */
        uint8_t alpha = (*src & 0xF0);
        dest->alpha = alpha;
        dest->red = UDIV255(B(color) * alpha);
        dest->green = UDIV255(G(color) * alpha);
        dest->blue = UDIV255(R(color) * alpha);
        dest++;

        /* low 4bit */
        alpha = (*src & 0x0F) << 4;
        dest->alpha = alpha;
        dest->red = UDIV255(B(color) * alpha);
        dest->green = UDIV255(G(color) * alpha);
        dest->blue = UDIV255(R(color) * alpha);

        dest++;
        src++;
    }
});

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

extern "C" {

    void gpu_init(void)
    {
        vg_lite_init(0, 0);
    }

    vg_lite_error_t vg_lite_allocate(vg_lite_buffer_t * buffer)
    {
        if(buffer->format == VG_LITE_RGBA8888_ETC2_EAC && (buffer->width % 16 || buffer->height % 4)) {
            return VG_LITE_INVALID_ARGUMENT;
        }

        /* Reset planar. */
        buffer->yuv.uv_planar = buffer->yuv.v_planar = buffer->yuv.alpha_planar = 0;

        /* Align height in case format is tiled. */
        if(buffer->format >= VG_LITE_YUY2 && buffer->format <= VG_LITE_NV16) {
            buffer->height = VG_LITE_ALIGN(buffer->height, 4);
            buffer->yuv.swizzle = VG_LITE_SWIZZLE_UV;
        }

        if(buffer->format >= VG_LITE_YUY2_TILED && buffer->format <= VG_LITE_AYUY2_TILED) {
            buffer->height = VG_LITE_ALIGN(buffer->height, 4);
            buffer->tiled = VG_LITE_TILED;
            buffer->yuv.swizzle = VG_LITE_SWIZZLE_UV;
        }

        vg_lite_uint32_t mul, div, align;
        get_format_bytes(buffer->format, &mul, &div, &align);
        vg_lite_uint32_t stride = VG_LITE_ALIGN((buffer->width * mul / div), align);

        buffer->stride = stride;
        buffer->memory = aligned_alloc(LV_VG_LITE_THORVG_BUF_ADDR_ALIGN, stride * buffer->height);
        LV_ASSERT(buffer->memory);
        buffer->address = (vg_lite_uint32_t)(uintptr_t)buffer->memory;
        buffer->handle = buffer->memory;
        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_free(vg_lite_buffer_t * buffer)
    {
        LV_ASSERT(buffer->memory);
        free(buffer->memory);
        memset(buffer, 0, sizeof(vg_lite_buffer_t));
        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_upload_buffer(vg_lite_buffer_t * buffer, vg_lite_uint8_t * data[3], vg_lite_uint32_t stride[3])
    {
        LV_UNUSED(buffer);
        LV_UNUSED(data);
        LV_UNUSED(stride);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_map(vg_lite_buffer_t * buffer, vg_lite_map_flag_t flag, int32_t fd)
    {
        LV_UNUSED(buffer);
        LV_UNUSED(flag);
        LV_UNUSED(fd);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_unmap(vg_lite_buffer_t * buffer)
    {
        LV_UNUSED(buffer);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_clear(vg_lite_buffer_t * target, vg_lite_rectangle_t * rectangle, vg_lite_color_t color)
    {
        auto ctx = vg_lite_ctx::get_instance();
        TVG_CHECK_RETURN_VG_ERROR(canvas_set_target(ctx, target));

        auto shape = Shape::gen();
        TVG_CHECK_RETURN_VG_ERROR(shape_append_rect(shape, target, rectangle));
        TVG_CHECK_RETURN_VG_ERROR(shape->fill(TVG_COLOR(color)));
        TVG_CHECK_RETURN_VG_ERROR(ctx->canvas->push(std::move(shape)));

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_blit(vg_lite_buffer_t * target,
                                 vg_lite_buffer_t * source,
                                 vg_lite_matrix_t * matrix,
                                 vg_lite_blend_t blend,
                                 vg_lite_color_t color,
                                 vg_lite_filter_t filter)
    {
        LV_UNUSED(filter);
        auto ctx = vg_lite_ctx::get_instance();
        canvas_set_target(ctx, target);

        auto picture = Picture::gen();

        TVG_CHECK_RETURN_VG_ERROR(picture_load(ctx, picture, source, color));
        TVG_CHECK_RETURN_VG_ERROR(picture->transform(matrix_conv(matrix)));
        TVG_CHECK_RETURN_VG_ERROR(picture->blend(blend_method_conv(blend)));
        TVG_CHECK_RETURN_VG_ERROR(ctx->canvas->push(std::move(picture)));

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_blit2(vg_lite_buffer_t * target,
                                  vg_lite_buffer_t * source0,
                                  vg_lite_buffer_t * source1,
                                  vg_lite_matrix_t * matrix0,
                                  vg_lite_matrix_t * matrix1,
                                  vg_lite_blend_t blend,
                                  vg_lite_filter_t filter)
    {
        if(!vg_lite_query_feature(gcFEATURE_BIT_VG_DOUBLE_IMAGE)) {
            return VG_LITE_NOT_SUPPORT;
        }

        vg_lite_error_t error;

        VG_LITE_RETURN_ERROR(vg_lite_blit(target, source0, matrix0, blend, 0, filter));
        VG_LITE_RETURN_ERROR(vg_lite_blit(target, source1, matrix1, blend, 0, filter));

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_blit_rect(vg_lite_buffer_t * target,
                                      vg_lite_buffer_t * source,
                                      vg_lite_rectangle_t * rect,
                                      vg_lite_matrix_t * matrix,
                                      vg_lite_blend_t blend,
                                      vg_lite_color_t color,
                                      vg_lite_filter_t filter)
    {
        LV_UNUSED(filter);
        auto ctx = vg_lite_ctx::get_instance();
        TVG_CHECK_RETURN_VG_ERROR(canvas_set_target(ctx, target));

        auto shape = Shape::gen();
        TVG_CHECK_RETURN_VG_ERROR(shape_append_rect(shape, target, rect));
        TVG_CHECK_RETURN_VG_ERROR(shape->transform(matrix_conv(matrix)));

        auto picture = tvg::Picture::gen();
        TVG_CHECK_RETURN_VG_ERROR(picture_load(ctx, picture, source, color));
        TVG_CHECK_RETURN_VG_ERROR(picture->transform(matrix_conv(matrix)));
        TVG_CHECK_RETURN_VG_ERROR(picture->blend(blend_method_conv(blend)));
        TVG_CHECK_RETURN_VG_ERROR(picture->composite(std::move(shape), CompositeMethod::ClipPath));
        TVG_CHECK_RETURN_VG_ERROR(ctx->canvas->push(std::move(picture)));

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_init(int32_t tessellation_width, int32_t tessellation_height)
    {
        LV_UNUSED(tessellation_width);
        LV_UNUSED(tessellation_height);
#if LV_VG_LITE_THORVG_THREAD_RENDER
        /* Threads Count */
        auto threads = std::thread::hardware_concurrency();
        if(threads > 0) {
            --threads; /* Allow the designated main thread capacity */
        }
#endif

        /* Initialize ThorVG Engine */
        TVG_CHECK_RETURN_VG_ERROR(Initializer::init(TVG_CANVAS_ENGINE, 0));
        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_close(void)
    {
        TVG_CHECK_RETURN_VG_ERROR(Initializer::term(TVG_CANVAS_ENGINE));
        return VG_LITE_SUCCESS;
    }

    static void picture_bgra8888_to_bgr565(vg_color16_t * dest, const vg_color32_t * src, vg_lite_uint32_t px_size)
    {
        while(px_size--) {
            dest->red = src->red >> 3;
            dest->green = src->green >> 2;
            dest->blue = src->blue >> 3;
            src++;
            dest++;
        }
    }

    static void picture_bgra8888_to_bgra5658(vg_color16_alpha_t * dest, const vg_color32_t * src, vg_lite_uint32_t px_size)
    {
        while(px_size--) {
            dest->c.red = src->red >> 3;
            dest->c.green = src->green >> 2;
            dest->c.blue = src->blue >> 3;
            dest->alpha = src->alpha;
            src++;
            dest++;
        }
    }

    static void picture_bgra8888_to_bgr888(vg_color24_t * dest, const vg_color32_t * src, vg_lite_uint32_t px_size)
    {
        while(px_size--) {
            dest->red = src->red;
            dest->green = src->green;
            dest->blue = src->blue;
            src++;
            dest++;
        }
    }

    vg_lite_error_t vg_lite_finish(void)
    {
        vg_lite_ctx * ctx = vg_lite_ctx::get_instance();

        if(ctx->canvas->draw() == Result::InsufficientCondition) {
            return VG_LITE_SUCCESS;
        }

        TVG_CHECK_RETURN_VG_ERROR(ctx->canvas->sync());
        TVG_CHECK_RETURN_VG_ERROR(ctx->canvas->clear(true));

        /* make sure target buffer is valid */
        LV_ASSERT_NULL(ctx->target_buffer);

        /* If target_buffer is not in a format supported by thorvg, software conversion is required. */
        switch(ctx->target_format) {
            case VG_LITE_BGR565:
                picture_bgra8888_to_bgr565(
                    (vg_color16_t *)ctx->target_buffer,
                    (const vg_color32_t *)ctx->get_temp_target_buffer(),
                    ctx->target_px_size);
                break;
            case VG_LITE_BGRA5658:
                picture_bgra8888_to_bgra5658(
                    (vg_color16_alpha_t *)ctx->target_buffer,
                    (const vg_color32_t *)ctx->get_temp_target_buffer(),
                    ctx->target_px_size);
                break;
            case VG_LITE_BGR888:
                picture_bgra8888_to_bgr888(
                    (vg_color24_t *)ctx->target_buffer,
                    (const vg_color32_t *)ctx->get_temp_target_buffer(),
                    ctx->target_px_size);
                break;
            case VG_LITE_BGRA8888:
            case VG_LITE_BGRX8888:
                /* No conversion required. */
                break;
            default:
                LV_LOG_ERROR("unsupported format: %d", ctx->target_format);
                LV_ASSERT(false);
                break;
        }

        /* finish convert, clean target buffer info */
        ctx->target_buffer = nullptr;
        ctx->target_px_size = 0;

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_flush(void)
    {
        return vg_lite_finish();
    }

    vg_lite_error_t vg_lite_draw(vg_lite_buffer_t * target,
                                 vg_lite_path_t * path,
                                 vg_lite_fill_t fill_rule,
                                 vg_lite_matrix_t * matrix,
                                 vg_lite_blend_t blend,
                                 vg_lite_color_t color)
    {
        auto ctx = vg_lite_ctx::get_instance();
        TVG_CHECK_RETURN_VG_ERROR(canvas_set_target(ctx, target));

        auto shape = Shape::gen();
        TVG_CHECK_RETURN_VG_ERROR(shape_append_path(shape, path, matrix));
        TVG_CHECK_RETURN_VG_ERROR(shape->transform(matrix_conv(matrix)));
        TVG_CHECK_RETURN_VG_ERROR(shape->fill(fill_rule_conv(fill_rule)););
        TVG_CHECK_RETURN_VG_ERROR(shape->blend(blend_method_conv(blend)));
        TVG_CHECK_RETURN_VG_ERROR(shape->fill(TVG_COLOR(color)));
        TVG_CHECK_RETURN_VG_ERROR(ctx->canvas->push(std::move(shape)));

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_get_register(vg_lite_uint32_t address, vg_lite_uint32_t * result)
    {
        LV_UNUSED(address);
        LV_UNUSED(result);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_get_info(vg_lite_info_t * info)
    {
        info->api_version = VGLITE_API_VERSION_3_0;
        info->header_version = VGLITE_HEADER_VERSION;
        info->release_version = VGLITE_RELEASE_VERSION;
        info->reserved = 0;
        return VG_LITE_SUCCESS;
    }

    vg_lite_uint32_t vg_lite_get_product_info(char * name, vg_lite_uint32_t * chip_id, vg_lite_uint32_t * chip_rev)
    {
        strcpy(name, "GCNanoLiteV");
        *chip_id = 0x265;
        *chip_rev = 0x2000;
        return 1;
    }

    vg_lite_uint32_t vg_lite_query_feature(vg_lite_feature_t feature)
    {
        switch(feature) {
            case gcFEATURE_BIT_VG_IM_INDEX_FORMAT:
            case gcFEATURE_BIT_VG_SCISSOR:
            case gcFEATURE_BIT_VG_BORDER_CULLING:
            case gcFEATURE_BIT_VG_RGBA2_FORMAT:
            case gcFEATURE_BIT_VG_IM_FASTCLAER:
            case gcFEATURE_BIT_VG_GLOBAL_ALPHA:
            case gcFEATURE_BIT_VG_COLOR_KEY:
            case gcFEATURE_BIT_VG_24BIT:
            case gcFEATURE_BIT_VG_DITHER:
            case gcFEATURE_BIT_VG_USE_DST:

#if LV_VG_LITE_THORVG_LVGL_BLEND_SUPPORT
            case gcFEATURE_BIT_VG_LVGL_SUPPORT:
#endif

#if LV_VG_LITE_THORVG_YUV_SUPPORT
            case gcFEATURE_BIT_VG_YUV_INPUT:
#endif

#if LV_VG_LITE_THORVG_16PIXELS_ALIGN
            case gcFEATURE_BIT_VG_16PIXELS_ALIGN:
#endif
                return 1;
            default:
                break;
        }
        return 0;
    }

    vg_lite_error_t vg_lite_init_path(vg_lite_path_t * path,
                                      vg_lite_format_t data_format,
                                      vg_lite_quality_t quality,
                                      vg_lite_uint32_t path_length,
                                      void * path_data,
                                      vg_lite_float_t min_x, vg_lite_float_t min_y,
                                      vg_lite_float_t max_x, vg_lite_float_t max_y)
    {
        if(!path) {
            return VG_LITE_INVALID_ARGUMENT;
        }

        path->format = data_format;
        path->quality = quality;
        path->bounding_box[0] = min_x;
        path->bounding_box[1] = min_y;
        path->bounding_box[2] = max_x;
        path->bounding_box[3] = max_y;

        path->path_length = path_length;
        path->path = path_data;

        path->path_changed = 1;
        path->uploaded.address = 0;
        path->uploaded.bytes = 0;
        path->uploaded.handle = NULL;
        path->uploaded.memory = NULL;
        path->pdata_internal = 0;

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_init_arc_path(vg_lite_path_t * path,
                                          vg_lite_format_t data_format,
                                          vg_lite_quality_t quality,
                                          vg_lite_uint32_t path_length,
                                          void * path_data,
                                          vg_lite_float_t min_x, vg_lite_float_t min_y,
                                          vg_lite_float_t max_x, vg_lite_float_t max_y)
    {
        LV_UNUSED(path);
        LV_UNUSED(data_format);
        LV_UNUSED(quality);
        LV_UNUSED(path_length);
        LV_UNUSED(path_data);
        LV_UNUSED(min_x);
        LV_UNUSED(min_y);
        LV_UNUSED(max_x);
        LV_UNUSED(max_y);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_clear_path(vg_lite_path_t * path)
    {
        LV_UNUSED(path);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_uint32_t vg_lite_get_path_length(vg_lite_uint8_t * opcode,
                                             vg_lite_uint32_t count,
                                             vg_lite_format_t format)
    {
        LV_UNUSED(opcode);
        LV_UNUSED(count);
        LV_UNUSED(format);
        return 0;
    }

    vg_lite_error_t vg_lite_append_path(vg_lite_path_t * path,
                                        uint8_t * cmd,
                                        void * data,
                                        vg_lite_uint32_t seg_count)
    {
        LV_UNUSED(path);
        LV_UNUSED(cmd);
        LV_UNUSED(data);
        LV_UNUSED(seg_count);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_upload_path(vg_lite_path_t * path)
    {
        LV_UNUSED(path);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_set_CLUT(vg_lite_uint32_t count,
                                     vg_lite_uint32_t * colors)
    {
        if(!vg_lite_query_feature(gcFEATURE_BIT_VG_IM_INDEX_FORMAT)) {
            return VG_LITE_NOT_SUPPORT;
        }
        LV_ASSERT(colors);

        auto ctx = vg_lite_ctx::get_instance();
        ctx->set_CLUT(count, colors);
        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_draw_pattern(vg_lite_buffer_t * target,
                                         vg_lite_path_t * path,
                                         vg_lite_fill_t fill_rule,
                                         vg_lite_matrix_t * path_matrix,
                                         vg_lite_buffer_t * pattern_image,
                                         vg_lite_matrix_t * pattern_matrix,
                                         vg_lite_blend_t blend,
                                         vg_lite_pattern_mode_t pattern_mode,
                                         vg_lite_color_t pattern_color,
                                         vg_lite_color_t color,
                                         vg_lite_filter_t filter)
    {
        LV_UNUSED(pattern_mode);
        LV_UNUSED(pattern_color);
        LV_UNUSED(filter);

        auto ctx = vg_lite_ctx::get_instance();
        TVG_CHECK_RETURN_VG_ERROR(canvas_set_target(ctx, target));

        auto shape = Shape::gen();
        TVG_CHECK_RETURN_VG_ERROR(shape_append_path(shape, path, path_matrix));
        TVG_CHECK_RETURN_VG_ERROR(shape->fill(fill_rule_conv(fill_rule)));
        TVG_CHECK_RETURN_VG_ERROR(shape->transform(matrix_conv(path_matrix)));

        auto picture = tvg::Picture::gen();
        TVG_CHECK_RETURN_VG_ERROR(picture_load(ctx, picture, pattern_image, color));
        TVG_CHECK_RETURN_VG_ERROR(picture->transform(matrix_conv(pattern_matrix)));
        TVG_CHECK_RETURN_VG_ERROR(picture->blend(blend_method_conv(blend)));
        TVG_CHECK_RETURN_VG_ERROR(picture->composite(std::move(shape), CompositeMethod::ClipPath));
        TVG_CHECK_RETURN_VG_ERROR(ctx->canvas->push(std::move(picture)));

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_init_grad(vg_lite_linear_gradient_t * grad)
    {
        vg_lite_error_t error = VG_LITE_SUCCESS;

        /* Set the member values according to driver defaults. */
        grad->image.width = VLC_GRADIENT_BUFFER_WIDTH;
        grad->image.height = 1;
        grad->image.stride = 0;
        grad->image.format = VG_LITE_BGRA8888;

        /* Allocate the image for gradient. */
        error = vg_lite_allocate(&grad->image);

        grad->count = 0;

        return error;
    }

    vg_lite_error_t vg_lite_set_linear_grad(vg_lite_ext_linear_gradient_t * grad,
                                            vg_lite_uint32_t count,
                                            vg_lite_color_ramp_t * color_ramp,
                                            vg_lite_linear_gradient_parameter_t linear_gradient,
                                            vg_lite_gradient_spreadmode_t spread_mode,
                                            vg_lite_uint8_t pre_multiplied)
    {
        static vg_lite_color_ramp_t default_ramp[] = {
            {
                0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            },
            {
                1.0f,
                1.0f, 1.0f, 1.0f, 1.0f
            }
        };

        vg_lite_uint32_t i, trg_count;
        vg_lite_float_t prev_stop;
        vg_lite_color_ramp_t * src_ramp;
        vg_lite_color_ramp_t * src_ramp_last;
        vg_lite_color_ramp_t * trg_ramp;

        /* Reset the count. */
        trg_count = 0;

        if((linear_gradient.X0 == linear_gradient.X1) && (linear_gradient.Y0 == linear_gradient.Y1))
            return VG_LITE_INVALID_ARGUMENT;

        grad->linear_grad = linear_gradient;
        grad->pre_multiplied = pre_multiplied;
        grad->spread_mode = spread_mode;

        if(!count || count > VLC_MAX_COLOR_RAMP_STOPS || color_ramp == NULL)
            goto Empty_sequence_handler;

        for(i = 0; i < count; i++)
            grad->color_ramp[i] = color_ramp[i];
        grad->ramp_length = count;

        /* Determine the last source ramp. */
        src_ramp_last
            = grad->color_ramp
              + grad->ramp_length;

        /* Set the initial previous stop. */
        prev_stop = -1;

        /* Reset the count. */
        trg_count = 0;

        /* Walk through the source ramp. */
        for(
            src_ramp = grad->color_ramp, trg_ramp = grad->converted_ramp;
            (src_ramp < src_ramp_last) && (trg_count < VLC_MAX_COLOR_RAMP_STOPS + 2);
            src_ramp += 1) {
            /* Must be in increasing order. */
            if(src_ramp->stop < prev_stop) {
                /* Ignore the entire sequence. */
                trg_count = 0;
                break;
            }

            /* Update the previous stop value. */
            prev_stop = src_ramp->stop;

            /* Must be within [0..1] range. */
            if((src_ramp->stop < 0.0f) || (src_ramp->stop > 1.0f)) {
                /* Ignore. */
                continue;
            }

            /* Clamp color. */
            ClampColor(COLOR_FROM_RAMP(src_ramp), COLOR_FROM_RAMP(trg_ramp), 0);

            /* First stop greater then zero? */
            if((trg_count == 0) && (src_ramp->stop > 0.0f)) {
                /* Force the first stop to 0.0f. */
                trg_ramp->stop = 0.0f;

                /* Replicate the entry. */
                trg_ramp[1] = *trg_ramp;
                trg_ramp[1].stop = src_ramp->stop;

                /* Advance. */
                trg_ramp += 2;
                trg_count += 2;
            }
            else {
                /* Set the stop value. */
                trg_ramp->stop = src_ramp->stop;

                /* Advance. */
                trg_ramp += 1;
                trg_count += 1;
            }
        }

        /* Empty sequence? */
        if(trg_count == 0) {
            memcpy(grad->converted_ramp, default_ramp, sizeof(default_ramp));
            grad->converted_length = sizeof(default_ramp) / 5;
        }
        else {
            /* The last stop must be at 1.0. */
            if(trg_ramp[-1].stop != 1.0f) {
                /* Replicate the last entry. */
                *trg_ramp = trg_ramp[-1];

                /* Force the last stop to 1.0f. */
                trg_ramp->stop = 1.0f;

                /* Update the final entry count. */
                trg_count += 1;
            }

            /* Set new length. */
            grad->converted_length = trg_count;
        }
        return VG_LITE_SUCCESS;

Empty_sequence_handler:
        memcpy(grad->converted_ramp, default_ramp, sizeof(default_ramp));
        grad->converted_length = sizeof(default_ramp) / 5;

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_update_linear_grad(vg_lite_ext_linear_gradient_t * grad)
    {
        vg_lite_uint32_t ramp_length;
        vg_lite_color_ramp_t * color_ramp;
        vg_lite_uint32_t common, stop;
        vg_lite_uint32_t i, width;
        uint8_t * bits;
        vg_lite_float_t x0, y0, x1, y1, length;
        vg_lite_error_t error = VG_LITE_SUCCESS;

        /* Get shortcuts to the color ramp. */
        ramp_length = grad->converted_length;
        color_ramp = grad->converted_ramp;

        x0 = grad->linear_grad.X0;
        y0 = grad->linear_grad.Y0;
        x1 = grad->linear_grad.X1;
        y1 = grad->linear_grad.Y1;
        length = (vg_lite_float_t)sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));

        if(length <= 0)
            return VG_LITE_INVALID_ARGUMENT;
        /* Find the common denominator of the color ramp stops. */
        if(length < 1) {
            common = 1;
        }
        else {
            common = (vg_lite_uint32_t)length;
        }

        for(i = 0; i < ramp_length; ++i) {
            if(color_ramp[i].stop != 0.0f) {
                vg_lite_float_t mul = common * color_ramp[i].stop;
                vg_lite_float_t frac = mul - (vg_lite_float_t)floor(mul);
                if(frac > 0.00013f) { /* Suppose error for zero is 0.00013 */
                    common = MAX(common, (vg_lite_uint32_t)(1.0f / frac + 0.5f));
                }
            }
        }

        /* Compute the width of the required color array. */
        width = common + 1;

        /* Allocate the color ramp surface. */
        memset(&grad->image, 0, sizeof(grad->image));
        grad->image.width = width;
        grad->image.height = 1;
        grad->image.stride = 0;
        grad->image.image_mode = VG_LITE_NONE_IMAGE_MODE;
        grad->image.format = VG_LITE_ABGR8888;

        /* Allocate the image for gradient. */
        VG_LITE_RETURN_ERROR(vg_lite_allocate(&grad->image));
        memset(grad->image.memory, 0, grad->image.stride * grad->image.height);
        width = common + 1;
        /* Set pointer to color array. */
        bits = (uint8_t *)grad->image.memory;

        /* Start filling the color array. */
        stop = 0;
        for(i = 0; i < width; ++i) {
            vg_lite_float_t gradient;
            vg_lite_float_t color[4];
            vg_lite_float_t color1[4];
            vg_lite_float_t color2[4];
            vg_lite_float_t weight;

            if(i == 241)
                i = 241;
            /* Compute gradient for current color array entry. */
            gradient = (vg_lite_float_t)i / (vg_lite_float_t)(width - 1);

            /* Find the entry in the color ramp that matches or exceeds this
            ** gradient. */
            while(gradient > color_ramp[stop].stop) {
                ++stop;
            }

            if(gradient == color_ramp[stop].stop) {
                /* Perfect match weight 1.0. */
                weight = 1.0f;

                /* Use color ramp color. */
                color1[3] = color_ramp[stop].alpha;
                color1[2] = color_ramp[stop].blue;
                color1[1] = color_ramp[stop].green;
                color1[0] = color_ramp[stop].red;

                color2[3] = color2[2] = color2[1] = color2[0] = 0.0f;
            }
            else {
                if(stop == 0) {
                    return VG_LITE_INVALID_ARGUMENT;
                }
                /* Compute weight. */
                weight = (color_ramp[stop].stop - gradient)
                         / (color_ramp[stop].stop - color_ramp[stop - 1].stop);

                /* Grab color ramp color of previous stop. */
                color1[3] = color_ramp[stop - 1].alpha;
                color1[2] = color_ramp[stop - 1].blue;
                color1[1] = color_ramp[stop - 1].green;
                color1[0] = color_ramp[stop - 1].red;

                /* Grab color ramp color of current stop. */
                color2[3] = color_ramp[stop].alpha;
                color2[2] = color_ramp[stop].blue;
                color2[1] = color_ramp[stop].green;
                color2[0] = color_ramp[stop].red;
            }

            if(grad->pre_multiplied) {
                /* Pre-multiply the first color. */
                color1[2] *= color1[3];
                color1[1] *= color1[3];
                color1[0] *= color1[3];

                /* Pre-multiply the second color. */
                color2[2] *= color2[3];
                color2[1] *= color2[3];
                color2[0] *= color2[3];
            }

            /* Filter the colors per channel. */
            color[3] = LERP(color1[3], color2[3], weight);
            color[2] = LERP(color1[2], color2[2], weight);
            color[1] = LERP(color1[1], color2[1], weight);
            color[0] = LERP(color1[0], color2[0], weight);

            /* Pack the final color. */
            *bits++ = PackColorComponent(color[3]);
            *bits++ = PackColorComponent(color[2]);
            *bits++ = PackColorComponent(color[1]);
            *bits++ = PackColorComponent(color[0]);
        }

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_set_radial_grad(vg_lite_radial_gradient_t * grad,
                                            vg_lite_uint32_t count,
                                            vg_lite_color_ramp_t * color_ramp,
                                            vg_lite_radial_gradient_parameter_t radial_grad,
                                            vg_lite_gradient_spreadmode_t spread_mode,
                                            vg_lite_uint8_t pre_multiplied)
    {
        static vg_lite_color_ramp_t defaultRamp[] = {
            {
                0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            },
            {
                1.0f,
                1.0f, 1.0f, 1.0f, 1.0f
            }
        };

        vg_lite_uint32_t i, trgCount;
        vg_lite_float_t prevStop;
        vg_lite_color_ramp_t * srcRamp;
        vg_lite_color_ramp_t * srcRampLast;
        vg_lite_color_ramp_t * trgRamp;

        /* Reset the count. */
        trgCount = 0;

        if(radial_grad.r <= 0)
            return VG_LITE_INVALID_ARGUMENT;

        grad->radial_grad = radial_grad;
        grad->pre_multiplied = pre_multiplied;
        grad->spread_mode = spread_mode;

        if(!count || count > VLC_MAX_COLOR_RAMP_STOPS || color_ramp == NULL)
            goto Empty_sequence_handler;

        for(i = 0; i < count; i++)
            grad->color_ramp[i] = color_ramp[i];
        grad->ramp_length = count;

        /* Determine the last source ramp. */
        srcRampLast
            = grad->color_ramp
              + grad->ramp_length;

        /* Set the initial previous stop. */
        prevStop = -1;

        /* Reset the count. */
        trgCount = 0;

        /* Walk through the source ramp. */
        for(
            srcRamp = grad->color_ramp, trgRamp = grad->converted_ramp;
            (srcRamp < srcRampLast) && (trgCount < VLC_MAX_COLOR_RAMP_STOPS + 2);
            srcRamp += 1) {
            /* Must be in increasing order. */
            if(srcRamp->stop < prevStop) {
                /* Ignore the entire sequence. */
                trgCount = 0;
                break;
            }

            /* Update the previous stop value. */
            prevStop = srcRamp->stop;

            /* Must be within [0..1] range. */
            if((srcRamp->stop < 0.0f) || (srcRamp->stop > 1.0f)) {
                /* Ignore. */
                continue;
            }

            /* Clamp color. */
            ClampColor(COLOR_FROM_RAMP(srcRamp), COLOR_FROM_RAMP(trgRamp), 0);

            /* First stop greater then zero? */
            if((trgCount == 0) && (srcRamp->stop > 0.0f)) {
                /* Force the first stop to 0.0f. */
                trgRamp->stop = 0.0f;

                /* Replicate the entry. */
                trgRamp[1] = *trgRamp;
                trgRamp[1].stop = srcRamp->stop;

                /* Advance. */
                trgRamp += 2;
                trgCount += 2;
            }
            else {
                /* Set the stop value. */
                trgRamp->stop = srcRamp->stop;

                /* Advance. */
                trgRamp += 1;
                trgCount += 1;
            }
        }

        /* Empty sequence? */
        if(trgCount == 0) {
            memcpy(grad->converted_ramp, defaultRamp, sizeof(defaultRamp));
            grad->converted_length = sizeof(defaultRamp) / 5;
        }
        else {
            /* The last stop must be at 1.0. */
            if(trgRamp[-1].stop != 1.0f) {
                /* Replicate the last entry. */
                *trgRamp = trgRamp[-1];

                /* Force the last stop to 1.0f. */
                trgRamp->stop = 1.0f;

                /* Update the final entry count. */
                trgCount += 1;
            }

            /* Set new length. */
            grad->converted_length = trgCount;
        }
        return VG_LITE_SUCCESS;

Empty_sequence_handler:
        memcpy(grad->converted_ramp, defaultRamp, sizeof(defaultRamp));
        grad->converted_length = sizeof(defaultRamp) / 5;

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_update_radial_grad(vg_lite_radial_gradient_t * grad)
    {
        vg_lite_uint32_t ramp_length;
        vg_lite_color_ramp_t * colorRamp;
        vg_lite_uint32_t common, stop;
        vg_lite_uint32_t i, width;
        uint8_t * bits;
        vg_lite_error_t error = VG_LITE_SUCCESS;
        vg_lite_uint32_t align, mul, div;

        /* Get shortcuts to the color ramp. */
        ramp_length = grad->converted_length;
        colorRamp = grad->converted_ramp;

        if(grad->radial_grad.r <= 0)
            return VG_LITE_INVALID_ARGUMENT;

        /* Find the common denominator of the color ramp stops. */
        if(grad->radial_grad.r < 1) {
            common = 1;
        }
        else {
            common = (vg_lite_uint32_t)grad->radial_grad.r;
        }

        for(i = 0; i < ramp_length; ++i) {
            if(colorRamp[i].stop != 0.0f) {
                vg_lite_float_t m = common * colorRamp[i].stop;
                vg_lite_float_t frac = m - (vg_lite_float_t)floor(m);
                if(frac > 0.00013f) { /* Suppose error for zero is 0.00013 */
                    common = MAX(common, (vg_lite_uint32_t)(1.0f / frac + 0.5f));
                }
            }
        }

        /* Compute the width of the required color array. */
        width = common + 1;
        width = (width + 15) & (~0xf);

        /* Allocate the color ramp surface. */
        memset(&grad->image, 0, sizeof(grad->image));
        grad->image.width = width;
        grad->image.height = 1;
        grad->image.stride = 0;
        grad->image.image_mode = VG_LITE_NONE_IMAGE_MODE;
        grad->image.format = VG_LITE_ABGR8888;

        /* Allocate the image for gradient. */
        VG_LITE_RETURN_ERROR(vg_lite_allocate(&grad->image));

        get_format_bytes(VG_LITE_ABGR8888, &mul, &div, &align);
        width = grad->image.stride * div / mul;

        /* Set pointer to color array. */
        bits = (uint8_t *)grad->image.memory;

        /* Start filling the color array. */
        stop = 0;
        for(i = 0; i < width; ++i) {
            vg_lite_float_t gradient;
            vg_lite_float_t color[4];
            vg_lite_float_t color1[4];
            vg_lite_float_t color2[4];
            vg_lite_float_t weight;

            /* Compute gradient for current color array entry. */
            gradient = (vg_lite_float_t)i / (vg_lite_float_t)(width - 1);

            /* Find the entry in the color ramp that matches or exceeds this
            ** gradient. */
            while(gradient > colorRamp[stop].stop) {
                ++stop;
            }

            if(gradient == colorRamp[stop].stop) {
                /* Perfect match weight 1.0. */
                weight = 1.0f;

                /* Use color ramp color. */
                color1[3] = colorRamp[stop].alpha;
                color1[2] = colorRamp[stop].blue;
                color1[1] = colorRamp[stop].green;
                color1[0] = colorRamp[stop].red;

                color2[3] = color2[2] = color2[1] = color2[0] = 0.0f;
            }
            else {
                /* Compute weight. */
                weight = (colorRamp[stop].stop - gradient)
                         / (colorRamp[stop].stop - colorRamp[stop - 1].stop);

                /* Grab color ramp color of previous stop. */
                color1[3] = colorRamp[stop - 1].alpha;
                color1[2] = colorRamp[stop - 1].blue;
                color1[1] = colorRamp[stop - 1].green;
                color1[0] = colorRamp[stop - 1].red;

                /* Grab color ramp color of current stop. */
                color2[3] = colorRamp[stop].alpha;
                color2[2] = colorRamp[stop].blue;
                color2[1] = colorRamp[stop].green;
                color2[0] = colorRamp[stop].red;
            }

            if(grad->pre_multiplied) {
                /* Pre-multiply the first color. */
                color1[2] *= color1[3];
                color1[1] *= color1[3];
                color1[0] *= color1[3];

                /* Pre-multiply the second color. */
                color2[2] *= color2[3];
                color2[1] *= color2[3];
                color2[0] *= color2[3];
            }

            /* Filter the colors per channel. */
            color[3] = LERP(color1[3], color2[3], weight);
            color[2] = LERP(color1[2], color2[2], weight);
            color[1] = LERP(color1[1], color2[1], weight);
            color[0] = LERP(color1[0], color2[0], weight);

            /* Pack the final color. */
            *bits++ = PackColorComponent(color[3]);
            *bits++ = PackColorComponent(color[2]);
            *bits++ = PackColorComponent(color[1]);
            *bits++ = PackColorComponent(color[0]);
        }

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_set_grad(vg_lite_linear_gradient_t * grad,
                                     vg_lite_uint32_t count,
                                     vg_lite_uint32_t * colors,
                                     vg_lite_uint32_t * stops)
    {
        vg_lite_uint32_t i;

        grad->count = 0; /* Opaque B&W gradient */
        if(!count || count > VLC_MAX_GRADIENT_STOPS || colors == NULL || stops == NULL)
            return VG_LITE_SUCCESS;

        /* Check stops validity */
        for(i = 0; i < count; i++)
            if(stops[i] < VLC_GRADIENT_BUFFER_WIDTH) {
                if(!grad->count || stops[i] > grad->stops[grad->count - 1]) {
                    grad->stops[grad->count] = stops[i];
                    grad->colors[grad->count] = colors[i];
                    grad->count++;
                }
                else if(stops[i] == grad->stops[grad->count - 1]) {
                    /* Equal stops : use the color corresponding to the last stop
                    in the sequence */
                    grad->colors[grad->count - 1] = colors[i];
                }
            }

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_update_grad(vg_lite_linear_gradient_t * grad)
    {
        vg_lite_error_t error = VG_LITE_SUCCESS;
        int32_t r0, g0, b0, a0;
        int32_t r1, g1, b1, a1;
        int32_t lr, lg, lb, la;
        vg_lite_uint32_t i;
        int32_t j;
        int32_t ds, dr, dg, db, da;
        vg_lite_uint32_t * buffer = (vg_lite_uint32_t *)grad->image.memory;

        if(grad->count == 0) {
            /* If no valid stops have been specified (e.g., due to an empty input
             * array, out-of-range, or out-of-order stops), a stop at 0 with color
             * 0xFF000000 (opaque black) and a stop at 255 with color 0xFFFFFFFF
             * (opaque white) are implicitly defined. */
            grad->stops[0] = 0;
            grad->colors[0] = 0xFF000000; /* Opaque black */
            grad->stops[1] = 255;
            grad->colors[1] = 0xFFFFFFFF; /* Opaque white */
            grad->count = 2;
        }
        else if(grad->count && grad->stops[0] != 0) {
            /* If at least one valid stop has been specified, but none has been
             * defined with an offset of 0, an implicit stop is added with an
             * offset of 0 and the same color as the first user-defined stop. */
            for(i = 0; i < grad->stops[0]; i++)
                buffer[i] = grad->colors[0];
        }
        a0 = A(grad->colors[0]);
        r0 = R(grad->colors[0]);
        g0 = G(grad->colors[0]);
        b0 = B(grad->colors[0]);

        /* Calculate the colors for each pixel of the image. */
        for(i = 0; i < grad->count - 1; i++) {
            buffer[grad->stops[i]] = grad->colors[i];
            ds = grad->stops[i + 1] - grad->stops[i];
            a1 = A(grad->colors[i + 1]);
            r1 = R(grad->colors[i + 1]);
            g1 = G(grad->colors[i + 1]);
            b1 = B(grad->colors[i + 1]);

            da = a1 - a0;
            dr = r1 - r0;
            dg = g1 - g0;
            db = b1 - b0;

            for(j = 1; j < ds; j++) {
                la = a0 + da * j / ds;
                lr = r0 + dr * j / ds;
                lg = g0 + dg * j / ds;
                lb = b0 + db * j / ds;

                buffer[grad->stops[i] + j] = ARGB(la, lr, lg, lb);
            }

            a0 = a1;
            r0 = r1;
            g0 = g1;
            b0 = b1;
        }

        /* If at least one valid stop has been specified, but none has been defined
         * with an offset of 255, an implicit stop is added with an offset of 255
         * and the same color as the last user-defined stop. */
        for(i = grad->stops[grad->count - 1]; i < VLC_GRADIENT_BUFFER_WIDTH; i++)
            buffer[i] = grad->colors[grad->count - 1];

        return error;
    }

    vg_lite_error_t vg_lite_clear_linear_grad(vg_lite_ext_linear_gradient_t * grad)
    {
        vg_lite_error_t error = VG_LITE_SUCCESS;

        grad->count = 0;
        /* Release the image resource. */
        if(grad->image.handle != NULL) {
            error = vg_lite_free(&grad->image);
        }

        return error;
    }

    vg_lite_error_t vg_lite_clear_grad(vg_lite_linear_gradient_t * grad)
    {
        vg_lite_error_t error = VG_LITE_SUCCESS;

        grad->count = 0;
        /* Release the image resource. */
        if(grad->image.handle != NULL) {
            error = vg_lite_free(&grad->image);
        }

        return error;
    }

    vg_lite_error_t vg_lite_clear_radial_grad(vg_lite_radial_gradient_t * grad)
    {
        vg_lite_error_t error = VG_LITE_SUCCESS;

        grad->count = 0;
        /* Release the image resource. */
        if(grad->image.handle != NULL) {
            error = vg_lite_free(&grad->image);
        }

        return error;
    }

    vg_lite_matrix_t * vg_lite_get_linear_grad_matrix(vg_lite_ext_linear_gradient_t * grad)
    {
        return &grad->matrix;
    }

    vg_lite_matrix_t * vg_lite_get_grad_matrix(vg_lite_linear_gradient_t * grad)
    {
        return &grad->matrix;
    }

    vg_lite_matrix_t * vg_lite_get_radial_grad_matrix(vg_lite_radial_gradient_t * grad)
    {
        return &grad->matrix;
    }

    vg_lite_error_t vg_lite_draw_grad(vg_lite_buffer_t * target,
                                      vg_lite_path_t * path,
                                      vg_lite_fill_t fill_rule,
                                      vg_lite_matrix_t * matrix,
                                      vg_lite_linear_gradient_t * grad,
                                      vg_lite_blend_t blend)
    {
        auto ctx = vg_lite_ctx::get_instance();
        TVG_CHECK_RETURN_VG_ERROR(canvas_set_target(ctx, target));

        auto shape = Shape::gen();
        TVG_CHECK_RETURN_VG_ERROR(shape_append_path(shape, path, matrix));
        TVG_CHECK_RETURN_VG_ERROR(shape->transform(matrix_conv(matrix)));
        TVG_CHECK_RETURN_VG_ERROR(shape->fill(fill_rule_conv(fill_rule)););
        TVG_CHECK_RETURN_VG_ERROR(shape->blend(blend_method_conv(blend)));

        float x_min = path->bounding_box[0];
        float y_min = path->bounding_box[1];
        float x_max = path->bounding_box[2];
        float y_max = path->bounding_box[3];

        auto linearGrad = LinearGradient::gen();

        if(matrix->m[0][1] != 0) {
            /* vertical */
            linearGrad->linear(x_min, y_min, x_min, y_max);
        }
        else {
            /* horizontal */
            linearGrad->linear(x_min, y_min, x_max, y_min);
        }

        linearGrad->transform(matrix_conv(&grad->matrix));
        linearGrad->spread(FillSpread::Reflect);

        tvg::Fill::ColorStop colorStops[VLC_MAX_GRADIENT_STOPS];
        for(vg_lite_uint32_t i = 0; i < grad->count; i++) {
            colorStops[i].offset = grad->stops[i] / 255.0f;
            colorStops[i].r = R(grad->colors[i]);
            colorStops[i].g = G(grad->colors[i]);
            colorStops[i].b = B(grad->colors[i]);
            colorStops[i].a = A(grad->colors[i]);
        }
        TVG_CHECK_RETURN_VG_ERROR(linearGrad->colorStops(colorStops, grad->count));

        TVG_CHECK_RETURN_VG_ERROR(shape->fill(std::move(linearGrad)));
        TVG_CHECK_RETURN_VG_ERROR(ctx->canvas->push(std::move(shape)));

        return VG_LITE_SUCCESS;
    }

    vg_lite_error_t vg_lite_draw_radial_grad(vg_lite_buffer_t * target,
                                             vg_lite_path_t * path,
                                             vg_lite_fill_t fill_rule,
                                             vg_lite_matrix_t * path_matrix,
                                             vg_lite_radial_gradient_t * grad,
                                             vg_lite_color_t paint_color,
                                             vg_lite_blend_t blend,
                                             vg_lite_filter_t filter)
    {
        LV_UNUSED(target);
        LV_UNUSED(path);
        LV_UNUSED(fill_rule);
        LV_UNUSED(path_matrix);
        LV_UNUSED(grad);
        LV_UNUSED(paint_color);
        LV_UNUSED(blend);
        LV_UNUSED(filter);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_set_command_buffer_size(vg_lite_uint32_t size)
    {
        LV_UNUSED(size);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_set_scissor(int32_t x, int32_t y, int32_t right, int32_t bottom)
    {
        LV_UNUSED(x);
        LV_UNUSED(y);
        LV_UNUSED(right);
        LV_UNUSED(bottom);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_enable_scissor(void)
    {
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_disable_scissor(void)
    {
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_get_mem_size(vg_lite_uint32_t * size)
    {
        *size = 0;
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_source_global_alpha(vg_lite_global_alpha_t alpha_mode, uint8_t alpha_value)
    {
        LV_UNUSED(alpha_mode);
        LV_UNUSED(alpha_value);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_dest_global_alpha(vg_lite_global_alpha_t alpha_mode, uint8_t alpha_value)
    {
        LV_UNUSED(alpha_mode);
        LV_UNUSED(alpha_value);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_set_color_key(vg_lite_color_key4_t colorkey)
    {
        LV_UNUSED(colorkey);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_set_flexa_stream_id(uint8_t stream_id)
    {
        LV_UNUSED(stream_id);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_set_flexa_current_background_buffer(uint8_t stream_id,
                                                                vg_lite_buffer_t * buffer,
                                                                vg_lite_uint32_t background_segment_count,
                                                                vg_lite_uint32_t background_segment_size)
    {
        LV_UNUSED(stream_id);
        LV_UNUSED(buffer);
        LV_UNUSED(background_segment_count);
        LV_UNUSED(background_segment_size);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_enable_flexa(void)
    {
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_disable_flexa(void)
    {
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_set_flexa_stop_frame(void)
    {
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_enable_dither(void)
    {
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_disable_dither(void)
    {
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_set_tess_buffer(vg_lite_uint32_t physical, vg_lite_uint32_t size)
    {
        LV_UNUSED(physical);
        LV_UNUSED(size);
        return VG_LITE_NOT_SUPPORT;
    }

    vg_lite_error_t vg_lite_set_command_buffer(vg_lite_uint32_t physical, vg_lite_uint32_t size)
    {
        LV_UNUSED(physical);
        LV_UNUSED(size);
        return VG_LITE_NOT_SUPPORT;
    }
} /* extern "C" */

/**********************
 *   STATIC FUNCTIONS
 **********************/

static vg_lite_error_t vg_lite_error_conv(Result result)
{
    switch(result) {
        case Result::Success:
            return VG_LITE_SUCCESS;

        case Result::InvalidArguments:
            return VG_LITE_INVALID_ARGUMENT;

        case Result::InsufficientCondition:
            return VG_LITE_OUT_OF_RESOURCES;

        case Result::FailedAllocation:
            return VG_LITE_OUT_OF_MEMORY;

        case Result::NonSupport:
            return VG_LITE_NOT_SUPPORT;

        default:
            break;
    }

    return VG_LITE_TIMEOUT;
}

static Matrix matrix_conv(const vg_lite_matrix_t * matrix)
{
    return *(Matrix *)matrix;
}

static FillRule fill_rule_conv(vg_lite_fill_t fill)
{
    if(fill == VG_LITE_FILL_EVEN_ODD) {
        return FillRule::EvenOdd;
    }

    return FillRule::Winding;
}

static BlendMethod blend_method_conv(vg_lite_blend_t blend)
{
    switch(blend) {
        case VG_LITE_BLEND_NONE:
            return BlendMethod::SrcOver;

        case VG_LITE_BLEND_NORMAL_LVGL:
            return BlendMethod::Normal;

        case VG_LITE_BLEND_SRC_OVER:
            return BlendMethod::Normal;

        case VG_LITE_BLEND_SCREEN:
            return BlendMethod::Screen;

        case VG_LITE_BLEND_ADDITIVE:
            return BlendMethod::Add;

        case VG_LITE_BLEND_MULTIPLY:
            return BlendMethod::Multiply;

        default:
            break;
    }

    return BlendMethod::Normal;
}

static float vlc_get_arg(const void * data, vg_lite_format_t format)
{
    switch(format) {
        case VG_LITE_S8:
            return *((int8_t *)data);

        case VG_LITE_S16:
            return *((int16_t *)data);

        case VG_LITE_S32:
            return *((int32_t *)data);

        case VG_LITE_FP32:
            return *((float *)data);

        default:
            LV_LOG_ERROR("UNKNOW_FORMAT: %d", format);
            break;
    }

    return 0;
}

static uint8_t vlc_format_len(vg_lite_format_t format)
{
    switch(format) {
        case VG_LITE_S8:
            return 1;
        case VG_LITE_S16:
            return 2;
        case VG_LITE_S32:
            return 4;
        case VG_LITE_FP32:
            return 4;
        default:
            LV_LOG_ERROR("UNKNOW_FORMAT: %d", format);
            LV_ASSERT(false);
            break;
    }

    return 0;
}

static uint8_t vlc_op_arg_len(uint8_t vlc_op)
{
    switch(vlc_op) {
            VLC_OP_ARG_LEN(END, 0);
            VLC_OP_ARG_LEN(CLOSE, 0);
            VLC_OP_ARG_LEN(MOVE, 2);
            VLC_OP_ARG_LEN(MOVE_REL, 2);
            VLC_OP_ARG_LEN(LINE, 2);
            VLC_OP_ARG_LEN(LINE_REL, 2);
            VLC_OP_ARG_LEN(QUAD, 4);
            VLC_OP_ARG_LEN(QUAD_REL, 4);
            VLC_OP_ARG_LEN(CUBIC, 6);
            VLC_OP_ARG_LEN(CUBIC_REL, 6);
            VLC_OP_ARG_LEN(SCCWARC, 5);
            VLC_OP_ARG_LEN(SCCWARC_REL, 5);
            VLC_OP_ARG_LEN(SCWARC, 5);
            VLC_OP_ARG_LEN(SCWARC_REL, 5);
            VLC_OP_ARG_LEN(LCCWARC, 5);
            VLC_OP_ARG_LEN(LCCWARC_REL, 5);
            VLC_OP_ARG_LEN(LCWARC, 5);
            VLC_OP_ARG_LEN(LCWARC_REL, 5);
        default:
            LV_LOG_ERROR("UNKNOW_VLC_OP: 0x%x", vlc_op);
            LV_ASSERT(false);
            break;
    }

    return 0;
}

static Result shape_append_path(std::unique_ptr<Shape> & shape, vg_lite_path_t * path, vg_lite_matrix_t * matrix)
{
    uint8_t fmt_len = vlc_format_len(path->format);
    uint8_t * cur = (uint8_t *)path->path;
    uint8_t * end = cur + path->path_length;

    while(cur < end) {
        /* get op code */
        uint8_t op_code = VLC_GET_OP_CODE(cur);

        /* get arguments length */
        uint8_t arg_len = vlc_op_arg_len(op_code);

        /* skip op code */
        cur += fmt_len;

        switch(op_code) {
            case VLC_OP_MOVE: {
                    float x = VLC_GET_ARG(cur, 0);
                    float y = VLC_GET_ARG(cur, 1);
                    TVG_CHECK_RETURN_RESULT(shape->moveTo(x, y));
                }
                break;

            case VLC_OP_LINE: {
                    float x = VLC_GET_ARG(cur, 0);
                    float y = VLC_GET_ARG(cur, 1);
                    TVG_CHECK_RETURN_RESULT(shape->lineTo(x, y));
                }
                break;

            case VLC_OP_QUAD: {
                    /* hack pre point */
                    float qcx0 = VLC_GET_ARG(cur, -3);
                    float qcy0 = VLC_GET_ARG(cur, -2);
                    float qcx1 = VLC_GET_ARG(cur, 0);
                    float qcy1 = VLC_GET_ARG(cur, 1);
                    float x = VLC_GET_ARG(cur, 2);
                    float y = VLC_GET_ARG(cur, 3);

                    qcx0 += (qcx1 - qcx0) * 2 / 3;
                    qcy0 += (qcy1 - qcy0) * 2 / 3;
                    qcx1 = x + (qcx1 - x) * 2 / 3;
                    qcy1 = y + (qcy1 - y) * 2 / 3;

                    TVG_CHECK_RETURN_RESULT(shape->cubicTo(qcx0, qcy0, qcx1, qcy1, x, y));
                }
                break;

            case VLC_OP_CUBIC: {
                    float cx1 = VLC_GET_ARG(cur, 0);
                    float cy1 = VLC_GET_ARG(cur, 1);
                    float cx2 = VLC_GET_ARG(cur, 2);
                    float cy2 = VLC_GET_ARG(cur, 3);
                    float x = VLC_GET_ARG(cur, 4);
                    float y = VLC_GET_ARG(cur, 5);
                    TVG_CHECK_RETURN_RESULT(shape->cubicTo(cx1, cy1, cx2, cy2, x, y));
                }
                break;

            case VLC_OP_CLOSE:
            case VLC_OP_END: {
                    TVG_CHECK_RETURN_RESULT(shape->close());
                }
                break;

            default:
                break;
        }

        cur += arg_len * fmt_len;
    }

    float x_min = path->bounding_box[0];
    float y_min = path->bounding_box[1];
    float x_max = path->bounding_box[2];
    float y_max = path->bounding_box[3];

    if(math_equal(x_min, __FLT_MIN__) && math_equal(y_min, __FLT_MIN__)
       && math_equal(x_max, __FLT_MAX__) && math_equal(y_max, __FLT_MAX__)) {
        return Result::Success;
    }

    auto cilp = Shape::gen();
    TVG_CHECK_RETURN_RESULT(cilp->appendRect(x_min, y_min, x_max - x_min, y_max - y_min, 0, 0));
    TVG_CHECK_RETURN_RESULT(cilp->transform(matrix_conv(matrix)));
    TVG_CHECK_RETURN_RESULT(shape->composite(std::move(cilp), CompositeMethod::ClipPath));

    return Result::Success;
}

static Result shape_append_rect(std::unique_ptr<Shape> & shape, const vg_lite_buffer_t * target,
                                const vg_lite_rectangle_t * rect)
{
    if(rect) {
        TVG_CHECK_RETURN_RESULT(shape->appendRect(rect->x, rect->y, rect->width, rect->height, 0, 0));
    }
    else if(target) {
        TVG_CHECK_RETURN_RESULT(shape->appendRect(0, 0, target->width, target->height, 0, 0));
    }
    else {
        return Result::InvalidArguments;
    }

    return Result::Success;
}

static Result canvas_set_target(vg_lite_ctx * ctx, vg_lite_buffer_t * target)
{
    void * tvg_target_buffer = nullptr;

    /* if target_buffer needs to be changed, finish current drawing */
    if(ctx->target_buffer && ctx->target_buffer != target->memory) {
        vg_lite_finish();
    }

    ctx->target_buffer = target->memory;
    ctx->target_format = target->format;
    ctx->target_px_size = target->width * target->height;

    if(TVG_IS_VG_FMT_SUPPORT(target->format)) {
        /* if target format is supported by VG, use target buffer directly */
        tvg_target_buffer = target->memory;
    }
    else {
        /* if target format is not supported by VG, use internal buffer */
        tvg_target_buffer = ctx->get_temp_target_buffer(target->width, target->height);
    }

    Result res = ctx->canvas->target(
                     (uint32_t *)tvg_target_buffer,
                     target->width,
                     target->width,
                     target->height,
                     SwCanvas::ARGB8888);

    return res;
}

static vg_lite_uint32_t width_to_stride(vg_lite_uint32_t w, vg_lite_buffer_format_t color_format)
{
    if(vg_lite_query_feature(gcFEATURE_BIT_VG_16PIXELS_ALIGN)) {
        w = VG_LITE_ALIGN(w, 16);
    }

    vg_lite_uint32_t mul, div, align;
    get_format_bytes(color_format, &mul, &div, &align);
    return VG_LITE_ALIGN((w * mul / div), align);
}

static bool decode_indexed_line(
    vg_lite_buffer_format_t color_format,
    const vg_lite_uint32_t * palette,
    int32_t x, int32_t y,
    int32_t w_px, const uint8_t * in, vg_lite_uint32_t * out)
{
    uint8_t px_size;
    uint16_t mask;

    vg_lite_uint32_t w_byte = width_to_stride(w_px, color_format);

    in += w_byte * y; /*First pixel*/
    out += w_px * y;

    int8_t shift = 0;
    switch(color_format) {
        case VG_LITE_INDEX_1:
            px_size = 1;
            in += x / 8; /*8pixel per byte*/
            shift = 7 - (x & 0x7);
            break;
        case VG_LITE_INDEX_2:
            px_size = 2;
            in += x / 4; /*4pixel per byte*/
            shift = 6 - 2 * (x & 0x3);
            break;
        case VG_LITE_INDEX_4:
            px_size = 4;
            in += x / 2; /*2pixel per byte*/
            shift = 4 - 4 * (x & 0x1);
            break;
        case VG_LITE_INDEX_8:
            px_size = 8;
            in += x;
            shift = 0;
            break;
        default:
            LV_ASSERT(false);
            return false;
    }

    mask = (1 << px_size) - 1; /*E.g. px_size = 2; mask = 0x03*/

    int32_t i;
    for(i = 0; i < w_px; i++) {
        uint8_t val_act = (*in >> shift) & mask;
        out[i] = palette[val_act];

        shift -= px_size;
        if(shift < 0) {
            shift = 8 - px_size;
            in++;
        }
    }
    return true;
}

static Result picture_load(vg_lite_ctx * ctx, std::unique_ptr<Picture> & picture, const vg_lite_buffer_t * source,
                           vg_lite_color_t color)
{
    vg_lite_uint32_t * image_buffer;
    LV_ASSERT(VG_LITE_IS_ALIGNED(source->memory, LV_VG_LITE_THORVG_BUF_ADDR_ALIGN));

#if LV_VG_LITE_THORVG_16PIXELS_ALIGN
    LV_ASSERT(VG_LITE_IS_ALIGNED(source->width, 16));
#endif

    if(source->format == VG_LITE_BGRA8888 && source->image_mode == VG_LITE_NORMAL_IMAGE_MODE) {
        image_buffer = (vg_lite_uint32_t *)source->memory;
    }
    else {
        vg_lite_uint32_t width = source->width;
        vg_lite_uint32_t height = source->height;
        vg_lite_uint32_t px_size = width * height;
        image_buffer = ctx->get_image_buffer(width, height);

        vg_lite_buffer_t target;
        memset(&target, 0, sizeof(target));
        target.memory = image_buffer;
        target.format = VG_LITE_BGRA8888;
        target.width = width;
        target.height = height;
        target.stride = width_to_stride(width, target.format);

        switch(source->format) {
            case VG_LITE_INDEX_1:
            case VG_LITE_INDEX_2:
            case VG_LITE_INDEX_4:
            case VG_LITE_INDEX_8: {
                    const vg_lite_uint32_t * clut_colors = ctx->get_CLUT(source->format);
                    for(vg_lite_uint32_t y = 0; y < height; y++) {
                        decode_indexed_line(source->format, clut_colors, 0, y, width, (uint8_t *)source->memory, image_buffer);
                    }
                }
                break;

            case VG_LITE_A4: {
                    conv_alpha4_to_bgra8888.convert(&target, source, color);
                }
                break;

            case VG_LITE_A8: {
                    conv_alpha8_to_bgra8888.convert(&target, source, color);
                }
                break;

            case VG_LITE_BGRX8888: {
                    conv_bgrx8888_to_bgra8888.convert(&target, source);
                }
                break;

            case VG_LITE_BGR888: {
                    conv_bgr888_to_bgra8888.convert(&target, source);
                }
                break;

            case VG_LITE_BGRA5658: {
                    conv_bgra5658_to_bgra8888.convert(&target, source);
                }
                break;

            case VG_LITE_BGR565: {
                    conv_bgr565_to_bgra8888.convert(&target, source);
                }
                break;

#if LV_VG_LITE_THORVG_YUV_SUPPORT
            case VG_LITE_NV12: {
                    libyuv::NV12ToARGB((const uint8_t *)source->memory, source->stride, (const uint8_t *)source->yuv.uv_memory,
                                       source->yuv.uv_stride,
                                       (uint8_t *)image_buffer, source->width * sizeof(vg_lite_uint32_t), width, height);
                }
                break;
#endif

            case VG_LITE_BGRA8888: {
                    memcpy(image_buffer, source->memory, px_size * sizeof(vg_color32_t));
                }
                break;

            default:
                LV_LOG_ERROR("unsupported format: %d", source->format);
                LV_ASSERT(false);
                break;
        }

        /* multiply color */
        if(source->image_mode == VG_LITE_MULTIPLY_IMAGE_MODE && !VG_LITE_IS_ALPHA_FORMAT(source->format)) {
            vg_color32_t * dest = (vg_color32_t *)image_buffer;
            while(px_size--) {
                dest->alpha = UDIV255(dest->alpha * A(color));
                dest->red = UDIV255(dest->red * B(color));
                dest->green = UDIV255(dest->green * G(color));
                dest->blue = UDIV255(dest->blue * R(color));
                dest++;
            }
        }
    }

    TVG_CHECK_RETURN_RESULT(picture->load((uint32_t *)image_buffer, source->width, source->height, true));

    return Result::Success;
}

static void ClampColor(FLOATVECTOR4 Source, FLOATVECTOR4 Target, uint8_t Premultiplied)
{
    vg_lite_float_t colorMax;
    /* Clamp the alpha channel. */
    Target[3] = CLAMP(Source[3], 0.0f, 1.0f);

    /* Determine the maximum value for the color channels. */
    colorMax = Premultiplied ? Target[3] : 1.0f;

    /* Clamp the color channels. */
    Target[0] = CLAMP(Source[0], 0.0f, colorMax);
    Target[1] = CLAMP(Source[1], 0.0f, colorMax);
    Target[2] = CLAMP(Source[2], 0.0f, colorMax);
}

static uint8_t PackColorComponent(vg_lite_float_t value)
{
    /* Compute the rounded normalized value. */
    vg_lite_float_t rounded = value * 255.0f + 0.5f;

    /* Get the integer part. */
    int32_t roundedInt = (int32_t)rounded;

    /* Clamp to 0..1 range. */
    uint8_t clamped = (uint8_t)CLAMP(roundedInt, 0, 255);

    /* Return result. */
    return clamped;
}

/* Get the bpp information of a color format. */
static void get_format_bytes(vg_lite_buffer_format_t format,
                             vg_lite_uint32_t * mul,
                             vg_lite_uint32_t * div,
                             vg_lite_uint32_t * bytes_align)
{
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
        /* ABGR8565_PLANAR buffer memory = RGB565 + alpha. */
        case VG_LITE_ABGR8565_PLANAR:
        case VG_LITE_ARGB8565_PLANAR:
        case VG_LITE_RGBA5658_PLANAR:
        case VG_LITE_BGRA5658_PLANAR:
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
            *mul = 3;
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

        /* OpenVG format*/
        case VG_sRGBX_8888:
        case VG_sRGBA_8888:
        case VG_sRGBA_8888_PRE:
        case VG_lRGBX_8888:
        case VG_lRGBA_8888:
        case VG_lRGBA_8888_PRE:
        case VG_sXRGB_8888:
        case VG_sARGB_8888:
        case VG_sARGB_8888_PRE:
        case VG_lXRGB_8888:
        case VG_lARGB_8888:
        case VG_lARGB_8888_PRE:
        case VG_sBGRX_8888:
        case VG_sBGRA_8888:
        case VG_sBGRA_8888_PRE:
        case VG_lBGRX_8888:
        case VG_lBGRA_8888:
        case VG_sXBGR_8888:
        case VG_sABGR_8888:
        case VG_lBGRA_8888_PRE:
        case VG_sABGR_8888_PRE:
        case VG_lXBGR_8888:
        case VG_lABGR_8888:
        case VG_lABGR_8888_PRE:
            *mul = 4;
            break;

        case VG_sRGBA_5551:
        case VG_sRGBA_4444:
        case VG_sARGB_1555:
        case VG_sARGB_4444:
        case VG_sBGRA_5551:
        case VG_sBGRA_4444:
        case VG_sABGR_1555:
        case VG_sABGR_4444:
        case VG_sRGB_565:
        case VG_sBGR_565:
            *mul = 2;
            break;

        case VG_sL_8:
        case VG_lL_8:
        case VG_A_8:
            break;

        case VG_BW_1:
        case VG_A_4:
        case VG_A_1:
            *div = 2;
            break;

        default:
            break;
    }
}
#endif
