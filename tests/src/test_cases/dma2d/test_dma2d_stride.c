/*The stub draws no pixels, so the tests read back the offsets the unit programs into the registers*/
#if LV_BUILD_TEST

#include "refr/lv_test_refr.h"
#include "dma2d/lv_test_dma2d_hal.h"
#include <src/draw/dma2d/lv_draw_dma2d_private.h>

#define DISP_W          60
#define DISP_H          40
#define DISP_CF         LV_COLOR_FORMAT_XRGB8888
#define PX_SIZE         4

#define STRIDE_PADDING  64

#define IMG_W           10
#define IMG_H           8
/*Wider than IMG_W * PX_SIZE, so that FGOR is not 0*/
#define IMG_STRIDE      64

typedef void (*scene_cb_t)(void);
typedef void (*check_cb_t)(uint32_t stride_px);

static uint8_t * disp_raw_buf;
static const char * mode_name;

static uint32_t img_pixels[IMG_H * IMG_STRIDE / PX_SIZE];

static lv_image_dsc_t img_dsc = {
    .header = {
        .magic = LV_IMAGE_HEADER_MAGIC,
        .cf = LV_COLOR_FORMAT_ARGB8888,
        .w = IMG_W,
        .h = IMG_H,
        .stride = IMG_STRIDE,
    },
    .data_size = sizeof(img_pixels),
    .data = (const uint8_t *)img_pixels,
};

static void disp_create(lv_display_render_mode_t mode, bool padded_stride, uint32_t * stride);
static void disp_delete(void);
static void run_scene(scene_cb_t scene, check_cb_t check, bool padded_stride);
static uint32_t transfer_mode(const DMA2D_TypeDef * t);
static uint32_t transfer_w(const DMA2D_TypeDef * t);

void setUp(void)
{
    refr_ctx_reset();
    lv_test_dma2d_reset();
}

void tearDown(void)
{
    disp_delete();
}

static void scene_fill_opaque(void)
{
    refr_screen_set_color(REFR_COLOR_BLACK);
    refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);
}

static void check_fill_opaque(uint32_t stride_px)
{
    uint32_t i;
    uint32_t cnt = 0;
    for(i = 0; i < lv_test_dma2d_count(); i++) {
        const DMA2D_TypeDef * t = lv_test_dma2d_get(i);
        if(transfer_mode(t) != LV_DRAW_DMA2D_MODE_REGISTER_TO_MEMORY) continue;
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(stride_px - transfer_w(t), t->OOR, mode_name);
        cnt++;
    }
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(2, cnt, mode_name);
}

void test_fill_opaque_uses_buffer_stride(void)
{
    run_scene(scene_fill_opaque, check_fill_opaque, true);
}

static void scene_fill_translucent(void)
{
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * rect = refr_rect_create(refr_screen(), 10, 10, 20, 20, REFR_COLOR_RED);
    lv_obj_set_style_bg_opa(rect, LV_OPA_50, 0);
}

static void check_fill_translucent(uint32_t stride_px)
{
    uint32_t i;
    uint32_t cnt = 0;
    for(i = 0; i < lv_test_dma2d_count(); i++) {
        const DMA2D_TypeDef * t = lv_test_dma2d_get(i);
        if(transfer_mode(t) != LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY_WITH_BLENDING) continue;
        uint32_t expected = stride_px - transfer_w(t);
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(expected, t->OOR, mode_name);
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(expected, t->FGOR, mode_name);
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(expected, t->BGOR, mode_name);
        cnt++;
    }
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1, cnt, mode_name);
}

void test_fill_translucent_uses_buffer_stride(void)
{
    run_scene(scene_fill_translucent, check_fill_translucent, true);
}

static void scene_image(void)
{
    refr_screen_set_color(REFR_COLOR_BLACK);
    lv_obj_t * img = lv_image_create(refr_screen());
    lv_image_set_src(img, &img_dsc);
    lv_obj_set_pos(img, 5, 5);
}

static void check_image(uint32_t stride_px)
{
    uint32_t i;
    uint32_t cnt = 0;
    for(i = 0; i < lv_test_dma2d_count(); i++) {
        const DMA2D_TypeDef * t = lv_test_dma2d_get(i);
        if(transfer_mode(t) != LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY_WITH_BLENDING) continue;
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(IMG_W, transfer_w(t), mode_name);
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(stride_px - IMG_W, t->OOR, mode_name);
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(stride_px - IMG_W, t->BGOR, mode_name);
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(IMG_STRIDE / PX_SIZE - IMG_W, t->FGOR, mode_name);
        cnt++;
    }
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(1, cnt, mode_name);
}

void test_image_uses_buffer_stride(void)
{
    run_scene(scene_image, check_image, true);
}

void test_automatic_stride_unchanged(void)
{
    run_scene(scene_fill_opaque, check_fill_opaque, false);
    run_scene(scene_fill_translucent, check_fill_translucent, false);
    run_scene(scene_image, check_image, false);
}

static void disp_create(lv_display_render_mode_t mode, bool padded_stride, uint32_t * stride)
{
    uint32_t auto_stride = lv_draw_buf_width_to_stride(DISP_W, DISP_CF);
    uint32_t buf_stride = padded_stride ? auto_stride + STRIDE_PADDING : auto_stride;

    refr_ctx.cf = DISP_CF;
    refr_ctx.hor_res = DISP_W;
    refr_ctx.ver_res = DISP_H;

    lv_display_t * disp = lv_display_create(DISP_W, DISP_H);
    TEST_ASSERT_NOT_NULL(disp);
    lv_display_set_color_format(disp, DISP_CF);

    uint32_t buf_size = LV_ROUND_UP(buf_stride * DISP_H, LV_DRAW_BUF_ALIGN);
    disp_raw_buf = lv_malloc_zeroed(buf_size + LV_DRAW_BUF_ALIGN);
    TEST_ASSERT_NOT_NULL(disp_raw_buf);
    uint8_t * aligned = lv_draw_buf_align(disp_raw_buf, DISP_CF);
    if(padded_stride) {
        lv_display_set_buffers_with_stride(disp, aligned, NULL, buf_size, buf_stride, mode);
    }
    else {
        lv_display_set_buffers(disp, aligned, NULL, buf_size, mode);
    }
    lv_display_set_flush_cb(disp, refr_flush_cb);
    refr_hide_sysmon(disp);
    refr_ctx.disp = disp;

    TEST_ASSERT_EQUAL_UINT32(buf_stride, disp->buf_act->header.stride);
    *stride = buf_stride;
}

static void disp_delete(void)
{
    refr_disp_delete();
    if(disp_raw_buf) {
        lv_free(disp_raw_buf);
        disp_raw_buf = NULL;
    }
}

static void run_scene(scene_cb_t scene, check_cb_t check, bool padded_stride)
{
    static const lv_display_render_mode_t modes[] = {LV_DISPLAY_RENDER_MODE_DIRECT, LV_DISPLAY_RENDER_MODE_FULL};
    static const char * const names[] = {"DIRECT", "FULL"};
    uint32_t i;

    for(i = 0; i < sizeof(modes) / sizeof(modes[0]); i++) {
        uint32_t stride;
        mode_name = names[i];
        disp_create(modes[i], padded_stride, &stride);

        scene();
        lv_test_dma2d_reset();
        refr_frame();

        TEST_ASSERT_TRUE_MESSAGE(lv_test_dma2d_count() > 0, mode_name);
        TEST_ASSERT_TRUE_MESSAGE(lv_test_dma2d_count() <= LV_TEST_DMA2D_LOG_MAX, mode_name);
        check(stride / PX_SIZE);

        disp_delete();
    }
}

static uint32_t transfer_mode(const DMA2D_TypeDef * t)
{
    return (t->CR >> DMA2D_CR_MODE_Pos) & 0x7;
}

static uint32_t transfer_w(const DMA2D_TypeDef * t)
{
    return t->NLR >> DMA2D_NLR_PL_Pos;
}

#endif
