#include "../../lv_examples.h"
#define LV_USE_MENU 1 //TODO
#if LV_USE_MENU && LV_BUILD_EXAMPLES

static lv_obj_t * menu;

static void back_event_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);

    if(lv_menu_item_back_btn_is_root(obj)) {
        lv_obj_t * mbox1 = lv_msgbox_create(NULL, "Hello", "Root back btn click.", NULL, true);
        lv_obj_center(mbox1);
    }
}

static void switch_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        lv_menu_set_mode_sidebar(menu, lv_obj_has_state(obj, LV_STATE_CHECKED) ? LV_MENU_MODE_SIDEBAR_ENABLED : LV_MENU_MODE_SIDEBAR_DISABLED);
    }
}
void lv_example_menu_1(void)
{
    menu = lv_menu_create(lv_scr_act());
    lv_menu_set_mode_sidebar(menu, LV_MENU_MODE_SIDEBAR_ENABLED);
    lv_obj_add_event_cb(menu, back_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_set_size(menu, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
    lv_obj_center(menu);

    lv_menu_item_t * sub_menu_display_items = lv_menu_item_create();
    lv_menu_item_set_text(menu, sub_menu_display_items, LV_SYMBOL_SETTINGS, "Display");
    lv_menu_item_add_seperator(menu, sub_menu_display_items, LV_MENU_SECTION_OFF);
    lv_menu_item_add_slider(menu, sub_menu_display_items, LV_MENU_SECTION_ON, LV_SYMBOL_SETTINGS, "Brightness", 0, 150, 100);

    lv_menu_item_t * sub_menu_sound_items = lv_menu_item_create();
    lv_menu_item_set_text(menu, sub_menu_sound_items, LV_SYMBOL_AUDIO, "Sound");
    lv_menu_item_add_seperator(menu, sub_menu_sound_items, LV_MENU_SECTION_OFF);
    lv_menu_item_add_switch(menu, sub_menu_sound_items, LV_MENU_SECTION_ON, LV_SYMBOL_AUDIO, "Sound", false);

    lv_menu_item_t * sub_menu_mechanics_items = lv_menu_item_create();
    lv_obj_t * sub_menu_mechanics_menu_obj = lv_menu_item_set_text(menu, sub_menu_mechanics_items, LV_SYMBOL_SETTINGS, "Mechanics");
    lv_menu_item_add_seperator(menu, sub_menu_mechanics_items, LV_MENU_SECTION_OFF);
    lv_menu_item_add_slider(menu, sub_menu_mechanics_items, LV_MENU_SECTION_ON, LV_SYMBOL_SETTINGS, "Velocity", 0, 150, 120);
    lv_menu_item_add_slider(menu, sub_menu_mechanics_items, LV_MENU_SECTION_ON, LV_SYMBOL_SETTINGS, "Acceleration", 0, 150, 50);
    lv_menu_item_add_slider(menu, sub_menu_mechanics_items, LV_MENU_SECTION_ON, LV_SYMBOL_SETTINGS, "Weight limit", 0, 150, 80);

    lv_menu_item_t * sub_menu_mode_items = lv_menu_item_create();
    lv_menu_item_set_text(menu, sub_menu_mode_items, LV_SYMBOL_SETTINGS, "Menu mode");
    lv_menu_item_add_seperator(menu, sub_menu_mode_items, LV_MENU_SECTION_OFF);
    lv_obj_t * switch_cont = lv_menu_item_add_switch(menu, sub_menu_mode_items, LV_MENU_SECTION_ON, LV_SYMBOL_SETTINGS, "Sidebar enable", true);
    lv_obj_add_event_cb(lv_obj_get_child(switch_cont, 2), switch_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_menu_item_t * sub_menu_about_softwareinfo_items = lv_menu_item_create();
    lv_menu_item_set_text(menu, sub_menu_about_softwareinfo_items, NULL, "Software information");
    lv_menu_item_add_text(menu, sub_menu_about_softwareinfo_items, LV_MENU_SECTION_ON, NULL, "Version 1.0");

    lv_menu_item_t * sub_menu_about_legalinfo_items = lv_menu_item_create();
    lv_menu_item_set_text(menu, sub_menu_about_legalinfo_items, NULL, "Legal information");
    for(uint32_t i=0; i<15; i++){
        lv_menu_item_add_text(menu, sub_menu_about_legalinfo_items, LV_MENU_SECTION_ON, NULL, "This is a long long long long long long long long long text, if it is long enough it may scroll. ");
    }

    lv_menu_item_t * sub_menu_about_items = lv_menu_item_create();

    lv_menu_item_set_text(menu, sub_menu_about_items, NULL, "About");
    lv_menu_item_add_seperator(menu, sub_menu_about_items, LV_MENU_SECTION_OFF);
    lv_menu_item_add_menu(menu, sub_menu_about_items, LV_MENU_SECTION_ON, sub_menu_about_softwareinfo_items);
    lv_menu_item_add_menu(menu, sub_menu_about_items, LV_MENU_SECTION_ON, sub_menu_about_legalinfo_items);

    lv_menu_item_t * main_menu_items = lv_menu_item_create();
    lv_menu_item_add_text(menu, main_menu_items, LV_MENU_SECTION_OFF, NULL, "Settings");
    lv_menu_item_add_menu(menu, main_menu_items, LV_MENU_SECTION_ON, sub_menu_mechanics_items);
    lv_menu_item_add_menu(menu, main_menu_items, LV_MENU_SECTION_ON, sub_menu_sound_items);
    lv_menu_item_add_menu(menu, main_menu_items, LV_MENU_SECTION_ON, sub_menu_display_items);
    lv_menu_item_add_text(menu, main_menu_items, LV_MENU_SECTION_OFF, NULL, "Others");
    lv_menu_item_add_menu(menu, main_menu_items, LV_MENU_SECTION_ON, sub_menu_about_items);
    lv_menu_item_add_menu(menu, main_menu_items, LV_MENU_SECTION_ON, sub_menu_mode_items);

    lv_menu_set(menu, main_menu_items);

    lv_event_send(sub_menu_mechanics_menu_obj, LV_EVENT_CLICKED, NULL);
}

#endif
