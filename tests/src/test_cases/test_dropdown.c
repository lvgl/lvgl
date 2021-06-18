#include "../lvgl.h"
#if LV_BUILD_TEST

#include "unity/unity.h"

void test_dropdown_create_delete(void);
void test_dropdown_set_text_and_symbol(void);
void test_dropdown_set_options(void);
void test_dropdown_select(void);
void test_dropdown_render(void);

void test_dropdown_create_delete(void)
{
  lv_dropdown_create(lv_scr_act());
  TEST_ASSERT_EQUAL(1, lv_obj_get_child_cnt(lv_scr_act()));
  
  lv_obj_t * dd2 = lv_dropdown_create(lv_scr_act());
  lv_obj_set_pos(dd2, 200, 0);
  TEST_ASSERT_EQUAL(2, lv_obj_get_child_cnt(lv_scr_act()));
  TEST_ASSERT_NULL(lv_dropdown_get_list(dd2));
  lv_dropdown_open(dd2);
  TEST_ASSERT_EQUAL(3, lv_obj_get_child_cnt(lv_scr_act()));
  TEST_ASSERT_NOT_NULL(lv_dropdown_get_list(dd2));
  lv_dropdown_open(dd2);    /*Try to pen again*/
  TEST_ASSERT_EQUAL(3, lv_obj_get_child_cnt(lv_scr_act()));

  lv_obj_t * dd3 = lv_dropdown_create(lv_scr_act());
  lv_obj_set_pos(dd3, 400, 0);
  TEST_ASSERT_EQUAL(4, lv_obj_get_child_cnt(lv_scr_act()));
  lv_dropdown_open(dd3);
  TEST_ASSERT_EQUAL(5, lv_obj_get_child_cnt(lv_scr_act()));
  lv_dropdown_close(dd3);
  TEST_ASSERT_EQUAL(4, lv_obj_get_child_cnt(lv_scr_act()));
  lv_dropdown_close(dd3);   /*Try to close again*/
  TEST_ASSERT_EQUAL(4, lv_obj_get_child_cnt(lv_scr_act()));
  
  lv_obj_del(dd2);
  TEST_ASSERT_EQUAL(2, lv_obj_get_child_cnt(lv_scr_act()));
 
  lv_obj_clean(lv_scr_act());
  TEST_ASSERT_EQUAL(0, lv_obj_get_child_cnt(lv_scr_act()));

}

void test_dropdown_set_text_and_symbol(void)
{
  TEST_ASSERT_EQUAL(0, 0);
}

void test_dropdown_set_options(void)
{

  lv_mem_monitor_t m1;
  lv_mem_monitor(&m1);

  lv_obj_t * dd1 = lv_dropdown_create(lv_scr_act());
  TEST_ASSERT_EQUAL_STRING("Option 1\nOption 2\nOption 3", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(3, lv_dropdown_get_option_cnt(dd1));
  
  lv_dropdown_set_options(dd1, "a1\nb2\nc3\nd4\ne5\nf6");
  TEST_ASSERT_EQUAL_STRING("a1\nb2\nc3\nd4\ne5\nf6", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(6, lv_dropdown_get_option_cnt(dd1));
  
  lv_obj_set_width(dd1, 200);
  lv_dropdown_open(dd1);
  lv_obj_update_layout(dd1);
  TEST_ASSERT_EQUAL(200, lv_obj_get_width(lv_dropdown_get_list(dd1)));
  
  lv_dropdown_close(dd1);

  lv_dropdown_add_option(dd1, "x0", 0);
  TEST_ASSERT_EQUAL_STRING("x0\na1\nb2\nc3\nd4\ne5\nf6", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(7, lv_dropdown_get_option_cnt(dd1));
  
  lv_dropdown_add_option(dd1, "y0", 3);
  TEST_ASSERT_EQUAL_STRING("x0\na1\nb2\ny0\nc3\nd4\ne5\nf6", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(8, lv_dropdown_get_option_cnt(dd1));
  
  lv_dropdown_add_option(dd1, "z0", LV_DROPDOWN_POS_LAST);
  TEST_ASSERT_EQUAL_STRING("x0\na1\nb2\ny0\nc3\nd4\ne5\nf6\nz0", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(9, lv_dropdown_get_option_cnt(dd1));

  lv_dropdown_clear_options(dd1);
  TEST_ASSERT_EQUAL_STRING("", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(0, lv_dropdown_get_option_cnt(dd1));

  lv_dropdown_set_options(dd1, "o1\no2"); /*Just to add some content before lv_dropdown_set_options_static*/

  lv_dropdown_set_options_static(dd1, "a1\nb2\nc3\nd4\ne5\nf6");
  TEST_ASSERT_EQUAL_STRING("a1\nb2\nc3\nd4\ne5\nf6", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(6, lv_dropdown_get_option_cnt(dd1));

  lv_dropdown_add_option(dd1, "x0", 0);
  TEST_ASSERT_EQUAL_STRING("x0\na1\nb2\nc3\nd4\ne5\nf6", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(7, lv_dropdown_get_option_cnt(dd1));
  
  lv_dropdown_add_option(dd1, "y0", 3);
  TEST_ASSERT_EQUAL_STRING("x0\na1\nb2\ny0\nc3\nd4\ne5\nf6", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(8, lv_dropdown_get_option_cnt(dd1));
  
  lv_dropdown_add_option(dd1, "z0", LV_DROPDOWN_POS_LAST);
  TEST_ASSERT_EQUAL_STRING("x0\na1\nb2\ny0\nc3\nd4\ne5\nf6\nz0", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(9, lv_dropdown_get_option_cnt(dd1));

  lv_dropdown_clear_options(dd1);
  TEST_ASSERT_EQUAL_STRING("", lv_dropdown_get_options(dd1));
  TEST_ASSERT_EQUAL(0, lv_dropdown_get_option_cnt(dd1));

  lv_obj_del(dd1);

  lv_mem_monitor_t m2;
  lv_mem_monitor(&m2);
  TEST_ASSERT_UINT32_WITHIN(48, m1.free_size, m2.free_size);
}

void test_dropdown_select(void)
{
  lv_obj_t * dd1 = lv_dropdown_create(lv_scr_act());
  lv_dropdown_set_selected(dd1, 2);
  
  TEST_ASSERT_EQUAL(2, lv_dropdown_get_selected(dd1));
  
  char buf[32];
  memset(buf, 0x00, sizeof(buf));
  lv_dropdown_get_selected_str(dd1, buf, sizeof(buf));  
  TEST_ASSERT_EQUAL_STRING("Option 3", buf);
  
  memset(buf, 0x00, sizeof(buf));
  lv_dropdown_get_selected_str(dd1, buf, 4);
  TEST_ASSERT_EQUAL_STRING("Opt", buf);

  /*Out of range*/
  lv_dropdown_set_selected(dd1, 3);
  TEST_ASSERT_EQUAL(2, lv_dropdown_get_selected(dd1));
}

void test_dropdown_render(void)
{
  lv_obj_clean(lv_scr_act());

  lv_obj_t * dd1 = lv_dropdown_create(lv_scr_act());
  lv_obj_set_pos(dd1, 10, 10);
  lv_dropdown_set_selected(dd1, 1);

  lv_obj_t * dd2 = lv_dropdown_create(lv_scr_act());
  lv_obj_set_pos(dd2, 200, 10);
  lv_obj_set_width(dd2, 200);
  lv_dropdown_set_selected(dd2, 2);
  lv_dropdown_open(dd2);

  lv_obj_t * dd3 = lv_dropdown_create(lv_scr_act());
  lv_obj_set_style_pad_hor(dd3, 5, 0);
  lv_obj_set_style_pad_ver(dd3, 20, 0);
  lv_obj_set_pos(dd3, 500, 150);
  lv_dropdown_set_selected(dd3, 2);
  lv_dropdown_set_dir(dd3, LV_DIR_LEFT);
  lv_dropdown_set_symbol(dd3, LV_SYMBOL_LEFT);
  lv_dropdown_set_options(dd3, "a0\na1\na2\na3\na4\na5\na6\na7\na8\na9\na10\na11\na12\na13\na14\na15\na16");
  lv_dropdown_open(dd3);
  lv_dropdown_set_selected(dd3, 3);
  lv_obj_t * list = lv_dropdown_get_list(dd3);
  lv_obj_set_style_text_line_space(list, 5, 0);
  lv_obj_set_style_bg_color(list, lv_color_hex3(0xf00), LV_PART_SELECTED | LV_STATE_CHECKED);

  
  TEST_ASSERT_EQUAL_SCREENSHOT("dropdown_1.png");
}


#endif
