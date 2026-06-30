#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "unity/unity.h"

/*
 * Test: find_scroll_obj should not be affected by child object's
 * transform rotation when determining scroll direction for ancestors.
 *
 * Reproduces the bug where a rotated child causes hor_en/ver_en to be incorrectly accumulated,
 * preventing the scroll search from reaching the actual scrollable
 * parent (e.g. a horizontal swiper above a vertical list).
 *
 * Widget tree:
 *   screen
 *     └── container (horizontal scrollable, like swiperObj_)
 *           └── list_obj (vertical scrollable, no SCROLL_CHAIN_VER)
 *                 └── rotated_obj (transform: rotate 90deg)
 *                       └── inner_obj (touch target)
 *
 * Without the fix: horizontal swipe on inner_obj -> rotated_obj sets
 * ver_en=true -> list_obj breaks on no chain_ver -> container not found.
 *
 * With the fix: ver_en is reset at list_obj iteration -> container found.
 */

static lv_obj_t * container;
static lv_obj_t * list_obj;
static lv_obj_t * rotated_obj;
static lv_obj_t * inner_obj;

static bool scroll_begin_called;

static void scroll_begin_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    scroll_begin_called = true;
}

void setUp(void)
{
    scroll_begin_called = false;

    /* Container: horizontal scrollable (simulates swiperObj_) */
    container = lv_obj_create(lv_screen_active());
    lv_obj_set_size(container, 200, 200);
    lv_obj_center(container);
    lv_obj_add_flag(container, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(container, LV_DIR_HOR);
    lv_obj_add_event_cb(container, scroll_begin_cb, LV_EVENT_SCROLL_BEGIN, NULL);

    /* Wide content to make container horizontally scrollable */
    lv_obj_t * wide_content = lv_obj_create(container);
    lv_obj_set_size(wide_content, 600, 200);

    /* List: vertical scrollable, remove SCROLL_CHAIN_VER (like widget_list) */
    list_obj = lv_obj_create(container);
    lv_obj_set_size(list_obj, 200, 200);
    lv_obj_add_flag(list_obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scroll_dir(list_obj, LV_DIR_VER);
    lv_obj_remove_flag(list_obj, LV_OBJ_FLAG_SCROLL_CHAIN_VER);

    /* Rotated container (like barcode div with rotate 90deg) */
    rotated_obj = lv_obj_create(list_obj);
    lv_obj_set_size(rotated_obj, 100, 50);
    lv_obj_center(rotated_obj);
    lv_obj_set_style_transform_rotation(rotated_obj, 900, LV_PART_MAIN);
    lv_obj_remove_flag(rotated_obj, LV_OBJ_FLAG_SCROLLABLE);

    /* Inner touch target (like barcode obj) */
    inner_obj = lv_obj_create(rotated_obj);
    lv_obj_set_size(inner_obj, 80, 30);
    lv_obj_center(inner_obj);
    lv_obj_remove_flag(inner_obj, LV_OBJ_FLAG_SCROLLABLE);

    lv_test_wait(50);
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

void test_scroll_through_rotated_child(void)
{
    /* Simulate horizontal swipe on inner_obj area */
    int32_t cx = (inner_obj->coords.x1 + inner_obj->coords.x2) / 2;
    int32_t cy = (inner_obj->coords.y1 + inner_obj->coords.y2) / 2;

    lv_test_mouse_move_to(cx, cy);
    lv_test_mouse_press();
    lv_test_wait(50);

    /* Horizontal drag: move right in multiple steps */
    int i;
    for(i = 0; i < 10; i++) {
        lv_test_mouse_move_by(10, 0);
        lv_test_wait(16);
    }

    lv_test_mouse_release();
    lv_test_wait(50);

    /* The container should have received the scroll event.
     * Before the fix, scroll_begin_called would be false because
     * find_scroll_obj broke at list_obj due to polluted ver_en. */
    TEST_ASSERT_TRUE_MESSAGE(scroll_begin_called,
                             "Container should receive scroll when swiping horizontally "
                             "through a rotated child object");
}

void test_scroll_no_rotation_still_works(void)
{
    /* Remove rotation to verify normal case is unaffected */
    lv_obj_set_style_transform_rotation(rotated_obj, 0, LV_PART_MAIN);
    lv_test_wait(50);

    int32_t cx = (inner_obj->coords.x1 + inner_obj->coords.x2) / 2;
    int32_t cy = (inner_obj->coords.y1 + inner_obj->coords.y2) / 2;

    lv_test_mouse_move_to(cx, cy);
    lv_test_mouse_press();
    lv_test_wait(50);

    int i;
    for(i = 0; i < 10; i++) {
        lv_test_mouse_move_by(10, 0);
        lv_test_wait(16);
    }

    lv_test_mouse_release();
    lv_test_wait(50);

    TEST_ASSERT_TRUE_MESSAGE(scroll_begin_called,
                             "Container should receive scroll in non-rotated case");
}

#endif
