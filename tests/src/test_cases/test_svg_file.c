#if LV_BUILD_TEST
#include "../lvgl.h"
#include "src/draw/lv_draw_vector.h"

#include "unity/unity.h"

#ifndef NON_AMD64_BUILD
    #define EXT_NAME ".lp64.png"
#else
    #define EXT_NAME ".lp32.png"
#endif

typedef struct {
    lv_svg_render_obj_t * draw_list;
    float scale;
} svg_test_ctx_t;

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

static void load_image_cb(const char * image_url, lv_draw_image_dsc_t * img_dsc)
{
    TEST_ASSERT_NOT_NULL(image_url);
    TEST_ASSERT_NOT_NULL(img_dsc);

    static char image_path[128];
    lv_snprintf(image_path, sizeof(image_path), "A:src/test_assets/svg/%s", image_url);

    lv_result_t res = lv_image_decoder_get_info(image_path, &img_dsc->header);
    TEST_ASSERT_EQUAL(LV_RESULT_OK, res);
    img_dsc->src = image_path;
}

static const char * get_font_path_cb(const char * font_family)
{
    TEST_ASSERT_NOT_NULL(font_family);
    return "./src/test_files/fonts/noto/NotoSansSC-Regular.ttf";
}

static void on_draw_event(lv_event_t * e)
{
    lv_layer_t * layer = lv_event_get_layer(e);
    lv_obj_t * obj = lv_event_get_current_target(e);

    int32_t w = lv_obj_get_width(obj);
    int32_t h = lv_obj_get_height(obj);
    svg_test_ctx_t * ctx = lv_event_get_user_data(e);

    lv_draw_vector_dsc_t * dsc = lv_draw_vector_dsc_create(layer);

    lv_area_t clear_area = {0, 0, w - 1, h - 1};
    lv_draw_vector_dsc_set_fill_color(dsc, lv_color_white());
    lv_draw_vector_dsc_clear_area(dsc, &clear_area);

    lv_matrix_t matrix;
    lv_matrix_identity(&matrix);
    lv_matrix_scale(&matrix, ctx->scale, ctx->scale);

    lv_draw_vector_dsc_set_transform(dsc, &matrix);
    lv_draw_svg_render(dsc, ctx->draw_list);

    lv_draw_vector(dsc);
    lv_draw_vector_dsc_delete(dsc);
}

static void test_svg_draw(const char * svg_name, float scale)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, lv_pct(100), lv_pct(100));

    static lv_svg_render_hal_t svg_render_hal = {
        .load_image = load_image_cb,
        .get_font_path = get_font_path_cb,
    };

    lv_svg_render_init(&svg_render_hal);

    char svg_path[128];
    lv_snprintf(svg_path, sizeof(svg_path), "A:src/test_assets/svg/%s.svg", svg_name);

    svg_test_ctx_t ctx = { 0 };
    ctx.scale = scale;

    uint32_t size;
    void * svg_data = lv_fs_load_with_alloc(svg_path, &size);
    TEST_ASSERT_NOT_NULL(svg_data);
    TEST_ASSERT_GREATER_THAN_UINT32(0, size);

    lv_svg_node_t * svg_doc = lv_svg_load_data(svg_data, size);
    TEST_ASSERT_NOT_NULL(svg_doc);
    ctx.draw_list = lv_svg_render_create(svg_doc);

    lv_obj_add_event_cb(obj, on_draw_event, LV_EVENT_DRAW_MAIN, &ctx);

    char ref_img_path[128];
    lv_snprintf(ref_img_path, sizeof(ref_img_path), "draw/svg_file_%s" EXT_NAME, svg_name);
    TEST_ASSERT_EQUAL_SCREENSHOT(ref_img_path);

    lv_obj_delete(obj);
    lv_svg_render_delete(ctx.draw_list);
    lv_svg_node_delete(svg_doc);
    lv_free(svg_data);
}

void test_svg_file(void)
{
    test_svg_draw("bg_with_text", 1.0f);
    test_svg_draw("tiger", 0.5f);
    test_svg_draw("g_general_attributes", 1.0f);
    test_svg_draw("g_exclusive_attributes", 1.0f);
    test_svg_draw("use_general_attributes", 1.0f);
    test_svg_draw("defs_exclusive_attributes", 1.0f);
    test_svg_draw("rect_general_attributes", 1.0f);
    test_svg_draw("circle_general_attributes", 1.0f);
    test_svg_draw("ellipse_general_attributes", 1.0f);
    test_svg_draw("line_general_attributes", 2.0f);
    test_svg_draw("polygon_general_attributes", 0.5f);
    test_svg_draw("path_general_attributes", 1.0f);
    test_svg_draw("solidcolor_general_attributes", 1.0f);
    test_svg_draw("linear_gradient_general_attributes", 1.0f);
    test_svg_draw("radial_gradient_general_attributes", 2.0f);
    test_svg_draw("text_general_attributes", 1.0f);
    test_svg_draw("tspan_general_attributes", 1.0f);
    test_svg_draw("image_general_attributes", 1.0f);
    test_svg_draw("image_general_attributes1", 1.0f);
    test_svg_draw("image_general_attributes2", 1.0f);
    test_svg_draw("image_general_attributes3", 1.0f);
}

#endif
