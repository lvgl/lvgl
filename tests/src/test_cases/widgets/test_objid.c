#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"
#include <string.h>

void test_obj_id_should_match_class_name(void)
{
    char buf[128];
    lv_obj_t * obj = lv_obj_create(NULL);
    lv_obj_stringify_id(obj, buf, sizeof(buf));

    TEST_ASSERT_TRUE(strncmp("lv_obj", buf, strlen("lv_obj")) == 0);

    lv_obj_t * img = lv_image_create(NULL);
    lv_obj_stringify_id(img, buf, sizeof(buf));
    TEST_ASSERT_TRUE(strncmp("lv_image", buf, strlen("lv_image")) == 0);
}

void test_obj_id_should_grow_by_one(void)
{
    uint32_t id1, id2;
    lv_obj_t * obj1 = lv_label_create(NULL);
    id1 = (lv_uintptr_t)obj1->id;
    lv_obj_t * obj2 = lv_label_create(NULL);
    id2 = (lv_uintptr_t)obj2->id;
    TEST_ASSERT_EQUAL(id1 + 1, id2);
}

void test_obj_id_get_child(void)
{
    lv_obj_t * parent = lv_obj_create(lv_screen_active());
    lv_obj_t * child = lv_label_create(parent);
    lv_obj_t * grandchild = lv_label_create(child);

    lv_obj_set_id(child, (void *)(lv_uintptr_t)1);
    lv_obj_set_id(grandchild, (void *)(lv_uintptr_t)2);

    TEST_ASSERT_EQUAL_PTR(child, lv_obj_find_by_id(NULL, (void *)(lv_uintptr_t)1));
    TEST_ASSERT_EQUAL_PTR(grandchild, lv_obj_find_by_id(NULL, (void *)(lv_uintptr_t)2));
}

#endif
