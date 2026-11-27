#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"
#define CANVAS_SIZE     40

#define DRAWN_COLOR     lv_color_hex(0xff0000)
#define REFUSED_COLOR   lv_color_hex(0x00ff00)

static lv_draw_unit_t * fake_unit;
static lv_draw_unit_t * sw_unit;
static lv_obj_t * canvas;
static lv_draw_buf_t * canvas_buf;

static void fake_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                           const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(layer);
    LV_UNUSED(clip_area);
    LV_UNUSED(dsc);
    LV_UNUSED(coords);
}

static bool fake_can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(layer);
    return !lv_color_eq(dsc->color, REFUSED_COLOR);
}

static void fake_draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                            const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(layer);
    LV_UNUSED(clip_area);
    LV_UNUSED(dsc);
    LV_UNUSED(coords);
}

static const lv_draw_unit_draw_cbs_t fake_draw_cbs = {
    .draw_fill_cb = fake_draw_fill,
    .can_draw_fill_cb = fake_can_draw_fill,
    .draw_image_cb = fake_draw_image,
    .draw_layer_cb = fake_draw_image,
};

static void fake_unit_create(void)
{
    fake_unit = lv_draw_create_unit(sizeof(lv_draw_unit_t));
    fake_unit->name = "FAKE";
    fake_unit->draw_cbs = &fake_draw_cbs;
    fake_unit->fallback = sw_unit;
}

static void fake_unit_delete(void)
{
    lv_draw_global_info_t * info = &LV_GLOBAL_DEFAULT()->draw_info;
    lv_draw_unit_t ** u = &info->unit_head;
    while(*u && *u != fake_unit) u = &(*u)->next;
    TEST_ASSERT_NOT_NULL(*u);
    *u = fake_unit->next;
    info->unit_cnt--;
    lv_free(fake_unit);
    fake_unit = NULL;
}

void setUp(void)
{
    sw_unit = lv_draw_sw_get_draw_unit();

    canvas_buf = lv_draw_buf_create(CANVAS_SIZE, CANVAS_SIZE, LV_COLOR_FORMAT_ARGB8888, 0);
    canvas = lv_canvas_create(lv_screen_active());
    lv_canvas_set_draw_buf(canvas, canvas_buf);
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);

    fake_unit_create();
    lv_draw_reset_counters();
}

void tearDown(void)
{
    fake_unit_delete();
    lv_obj_clean(lv_screen_active());
    canvas = NULL;
    lv_draw_buf_destroy(canvas_buf);
    canvas_buf = NULL;
    lv_draw_reset_counters();
}

static const lv_area_t full_area = {0, 0, CANVAS_SIZE - 1, CANVAS_SIZE - 1};

static void fill(lv_layer_t * layer, lv_color_t color, const lv_area_t * area)
{
    lv_draw_fill_dsc_t dsc;
    lv_draw_fill_dsc_init(&dsc);
    dsc.color = color;
    lv_draw_fill(layer, &dsc, area);
}

static void image(lv_layer_t * layer)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_rgb565a8);
    lv_draw_image_dsc_t dsc;
    lv_draw_image_dsc_init(&dsc);
    dsc.src = &test_image_cogwheel_rgb565a8;
    lv_draw_image(layer, &dsc, &full_area);
}

static void assert_counts(lv_draw_dsc_type_t type, uint32_t fake, uint32_t sw, uint32_t dropped)
{
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(fake, lv_draw_unit_get_draw_count(fake_unit, type), "main unit");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(sw, lv_draw_unit_get_draw_count(sw_unit, type), "fallback unit");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(dropped, lv_draw_get_dropped_count(type), "dropped");
}

void test_draw_counters_start_at_zero(void)
{
    assert_counts(LV_DRAW_DSC_TYPE_FILL, 0, 0, 0);
    assert_counts(LV_DRAW_DSC_TYPE_IMAGE, 0, 0, 0);
    assert_counts(LV_DRAW_DSC_TYPE_BORDER, 0, 0, 0);
}

void test_draw_counters_count_the_main_unit(void)
{
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    fill(&layer, DRAWN_COLOR, &full_area);
    lv_canvas_finish_layer(canvas, &layer);

    assert_counts(LV_DRAW_DSC_TYPE_FILL, 1, 0, 0);
}

void test_draw_counters_count_per_type(void)
{
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    fill(&layer, DRAWN_COLOR, &full_area);
    fill(&layer, DRAWN_COLOR, &full_area);
    image(&layer);
    lv_canvas_finish_layer(canvas, &layer);

    assert_counts(LV_DRAW_DSC_TYPE_FILL, 2, 0, 0);
    assert_counts(LV_DRAW_DSC_TYPE_IMAGE, 1, 0, 0);
    assert_counts(LV_DRAW_DSC_TYPE_BORDER, 0, 0, 0);
}

void test_draw_counters_count_the_fallback(void)
{
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    fill(&layer, REFUSED_COLOR, &full_area);
    lv_canvas_finish_layer(canvas, &layer);

    /* The main unit refused the fill: it counts for the unit that drew it */
    assert_counts(LV_DRAW_DSC_TYPE_FILL, 0, 1, 0);
}

void test_draw_counters_count_mixed_main_and_fallback(void)
{
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    fill(&layer, DRAWN_COLOR, &full_area);
    fill(&layer, REFUSED_COLOR, &full_area);
    fill(&layer, DRAWN_COLOR, &full_area);
    lv_canvas_finish_layer(canvas, &layer);

    assert_counts(LV_DRAW_DSC_TYPE_FILL, 2, 1, 0);
}

void test_draw_counters_count_dropped(void)
{
    fake_unit->fallback = NULL;

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    fill(&layer, REFUSED_COLOR, &full_area);
    lv_canvas_finish_layer(canvas, &layer);

    /* No unit drew it: it counts as dropped and for no unit */
    assert_counts(LV_DRAW_DSC_TYPE_FILL, 0, 0, 1);
}

void test_draw_counters_count_each_part_of_a_rect(void)
{
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_rect_dsc_t dsc;
    lv_draw_rect_dsc_init(&dsc);
    dsc.bg_color = DRAWN_COLOR;
    dsc.bg_opa = LV_OPA_COVER;
    dsc.border_color = lv_color_white();
    dsc.border_width = 2;
    dsc.border_opa = LV_OPA_COVER;
    lv_draw_rect(&layer, &dsc, &full_area);
    lv_canvas_finish_layer(canvas, &layer);

    /* The fill goes to the main unit, the border (which it can't draw) to the fallback */
    assert_counts(LV_DRAW_DSC_TYPE_FILL, 1, 0, 0);
    assert_counts(LV_DRAW_DSC_TYPE_BORDER, 0, 1, 0);
}

void test_draw_counters_count_a_child_layer(void)
{
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    const lv_area_t area = {5, 5, 34, 34};
    lv_layer_t * child = lv_draw_layer_create(&layer, LV_COLOR_FORMAT_ARGB8888, &area);
    TEST_ASSERT_NOT_NULL(child);
    fill(child, DRAWN_COLOR, &area);

    lv_draw_image_dsc_t dsc;
    lv_draw_image_dsc_init(&dsc);
    dsc.src = child;
    lv_draw_layer(&layer, &dsc, &area);
    lv_canvas_finish_layer(canvas, &layer);

    /* The fill on the child layer and the blending of the child both count */
    assert_counts(LV_DRAW_DSC_TYPE_FILL, 1, 0, 0);
    assert_counts(LV_DRAW_DSC_TYPE_LAYER, 1, 0, 0);
}

void test_draw_counters_count_a_refresh(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 50, 50);
    lv_obj_set_style_bg_color(obj, DRAWN_COLOR, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_refr_now(NULL);
    lv_draw_reset_counters();

    lv_obj_invalidate(obj);
    lv_refr_now(NULL);

    /* Draws made by a display refresh count too: at least the object's background on the main unit */
    TEST_ASSERT_GREATER_OR_EQUAL_UINT32(1, lv_draw_unit_get_draw_count(fake_unit, LV_DRAW_DSC_TYPE_FILL));
    TEST_ASSERT_EQUAL_UINT32(0, lv_draw_get_dropped_count(LV_DRAW_DSC_TYPE_FILL));
}

void test_draw_counters_reset(void)
{
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    fill(&layer, DRAWN_COLOR, &full_area);
    fill(&layer, REFUSED_COLOR, &full_area);
    image(&layer);
    lv_canvas_finish_layer(canvas, &layer);

    fake_unit->fallback = NULL;
    lv_canvas_init_layer(canvas, &layer);
    fill(&layer, REFUSED_COLOR, &full_area);
    lv_canvas_finish_layer(canvas, &layer);

    lv_draw_reset_counters();

    /* Every counter of every unit and the dropped counters are back to 0 */
    assert_counts(LV_DRAW_DSC_TYPE_FILL, 0, 0, 0);
    assert_counts(LV_DRAW_DSC_TYPE_IMAGE, 0, 0, 0);
}

void test_draw_counters_keep_counting_after_reset(void)
{
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    fill(&layer, DRAWN_COLOR, &full_area);
    lv_canvas_finish_layer(canvas, &layer);

    lv_draw_reset_counters();

    lv_canvas_init_layer(canvas, &layer);
    fill(&layer, DRAWN_COLOR, &full_area);
    lv_canvas_finish_layer(canvas, &layer);

    assert_counts(LV_DRAW_DSC_TYPE_FILL, 1, 0, 0);
}

#endif
