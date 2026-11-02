#include "../../lv_examples.h"
#if LV_USE_TILEVIEW && LV_USE_FLEX && LV_USE_BUTTON && LV_USE_LABEL && LV_BUILD_EXAMPLES

/**
 * Create a list: a flex container that stacks its children in a column.
 */
static lv_obj_t * list_create(lv_obj_t * parent)
{
    lv_obj_t * list = lv_obj_create(parent);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    return list;
}

/**
 * Add a full-width list button holding an optional icon and a text label.
 */
static lv_obj_t * list_add_button(lv_obj_t * list, const void * icon, const char * txt)
{
    lv_obj_t * btn = lv_button_create(list);
    lv_obj_set_size(btn, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW);

#if LV_USE_IMAGE == 1
    if(icon) {
        lv_obj_t * img = lv_image_create(btn);
        lv_image_set_src(img, icon);
    }
#endif

    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, txt);
    lv_obj_set_flex_grow(label, 1);

    return btn;
}

/**
 * @title L-shaped tile view with scroll chaining
 * @brief Three tiles in an L layout where a ten-item list chains its scroll to the tile view.
 *
 * `lv_tileview_add_tile` places a label tile at (0, 0) allowing
 * `LV_DIR_BOTTOM`, a button tile at (0, 1) allowing `LV_DIR_TOP` and
 * `LV_DIR_RIGHT`, and a list tile at (1, 1) allowing `LV_DIR_LEFT`.
 * The button tile holds a centered `lv_button` labeled
 * `Scroll up or right`; the list tile holds a full-size
 * `LV_FLEX_FLOW_COLUMN` container with ten buttons `One` through `Ten`
 * whose scroll chains back into the tile view when the list reaches its
 * edge.
 */
void lv_example_tileview_l_shape(void)
{
    lv_obj_t * tv = lv_tileview_create(lv_screen_active());

    /*Tile1: just a label*/
    lv_obj_t * tile1 = lv_tileview_add_tile(tv, 0, 0, LV_DIR_BOTTOM);
    lv_obj_t * label = lv_label_create(tile1);
    lv_label_set_text(label, "Scroll down");
    lv_obj_center(label);

    /*Tile2: a button*/
    lv_obj_t * tile2 = lv_tileview_add_tile(tv, 0, 1, (lv_dir_t)(LV_DIR_TOP | LV_DIR_RIGHT));

    lv_obj_t * btn = lv_button_create(tile2);

    label = lv_label_create(btn);
    lv_label_set_text(label, "Scroll up or right");

    lv_obj_set_size(btn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(btn);

    /*Tile3: a list*/
    lv_obj_t * tile3 = lv_tileview_add_tile(tv, 1, 1, LV_DIR_LEFT);
    lv_obj_t * list = list_create(tile3);
    lv_obj_set_size(list, LV_PCT(100), LV_PCT(100));

    list_add_button(list, NULL, "One");
    list_add_button(list, NULL, "Two");
    list_add_button(list, NULL, "Three");
    list_add_button(list, NULL, "Four");
    list_add_button(list, NULL, "Five");
    list_add_button(list, NULL, "Six");
    list_add_button(list, NULL, "Seven");
    list_add_button(list, NULL, "Eight");
    list_add_button(list, NULL, "Nine");
    list_add_button(list, NULL, "Ten");

}

#endif
