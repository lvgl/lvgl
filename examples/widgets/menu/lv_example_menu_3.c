#include "../../lv_examples.h"
#define LV_USE_MENU 1 //TODO
#if LV_USE_MENU && LV_BUILD_EXAMPLES

static lv_obj_t * header_label;

static void event_handler(lv_event_t * e)
{
    lv_obj_t * menu = lv_event_get_user_data(e);

    lv_menu_item_t * menu_item = lv_menu_get_main_item(menu);

    if(menu_item == NULL) return;

    lv_obj_t * cont = menu_item->obj;

    if(cont == NULL) return;

    lv_obj_t * label = lv_obj_get_child(cont, 0);

    /*Obtain text from menu object and let this be the header text*/
    if(lv_obj_check_type(label, &lv_label_class)){
        lv_label_set_text(header_label, lv_label_get_text(label));
    }
}

void lv_example_menu_3(void)
{
    /*Create a menu object*/
    lv_obj_t * menu = lv_menu_create(lv_scr_act());
    lv_obj_add_event_cb(menu, event_handler, LV_EVENT_VALUE_CHANGED, menu);
    lv_obj_set_size(menu, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
    lv_obj_center(menu);

    /*Create a custom header*/
    lv_obj_t * header = lv_menu_get_main_header(menu);
    header_label = lv_label_create(header);
    lv_label_set_text(header_label, "Placeholder text");

    /*Create a menu item*/
    lv_menu_item_t * main_menu_items = lv_menu_item_create();

    lv_menu_item_add_seperator(menu, main_menu_items, LV_MENU_SECTION_OFF);

    lv_obj_t * cont;
    lv_obj_t * label;

    /*Create a sub menu items*/
    lv_menu_item_t * sub_menu_1_items = lv_menu_item_create();

    cont = lv_menu_cont_create(menu);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Item 1 (Click me!)");
    lv_menu_item_set_obj(menu, sub_menu_1_items, cont);

    cont = lv_menu_cont_create(menu);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Hello, I am hiding here");
    lv_menu_item_add_obj(menu, sub_menu_1_items, LV_MENU_SECTION_ON, cont);

    lv_menu_item_t * sub_menu_2_items = lv_menu_item_create();

    cont = lv_menu_cont_create(menu);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Item 2 (Click me!)");
    lv_menu_item_set_obj(menu, sub_menu_2_items, cont);

    cont = lv_menu_cont_create(menu);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Hello, I am hiding here");
    lv_menu_item_add_obj(menu, sub_menu_2_items, LV_MENU_SECTION_ON, cont);

    lv_menu_item_t * sub_menu_3_items = lv_menu_item_create();

    cont = lv_menu_cont_create(menu);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Item 3 (Click me!)");
    lv_menu_item_set_obj(menu, sub_menu_3_items, cont);

    cont = lv_menu_cont_create(menu);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Hello, I am hiding here");
    lv_menu_item_add_obj(menu, sub_menu_3_items, LV_MENU_SECTION_ON, cont);


    lv_menu_item_add_menu(menu, main_menu_items, LV_MENU_SECTION_ON, sub_menu_1_items);
    lv_menu_item_add_menu(menu, main_menu_items, LV_MENU_SECTION_ON, sub_menu_2_items);
    lv_menu_item_add_menu(menu, main_menu_items, LV_MENU_SECTION_ON, sub_menu_3_items);

    lv_menu_set(menu, main_menu_items);
}

#endif
