#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

static lv_obj_t * active_screen = NULL;
static lv_obj_t * btnmatrix = NULL;

static const char * one_row_map[] = {"btn1", "btn2", "btn3", LV_BTNMATRIX_END_OF_MAP_MARKER};
static const char * multi_row_map[] = {"btn1", "btn2", "\n", "btn3", LV_BTNMATRIX_END_OF_MAP_MARKER};

void setUp(void)
{
    active_screen = lv_scr_act();
    btnmatrix = lv_btnmatrix_create(active_screen);
}

void tearDown(void)
{

}

void test_btnmatrix_should_have_map(void)
{
    lv_btnmatrix_t * p = (lv_btnmatrix_t *) btnmatrix;

    lv_btnmatrix_set_map(btnmatrix, one_row_map);

    TEST_ASSERT_EQUAL(p->btn_cnt, 3);
    TEST_ASSERT_EQUAL(p->row_cnt, 1);
}

void test_btnmatrix_should_have_map_multi_row(void)
{
    lv_btnmatrix_t * p = (lv_btnmatrix_t *) btnmatrix;

    lv_btnmatrix_set_map(btnmatrix, multi_row_map);

    TEST_ASSERT_EQUAL(p->btn_cnt, 3);
    TEST_ASSERT_EQUAL(p->row_cnt, 2);
}

void test_btnmatrix_oneline_should_return_button_text_btn_id_not_valid(void)
{
    lv_btnmatrix_t * p = (lv_btnmatrix_t *) btnmatrix;
    uint16_t invalid_btn_id = 0U;
    char * btn_txt = NULL;

    lv_btnmatrix_set_map(btnmatrix, one_row_map);
    invalid_btn_id = p->btn_cnt + 1;

    btn_txt = lv_btnmatrix_get_btn_text(btnmatrix, invalid_btn_id);
    TEST_ASSERT_NULL(btn_txt);

    btn_txt = lv_btnmatrix_get_btn_text(btnmatrix, LV_BTNMATRIX_BTN_NONE);
    TEST_ASSERT_NULL(btn_txt);
}

void test_btnmatrix_single_line_map_should_return_button_text(void)
{
    uint16_t valid_btn_id = 0U;
    char * btn_txt = NULL;

    lv_btnmatrix_set_map(btnmatrix, one_row_map);
    btn_txt = lv_btnmatrix_get_btn_text(btnmatrix, valid_btn_id);
    TEST_ASSERT_EQUAL(btn_txt, one_row_map[valid_btn_id]);
}

void test_btnmatrix_multi_line_map_should_return_button_text(void)
{
    uint16_t valid_btn_id = 2U;
    char * btn_txt = NULL;

    lv_btnmatrix_set_map(btnmatrix, multi_row_map);
    btn_txt = lv_btnmatrix_get_btn_text(btnmatrix, valid_btn_id);
    TEST_ASSERT_EQUAL(btn_txt, one_row_map[valid_btn_id]);
}

#endif
