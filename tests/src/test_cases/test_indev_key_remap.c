#if LV_BUILD_TEST
#include "../lvgl.h"
#include "unity/unity.h"
#include <assert.h>

#define MAX_KEYS 6

struct key_buffer {
    size_t count;
    lv_key_t keys[MAX_KEYS];
};

static void key_buffer_add(struct key_buffer * key_buffer, lv_key_t key)
{
    assert(key_buffer->count + 1 <= MAX_KEYS);

    key_buffer->keys[key_buffer->count++] = key;
}

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

static void event_cb(lv_event_t * e)
{
    struct key_buffer * key_buffer = lv_event_get_user_data(e);

    switch(lv_event_get_code(e)) {
        case LV_EVENT_KEY:
            key_buffer_add(key_buffer, lv_indev_get_key(lv_indev_active()));
            break;
        default:
            break;
    }
}

static lv_key_t remap_cb(lv_indev_t * i, lv_key_t key)
{
    LV_UNUSED(i);

    switch((int)key) {
        case 'a':
            return LV_KEY_LEFT;
        case LV_KEY_ESC:
            return 'Z';
        default:
            return key;
    }
}

void test_indev_key_remap(void)
{
    struct key_buffer key_buffer = {
        .count = 0,
    };

    lv_group_t * group = lv_group_create();
    lv_indev_t * indev = lv_test_indev_get_indev(LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_group(indev, group);

    lv_obj_t * btn = lv_button_create(lv_screen_active());
    lv_obj_set_size(btn, 100, 100);
    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_ALL, &key_buffer);
    lv_group_add_obj(group, btn);

    lv_test_key_hit('a');
    lv_test_key_hit(LV_KEY_ESC);

    /* Remap 'a' -> LV_KEY_LEFT, LV_KEY_ESC -> 'Z' */

    lv_indev_set_key_remap_cb(indev, remap_cb);

    lv_test_key_hit('a');
    lv_test_key_hit(LV_KEY_ESC);

    /* Disable remapping */

    lv_indev_set_key_remap_cb(indev, NULL);

    lv_test_key_hit('a');
    lv_test_key_hit(LV_KEY_ESC);

    TEST_ASSERT_EQUAL_UINT32(6, key_buffer.count);
    TEST_ASSERT_EQUAL_UINT32('a', key_buffer.keys[0]);
    TEST_ASSERT_EQUAL_UINT32(LV_KEY_ESC, key_buffer.keys[1]);
    TEST_ASSERT_EQUAL_UINT32(LV_KEY_LEFT, key_buffer.keys[2]);
    TEST_ASSERT_EQUAL_UINT32('Z', key_buffer.keys[3]);
    TEST_ASSERT_EQUAL_UINT32('a', key_buffer.keys[4]);
    TEST_ASSERT_EQUAL_UINT32(LV_KEY_ESC, key_buffer.keys[5]);
}

#endif
