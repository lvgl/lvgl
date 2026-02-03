#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
}

void test_image_cache_dump(void)
{
    /* Dump should not crash or memory leaks */
    lv_image_cache_dump();
    lv_image_header_cache_dump();
}

#endif
