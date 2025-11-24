#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../src/themes/lv_theme_private.h"
#include "unity/unity.h"

void setUp(void)
{
    /* Function run before every test */
}

void tearDown(void)
{
    /* Function run after every test */
    lv_obj_clean(lv_screen_active());
}

static void theme_apply(lv_theme_t * th, lv_obj_t * obj)
{
    LV_UNUSED(th);
    LV_UNUSED(obj);
}

void test_theme(void)
{
    lv_theme_t * theme = lv_display_get_theme(NULL);
    TEST_ASSERT_NOT_NULL(theme);

    lv_theme_t new_theme;
    lv_memzero(&new_theme, sizeof(lv_theme_t));
    new_theme.font_small = &lv_font_montserrat_16;
    new_theme.font_normal = &lv_font_montserrat_18;
    new_theme.font_large = &lv_font_montserrat_20;
    new_theme.color_primary = lv_color_hex(0xFF0000);
    new_theme.color_secondary = lv_color_hex(0x00FF00);

    lv_theme_set_parent(&new_theme, theme);
    TEST_ASSERT_EQUAL_PTR(theme, new_theme.parent);

    lv_theme_set_apply_cb(&new_theme, theme_apply);
    TEST_ASSERT_EQUAL_PTR(theme_apply, new_theme.apply_cb);

    lv_theme_set_apply_cb(&new_theme, NULL);
    TEST_ASSERT_NULL(new_theme.apply_cb);

    lv_display_set_theme(NULL, &new_theme);

    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    TEST_ASSERT_EQUAL_PTR(&lv_font_montserrat_16, lv_theme_get_font_small(obj));
    TEST_ASSERT_EQUAL_PTR(&lv_font_montserrat_18, lv_theme_get_font_normal(obj));
    TEST_ASSERT_EQUAL_PTR(&lv_font_montserrat_20, lv_theme_get_font_large(obj));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex(0xFF0000), lv_theme_get_color_primary(NULL));
    TEST_ASSERT_EQUAL_COLOR(lv_color_hex(0x00FF00), lv_theme_get_color_secondary(NULL));

    /* Verify default values */
    lv_display_set_theme(NULL, NULL);
    TEST_ASSERT_EQUAL_PTR(LV_FONT_DEFAULT, lv_theme_get_font_small(obj));
    TEST_ASSERT_EQUAL_PTR(LV_FONT_DEFAULT, lv_theme_get_font_normal(obj));
    TEST_ASSERT_EQUAL_PTR(LV_FONT_DEFAULT, lv_theme_get_font_large(obj));
    TEST_ASSERT_EQUAL_COLOR(lv_palette_main(LV_PALETTE_BLUE_GREY), lv_theme_get_color_primary(NULL));
    TEST_ASSERT_EQUAL_COLOR(lv_palette_main(LV_PALETTE_BLUE), lv_theme_get_color_secondary(NULL));
}

static void test_widgets(const char * img_name)
{
    lv_obj_t * scr_act = lv_screen_active();
    lv_obj_set_flex_flow(scr_act, LV_FLEX_FLOW_ROW_WRAP);

    lv_obj_t * tabview = lv_tabview_create(scr_act);
    lv_obj_set_size(tabview, 100, 100);
    lv_tabview_add_tab(tabview, "Tab 1");

    lv_obj_t * win = lv_win_create(scr_act);
    lv_obj_set_size(win, 100, 100);
    lv_win_add_title(win, "Window");
    lv_win_add_button(win, LV_SYMBOL_CLOSE, 20);

    lv_button_create(scr_act);

    lv_obj_t * btnm = lv_buttonmatrix_create(scr_act);
    lv_obj_set_size(btnm, 100, 100);
    static const char * btn_map[] = { "A", "B", NULL };
    lv_buttonmatrix_set_map(btnm, btn_map);

    lv_obj_t * msgbox = lv_msgbox_create(scr_act);
    lv_obj_set_width(msgbox, 100);
    lv_msgbox_add_text(msgbox, "Msgbox");

    lv_obj_t * bar = lv_bar_create(scr_act);
    lv_obj_set_width(bar, 100);

    lv_obj_t * slider = lv_slider_create(scr_act);
    lv_obj_set_width(slider, 100);

    lv_obj_t * table = lv_table_create(scr_act);
    lv_obj_set_size(table, 100, 100);

    lv_obj_t * checkbox = lv_checkbox_create(scr_act);
    lv_obj_add_state(checkbox, LV_STATE_CHECKED);

    lv_obj_t * sw = lv_switch_create(scr_act);
    lv_obj_add_state(sw, LV_STATE_CHECKED);

    lv_obj_t * chart = lv_chart_create(scr_act);
    lv_obj_set_size(chart, 100, 100);

    lv_roller_create(scr_act);

    lv_obj_t * dropdown = lv_dropdown_create(scr_act);
    lv_dropdown_open(dropdown);

    lv_obj_t * arc = lv_arc_create(scr_act);
    lv_obj_set_size(arc, 40, 40);

    lv_obj_t * textarea = lv_textarea_create(scr_act);
    lv_obj_set_size(textarea, 100, 50);
    lv_textarea_add_text(textarea, "Text area");

    lv_obj_t * calendar = lv_calendar_create(scr_act);
    lv_obj_set_height(calendar, 150);

    lv_obj_t * keyboard = lv_keyboard_create(scr_act);
    lv_obj_set_size(keyboard, 300, 150);

    lv_obj_t * list = lv_list_create(scr_act);
    lv_obj_set_size(list, 100, 100);
    lv_list_add_text(list, "List item");
    lv_list_add_button(list, LV_SYMBOL_OK, "List button");

    lv_spinbox_create(scr_act);

    lv_obj_t * tileview = lv_tileview_create(scr_act);
    lv_obj_set_size(tileview, 100, 100);
    lv_obj_t * tile = lv_tileview_add_tile(tileview, 0, 0, LV_DIR_ALL);
    lv_obj_t * tile_label = lv_label_create(tile);
    lv_label_set_text(tile_label, "Tile: 0;0");

    lv_led_create(scr_act);

    lv_obj_t * menu = lv_menu_create(scr_act);
    lv_obj_set_size(menu, 100, 100);

    lv_spinner_create(scr_act);

    lv_test_wait(1000); /*Wait for the transitions*/

    TEST_ASSERT_EQUAL_SCREENSHOT(img_name);

    lv_obj_clean(scr_act);
}

void test_theme_default(void)
{
    TEST_ASSERT_TRUE(lv_theme_default_is_inited());
    TEST_ASSERT_NOT_NULL(lv_theme_default_get());

    lv_theme_t * theme = lv_theme_default_init(NULL,
                                               lv_palette_main(LV_PALETTE_BLUE),
                                               lv_palette_main(LV_PALETTE_RED),
                                               false, LV_FONT_DEFAULT);
    TEST_ASSERT_EQUAL_PTR(theme, lv_theme_default_get());
    lv_display_set_theme(NULL, theme);
    test_widgets("theme_default_light.png");

    theme = lv_theme_default_init(NULL,
                                  lv_palette_main(LV_PALETTE_BLUE),
                                  lv_palette_main(LV_PALETTE_RED),
                                  true, LV_FONT_DEFAULT);
    TEST_ASSERT_EQUAL_PTR(theme, lv_theme_default_get());
    lv_display_set_theme(NULL, theme);
    test_widgets("theme_default_dark.png");

    lv_display_set_theme(NULL, NULL);

    lv_theme_default_deinit();
    TEST_ASSERT_FALSE(lv_theme_default_is_inited());
    TEST_ASSERT_NULL(lv_theme_default_get());
}

void test_theme_mono(void)
{
    lv_theme_t * theme = lv_theme_mono_init(NULL, false, LV_FONT_DEFAULT);
    TEST_ASSERT_EQUAL_PTR(theme, lv_theme_mono_get());
    lv_display_set_theme(NULL, theme);
    test_widgets("theme_mono_light.png");

    theme = lv_theme_mono_init(NULL, true, LV_FONT_DEFAULT);
    TEST_ASSERT_EQUAL_PTR(theme, lv_theme_mono_get());
    lv_display_set_theme(NULL, theme);
    test_widgets("theme_mono_dark.png");

    lv_display_set_theme(NULL, NULL);

    lv_theme_mono_deinit();
    TEST_ASSERT_FALSE(lv_theme_mono_is_inited());
    TEST_ASSERT_NULL(lv_theme_mono_get());
}

void test_theme_simple(void)
{
    lv_theme_t * theme = lv_theme_simple_init(NULL);
    TEST_ASSERT_EQUAL_PTR(theme, lv_theme_simple_get());
    lv_display_set_theme(NULL, theme);
    test_widgets("theme_simple.png");

    lv_display_set_theme(NULL, NULL);

    lv_theme_simple_deinit();
    TEST_ASSERT_FALSE(lv_theme_simple_is_inited());
    TEST_ASSERT_NULL(lv_theme_simple_get());
}

#endif
