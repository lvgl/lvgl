#if LV_BUILD_TEST
#include "../lvgl.h"

#include <stdlib.h>
#include "unity/unity.h"

void test_image_property(void)
{
    lv_obj_t * obj = lv_image_create(lv_screen_active());
    const char * src = LV_SYMBOL_OK;
    lv_image_set_src(obj, src);
    lv_property_t prop;
    prop = lv_obj_get_property(obj, LV_PROPERTY_IMAGE_SRC);
    TEST_ASSERT_TRUE(LV_PROPERTY_ID_TYPE(prop.id) == LV_PROPERTY_TYPE_IMGSRC);
    TEST_ASSERT_EQUAL_STRING(prop.ptr, src);

    lv_point_t point = {0xaa, 0x55};
    prop.id = LV_PROPERTY_IMAGE_PIVOT;
    prop.ptr = &point;
    lv_obj_set_property(obj, &prop);
    TEST_ASSERT_TRUE(LV_PROPERTY_ID_TYPE(prop.id) == LV_PROPERTY_TYPE_POINTER);
    TEST_ASSERT_TRUE(prop.ptr == &point);

    static const lv_prop_id_t int_ids[] = {
        LV_PROPERTY_IMAGE_OFFSET_X,
        LV_PROPERTY_IMAGE_OFFSET_Y,
        LV_PROPERTY_IMAGE_ROTATION,
        LV_PROPERTY_IMAGE_SCALE,
        LV_PROPERTY_IMAGE_SCALE_X,
        LV_PROPERTY_IMAGE_SCALE_Y,
        LV_PROPERTY_IMAGE_BLEND_MODE,
        LV_PROPERTY_IMAGE_ANTIALIAS, /*1bit*/
        LV_PROPERTY_IMAGE_ALIGN,
    };

    for(unsigned i = 0; i < sizeof(int_ids) / sizeof(int_ids[0]); i++) {
        int v = rand() % 16;
        if(int_ids[i] == LV_PROPERTY_IMAGE_ANTIALIAS) v = !!v;

        prop.id = int_ids[i];
        prop.num = v;
        lv_obj_set_property(obj, &prop);
        prop = lv_obj_get_property(obj, prop.id);
        TEST_ASSERT_TRUE(LV_PROPERTY_ID_TYPE(prop.id) == LV_PROPERTY_TYPE_INT);
        TEST_ASSERT_EQUAL_INT(prop.num, v);
    }
}

#endif
