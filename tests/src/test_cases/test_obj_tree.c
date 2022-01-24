#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

void test_obj_tree_1(void);
void test_obj_tree_2(void);

void test_obj_tree_1(void)
{
  TEST_ASSERT_EQUAL(lv_obj_get_child_cnt(lv_scr_act()), 0);
}

void test_obj_tree_2(void)
{

  lv_obj_create(lv_scr_act());
  lv_obj_t * o2 = lv_obj_create(lv_scr_act());
  lv_obj_create(lv_scr_act());
  TEST_ASSERT_EQUAL(lv_obj_get_child_cnt(lv_scr_act()), 3);
  
  lv_obj_del(o2);
  TEST_ASSERT_EQUAL(lv_obj_get_child_cnt(lv_scr_act()), 2);
  
  lv_obj_clean(lv_scr_act());
  TEST_ASSERT_EQUAL(lv_obj_get_child_cnt(lv_scr_act()), 0);
  
 lv_color_t c1 = lv_color_hex(0x444444);
 lv_color_t c2 = lv_color_hex3(0x444);
 TEST_ASSERT_EQUAL_COLOR(c1, c2);
 
 lv_obj_remove_style_all(lv_scr_act());
 lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x112233), 0);
 lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_COVER, 0);
 
 //TEST_ASSERT_EQUAL_SCREENSHOT("scr1.png")
}

#endif
