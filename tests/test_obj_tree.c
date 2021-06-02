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
  return;
  lv_obj_create(lv_scr_act());
  lv_obj_t * o2 = lv_obj_create(lv_scr_act());
  lv_obj_create(lv_scr_act());
  TEST_ASSERT_EQUAL(lv_obj_get_child_cnt(lv_scr_act()), 3);
  
  lv_obj_del(o2);
  TEST_ASSERT_EQUAL(lv_obj_get_child_cnt(lv_scr_act()), 2);
  
  lv_obj_clean(lv_scr_act());
  TEST_ASSERT_EQUAL(lv_obj_get_child_cnt(lv_scr_act()), 0);
}
