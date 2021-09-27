#include "../../lv_examples.h"
#define LV_USE_MENU 1 //TODO
#if LV_USE_MENU && LV_USE_MSGBOX && LV_BUILD_EXAMPLES

static void back_event_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_t * menu = lv_event_get_user_data(e);

    if(lv_menu_item_back_btn_is_root(menu, obj)) {
        lv_obj_t * mbox1 = lv_msgbox_create(NULL, "Hello", "Root back btn click.", NULL, true);
        lv_obj_center(mbox1);
    }
}

void lv_example_menu_2(void)
{
    lv_obj_t * menu = lv_menu_create(lv_scr_act());
    lv_menu_set_mode_root_back_btn(menu, LV_MENU_MODE_ROOT_BACK_BTN_ENABLED);
    lv_obj_add_event_cb(menu, back_event_handler, LV_EVENT_CLICKED, menu);
    lv_obj_set_size(menu, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
    lv_obj_center(menu);

    lv_obj_t * cont;
    lv_obj_t * label;

    /*Create a menu item*/
    lv_menu_item_t * main_menu_items = lv_menu_item_create();

    cont = lv_menu_cont_create(menu);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_CLICKABLE);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Item 1");
    lv_menu_item_add_obj(menu, main_menu_items, LV_MENU_SECTION_ON, cont);

    cont = lv_menu_cont_create(menu);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_CLICKABLE);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Item 2");
    lv_menu_item_add_obj(menu, main_menu_items, LV_MENU_SECTION_ON, cont);

    /*Create a sub menu item*/
    lv_menu_item_t * sub_menu_items = lv_menu_item_create();

    cont = lv_menu_cont_create(menu);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Item 3 (Click me!)");
    lv_menu_item_set_obj(menu, sub_menu_items, cont);

    cont = lv_menu_cont_create(menu);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_CLICKABLE);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Hello, I am hiding here");
    lv_menu_item_add_obj(menu, sub_menu_items, LV_MENU_SECTION_ON, cont);

    lv_menu_item_add_menu(menu, main_menu_items, LV_MENU_SECTION_ON, sub_menu_items);

    lv_menu_set(menu, main_menu_items);
}

#endif
