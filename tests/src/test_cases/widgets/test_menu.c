#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "unity/unity.h"

void setUp(void)
{
}

void tearDown(void)
{
    lv_obj_clean(lv_screen_active());
}

void test_menu_create(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    TEST_ASSERT_NOT_NULL(menu);
    TEST_ASSERT_TRUE(lv_obj_has_class(menu, &lv_menu_class));

    lv_obj_t * main_header = lv_menu_get_main_header(menu);
    TEST_ASSERT_NOT_NULL(main_header);

    lv_obj_t * back_btn = lv_menu_get_main_header_back_button(menu);
    TEST_ASSERT_NOT_NULL(back_btn);
}

void test_menu_delete(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    TEST_ASSERT_NOT_NULL(menu);

    lv_obj_delete(menu);
    TEST_ASSERT_EQUAL(0, lv_obj_get_child_count(lv_screen_active()));
}

void test_menu_page_create(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());

    lv_obj_t * page1 = lv_menu_page_create(menu, NULL);
    TEST_ASSERT_NOT_NULL(page1);
    TEST_ASSERT_TRUE(lv_obj_has_class(page1, &lv_menu_page_class));

    lv_obj_t * page2 = lv_menu_page_create(menu, "Test Page");
    TEST_ASSERT_NOT_NULL(page2);
}

void test_menu_page_title(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_t * page = lv_menu_page_create(menu, "Initial Title");
    TEST_ASSERT_NOT_NULL(page);

    lv_menu_set_page_title(page, "New Title");

    static const char * static_title = "Static Title";
    lv_menu_set_page_title_static(page, static_title);

    lv_menu_set_page_title(page, NULL);
}

void test_menu_cont_create(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_t * page = lv_menu_page_create(menu, "Page");

    lv_obj_t * cont = lv_menu_cont_create(page);
    TEST_ASSERT_NOT_NULL(cont);
    TEST_ASSERT_TRUE(lv_obj_has_class(cont, &lv_menu_cont_class));
}

void test_menu_section_create(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_t * page = lv_menu_page_create(menu, "Page");

    lv_obj_t * section = lv_menu_section_create(page);
    TEST_ASSERT_NOT_NULL(section);
    TEST_ASSERT_TRUE(lv_obj_has_class(section, &lv_menu_section_class));

    lv_obj_t * cont = lv_menu_cont_create(section);
    TEST_ASSERT_NOT_NULL(cont);
}

void test_menu_separator_create(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_t * page = lv_menu_page_create(menu, "Page");

    lv_obj_t * sep = lv_menu_separator_create(page);
    TEST_ASSERT_NOT_NULL(sep);
    TEST_ASSERT_TRUE(lv_obj_has_class(sep, &lv_menu_separator_class));
}

void test_menu_set_page(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_t * page1 = lv_menu_page_create(menu, "Page 1");
    lv_obj_t * page2 = lv_menu_page_create(menu, "Page 2");

    TEST_ASSERT_NULL(lv_menu_get_cur_main_page(menu));

    lv_menu_set_page(menu, page1);
    TEST_ASSERT_EQUAL_PTR(page1, lv_menu_get_cur_main_page(menu));

    lv_menu_set_page(menu, page2);
    TEST_ASSERT_EQUAL_PTR(page2, lv_menu_get_cur_main_page(menu));

    lv_menu_set_page(menu, NULL);
    TEST_ASSERT_NULL(lv_menu_get_cur_main_page(menu));
}

void test_menu_clear_history(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_set_size(menu, 300, 400);
    lv_obj_center(menu);

    lv_obj_t * page1 = lv_menu_page_create(menu, "Page 1");
    lv_obj_t * page2 = lv_menu_page_create(menu, "Page 2");
    lv_obj_t * page3 = lv_menu_page_create(menu, "Page 3");

    lv_menu_set_mode_root_back_button(menu, LV_MENU_ROOT_BACK_BUTTON_ENABLED);

    lv_menu_set_page(menu, page1);
    lv_menu_set_page(menu, page2);
    lv_menu_set_page(menu, page3);
    TEST_ASSERT_EQUAL_PTR(page3, lv_menu_get_cur_main_page(menu));

    lv_menu_clear_history(menu);

    /* After clear history, back button click should not navigate back */
    lv_refr_now(NULL);
    lv_obj_t * back_btn = lv_menu_get_main_header_back_button(menu);
    lv_area_t coords;
    lv_obj_get_coords(back_btn, &coords);
    lv_test_mouse_click_at((coords.x1 + coords.x2) / 2, (coords.y1 + coords.y2) / 2);

    /* Should still be on page3 since history was cleared */
    TEST_ASSERT_EQUAL_PTR(page3, lv_menu_get_cur_main_page(menu));
}

void test_menu_sidebar(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_t * main_page = lv_menu_page_create(menu, "Main Page");
    lv_obj_t * sidebar_page = lv_menu_page_create(menu, "Sidebar Page");

    TEST_ASSERT_NULL(lv_menu_get_cur_sidebar_page(menu));
    TEST_ASSERT_NULL(lv_menu_get_sidebar_header(menu));
    TEST_ASSERT_NULL(lv_menu_get_sidebar_header_back_button(menu));

    lv_menu_set_sidebar_page(menu, sidebar_page);
    TEST_ASSERT_EQUAL_PTR(sidebar_page, lv_menu_get_cur_sidebar_page(menu));
    TEST_ASSERT_NOT_NULL(lv_menu_get_sidebar_header(menu));
    TEST_ASSERT_NOT_NULL(lv_menu_get_sidebar_header_back_button(menu));

    lv_menu_set_page(menu, main_page);
    TEST_ASSERT_EQUAL_PTR(main_page, lv_menu_get_cur_main_page(menu));

    lv_menu_set_sidebar_page(menu, NULL);
    TEST_ASSERT_NULL(lv_menu_get_cur_sidebar_page(menu));
}

void test_menu_header_mode(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_t * page = lv_menu_page_create(menu, "Page");
    lv_menu_set_page(menu, page);

    lv_menu_set_mode_header(menu, LV_MENU_HEADER_TOP_FIXED);
    lv_menu_set_mode_header(menu, LV_MENU_HEADER_TOP_UNFIXED);
    lv_menu_set_mode_header(menu, LV_MENU_HEADER_BOTTOM_FIXED);
    lv_menu_set_mode_header(menu, LV_MENU_HEADER_BOTTOM_FIXED);
}

void test_menu_header_mode_with_sidebar(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_t * main_page = lv_menu_page_create(menu, "Main");
    lv_obj_t * sidebar_page = lv_menu_page_create(menu, "Sidebar");

    lv_menu_set_sidebar_page(menu, sidebar_page);
    lv_menu_set_page(menu, main_page);

    lv_menu_set_mode_header(menu, LV_MENU_HEADER_TOP_UNFIXED);
    lv_menu_set_mode_header(menu, LV_MENU_HEADER_BOTTOM_FIXED);
}

void test_menu_root_back_button(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_t * page = lv_menu_page_create(menu, "Page");

    lv_menu_set_mode_root_back_button(menu, LV_MENU_ROOT_BACK_BUTTON_DISABLED);
    lv_menu_set_page(menu, page);
    lv_menu_set_mode_root_back_button(menu, LV_MENU_ROOT_BACK_BUTTON_ENABLED);
    lv_menu_set_mode_root_back_button(menu, LV_MENU_ROOT_BACK_BUTTON_ENABLED);
}

void test_menu_back_button_is_root(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_t * page1 = lv_menu_page_create(menu, "Page 1");
    lv_obj_t * sidebar_page = lv_menu_page_create(menu, "Sidebar");

    lv_obj_t * main_back_btn = lv_menu_get_main_header_back_button(menu);

    lv_menu_set_page(menu, page1);
    TEST_ASSERT_TRUE(lv_menu_back_button_is_root(menu, main_back_btn));

    lv_menu_set_sidebar_page(menu, sidebar_page);
    lv_obj_t * sidebar_back_btn = lv_menu_get_sidebar_header_back_button(menu);
    TEST_ASSERT_TRUE(lv_menu_back_button_is_root(menu, sidebar_back_btn));

    lv_obj_t * dummy = lv_obj_create(lv_screen_active());
    TEST_ASSERT_FALSE(lv_menu_back_button_is_root(menu, dummy));
    lv_obj_delete(dummy);
}

void test_menu_load_page_event(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_t * page1 = lv_menu_page_create(menu, "Page 1");
    lv_obj_t * page2 = lv_menu_page_create(menu, "Page 2");

    lv_menu_set_page(menu, page1);

    lv_obj_t * cont = lv_menu_cont_create(page1);
    lv_menu_set_load_page_event(menu, cont, page2);
}

void test_menu_navigation_deep(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());

    lv_obj_t * root_page = lv_menu_page_create(menu, "Root");
    lv_obj_t * level1_page = lv_menu_page_create(menu, "Level 1");
    lv_obj_t * level2_page = lv_menu_page_create(menu, "Level 2");
    lv_obj_t * level3_page = lv_menu_page_create(menu, "Level 3");

    lv_menu_set_mode_root_back_button(menu, LV_MENU_ROOT_BACK_BUTTON_ENABLED);

    lv_menu_set_page(menu, root_page);
    TEST_ASSERT_EQUAL_PTR(root_page, lv_menu_get_cur_main_page(menu));

    lv_menu_set_page(menu, level1_page);
    TEST_ASSERT_EQUAL_PTR(level1_page, lv_menu_get_cur_main_page(menu));

    lv_menu_set_page(menu, level2_page);
    TEST_ASSERT_EQUAL_PTR(level2_page, lv_menu_get_cur_main_page(menu));

    lv_menu_set_page(menu, level3_page);
    TEST_ASSERT_EQUAL_PTR(level3_page, lv_menu_get_cur_main_page(menu));

    lv_menu_set_page(menu, NULL);
    TEST_ASSERT_NULL(lv_menu_get_cur_main_page(menu));
}

void test_menu_sidebar_navigation(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());

    lv_obj_t * sidebar_page = lv_menu_page_create(menu, "Sidebar");
    lv_obj_t * main1 = lv_menu_page_create(menu, "Main 1");
    lv_obj_t * main2 = lv_menu_page_create(menu, "Main 2");

    lv_menu_set_sidebar_page(menu, sidebar_page);

    lv_obj_t * cont1 = lv_menu_cont_create(sidebar_page);
    lv_menu_set_load_page_event(menu, cont1, main1);

    lv_obj_t * cont2 = lv_menu_cont_create(sidebar_page);
    lv_menu_set_load_page_event(menu, cont2, main2);

    lv_menu_set_page(menu, main1);
    TEST_ASSERT_EQUAL_PTR(main1, lv_menu_get_cur_main_page(menu));

    lv_menu_set_page(menu, main2);
    TEST_ASSERT_EQUAL_PTR(main2, lv_menu_get_cur_main_page(menu));
}

void test_menu_memory_leak(void)
{
    lv_mem_monitor_t m1;
    lv_mem_monitor(&m1);

    lv_obj_t * menu = lv_menu_create(lv_screen_active());

    lv_obj_t * page1 = lv_menu_page_create(menu, "Page 1");
    lv_obj_t * page2 = lv_menu_page_create(menu, "Page 2");
    lv_obj_t * sidebar_page = lv_menu_page_create(menu, "Sidebar");

    lv_menu_cont_create(page1);
    lv_menu_section_create(page1);
    lv_menu_separator_create(page1);

    lv_menu_set_sidebar_page(menu, sidebar_page);
    lv_menu_set_page(menu, page1);
    lv_menu_set_page(menu, page2);

    lv_obj_delete(menu);

    lv_mem_monitor_t m2;
    lv_mem_monitor(&m2);

    TEST_ASSERT_UINT_WITHIN(256, m1.free_size, m2.free_size);
}

void test_menu_page_title_variations(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());

    lv_obj_t * page1 = lv_menu_page_create(menu, NULL);
    lv_menu_set_page_title(page1, "Dynamic Title 1");
    lv_menu_set_page_title(page1, "Dynamic Title 2");
    lv_menu_set_page_title(page1, "Dynamic Title 3");

    static const char * static_title = "Static Title";
    lv_menu_set_page_title_static(page1, static_title);
    lv_menu_set_page_title(page1, "Back to Dynamic");

    lv_menu_set_page_title_static(page1, static_title);
    lv_menu_set_page_title_static(page1, NULL);

    lv_menu_set_page_title(page1, "Will be cleared");
    lv_menu_set_page_title(page1, NULL);
}

void test_menu_render_basic(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_set_size(menu, 300, 400);
    lv_obj_center(menu);

    lv_obj_t * page = lv_menu_page_create(menu, "Main Page");
    lv_obj_t * cont = lv_menu_cont_create(page);
    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, "Menu Item 1");

    lv_obj_t * cont2 = lv_menu_cont_create(page);
    lv_obj_t * label2 = lv_label_create(cont2);
    lv_label_set_text(label2, "Menu Item 2");

    lv_menu_set_page(menu, page);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/menu_basic.png");
}

void test_menu_render_sidebar(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_set_size(menu, 400, 300);
    lv_obj_center(menu);

    lv_obj_t * sidebar_page = lv_menu_page_create(menu, NULL);
    lv_obj_t * sidebar_cont1 = lv_menu_cont_create(sidebar_page);
    lv_obj_t * sidebar_label1 = lv_label_create(sidebar_cont1);
    lv_label_set_text(sidebar_label1, "Settings");

    lv_obj_t * sidebar_cont2 = lv_menu_cont_create(sidebar_page);
    lv_obj_t * sidebar_label2 = lv_label_create(sidebar_cont2);
    lv_label_set_text(sidebar_label2, "About");

    lv_obj_t * main_page = lv_menu_page_create(menu, "Settings");
    lv_obj_t * main_cont = lv_menu_cont_create(main_page);
    lv_obj_t * main_label = lv_label_create(main_cont);
    lv_label_set_text(main_label, "Setting Content");

    lv_menu_set_sidebar_page(menu, sidebar_page);
    lv_menu_set_page(menu, main_page);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/menu_sidebar.png");
}

void test_menu_render_section(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_set_size(menu, 300, 400);
    lv_obj_center(menu);

    lv_obj_t * page = lv_menu_page_create(menu, "Sections");

    lv_obj_t * section1 = lv_menu_section_create(page);
    lv_obj_t * cont1 = lv_menu_cont_create(section1);
    lv_obj_t * label1 = lv_label_create(cont1);
    lv_label_set_text(label1, "Section 1 Item");

    lv_menu_separator_create(page);

    lv_obj_t * section2 = lv_menu_section_create(page);
    lv_obj_t * cont2 = lv_menu_cont_create(section2);
    lv_obj_t * label2 = lv_label_create(cont2);
    lv_label_set_text(label2, "Section 2 Item");

    lv_menu_set_page(menu, page);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/menu_section.png");
}

void test_menu_render_bottom_header(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_set_size(menu, 300, 200);
    lv_obj_center(menu);

    lv_obj_t * page = lv_menu_page_create(menu, "Bottom Header");
    lv_obj_t * cont = lv_menu_cont_create(page);
    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, "Content");

    lv_menu_set_page(menu, page);
    lv_menu_set_mode_header(menu, LV_MENU_HEADER_BOTTOM_FIXED);
    lv_menu_set_mode_root_back_button(menu, LV_MENU_ROOT_BACK_BUTTON_ENABLED);

    TEST_ASSERT_EQUAL_SCREENSHOT("widgets/menu_bottom_header.png");
}

void test_menu_refr_via_root_back_button(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_set_size(menu, 300, 400);
    lv_obj_center(menu);

    lv_obj_t * page1 = lv_menu_page_create(menu, "Page 1");
    lv_obj_t * page2 = lv_menu_page_create(menu, "Page 2");

    lv_menu_set_mode_root_back_button(menu, LV_MENU_ROOT_BACK_BUTTON_ENABLED);

    lv_menu_set_page(menu, page1);
    lv_menu_set_page(menu, page2);
    TEST_ASSERT_EQUAL_PTR(page2, lv_menu_get_cur_main_page(menu));

    lv_refr_now(NULL);

    lv_obj_t * back_btn = lv_menu_get_main_header_back_button(menu);
    lv_area_t coords;
    lv_obj_get_coords(back_btn, &coords);
    lv_test_mouse_click_at((coords.x1 + coords.x2) / 2, (coords.y1 + coords.y2) / 2);
    TEST_ASSERT_EQUAL_PTR(page1, lv_menu_get_cur_main_page(menu));
}

void test_menu_load_page_event_click(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_t * page1 = lv_menu_page_create(menu, "Page 1");
    lv_obj_t * page2 = lv_menu_page_create(menu, "Page 2");

    lv_menu_set_page(menu, page1);

    lv_obj_t * cont = lv_menu_cont_create(page1);
    lv_menu_set_load_page_event(menu, cont, page2);

    lv_refr_now(NULL);

    lv_area_t coords;
    lv_obj_get_coords(cont, &coords);
    lv_test_mouse_click_at((coords.x1 + coords.x2) / 2, (coords.y1 + coords.y2) / 2);

    TEST_ASSERT_EQUAL_PTR(page2, lv_menu_get_cur_main_page(menu));
}

void test_menu_sidebar_load_page_event_click(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_set_size(menu, 400, 300);
    lv_obj_center(menu);

    lv_obj_t * sidebar_page = lv_menu_page_create(menu, NULL);
    lv_obj_t * main_page1 = lv_menu_page_create(menu, "Main 1");
    lv_obj_t * main_page2 = lv_menu_page_create(menu, "Main 2");

    lv_menu_set_sidebar_page(menu, sidebar_page);

    lv_obj_t * cont1 = lv_menu_cont_create(sidebar_page);
    lv_obj_t * label1 = lv_label_create(cont1);
    lv_label_set_text(label1, "Item 1");
    lv_menu_set_load_page_event(menu, cont1, main_page1);

    lv_obj_t * cont2 = lv_menu_cont_create(sidebar_page);
    lv_obj_t * label2 = lv_label_create(cont2);
    lv_label_set_text(label2, "Item 2");
    lv_menu_set_load_page_event(menu, cont2, main_page2);

    lv_menu_set_page(menu, main_page1);
    lv_refr_now(NULL);

    lv_area_t coords;
    lv_obj_get_coords(cont2, &coords);
    lv_test_mouse_click_at((coords.x1 + coords.x2) / 2, (coords.y1 + coords.y2) / 2);

    TEST_ASSERT_EQUAL_PTR(main_page2, lv_menu_get_cur_main_page(menu));
}

void test_menu_back_button_click(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_set_size(menu, 300, 400);
    lv_obj_center(menu);

    lv_obj_t * page1 = lv_menu_page_create(menu, "Page 1");
    lv_obj_t * page2 = lv_menu_page_create(menu, "Page 2");
    lv_obj_t * page3 = lv_menu_page_create(menu, "Page 3");

    lv_menu_set_mode_root_back_button(menu, LV_MENU_ROOT_BACK_BUTTON_ENABLED);

    lv_menu_set_page(menu, page1);
    lv_menu_set_page(menu, page2);
    lv_menu_set_page(menu, page3);
    TEST_ASSERT_EQUAL_PTR(page3, lv_menu_get_cur_main_page(menu));

    lv_refr_now(NULL);

    lv_obj_t * back_btn = lv_menu_get_main_header_back_button(menu);
    lv_area_t coords;
    lv_obj_get_coords(back_btn, &coords);

    lv_test_mouse_click_at((coords.x1 + coords.x2) / 2, (coords.y1 + coords.y2) / 2);
    TEST_ASSERT_EQUAL_PTR(page2, lv_menu_get_cur_main_page(menu));

    lv_test_mouse_click_at((coords.x1 + coords.x2) / 2, (coords.y1 + coords.y2) / 2);
    TEST_ASSERT_EQUAL_PTR(page1, lv_menu_get_cur_main_page(menu));
}

void test_menu_set_load_page_event_twice(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_t * page1 = lv_menu_page_create(menu, "Page 1");
    lv_obj_t * page2 = lv_menu_page_create(menu, "Page 2");
    lv_obj_t * page3 = lv_menu_page_create(menu, "Page 3");

    lv_menu_set_page(menu, page1);

    lv_obj_t * cont = lv_menu_cont_create(page1);
    lv_menu_set_load_page_event(menu, cont, page2);
    lv_menu_set_load_page_event(menu, cont, page3);

    lv_refr_now(NULL);

    lv_area_t coords;
    lv_obj_get_coords(cont, &coords);
    lv_test_mouse_click_at((coords.x1 + coords.x2) / 2, (coords.y1 + coords.y2) / 2);

    TEST_ASSERT_EQUAL_PTR(page3, lv_menu_get_cur_main_page(menu));
}

void test_menu_sidebar_header_mode_variations(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_t * main_page = lv_menu_page_create(menu, "Main");
    lv_obj_t * sidebar_page = lv_menu_page_create(menu, "Sidebar Title");

    lv_menu_set_sidebar_page(menu, sidebar_page);
    lv_menu_set_page(menu, main_page);

    /* Start with BOTTOM_FIXED to ensure mode change triggers refresh */
    lv_menu_set_mode_header(menu, LV_MENU_HEADER_BOTTOM_FIXED);
    lv_refr_now(NULL);

    lv_menu_set_mode_header(menu, LV_MENU_HEADER_TOP_UNFIXED);
    lv_refr_now(NULL);

    lv_menu_set_mode_header(menu, LV_MENU_HEADER_TOP_FIXED);
    lv_refr_now(NULL);
}

void test_menu_sidebar_page_title_update(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_t * main_page = lv_menu_page_create(menu, "Main");
    lv_obj_t * sidebar_page = lv_menu_page_create(menu, "Sidebar Title");

    lv_menu_set_sidebar_page(menu, sidebar_page);
    lv_menu_set_page(menu, main_page);

    lv_menu_set_page_title(sidebar_page, "New Sidebar Title");
    lv_obj_send_event(menu, LV_EVENT_VALUE_CHANGED, NULL);
    lv_refr_now(NULL);

    lv_menu_set_page_title(sidebar_page, NULL);
    lv_obj_send_event(menu, LV_EVENT_VALUE_CHANGED, NULL);
    lv_refr_now(NULL);
}

void test_menu_header_hidden_when_empty(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_t * page = lv_menu_page_create(menu, NULL);

    lv_menu_set_mode_root_back_button(menu, LV_MENU_ROOT_BACK_BUTTON_DISABLED);
    lv_menu_set_page(menu, page);

    lv_obj_t * header = lv_menu_get_main_header(menu);
    lv_refr_now(NULL);

    TEST_ASSERT_TRUE(lv_obj_has_flag(header, LV_OBJ_FLAG_HIDDEN));
}

void test_menu_group_focus(void)
{
    lv_group_t * g = lv_group_create();
    lv_group_set_default(g);

    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_t * page1 = lv_menu_page_create(menu, "Page 1");
    lv_obj_t * page2 = lv_menu_page_create(menu, "Page 2");

    lv_menu_set_page(menu, page1);

    lv_obj_t * cont = lv_menu_cont_create(page1);
    lv_menu_set_load_page_event(menu, cont, page2);

    lv_group_add_obj(g, cont);

    lv_refr_now(NULL);

    lv_area_t coords;
    lv_obj_get_coords(cont, &coords);
    lv_test_mouse_click_at((coords.x1 + coords.x2) / 2, (coords.y1 + coords.y2) / 2);

    /* Verify navigation occurred */
    TEST_ASSERT_EQUAL_PTR(page2, lv_menu_get_cur_main_page(menu));

    lv_group_set_default(NULL);
    lv_group_delete(g);
}

void test_menu_sidebar_tab_switch(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_set_size(menu, 400, 300);
    lv_obj_center(menu);

    lv_obj_t * sidebar_page = lv_menu_page_create(menu, NULL);
    lv_obj_t * main_page1 = lv_menu_page_create(menu, "Main 1");
    lv_obj_t * main_page2 = lv_menu_page_create(menu, "Main 2");

    lv_menu_set_sidebar_page(menu, sidebar_page);

    lv_obj_t * cont1 = lv_menu_cont_create(sidebar_page);
    lv_obj_t * label1 = lv_label_create(cont1);
    lv_label_set_text(label1, "Tab 1");
    lv_menu_set_load_page_event(menu, cont1, main_page1);

    lv_obj_t * cont2 = lv_menu_cont_create(sidebar_page);
    lv_obj_t * label2 = lv_label_create(cont2);
    lv_label_set_text(label2, "Tab 2");
    lv_menu_set_load_page_event(menu, cont2, main_page2);

    lv_refr_now(NULL);

    lv_area_t coords1;
    lv_obj_get_coords(cont1, &coords1);
    lv_test_mouse_click_at((coords1.x1 + coords1.x2) / 2, (coords1.y1 + coords1.y2) / 2);
    TEST_ASSERT_EQUAL_PTR(main_page1, lv_menu_get_cur_main_page(menu));

    lv_area_t coords2;
    lv_obj_get_coords(cont2, &coords2);
    lv_test_mouse_click_at((coords2.x1 + coords2.x2) / 2, (coords2.y1 + coords2.y2) / 2);
    TEST_ASSERT_EQUAL_PTR(main_page2, lv_menu_get_cur_main_page(menu));

    lv_test_mouse_click_at((coords1.x1 + coords1.x2) / 2, (coords1.y1 + coords1.y2) / 2);
    TEST_ASSERT_EQUAL_PTR(main_page1, lv_menu_get_cur_main_page(menu));
}

void test_menu_sidebar_root_back_disabled(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_set_size(menu, 400, 300);
    lv_obj_center(menu);

    lv_obj_t * sidebar_page = lv_menu_page_create(menu, NULL);
    lv_obj_t * main_page = lv_menu_page_create(menu, "Main");

    lv_menu_set_mode_root_back_button(menu, LV_MENU_ROOT_BACK_BUTTON_DISABLED);
    lv_menu_set_sidebar_page(menu, sidebar_page);
    lv_menu_set_page(menu, main_page);

    lv_refr_now(NULL);

    lv_obj_t * sidebar_back_btn = lv_menu_get_sidebar_header_back_button(menu);
    TEST_ASSERT_TRUE(lv_obj_has_flag(sidebar_back_btn, LV_OBJ_FLAG_HIDDEN));
}

void test_menu_selected_tab_clear_without_sidebar(void)
{
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_set_size(menu, 400, 300);
    lv_obj_center(menu);

    lv_obj_t * sidebar_page = lv_menu_page_create(menu, NULL);
    lv_obj_t * main_page1 = lv_menu_page_create(menu, "Main 1");
    lv_obj_t * main_page2 = lv_menu_page_create(menu, "Main 2");

    lv_menu_set_sidebar_page(menu, sidebar_page);

    lv_obj_t * cont1 = lv_menu_cont_create(sidebar_page);
    lv_menu_set_load_page_event(menu, cont1, main_page1);

    lv_refr_now(NULL);

    lv_area_t coords1;
    lv_obj_get_coords(cont1, &coords1);
    lv_test_mouse_click_at((coords1.x1 + coords1.x2) / 2, (coords1.y1 + coords1.y2) / 2);

    lv_menu_set_sidebar_page(menu, NULL);
    lv_menu_set_page(menu, main_page2);

    lv_refr_now(NULL);
}

#endif
