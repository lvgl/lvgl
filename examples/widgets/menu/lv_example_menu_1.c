#include "../../lv_examples.h"
#define LV_USE_MENU 1 //TODO
#if LV_USE_MENU && LV_BUILD_EXAMPLES

void lv_example_menu_1(void)
{
    /*Create a menu object*/
    lv_obj_t * menu = lv_menu_create(lv_scr_act());
    lv_obj_set_size(menu, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
    lv_obj_center(menu);

    lv_obj_t * cont;
    lv_obj_t * label;

    /*Create a menu item*/
    lv_menu_item_t * main_menu_items = lv_menu_item_create();

    lv_menu_item_add_seperator(menu, main_menu_items, LV_MENU_SECTION_OFF);

    cont = lv_menu_cont_create(menu);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Item 1");
    lv_menu_item_add_obj(menu, main_menu_items, LV_MENU_SECTION_ON, cont);

    cont = lv_menu_cont_create(menu);
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
    label = lv_label_create(cont);
    lv_label_set_text(label, "Hello, I am hiding here");
    lv_menu_item_add_obj(menu, sub_menu_items, LV_MENU_SECTION_ON, cont);

    lv_menu_item_add_menu(menu, main_menu_items, LV_MENU_SECTION_ON, sub_menu_items);

    lv_menu_set(menu, main_menu_items);
}

#endif
