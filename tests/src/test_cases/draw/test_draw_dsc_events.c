#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static uint32_t begin_cnt;
static uint32_t end_cnt;
static lv_draw_dsc_t * begin_dsc;

void setUp(void)
{
    begin_cnt = 0;
    end_cnt = 0;
    begin_dsc = NULL;
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

static void fill(lv_event_t * e, lv_color_t color, const lv_area_t * area)
{
    lv_obj_t * obj = lv_event_get_target_obj(e);
    lv_draw_dsc_t * dsc = lv_event_get_draw_dsc(e);

    lv_draw_fill_dsc_t fill_dsc;
    lv_draw_fill_dsc_init(&fill_dsc);
    fill_dsc.color = color;
    /* Draws made in a draw event mustn't send draw events again, even for the same widget */
    fill_dsc.base.obj = obj;
    lv_draw_fill(dsc->layer, &fill_dsc, area);
}

/* The widget is a red square. The green square drawn in LV_EVENT_DRAW_DSC_BEGIN overlaps its top left quarter and
 * goes below it, the blue square drawn in LV_EVENT_DRAW_DSC_END overlaps its bottom right quarter and goes on top. */
static void begin_cb(lv_event_t * e)
{
    lv_draw_dsc_t * dsc = lv_event_get_draw_dsc(e);
    begin_cnt++;
    begin_dsc = dsc;
    if(lv_draw_dsc_get_type(dsc) != LV_DRAW_DSC_TYPE_FILL) return;

    lv_area_t area = lv_draw_dsc_get_fill_dsc(dsc)->coords;
    lv_area_move(&area, -lv_area_get_width(&area) / 2, -lv_area_get_height(&area) / 2);
    fill(e, lv_color_hex(0x00ff00), &area);
}

static void end_cb(lv_event_t * e)
{
    lv_draw_dsc_t * dsc = lv_event_get_draw_dsc(e);
    end_cnt++;
    TEST_ASSERT_EQUAL_PTR(begin_dsc, dsc);
    if(lv_draw_dsc_get_type(dsc) != LV_DRAW_DSC_TYPE_FILL) return;

    lv_area_t area = lv_draw_dsc_get_fill_dsc(dsc)->coords;
    lv_area_move(&area, lv_area_get_width(&area) / 2, lv_area_get_height(&area) / 2);
    fill(e, lv_color_hex(0x0000ff), &area);
}

static void ext_draw_size_cb(lv_event_t * e)
{
    /* Let the squares drawn in the events be drawn outside of the widget */
    int32_t * s = lv_event_get_param(e);
    *s = LV_MAX(*s, 50);
}

static lv_obj_t * create_obj(lv_color_t color)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, 100, 100);
    lv_obj_center(obj);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, color, 0);
    lv_obj_add_event_cb(obj, ext_draw_size_cb, LV_EVENT_REFR_EXT_DRAW_SIZE, NULL);
    lv_obj_refresh_ext_draw_size(obj);
    lv_obj_set_send_draw_dsc_events(obj, true);
    return obj;
}

void test_draw_dsc_events_order(void)
{
    lv_obj_t * obj = create_obj(lv_color_hex(0xff0000));
    lv_obj_add_event_cb(obj, begin_cb, LV_EVENT_DRAW_DSC_BEGIN, NULL);
    lv_obj_add_event_cb(obj, end_cb, LV_EVENT_DRAW_DSC_END, NULL);
    lv_refr_now(NULL);

    /* Only the background fill sends events, the fills drawn in the events don't */
    TEST_ASSERT_EQUAL_UINT32(1, begin_cnt);
    TEST_ASSERT_EQUAL_UINT32(1, end_cnt);

    TEST_ASSERT_EQUAL_SCREENSHOT("draw/draw_dsc_events_order.png");
}

static void move_begin_cb(lv_event_t * e)
{
    lv_draw_dsc_t * dsc = lv_event_get_draw_dsc(e);
    if(lv_draw_dsc_get_type(dsc) != LV_DRAW_DSC_TYPE_FILL) return;

    /* Changing the area in LV_EVENT_DRAW_DSC_BEGIN resizes the draw (within the widget's clip area) */
    lv_draw_dsc_get_fill_dsc(dsc)->coords.x1 += 50;
}

static void move_end_cb(lv_event_t * e)
{
    lv_draw_dsc_t * dsc = lv_event_get_draw_dsc(e);
    if(lv_draw_dsc_get_type(dsc) != LV_DRAW_DSC_TYPE_FILL) return;

    lv_area_t area = lv_draw_dsc_get_fill_dsc(dsc)->coords;
    TEST_ASSERT_EQUAL_INT32(lv_event_get_target_obj(e)->coords.x1 + 50, area.x1);
}

void test_draw_dsc_events_move(void)
{
    lv_obj_t * obj = create_obj(lv_color_hex(0x00ff00));
    lv_obj_add_event_cb(obj, move_begin_cb, LV_EVENT_DRAW_DSC_BEGIN, NULL);
    lv_obj_add_event_cb(obj, move_end_cb, LV_EVENT_DRAW_DSC_END, NULL);

    /* Only the right half of the widget is drawn */
    TEST_ASSERT_EQUAL_SCREENSHOT("draw/draw_dsc_events_move.png");
}

static void resize_image_begin_cb(lv_event_t * e)
{
    lv_draw_dsc_t * dsc = lv_event_get_draw_dsc(e);
    if(lv_draw_dsc_get_type(dsc) != LV_DRAW_DSC_TYPE_IMAGE) return;
    begin_cnt++;

    /* Draw only the left 60 px of the image */
    lv_draw_dsc_get_image_dsc(dsc)->coords.x2 -= 40;
}

static void resize_image_end_cb(lv_event_t * e)
{
    lv_draw_dsc_t * dsc = lv_event_get_draw_dsc(e);
    if(lv_draw_dsc_get_type(dsc) != LV_DRAW_DSC_TYPE_IMAGE) return;
    end_cnt++;
    lv_draw_image_dsc_t * image_dsc = lv_draw_dsc_get_image_dsc(dsc);

    /* The task is the last one of the layer */
    lv_draw_task_t * t = dsc->layer->draw_task_head;
    while(t->next) t = t->next;
    TEST_ASSERT_EQUAL_PTR(dsc, t->draw_dsc);

    /* The task follows the new area */
    TEST_ASSERT_EQUAL_MEMORY(&image_dsc->coords, &t->area, sizeof(lv_area_t));
    lv_area_t expected;
    lv_draw_image_get_transformed_area(&expected, image_dsc, &image_dsc->coords);
    TEST_ASSERT_EQUAL_MEMORY(&expected, &t->_real_area, sizeof(lv_area_t));

    /* The image area wasn't set, so it follows the drawn area */
    TEST_ASSERT_EQUAL_MEMORY(&image_dsc->coords, &image_dsc->image_area, sizeof(lv_area_t));
}

void test_draw_dsc_events_resize_rotated_image(void)
{
    LV_IMAGE_DECLARE(test_image_cogwheel_rgb565a8);
    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_image_set_src(img, &test_image_cogwheel_rgb565a8);
    lv_image_set_rotation(img, 450);
    lv_obj_center(img);
    lv_obj_set_send_draw_dsc_events(img, true);
    lv_obj_add_event_cb(img, resize_image_begin_cb, LV_EVENT_DRAW_DSC_BEGIN, NULL);
    lv_obj_add_event_cb(img, resize_image_end_cb, LV_EVENT_DRAW_DSC_END, NULL);

    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL_UINT32(1, begin_cnt);
    TEST_ASSERT_EQUAL_UINT32(1, end_cnt);
}

/* Mask a child layer and draw it back, like a widget with clip_corner does */
static void draw_main_mask_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target_obj(e);
    lv_layer_t * layer = lv_event_get_layer(e);
    lv_layer_t * child = lv_draw_layer_create(layer, LV_COLOR_FORMAT_ARGB8888, &obj->coords);

    lv_draw_mask_rect_dsc_t dsc;
    lv_draw_mask_rect_dsc_init(&dsc);
    dsc.base.obj = obj;
    dsc.area = obj->coords;
    dsc.radius = 10;
    lv_draw_mask_rect(child, &dsc);

    lv_draw_image_dsc_t img;
    lv_draw_image_dsc_init(&img);
    img.src = child;
    lv_draw_layer(layer, &img, &obj->coords);
}

static uint32_t mask_cnt;

static void mask_begin_cb(lv_event_t * e)
{
    if(lv_draw_dsc_get_type(lv_event_get_draw_dsc(e)) == LV_DRAW_DSC_TYPE_MASK_RECTANGLE) mask_cnt++;
}

void test_draw_dsc_events_mask_sent_once(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_send_draw_dsc_events(obj, true);
    lv_obj_add_event_cb(obj, mask_begin_cb, LV_EVENT_DRAW_DSC_BEGIN, NULL);
    lv_obj_add_event_cb(obj, draw_main_mask_cb, LV_EVENT_DRAW_MAIN, NULL);

    mask_cnt = 0;
    lv_refr_now(NULL);
    TEST_ASSERT_EQUAL_UINT32(1, mask_cnt);
}

#endif
