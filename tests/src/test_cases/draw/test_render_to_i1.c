#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "../demos/lv_demos.h"

#include "unity/unity.h"

static void capture_i1_pixels(const lv_draw_buf_t * draw_buf, uint8_t * pixels)
{
    const uint32_t width = draw_buf->header.w;
    const uint32_t height = draw_buf->header.h;

    for(uint32_t y = 0; y < height; y++) {
        for(uint32_t x = 0; x < width; x++) {
            const uint8_t * byte = lv_draw_buf_goto_xy(draw_buf, x, y);
            uint32_t bit_idx = draw_buf->header.vtiled ? y & 7 : x & 7;
            uint32_t bit = draw_buf->header.lsb_first ? bit_idx : 7 - bit_idx;
            pixels[y * width + x] = (*byte >> bit) & 1;
        }
    }
}

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_display_t * display = lv_display_get_default();
    lv_display_set_vtiled(display, false);
    lv_display_set_lsb_first(display, false);
    lv_display_set_color_format(display, LV_COLOR_FORMAT_XRGB8888);
}

void test_render_to_i1(void)
{
    /*NanoVG reads back the FBO as 32bpp BGRA, so non-XRGB8888 targets don't apply*/
#if LV_BIN_DECODER_RAM_LOAD && LV_USE_DRAW_VG_LITE == 0 && LV_USE_DRAW_NANOVG == 0
    static const struct {
        bool vtiled;
        bool lsb_first;
    } layouts[] = {
        {false, false},
        {false, true},
        {true, false},
        {true, true},
    };

    lv_display_t * display = lv_display_get_default();
    lv_display_set_color_format(display, LV_COLOR_FORMAT_I1);
    lv_draw_buf_t * draw_buf = lv_display_get_buf_active(display);
    void * unaligned = draw_buf->unaligned_data;
    uint32_t data_size = draw_buf->data_size;

    lv_opa_t opa_values[2] = {0xff, 0xc0};
    uint32_t opa;
    for(opa = 0; opa < 2; opa++) {
        uint32_t i;
        for(i = 0; i < LV_DEMO_RENDER_SCENE_NUM; i++) {
            uint32_t width = lv_display_get_horizontal_resolution(display);
            uint32_t height = lv_display_get_vertical_resolution(display);
            uint8_t * reference = lv_malloc(width * height);
            uint8_t * actual = lv_malloc(width * height);
            TEST_ASSERT_NOT_NULL(reference);
            TEST_ASSERT_NOT_NULL(actual);

            for(size_t layout = 0; layout < sizeof(layouts) / sizeof(layouts[0]); layout++) {
                lv_display_set_vtiled(display, layouts[layout].vtiled);
                lv_display_set_lsb_first(display, layouts[layout].lsb_first);
                void * aligned = lv_draw_buf_align(unaligned, LV_COLOR_FORMAT_I1);
                uint32_t align_offset = (uint8_t *)aligned - (uint8_t *)unaligned;
                uint32_t stride = lv_draw_buf_width_to_stride_packed(800, LV_COLOR_FORMAT_I1,
                                                                     layouts[layout].vtiled);
                TEST_ASSERT_TRUE(stride * lv_draw_buf_stride_rows(480, LV_COLOR_FORMAT_I1,
                                                                  layouts[layout].vtiled) <=
                                 data_size - align_offset);
                TEST_ASSERT_NOT_NULL(lv_draw_buf_init_with_mono_flags(
                    draw_buf, 800, 480, LV_COLOR_FORMAT_I1, LV_STRIDE_AUTO, aligned,
                    data_size - align_offset, layouts[layout].vtiled,
                    layouts[layout].lsb_first) == LV_RESULT_OK ? draw_buf : NULL);
                draw_buf->unaligned_data = unaligned;
                lv_display_set_draw_buffers(display, draw_buf, NULL);
                lv_display_set_render_mode(display, LV_DISPLAY_RENDER_MODE_DIRECT);
                lv_demo_render(i, opa_values[opa]);
                lv_refr_now(display);
                capture_i1_pixels(draw_buf, actual);
                if(layout == 0) {
                    lv_memcpy(reference, actual, width * height);
                }
                else {
                    TEST_ASSERT_EQUAL_UINT8_ARRAY(reference, actual, width * height);
                }

                char buf[128];
                lv_snprintf(buf, sizeof(buf), "draw/render/i1/demo_render_%s_opa_%d.png",
                            lv_demo_render_get_scene_name(i), opa_values[opa]);
                TEST_ASSERT_EQUAL_SCREENSHOT(buf);
            }
            lv_free(actual);
            lv_free(reference);
        }
    }
#else
    /*Without LV_BIN_DECODER_RAM_LOAD can't test rotated images*/
    TEST_PASS();
#endif
}

#endif
