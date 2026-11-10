#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "alpha/lv_test_alpha_scene.h"

#include "unity/unity.h"

/* An ordinary semi-transparent widget on a transparent screen, drawn over a
 * background (see lv_test_alpha_scene.h), and its pixel read back by the
 * display buffer's color format. */

#if LV_USE_TINY_TTF
    /* Larger glyphs than the largest built-in font */
    #define LABEL_FONT_SIZE 120

    static lv_font_t * label_font;
#endif

void setUp(void)
{
    lv_display_set_color_format(lv_display_get_default(), LV_COLOR_FORMAT_ARGB8888);
#if LV_USE_TINY_TTF
    extern const uint8_t test_ubuntu_font[];
    extern size_t test_ubuntu_font_size;
    label_font = lv_tiny_ttf_create_data(test_ubuntu_font, test_ubuntu_font_size, LABEL_FONT_SIZE);
    lv_test_alpha_scene_set_label_font(label_font);
#endif
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
#if LV_USE_TINY_TTF
    lv_test_alpha_scene_set_label_font(&lv_font_montserrat_48);
    lv_tiny_ttf_destroy(label_font);
#endif
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

    lv_test_alpha_pixel_t px = lv_test_alpha_scene_read(buf, point);
    print_pixel(buf, &px, expected);
    /* Blending two colors rounds once more than drawing on nothing. Alpha has
     * to be exact where nothing is blended. */
    uint8_t color_tolerance = bg == LV_TEST_ALPHA_BG_NONE ? 1 : 2;
    uint8_t alpha_tolerance = bg == LV_TEST_ALPHA_BG_TRANSLUCENT ? 1 : 0;
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

void test_vg_lite_alpha_label_opa_77_on_opaque(void)
{
    assert_case(LV_TEST_ALPHA_WIDGET_LABEL_OPA_77, LV_TEST_ALPHA_BG_OPAQUE, LV_COLOR_FORMAT_ARGB8888);
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

#endif
