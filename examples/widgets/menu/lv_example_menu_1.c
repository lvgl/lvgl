#include "../../lv_examples.h"
#if LV_USE_MENU && LV_BUILD_EXAMPLES

/**
 * @title Basic menu with sub page
 * @brief A three-item main page whose third entry opens a hidden sub page.
 *
 * A display-sized `lv_menu` holds a main page with three
 * `lv_menu_cont` items labeled `Item 1`, `Item 2`, and
 * `Item 3 (Click me!)`. A separate sub page created with
 * `lv_menu_page_create` carries a greeting label, and
 * `lv_menu_set_load_page_event` wires the third item to load that sub
 * page when clicked. `lv_menu_set_page` opens the main page.
 */
void lv_example_menu_1(void)
{
    /*Create a menu object*/
    lv_obj_t * menu = lv_menu_create(lv_screen_active());
    lv_obj_set_size(menu, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL));
    lv_obj_center(menu);

    lv_obj_t * cont;
    lv_obj_t * label;

    /*Create a sub page*/
    lv_obj_t * sub_page = lv_menu_page_create(menu, NULL);

    cont = lv_menu_cont_create(sub_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Hello, I am hiding here");

    /*Create a main page*/
    lv_obj_t * main_page = lv_menu_page_create(menu, NULL);

    cont = lv_menu_cont_create(main_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Item 1");

    cont = lv_menu_cont_create(main_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Item 2");

    cont = lv_menu_cont_create(main_page);
    label = lv_label_create(cont);
    lv_label_set_text(label, "Item 3 (Click me!)");
    lv_menu_set_load_page_event(menu, cont, sub_page);

    lv_menu_set_page(menu, main_page);
}

#endif
