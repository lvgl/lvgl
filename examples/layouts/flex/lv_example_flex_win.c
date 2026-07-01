/**
 * @file lv_example_flex_win.c
 */

#include "../../lv_examples.h"
#if LV_USE_FLEX && LV_BUILD_EXAMPLES

/*A window is a flex column with a header bar on top and a content area below. The
 *static helpers below build one on top of base widgets.
 *
 *The widgets are styled by hand to give the window its look: a flat background, a
 *grey header bar and a screen-coloured content area.*/

static lv_style_t style_win;
static lv_style_t style_header;
static lv_style_t style_content;

static void styles_init(void)
{
    /*Window background: a flat, edge-to-edge column with the base object's card
     *flattened away (no padding, border or radius).*/
    lv_style_init(&style_win);
    lv_style_set_pad_all(&style_win, 0);
    lv_style_set_pad_gap(&style_win, 0);
    lv_style_set_border_width(&style_win, 0);
    lv_style_set_radius(&style_win, 0);

    /*Title bar: a grey header with tight padding.*/
    lv_style_init(&style_header);
    lv_style_set_bg_opa(&style_header, LV_OPA_COVER);
    lv_style_set_bg_color(&style_header, lv_palette_lighten(LV_PALETTE_GREY, 2));
    lv_style_set_text_color(&style_header, lv_palette_darken(LV_PALETTE_GREY, 4));
    lv_style_set_pad_all(&style_header, 2);
    lv_style_set_pad_gap(&style_header, 2);
    lv_style_set_border_width(&style_header, 0);
    lv_style_set_radius(&style_header, 0);

    /*Content area: a screen-coloured body with normal padding.*/
    lv_style_init(&style_content);
    lv_style_set_bg_opa(&style_content, LV_OPA_COVER);
    lv_style_set_bg_color(&style_content, lv_palette_lighten(LV_PALETTE_GREY, 4));
    lv_style_set_pad_all(&style_content, 16);
    lv_style_set_border_width(&style_content, 0);
    lv_style_set_radius(&style_content, 0);
}

/**
 * Create a window: a flex column holding a header bar and a content area.
 */
static lv_obj_t * win_create(lv_obj_t * parent)
{
    lv_obj_t * win = lv_obj_create(parent);
    lv_obj_set_size(win, lv_obj_get_width(parent), lv_obj_get_height(parent));
    lv_obj_set_flex_flow(win, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(win, &style_win, 0);

    lv_obj_t * header = lv_obj_create(win);
    lv_obj_set_size(header, lv_pct(100), lv_display_get_dpi(lv_obj_get_display(win)) / 2);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_style(header, &style_header, 0);

    lv_obj_t * content = lv_obj_create(win);
    lv_obj_set_width(content, lv_pct(100));
    lv_obj_set_flex_grow(content, 1);
    lv_obj_add_style(content, &style_content, 0);

    return win;
}

static lv_obj_t * win_get_header(lv_obj_t * win)
{
    return lv_obj_get_child(win, 0);
}

static lv_obj_t * win_get_content(lv_obj_t * win)
{
    return lv_obj_get_child(win, 1);
}

/**
 * Add a title label that takes the free space in the header.
 */
static lv_obj_t * win_add_title(lv_obj_t * win, const char * txt)
{
    lv_obj_t * title = lv_label_create(win_get_header(win));
    lv_label_set_long_mode(title, LV_LABEL_LONG_MODE_DOTS);
    lv_label_set_text(title, txt);
    lv_obj_set_flex_grow(title, 1);
    return title;
}

/**
 * Add a fixed-width header button with an optional centered icon.
 */
static lv_obj_t * win_add_button(lv_obj_t * win, const void * icon, int32_t btn_w)
{
    lv_obj_t * btn = lv_button_create(win_get_header(win));
    lv_obj_set_size(btn, btn_w, lv_pct(100));

#if LV_USE_IMAGE == 1
    if(icon) {
        lv_obj_t * img = lv_image_create(btn);
        lv_image_set_src(img, icon);
        lv_obj_align(img, LV_ALIGN_CENTER, 0, 0);
    }
#endif

    return btn;
}

static void event_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target_obj(e);
    LV_LOG_USER("Button %d clicked", (int)lv_obj_get_index(obj));
}

/**
 * @title Window built from a flex container
 * @brief Build a window from a flex column.
 *
 * The static `win_*` helpers build the window from base widgets: a
 * `LV_FLEX_FLOW_COLUMN` container with a fixed-height `LV_FLEX_FLOW_ROW` header bar
 * and a `lv_obj_set_flex_grow` content area below. The title uses
 * `lv_obj_set_flex_grow` to push the buttons to the right, and the content holds a
 * long label so the body scrolls.
 */
void lv_example_flex_win(void)
{
    styles_init();

    lv_obj_t * win = win_create(lv_screen_active());
    lv_obj_t * btn;
    btn = win_add_button(win, LV_SYMBOL_LEFT, 40);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    win_add_title(win, "A title");

    btn = win_add_button(win, LV_SYMBOL_RIGHT, 40);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    btn = win_add_button(win, LV_SYMBOL_CLOSE, 60);
    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t * content = win_get_content(win);  /*Content can be added here*/
    lv_obj_t * label = lv_label_create(content);
    lv_label_set_text(label, "This is\n"
                      "a pretty\n"
                      "long text\n"
                      "to see how\n"
                      "the window\n"
                      "becomes\n"
                      "scrollable.\n"
                      "\n"
                      "\n"
                      "Some more\n"
                      "text to be\n"
                      "sure it\n"
                      "overflows. :)");
}

#endif
