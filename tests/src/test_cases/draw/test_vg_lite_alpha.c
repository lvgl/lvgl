#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "alpha/lv_test_alpha_scene.h"

#include "unity/unity.h"

/* An ordinary semi-transparent widget on a transparent screen, drawn over a
 * background (see lv_test_alpha_scene.h), and its pixel read back by the
 * display buffer's color format. */

/* Larger glyphs than the largest built-in font */
#define LABEL_FONT_SIZE 120

static lv_font_t * label_font;

void setUp(void)
{
    lv_display_set_color_format(lv_display_get_default(), LV_COLOR_FORMAT_ARGB8888);
    extern const uint8_t test_ubuntu_font[];
    extern size_t test_ubuntu_font_size;
    label_font = lv_tiny_ttf_create_data(test_ubuntu_font, test_ubuntu_font_size, LABEL_FONT_SIZE);
    lv_test_alpha_scene_set_label_font(label_font);
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
    lv_test_alpha_scene_set_label_font(&lv_font_montserrat_48);
    lv_tiny_ttf_destroy(label_font);
    lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_COVER, 0);
    lv_display_set_color_format(lv_display_get_default(), LV_COLOR_FORMAT_XRGB8888);
}

static lv_draw_buf_t * render_scene(lv_test_alpha_widget_id_t widget, lv_test_alpha_bg_id_t bg)
{
    lv_test_alpha_scene_create(lv_screen_active(), widget, bg);
    lv_obj_invalidate(lv_screen_active());
    lv_refr_now(NULL);
    return lv_display_get_buf_active(lv_display_get_default());
}

static void print_pixel(const lv_draw_buf_t * buf, const lv_test_alpha_pixel_t * px, lv_color32_t expected)
{
    /* Unity's printf has no field widths */
    char msg[160];
    lv_snprintf(msg, sizeof(msg),
                "BGRA in buffer: %02X %02X %02X %02X, cf: 0x%02X"
                " -> RGB %02X%02X%02X, alpha %02X (expected %02X%02X%02X, alpha %02X)",
                px->raw[0], px->raw[1], px->raw[2], px->raw[3], buf->header.cf,
                px->straight.red, px->straight.green, px->straight.blue, px->straight.alpha,
                expected.red, expected.green, expected.blue, expected.alpha);
    TEST_PRINTF("%s", msg);
}

/* Render the widget over the background on a display of color format `cf`
 * and check the widget's pixel, read by the buffer's color format. */
static void assert_case(lv_test_alpha_widget_id_t widget, lv_test_alpha_bg_id_t bg, lv_color_format_t cf)
{
#if LV_USE_DRAW_NANOVG
    /* NanoVG's headless readback ignores the display's alpha and color format */
    LV_UNUSED(widget);
    LV_UNUSED(bg);
    LV_UNUSED(cf);
    TEST_IGNORE_MESSAGE("NanoVG's headless readback can't show transparency");
#endif
    lv_display_set_color_format(lv_display_get_default(), cf);
    lv_point_t point = lv_test_alpha_scene_sample_point(widget);
    lv_draw_buf_t * buf = render_scene(widget, bg);
    lv_color32_t expected = lv_test_alpha_expected(widget, bg);

    /* A straight-alpha buffer has to hold straight pixels, not premultiplied ones marked with the flag */
    if(cf == LV_COLOR_FORMAT_ARGB8888) {
        TEST_ASSERT_FALSE_MESSAGE(lv_draw_buf_has_flag(buf, LV_IMAGE_FLAGS_PREMULTIPLIED), "premultiplied flag");
    }

    lv_test_alpha_pixel_t px = lv_test_alpha_scene_read(buf, point);
    print_pixel(buf, &px, expected);
    /* Blending two colors rounds once more than drawing on nothing. Alpha has
     * to be exact where nothing is blended. */
    uint8_t color_tolerance = bg == LV_TEST_ALPHA_BG_NONE ? 1 : 2;
    uint8_t alpha_tolerance = bg == LV_TEST_ALPHA_BG_TRANSLUCENT ? 1 : 0;
#if !LV_USE_DRAW_VG_LITE
    /* SW multiplies a glyph's coverage by its opacity and premultiplies with `>> 8`, not /255 */
    if(lv_test_alpha_widgets[widget].kind == LV_TEST_ALPHA_LABEL) {
        color_tolerance = 4;
        alpha_tolerance = 1;
    }
#endif
    TEST_ASSERT_UINT8_WITHIN_MESSAGE(color_tolerance, expected.red, px.straight.red, "red");
    TEST_ASSERT_UINT8_WITHIN_MESSAGE(color_tolerance, expected.green, px.straight.green, "green");
    TEST_ASSERT_UINT8_WITHIN_MESSAGE(color_tolerance, expected.blue, px.straight.blue, "blue");
    TEST_ASSERT_UINT8_WITHIN_MESSAGE(alpha_tolerance, expected.alpha, px.straight.alpha, "alpha");
}

void test_vg_lite_alpha_obj_opa_77_on_opaque(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_OBJ_OPA_77, LV_TEST_ALPHA_BG_OPAQUE, LV_COLOR_FORMAT_ARGB8888);
}

void test_vg_lite_alpha_button_opa_dd_on_opaque(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_BUTTON_OPA_DD, LV_TEST_ALPHA_BG_OPAQUE, LV_COLOR_FORMAT_ARGB8888);
}

void test_vg_lite_alpha_label_opa_77_on_none(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_LABEL_OPA_77, LV_TEST_ALPHA_BG_NONE, LV_COLOR_FORMAT_ARGB8888);
}

void test_vg_lite_alpha_label_opa_77_on_opaque(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_LABEL_OPA_77, LV_TEST_ALPHA_BG_OPAQUE, LV_COLOR_FORMAT_ARGB8888);
}

void test_vg_lite_alpha_label_opa_77_on_translucent(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_LABEL_OPA_77, LV_TEST_ALPHA_BG_TRANSLUCENT, LV_COLOR_FORMAT_ARGB8888);
}

void test_vg_lite_alpha_label_opa_77_on_none_premultiplied(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_LABEL_OPA_77, LV_TEST_ALPHA_BG_NONE, LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED);
}

void test_vg_lite_alpha_label_opa_77_on_opaque_premultiplied(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_LABEL_OPA_77, LV_TEST_ALPHA_BG_OPAQUE, LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED);
}

void test_vg_lite_alpha_label_opa_77_on_translucent_premultiplied(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_LABEL_OPA_77, LV_TEST_ALPHA_BG_TRANSLUCENT,
                LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED);
}

void test_vg_lite_alpha_vector_opa_77_on_opaque(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_VECTOR_OPA_77, LV_TEST_ALPHA_BG_OPAQUE, LV_COLOR_FORMAT_ARGB8888);
}

void test_vg_lite_alpha_button_opa_dd_on_none_premultiplied(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_BUTTON_OPA_DD, LV_TEST_ALPHA_BG_NONE, LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED);
}

void test_vg_lite_alpha_button_opa_dd_on_translucent_premultiplied(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_BUTTON_OPA_DD, LV_TEST_ALPHA_BG_TRANSLUCENT,
                LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED);
}

void test_vg_lite_alpha_vector_opa_cover_on_opaque_premultiplied(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_VECTOR_OPA_COVER, LV_TEST_ALPHA_BG_OPAQUE, LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED);
}

void test_vg_lite_alpha_vector_opa_77_on_opaque_premultiplied(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_VECTOR_OPA_77, LV_TEST_ALPHA_BG_OPAQUE, LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED);
}

void test_vg_lite_alpha_vector_opa_cover_on_translucent_premultiplied(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_VECTOR_OPA_COVER, LV_TEST_ALPHA_BG_TRANSLUCENT,
                LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED);
}

void test_vg_lite_alpha_vector_opa_77_on_translucent_premultiplied(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_VECTOR_OPA_77, LV_TEST_ALPHA_BG_TRANSLUCENT,
                LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED);
}

/* Translucent results in a straight-alpha display buffer */

void test_vg_lite_alpha_obj_opa_77_on_none(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_OBJ_OPA_77, LV_TEST_ALPHA_BG_NONE, LV_COLOR_FORMAT_ARGB8888);
}

void test_vg_lite_alpha_obj_opa_77_on_translucent(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_OBJ_OPA_77, LV_TEST_ALPHA_BG_TRANSLUCENT, LV_COLOR_FORMAT_ARGB8888);
}

void test_vg_lite_alpha_button_opa_dd_on_none(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_BUTTON_OPA_DD, LV_TEST_ALPHA_BG_NONE, LV_COLOR_FORMAT_ARGB8888);
}

void test_vg_lite_alpha_button_opa_dd_on_translucent(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_BUTTON_OPA_DD, LV_TEST_ALPHA_BG_TRANSLUCENT, LV_COLOR_FORMAT_ARGB8888);
}

void test_vg_lite_alpha_vector_opa_77_on_none(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_VECTOR_OPA_77, LV_TEST_ALPHA_BG_NONE, LV_COLOR_FORMAT_ARGB8888);
}

void test_vg_lite_alpha_vector_opa_77_on_translucent(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_VECTOR_OPA_77, LV_TEST_ALPHA_BG_TRANSLUCENT, LV_COLOR_FORMAT_ARGB8888);
}

/* An application's own straight-alpha buffer: a translucent rectangle drawn on a translucent ARGB8888 canvas */
void test_vg_lite_alpha_argb8888_canvas(void)
{
#if LV_USE_DRAW_NANOVG
    TEST_IGNORE_MESSAGE("NanoVG's readback replaces the canvas' content");
#endif
    lv_draw_buf_t * buf = lv_draw_buf_create(100, 100, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO);
    TEST_ASSERT_NOT_NULL(buf);
    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());
    lv_canvas_set_draw_buf(canvas, buf);
    lv_canvas_fill_bg(canvas, lv_color_hex(0x00ff00), 0x80);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_hex(0xff0000);
    rect_dsc.bg_opa = 0x77;
    lv_area_t area = {40, 40, 59, 59};
    lv_draw_rect(&layer, &rect_dsc, &area);
    lv_canvas_finish_layer(canvas, &layer);

    TEST_ASSERT_FALSE_MESSAGE(lv_draw_buf_has_flag(buf, LV_IMAGE_FLAGS_PREMULTIPLIED), "premultiplied flag");

    /* Red @ 0x77 over green @ 0x80, source-over in straight alpha */
    lv_color32_t px = *(const lv_color32_t *)lv_draw_buf_goto_xy(buf, 50, 50);
    TEST_ASSERT_UINT8_WITHIN_MESSAGE(2, 0xa2, px.red, "red");
    TEST_ASSERT_UINT8_WITHIN_MESSAGE(2, 0x5d, px.green, "green");
    TEST_ASSERT_UINT8_WITHIN_MESSAGE(2, 0x00, px.blue, "blue");
    TEST_ASSERT_UINT8_WITHIN_MESSAGE(1, 0xbb, px.alpha, "alpha");

    /* Outside the rectangle the background stays as it was */
    px = *(const lv_color32_t *)lv_draw_buf_goto_xy(buf, 10, 10);
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x00, px.red, "red");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0xff, px.green, "green");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x00, px.blue, "blue");
    TEST_ASSERT_EQUAL_HEX8_MESSAGE(0x80, px.alpha, "alpha");

    lv_obj_delete(canvas);
    lv_draw_buf_destroy(buf);
}

#endif
