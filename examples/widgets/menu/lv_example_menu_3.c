#include "../../lv_examples.h"
#define LV_USE_MENU 1 //TODO
#if LV_USE_MENU && LV_USE_MSGBOX && LV_BUILD_EXAMPLES

enum {
    LV_MENU_ITEM_BUILDER_VARIANT_1,
    LV_MENU_ITEM_BUILDER_VARIANT_2
};
typedef uint8_t lv_menu_builder_variant_t;

static void back_event_handler(lv_event_t * e);
static void switch_handler(lv_event_t * e);
static lv_obj_t * lv_menu_item_create_text(lv_obj_t * parent, const char * icon, const char * txt,
                                        lv_menu_builder_variant_t builder_variant);
static lv_obj_t * lv_menu_item_add_slider(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section,
                                   const char * icon, const char * txt, int32_t min, int32_t max, int32_t val);
static lv_obj_t * lv_menu_item_set_text(lv_obj_t * menu, lv_menu_item_t * menu_item, const char * icon, const char * txt);
static lv_obj_t * lv_menu_item_add_text(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section, const char * icon, const char * txt);
static lv_obj_t * lv_menu_item_add_switch(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section,
                                   const char * icon, const char * txt, bool chk);

void lv_example_menu_3(void)
{
    lv_obj_t * menu = lv_menu_create(lv_scr_act());
    lv_menu_set_mode_root_back_btn(menu, LV_MENU_MODE_ROOT_BACK_BTN_ENABLED);
    lv_menu_set_mode_sidebar(menu, LV_MENU_MODE_SIDEBAR_ENABLED);
    lv_obj_add_event_cb(menu, back_event_handler, LV_EVENT_CLICKED, menu);
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
    lv_obj_add_event_cb(lv_obj_get_child(switch_cont, 2), switch_handler, LV_EVENT_VALUE_CHANGED, menu);

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

static void back_event_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_t * menu = lv_event_get_user_data(e);

    if(lv_menu_item_back_btn_is_root(menu, obj)) {
        lv_obj_t * mbox1 = lv_msgbox_create(NULL, "Hello", "Root back btn click.", NULL, true);
        lv_obj_center(mbox1);
    }
}

static void switch_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * menu = lv_event_get_user_data(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        lv_menu_set_mode_sidebar(menu, lv_obj_has_state(obj, LV_STATE_CHECKED) ? LV_MENU_MODE_SIDEBAR_ENABLED : LV_MENU_MODE_SIDEBAR_DISABLED);
    }
}

static lv_obj_t * lv_menu_item_create_text(lv_obj_t * parent, const char * icon, const char * txt,
                                        lv_menu_builder_variant_t builder_variant)
{
    lv_obj_t * obj = lv_menu_cont_create(parent);

    lv_obj_t * img = NULL;
    lv_obj_t * label = NULL;

    if(icon) {
        img = lv_img_create(obj);
        lv_img_set_src(img, icon);
    }

    if(txt) {
        label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_flex_grow(label, 1);
    }

    if(builder_variant == LV_MENU_ITEM_BUILDER_VARIANT_2 && icon && txt) {
        lv_obj_add_flag(img, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        lv_obj_swap(img, label);
    }

    return obj;
}

static lv_obj_t * lv_menu_item_add_slider(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section,
                                   const char * icon, const char * txt, int32_t min, int32_t max, int32_t val)
{
    lv_obj_t * obj = lv_menu_item_create_text(((lv_menu_t *)menu)->storage, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t * slider = lv_slider_create(obj);
    lv_obj_set_flex_grow(slider, 1);
    lv_slider_set_range(slider, min, max);
    lv_slider_set_value(slider, val, LV_ANIM_OFF);

    if(icon == NULL) {
        lv_obj_add_flag(slider, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    }

    lv_menu_item_add_obj(menu, menu_item, section, obj);

    return obj;
}

static lv_obj_t * lv_menu_item_set_text(lv_obj_t * menu, lv_menu_item_t * menu_item, const char * icon, const char * txt)
{
    if(menu_item->obj != NULL) {
        lv_obj_del(menu_item->obj);
    }

    lv_obj_t * obj = lv_menu_item_create_text(((lv_menu_t *)menu)->storage, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);

    lv_menu_item_set_obj(menu, menu_item, obj);

    return obj;
}

static lv_obj_t * lv_menu_item_add_text(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section,
                                 const char * icon, const char * txt)
{
    lv_obj_t * obj = lv_menu_item_create_text(((lv_menu_t *)menu)->storage, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);

    lv_menu_item_add_obj(menu, menu_item, section, obj);

    return obj;
}

static lv_obj_t * lv_menu_item_add_switch(lv_obj_t * menu, lv_menu_item_t * menu_item, lv_menu_item_section_type_t section,
                                   const char * icon, const char * txt, bool chk)
{
    lv_obj_t * obj = lv_menu_item_create_text(((lv_menu_t *)menu)->storage, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);

    lv_obj_t * sw = lv_switch_create(obj);
    lv_obj_add_state(sw, chk ? LV_STATE_CHECKED : 0);

    lv_menu_item_add_obj(menu, menu_item, section, obj);

    return obj;
}

#endif

