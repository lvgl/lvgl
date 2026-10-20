#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static uint32_t checked_cnt;
static uint32_t type_cnt[16];

void setUp(void)
{
    checked_cnt = 0;
    lv_memzero(type_cnt, sizeof(type_cnt));
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

static void draw_task_added_cb(lv_event_t * e)
{
    lv_draw_task_t * t = lv_event_get_draw_task(e);
    lv_draw_dsc_t * dsc = lv_draw_task_get_draw_dsc(t);
    lv_draw_dsc_type_t type = lv_draw_task_get_type(t);

    TEST_ASSERT_EQUAL(type, lv_draw_dsc_get_type(dsc));

    /* The types drawn to an area have it in their own descriptor */
    lv_area_t task_area;
    lv_draw_task_get_area(t, &task_area);
    const lv_area_t * coords = NULL;
    if(type == LV_DRAW_DSC_TYPE_FILL) coords = &lv_draw_dsc_get_fill_dsc(dsc)->coords;
    else if(type == LV_DRAW_DSC_TYPE_BORDER) coords = &lv_draw_dsc_get_border_dsc(dsc)->coords;
    else if(type == LV_DRAW_DSC_TYPE_BOX_SHADOW) coords = &lv_draw_dsc_get_box_shadow_dsc(dsc)->coords;
    else if(type == LV_DRAW_DSC_TYPE_LABEL) coords = &lv_draw_dsc_get_label_dsc(dsc)->coords;
    if(coords) TEST_ASSERT_EQUAL_MEMORY(&task_area, coords, sizeof(lv_area_t));

    void * typed = NULL;
    switch(type) {
        case LV_DRAW_DSC_TYPE_FILL:
            typed = lv_draw_dsc_get_fill_dsc(dsc);
            TEST_ASSERT_EQUAL_PTR(lv_draw_task_get_fill_dsc(t), typed);
            break;
        case LV_DRAW_DSC_TYPE_BORDER:
            typed = lv_draw_dsc_get_border_dsc(dsc);
            TEST_ASSERT_EQUAL_PTR(lv_draw_task_get_border_dsc(t), typed);
            break;
        case LV_DRAW_DSC_TYPE_BOX_SHADOW:
            typed = lv_draw_dsc_get_box_shadow_dsc(dsc);
            TEST_ASSERT_EQUAL_PTR(lv_draw_task_get_box_shadow_dsc(t), typed);
            break;
        case LV_DRAW_DSC_TYPE_LABEL:
            typed = lv_draw_dsc_get_label_dsc(dsc);
            TEST_ASSERT_EQUAL_PTR(lv_draw_task_get_label_dsc(t), typed);
            break;
        case LV_DRAW_DSC_TYPE_LINE:
            typed = lv_draw_dsc_get_line_dsc(dsc);
            TEST_ASSERT_EQUAL_PTR(lv_draw_task_get_line_dsc(t), typed);
            break;
        case LV_DRAW_DSC_TYPE_ARC:
            typed = lv_draw_dsc_get_arc_dsc(dsc);
            TEST_ASSERT_EQUAL_PTR(lv_draw_task_get_arc_dsc(t), typed);
            break;
        default:
            typed = dsc;
            break;
    }
    TEST_ASSERT_EQUAL_PTR(dsc, typed);

    checked_cnt++;
    if(type < sizeof(type_cnt) / sizeof(type_cnt[0])) type_cnt[type]++;
}

static void enable_events(lv_obj_t * obj)
{
    lv_obj_set_send_draw_dsc_events(obj, true);
    lv_obj_add_event_cb(obj, draw_task_added_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
}

void test_draw_dsc_getters(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_set_style_shadow_width(obj, 10, 0);
    lv_obj_set_style_outline_width(obj, 2, 0);
    enable_events(obj);

    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_set_pos(label, 300, 10);
    lv_label_set_text(label, "Hello");
    enable_events(label);

    static lv_point_precise_t points[] = {{0, 0}, {50, 30}, {100, 0}};
    lv_obj_t * line = lv_line_create(lv_screen_active());
    lv_obj_set_pos(line, 10, 300);
    lv_line_set_points(line, points, 3);
    enable_events(line);

    lv_obj_t * arc = lv_arc_create(lv_screen_active());
    lv_obj_set_pos(arc, 300, 200);
    enable_events(arc);

    lv_refr_now(NULL);

    TEST_ASSERT_GREATER_THAN(0, type_cnt[LV_DRAW_DSC_TYPE_FILL]);
    TEST_ASSERT_GREATER_THAN(0, type_cnt[LV_DRAW_DSC_TYPE_BORDER]);
    TEST_ASSERT_GREATER_THAN(0, type_cnt[LV_DRAW_DSC_TYPE_BOX_SHADOW]);
    TEST_ASSERT_GREATER_THAN(0, type_cnt[LV_DRAW_DSC_TYPE_LABEL]);
    TEST_ASSERT_GREATER_THAN(0, type_cnt[LV_DRAW_DSC_TYPE_LINE]);
    TEST_ASSERT_GREATER_THAN(0, type_cnt[LV_DRAW_DSC_TYPE_ARC]);
}

#endif
