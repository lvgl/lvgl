#include "../../lv_examples.h"
#if LV_USE_MENU && LV_USE_USER_DATA && LV_BUILD_EXAMPLES

static lv_obj_t * header_label;

static void event_handler(lv_event_t * e)
{
    lv_obj_t * menu = lv_event_get_user_data(e);

    lv_obj_t * page = lv_menu_get_cur_main_page(menu);

    if(page == NULL) return;

    /*Obtain text from user data in page and let this be the header text*/
    lv_label_set_text(header_label, lv_obj_get_user_data(page));
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

    lv_obj_t * cont;
    lv_obj_t * label;

    /*Create sub pages*/
    lv_obj_t * sub_1_page = lv_menu_page_create(menu);
    lv_obj_set_user_data(sub_1_page, "Page 1");

    cont = lv_menu_cont_create(sub_1_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Hello, I am hiding here");

    lv_obj_t * sub_2_page = lv_menu_page_create(menu);
    lv_obj_set_user_data(sub_2_page, "Page 2");

    cont = lv_menu_cont_create(sub_2_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Hello, I am hiding here");

    lv_obj_t * sub_3_page = lv_menu_page_create(menu);
    lv_obj_set_user_data(sub_3_page, "Page 3");

    cont = lv_menu_cont_create(sub_3_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Hello, I am hiding here");

    /*Create a main page*/
    lv_obj_t * main_page = lv_menu_page_create(menu);
    cont = lv_menu_cont_create(main_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Item 1 (Click me!)");
    lv_menu_set_load_page_event(menu, cont, sub_1_page);

    cont = lv_menu_cont_create(main_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Item 2 (Click me!)");
    lv_menu_set_load_page_event(menu, cont, sub_2_page);

    cont = lv_menu_cont_create(main_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Item 3 (Click me!)");
    lv_menu_set_load_page_event(menu, cont, sub_3_page);

    lv_menu_set_page(menu, main_page);
}

#endif
