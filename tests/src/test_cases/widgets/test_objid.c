#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void test_obj_id_should_match_class_name(void)
{
    char buf[128];
    lv_obj_t * obj = lv_obj_create(NULL);
    lv_obj_stringify_id(obj, buf, sizeof(buf));

    TEST_ASSERT_TRUE(strncmp("obj", buf, strlen("obj")) == 0);

    lv_obj_t * img = lv_image_create(NULL);
    lv_obj_stringify_id(img, buf, sizeof(buf));
    TEST_ASSERT_TRUE(strncmp("image", buf, strlen("image")) == 0);
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

#endif
