#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"

#include "unity/unity.h"

void setUp(void);
void tearDown(void);
void test_tabview_create_successful_dir_top(void);
void test_tabview_create_successful_dir_bottom(void);
void test_tabview_create_successful_dir_left(void);
void test_tabview_create_successful_dir_right(void);
void test_tabview_add_one_tab_empty(void);
void test_tabview_add_one_tab_empty_hor(void);
void test_tabview_add_one_tab_no_scroll(void);
void test_tabview_add_one_tab_with_scroll(void);
void test_tabview_add_several_tabs_no_scroll(void);
void test_tabview_rename_tab(void);
void test_tabview_add_several_tabs_hor(void);
void test_tabview_get_content(void);
void test_tabview_get_tab_bar(void);
void test_tabview_set_act_non_existent(void);
void test_tabview_tab2_selected_event(void);
void test_tabview_update_on_external_scroll(void);
void test_tabview_translation_tag(void);

static lv_obj_t * active_screen = NULL;
static lv_obj_t * tabview = NULL;

void setUp(void)
{
    active_screen = lv_screen_active();
}

void tearDown(void)
{
    lv_obj_clean(active_screen);
}

void test_tabview_create_successful_dir_top(void)
{
    tabview = lv_tabview_create(active_screen);
    lv_tabview_set_tab_bar_size(tabview, 50);

    TEST_ASSERT_NOT_NULL(tabview);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/tabview_01.png");
}

void test_tabview_create_successful_dir_bottom(void)
{
    tabview = lv_tabview_create(active_screen);
    lv_tabview_set_tab_bar_position(tabview, LV_DIR_BOTTOM);
    lv_tabview_set_tab_bar_size(tabview, 50);

    TEST_ASSERT_NOT_NULL(tabview);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/tabview_02.png");
}

void test_tabview_create_successful_dir_left(void)
{
    tabview = lv_tabview_create(active_screen);
    lv_tabview_set_tab_bar_position(tabview, LV_DIR_LEFT);
    lv_tabview_set_tab_bar_size(tabview, 50);

    TEST_ASSERT_NOT_NULL(tabview);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/tabview_03.png");
}

void test_tabview_create_successful_dir_right(void)
{
    tabview = lv_tabview_create(active_screen);
    lv_tabview_set_tab_bar_position(tabview, LV_DIR_RIGHT);
    lv_tabview_set_tab_bar_size(tabview, 50);

    TEST_ASSERT_NOT_NULL(tabview);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/tabview_04.png");
}

void test_tabview_add_one_tab_empty(void)
{
    tabview = lv_tabview_create(active_screen);
    lv_tabview_set_tab_bar_position(tabview, LV_DIR_TOP);
    lv_tabview_set_tab_bar_size(tabview, 50);

    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Tab 1");

    TEST_ASSERT_NOT_NULL(tab1);
}

void test_tabview_add_one_tab_empty_hor(void)
{
    tabview = lv_tabview_create(active_screen);
    lv_tabview_set_tab_bar_position(tabview, LV_DIR_LEFT);
    lv_tabview_set_tab_bar_size(tabview, 50);

    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Tab 1");

    TEST_ASSERT_NOT_NULL(tab1);
}

void test_tabview_add_one_tab_no_scroll(void)
{
    tabview = lv_tabview_create(active_screen);
    lv_tabview_set_tab_bar_position(tabview, LV_DIR_TOP);
    lv_tabview_set_tab_bar_size(tabview, 50);

    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Tab 1");

    lv_obj_t * label = lv_label_create(tab1);
    const char * content = "Content of the first tab\n";
    lv_label_set_text(label, content);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/tabview_05.png");
}

void test_tabview_add_one_tab_with_scroll(void)
{
    tabview = lv_tabview_create(active_screen);
    lv_tabview_set_tab_bar_position(tabview, LV_DIR_TOP);
    lv_tabview_set_tab_bar_size(tabview, 50);

    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Tab 1");

    lv_obj_t * label = lv_label_create(tab1);
    lv_label_set_text(label, "Content of the first tab\n"
                      "If the content\n"
                      "of a tab\n"
                      "becomes too\n"
                      "longer\n"
                      "than the\n"
                      "container\n"
                      "then it\n"
                      "automatically\n"
                      "becomes\n"
                      "scrollable.\n"
                      "\n"
                      "\n"
                      "\n"
                      "Can you see it?");

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/tabview_06.png");
}

void test_tabview_add_several_tabs_no_scroll(void)
{
    tabview = lv_tabview_create(active_screen);
    lv_tabview_set_tab_bar_position(tabview, LV_DIR_TOP);
    lv_tabview_set_tab_bar_size(tabview, 50);

    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Tab 1");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "Tab 2");
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "Tab 3");

    lv_obj_t * label1 = lv_label_create(tab1);
    lv_label_set_text(label1, "Content of the first tab\n");

    /*avoid compiler error: unused variable*/
    LV_UNUSED(tab2);
    LV_UNUSED(tab3);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/tabview_07.png");
}

void test_tabview_rename_tab(void)
{
    tabview = lv_tabview_create(active_screen);
    lv_tabview_set_tab_bar_position(tabview, LV_DIR_TOP);
    lv_tabview_set_tab_bar_size(tabview, 50);

    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Tab 1");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "Tab 2");
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "Tab 3");

    /*avoid compiler error: unused variable*/
    LV_UNUSED(tab1);
    LV_UNUSED(tab2);
    LV_UNUSED(tab3);

    /*rename 2nd tab (0-based index)*/
    lv_tabview_set_tab_text(tabview, 1, "2nd Tab");

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/tabview_08.png");
}

void test_tabview_add_several_tabs_hor(void)
{
    tabview = lv_tabview_create(active_screen);
    lv_tabview_set_tab_bar_position(tabview, LV_DIR_TOP);
    lv_tabview_set_tab_bar_size(tabview, 50);

    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Tab 1");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "Tab 2");
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "Tab 3");

    lv_obj_t * label1 = lv_label_create(tab1);
    lv_label_set_text(label1, "Content of the first tab\n");

    /*avoid compiler error: unused variable*/
    LV_UNUSED(tab2);
    LV_UNUSED(tab3);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/tabview_09.png");
}

void test_tabview_get_content(void)
{
    tabview = lv_tabview_create(active_screen);

    lv_obj_t * content = lv_tabview_get_content(tabview);

    TEST_ASSERT_NOT_NULL(content);
}

void test_tabview_get_tab_bar(void)
{
    tabview = lv_tabview_create(active_screen);

    lv_obj_t * tab_bar = lv_tabview_get_tab_bar(tabview);

    TEST_ASSERT_NOT_NULL(tab_bar);
}

void test_tabview_set_act_non_existent(void)
{
    tabview = lv_tabview_create(active_screen);

    lv_tabview_set_active(tabview, 1, LV_ANIM_ON);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/tabview_10.png");
}

void test_tabview_tab2_selected_event(void)
{
    tabview = lv_tabview_create(active_screen);
    lv_tabview_set_tab_bar_position(tabview, LV_DIR_TOP);
    lv_tabview_set_tab_bar_size(tabview, 50);

    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Tab 1");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "Tab 2");

    /*avoid compiler error: unused variable*/
    LV_UNUSED(tab1);
    LV_UNUSED(tab2);

    lv_tabview_set_active(tabview, 1, LV_ANIM_OFF);

    TEST_ASSERT_EQUAL_UINT16(1, lv_tabview_get_tab_active(tabview));
}

void test_tabview_update_on_external_scroll(void)
{
    tabview = lv_tabview_create(active_screen);
    lv_tabview_set_tab_bar_position(tabview, LV_DIR_TOP);
    lv_tabview_set_tab_bar_size(tabview, 50);

    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Tab 1");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "Tab 2");
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "Tab 3");
    lv_obj_t * tab4 = lv_tabview_add_tab(tabview, "Tab 4");

    /*avoid compiler error: unused variable*/
    LV_UNUSED(tab1);
    LV_UNUSED(tab2);
    LV_UNUSED(tab4);

    lv_obj_t * cont = lv_obj_create(tab3);
    lv_obj_set_size(cont, 200, 300);
    lv_obj_set_pos(cont, 1000, 200);

    lv_obj_t * label1 = lv_label_create(cont);
    lv_obj_set_pos(label1, 400, 400);
    lv_label_set_text(label1, "Label1");

    lv_obj_t * label2 = lv_label_create(cont);
    lv_obj_set_pos(label2, 600, 600);
    lv_label_set_text(label1, "Label2");

    lv_obj_scroll_to_view_recursive(label1, LV_ANIM_OFF);

    TEST_ASSERT_TRUE(lv_obj_is_visible(label1));
    TEST_ASSERT_FALSE(lv_obj_is_visible(label2));
    TEST_ASSERT_EQUAL_UINT16(2, lv_tabview_get_tab_active(tabview));
}

void test_tabview_translation_tag(void)
{
    static const char * tags[] = {"tiger", NULL};
    static const char * languages[]    = {"en", "de", "es", NULL};
    static const char * translations[] = {
        "The Tiger", "Der Tiger", "El Tigre"
    };

    lv_translation_add_static(languages, tags, translations);
    tabview = lv_tabview_create(active_screen);
    lv_tabview_set_tab_translation_tag(tabview, "tiger");

    lv_obj_t * label = lv_obj_get_child_by_type(lv_tabview_get_tab_button(tabview, 0), 0, &lv_label_class);

    lv_translation_set_language("en");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "The Tiger");

    lv_translation_set_language("de");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "Der Tiger");

    lv_translation_set_language("es");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "El Tigre");

    /* Unknown language translates to the tag */
    lv_translation_set_language("fr");
    TEST_ASSERT_EQUAL_STRING(lv_label_get_text(label), "tiger");
}

void test_tabview_setting_tab_bar_position_after_size_should_result_in_the_same_result(void)
{
    tabview = lv_tabview_create(lv_screen_active());
    lv_tabview_set_tab_bar_size(tabview, lv_pct(50));
    lv_tabview_set_tab_bar_position(tabview, LV_DIR_LEFT);
    lv_tabview_add_tab(tabview, "Tab 1");

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/tabview_11.png");
    lv_obj_delete(tabview);

    tabview = lv_tabview_create(lv_screen_active());
    lv_tabview_set_tab_bar_position(tabview, LV_DIR_LEFT);
    lv_tabview_set_tab_bar_size(tabview, lv_pct(50));
    lv_tabview_add_tab(tabview, "Tab 1");

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/tabview_11.png");
}

void test_tabview_properties(void)
{
#if LV_USE_OBJ_PROPERTY
    lv_obj_t * tv = lv_tabview_create(lv_screen_active());
    lv_tabview_add_tab(tv, "Tab 1");
    lv_tabview_add_tab(tv, "Tab 2");
    lv_tabview_add_tab(tv, "Tab 3");

    lv_property_t prop = { };

    /* Test TAB_ACTIVE property */
    prop.id = LV_PROPERTY_TABVIEW_TAB_ACTIVE;
    prop.num = 1;
    TEST_ASSERT_TRUE(lv_obj_set_property(tv, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(1, lv_obj_get_property(tv, LV_PROPERTY_TABVIEW_TAB_ACTIVE).num);

    prop.num = 2;
    TEST_ASSERT_TRUE(lv_obj_set_property(tv, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(2, lv_obj_get_property(tv, LV_PROPERTY_TABVIEW_TAB_ACTIVE).num);

    /* Test TAB_BAR_POSITION property */
    prop.id = LV_PROPERTY_TABVIEW_TAB_BAR_POSITION;
    prop.num = LV_DIR_BOTTOM;
    TEST_ASSERT_TRUE(lv_obj_set_property(tv, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(LV_DIR_BOTTOM, lv_obj_get_property(tv, LV_PROPERTY_TABVIEW_TAB_BAR_POSITION).num);

    prop.num = LV_DIR_LEFT;
    TEST_ASSERT_TRUE(lv_obj_set_property(tv, &prop) == LV_RESULT_OK);
    TEST_ASSERT_EQUAL_INT(LV_DIR_LEFT, lv_obj_get_property(tv, LV_PROPERTY_TABVIEW_TAB_BAR_POSITION).num);

    lv_obj_delete(tv);
#endif
}

#endif
