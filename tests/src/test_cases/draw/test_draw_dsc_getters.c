#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

static uint32_t type_cnt[16];
static lv_obj_t * main_obj;

void setUp(void)
{
    lv_memzero(type_cnt, sizeof(type_cnt));
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

static void draw_dsc_begin_cb(lv_event_t * e)
{
    lv_draw_dsc_t * dsc = lv_event_get_draw_dsc(e);
    lv_draw_dsc_type_t type = lv_draw_dsc_get_type(dsc);

    void * typed = NULL;
    switch(type) {
        case LV_DRAW_DSC_TYPE_FILL:
            typed = lv_draw_dsc_get_fill_dsc(dsc);
            break;
        case LV_DRAW_DSC_TYPE_BORDER:
            typed = lv_draw_dsc_get_border_dsc(dsc);
            break;
        case LV_DRAW_DSC_TYPE_BOX_SHADOW:
            typed = lv_draw_dsc_get_box_shadow_dsc(dsc);
            break;
        case LV_DRAW_DSC_TYPE_LABEL:
            typed = lv_draw_dsc_get_label_dsc(dsc);
            break;
        case LV_DRAW_DSC_TYPE_LINE:
            typed = lv_draw_dsc_get_line_dsc(dsc);
            break;
        case LV_DRAW_DSC_TYPE_ARC:
            typed = lv_draw_dsc_get_arc_dsc(dsc);
            break;
        default:
            typed = dsc;
            break;
    }
    TEST_ASSERT_EQUAL_PTR(dsc, typed);

    /* Without a border, the background of a widget covers the widget */
    if(dsc->obj == main_obj && dsc->part == LV_PART_MAIN && type == LV_DRAW_DSC_TYPE_FILL) {
        TEST_ASSERT_EQUAL_MEMORY(&main_obj->coords, &lv_draw_dsc_get_fill_dsc(dsc)->coords, sizeof(lv_area_t));
    }

    if(type < sizeof(type_cnt) / sizeof(type_cnt[0])) type_cnt[type]++;
}

static void enable_events(lv_obj_t * obj)
{
    lv_obj_set_send_draw_dsc_events(obj, true);
    lv_obj_add_event_cb(obj, draw_dsc_begin_cb, LV_EVENT_DRAW_DSC_BEGIN, NULL);
}

void test_draw_dsc_getters(void)
{
    main_obj = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(main_obj);
    lv_obj_set_size(main_obj, 100, 100);
    lv_obj_set_style_bg_opa(main_obj, LV_OPA_COVER, 0);
    lv_obj_set_style_shadow_width(main_obj, 10, 0);
    lv_obj_set_style_outline_width(main_obj, 2, 0);
    enable_events(main_obj);

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
