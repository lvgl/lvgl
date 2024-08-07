#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../lv_test_indev.h"
#include "unity/unity.h"

#define TEST_HOVER_COUNTS   20

typedef struct _test_hover_t {
    /* data */
    char id[32];
    uint32_t counts;
} test_hover_t;

static test_hover_t label_hovered;
static test_hover_t btn_hovered;

static const lv_point_t pointer1[] = {
    {0, 0}, {110, 20}, {150, 26}, {120, 19}, {0, 0},
};

static const lv_point_t pointer2[] = {
    {0, 0}, {60, 100}, {80, 100}, {120, 120}, {0, 0},
};

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

static void hovered_event_cb(lv_event_t * e)
{
    test_hover_t * hover = lv_event_get_user_data(e);
    lv_log("Object(ID:%s) hovered %u/%d times.\n", hover->id, hover->counts, TEST_HOVER_COUNTS);
}

static void test_move_mouse(lv_point_t * point, uint8_t size)
{
    lv_point_t * p = point;

    for(uint8_t j = 0; j < TEST_HOVER_COUNTS; j++) {
        for(uint8_t i = 0; i < size; i++) {
            lv_test_mouse_move_to(p[i].x, p[i].y);
            lv_test_indev_wait(50);
        }
    }
}

void test_hover_basic(void)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_obj_set_size(label, 200, 20);
    lv_label_set_text(label, "Clickable text can be hovered!");
    lv_obj_set_pos(label, 100, 20);
    lv_obj_add_flag(label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_text_color(label, lv_color_hex(0x5be1b6), LV_PART_MAIN | LV_STATE_HOVERED);

    /*Set hover callback*/
    lv_obj_stringify_id(label, label_hovered.id, sizeof(label_hovered.id));
    label_hovered.counts = 0;
    lv_obj_add_event_cb(label, hovered_event_cb, LV_EVENT_HOVER_OVER, &label_hovered);

    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_set_pos(btn, 64, 100);
    lv_obj_set_size(btn, 128, 48);
    lv_obj_set_style_bg_opa(btn, 128, LV_PART_MAIN | LV_STATE_HOVERED);

    /*Set hover callback*/
    lv_obj_stringify_id(btn, btn_hovered.id, sizeof(btn_hovered.id));
    btn_hovered.counts = 0;
    lv_obj_add_event_cb(btn, hovered_event_cb, LV_EVENT_HOVER_OVER, &btn_hovered);

    test_move_mouse((lv_point_t *)pointer1, 5);
    test_move_mouse((lv_point_t *)pointer2, 5);
}

void test_hover_delete(void)
{
    for(int i = 0; i < 4; i++) {
        lv_obj_t * btn = lv_button_create(lv_screen_active());
        lv_obj_set_size(btn, 200, 100);

        lv_test_mouse_move_to(i * 10, 50);
        lv_test_indev_wait(50);

        lv_obj_delete(btn);  /*No crash while deleting the hovered button*/
        lv_test_indev_wait(50);
    }
}


#endif
