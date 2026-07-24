/**
 * @file lv_example_flex_list.c
 */

#include "../../lv_examples.h"
#if LV_USE_FLEX && LV_BUILD_EXAMPLES

/*A list is a flex column of full-width text rows and buttons. The static helpers
 *below build and populate one on top of a plain flex container.
 *
 *The widgets are styled by hand to give the list its look: a flat background,
 *full-width buttons with a grey bottom divider, and grey section headers.*/

static lv_obj_t * flex_list;

static lv_style_t style_list;
static lv_style_t style_header;
static lv_style_t style_button;
static lv_style_t style_button_pressed;

/**
 * Set up the styles that give the list its look.
 */
static void styles_init(void)
{
    /*List background: flatten the base object's card into a tight, edge-clipped column.*/
    lv_style_init(&style_list);
    lv_style_set_pad_ver(&style_list, 0);
    lv_style_set_pad_gap(&style_list, 0);
    lv_style_set_clip_corner(&style_list, true);

    /*Section header: a full-width grey bar with dark text. The transform makes the
     *grey reach the list edges despite the list's horizontal padding.*/
    lv_style_init(&style_header);
    lv_style_set_bg_opa(&style_header, LV_OPA_COVER);
    lv_style_set_bg_color(&style_header, lv_palette_lighten(LV_PALETTE_GREY, 2));
    lv_style_set_text_color(&style_header, lv_palette_darken(LV_PALETTE_GREY, 4));
    lv_style_set_transform_width(&style_header, 16);

    /*List button: a flat white row with a grey bottom divider, overriding the
     *default blue button look.*/
    lv_style_init(&style_button);
    lv_style_set_radius(&style_button, 0);
    lv_style_set_shadow_width(&style_button, 0);
    lv_style_set_bg_opa(&style_button, LV_OPA_COVER);
    lv_style_set_bg_color(&style_button, lv_color_white());
    lv_style_set_text_color(&style_button, lv_palette_darken(LV_PALETTE_GREY, 4));
    lv_style_set_border_width(&style_button, 1);
    lv_style_set_border_color(&style_button, lv_palette_lighten(LV_PALETTE_GREY, 2));
    lv_style_set_border_side(&style_button, LV_BORDER_SIDE_BOTTOM);
    lv_style_set_pad_all(&style_button, 8);
    lv_style_set_pad_column(&style_button, 8);

    /*Pressed feedback: a subtle dark recolor.*/
    lv_style_init(&style_button_pressed);
    lv_style_set_recolor(&style_button_pressed, lv_color_black());
    lv_style_set_recolor_opa(&style_button_pressed, LV_OPA_20);
}

/**
 * Create a list: a flex container that stacks its children in a column.
 */
static lv_obj_t * flex_list_create(lv_obj_t * parent)
{
    lv_obj_t * list = lv_obj_create(parent);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(list, &style_list, 0);
    return list;
}

/**
 * Add a full-width text row (a section header) to the list.
 */
static lv_obj_t * flex_list_add_text(lv_obj_t * list, const char * txt)
{
    lv_obj_t * label = lv_label_create(list);
    lv_obj_set_width(label, lv_pct(100));
    lv_obj_add_style(label, &style_header, 0);
    lv_label_set_text(label, txt);
    return label;
}

/**
 * Add a full-width button holding an optional icon and a scrolling text label.
 */
static lv_obj_t * flex_list_add_button(lv_obj_t * list, const void * icon, const char * txt)
{
    lv_obj_t * btn = lv_button_create(list);
    lv_obj_set_size(btn, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW);
    lv_obj_add_style(btn, &style_button, 0);
    lv_obj_add_style(btn, &style_button_pressed, LV_STATE_PRESSED);

#if LV_USE_IMAGE == 1
    if(icon) {
        lv_obj_t * img = lv_image_create(btn);
        lv_image_set_src(img, icon);
    }
#endif

    if(txt) {
        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_MODE_SCROLL_CIRCULAR);
        lv_obj_set_flex_grow(label, 1);
    }

    return btn;
}

/**
 * Find the text of a list button by looking for its child label.
 */
static const char * flex_list_get_button_text(lv_obj_t * btn)
{
    uint32_t i;
    for(i = 0; i < lv_obj_get_child_count(btn); i++) {
        lv_obj_t * child = lv_obj_get_child(btn, i);
        if(lv_obj_check_type(child, &lv_label_class)) {
            return lv_label_get_text(child);
        }
    }
    return "";
}

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target_obj(e);
    if(code == LV_EVENT_CLICKED) {
        LV_LOG_USER("Clicked: %s", flex_list_get_button_text(obj));
    }
}

/**
 * @title List built from a flex container
 * @brief Build a list from a plain flex column.
 *
 * A column-flow flex container stacks full-width text headers and buttons. The
 * static `flex_list_*` helpers wrap the few flex calls needed:
 * `LV_FLEX_FLOW_COLUMN` for the list, full-width rows for the items, and
 * `lv_obj_set_flex_grow` so each button's label fills the row next to its icon. The
 * list look is produced with styles applied by hand.
 */
void lv_example_flex_list(void)
{
    styles_init();

    /*Create a list from a flex column*/
    flex_list = flex_list_create(lv_screen_active());
    lv_obj_set_size(flex_list, 180, 220);
    lv_obj_center(flex_list);

    /*Add buttons to the list*/
    lv_obj_t * btn;
    flex_list_add_text(flex_list, "File");
    btn = flex_list_add_button(flex_list, LV_SYMBOL_FILE, "New");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = flex_list_add_button(flex_list, LV_SYMBOL_DIRECTORY, "Open");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = flex_list_add_button(flex_list, LV_SYMBOL_SAVE, "Save");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = flex_list_add_button(flex_list, LV_SYMBOL_CLOSE, "Delete");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = flex_list_add_button(flex_list, LV_SYMBOL_EDIT, "Edit");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    flex_list_add_text(flex_list, "Connectivity");
    btn = flex_list_add_button(flex_list, LV_SYMBOL_BLUETOOTH, "Bluetooth");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = flex_list_add_button(flex_list, LV_SYMBOL_GPS, "Navigation");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = flex_list_add_button(flex_list, LV_SYMBOL_USB, "USB");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = flex_list_add_button(flex_list, LV_SYMBOL_BATTERY_FULL, "Battery");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    flex_list_add_text(flex_list, "Exit");
    btn = flex_list_add_button(flex_list, LV_SYMBOL_OK, "Apply");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
    btn = flex_list_add_button(flex_list, LV_SYMBOL_CLOSE, "Close");
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
}

#endif
