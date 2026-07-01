/**
 * @file lv_example_menu_navigation.c
 */

#include "../../lv_examples.h"
#if LV_USE_FLEX && LV_BUILD_EXAMPLES

/*A menu is page navigation over base widgets: a header with a back button and a set
 *of pages, only one of which is visible at a time. The static helpers below build
 *the essentials: create pages, add clickable items, and wire an item to open
 *another page with a back button to return.*/

static lv_obj_t * menu_content;   /*Container the pages live in*/
static lv_obj_t * menu_back_btn;  /*Header back button*/
static lv_obj_t * menu_root_page; /*The first page shown*/
static lv_obj_t * menu_cur_page;  /*The page currently visible*/

/*The widgets are styled by hand to give the menu its flat look.*/
static lv_style_t style_menu;
static lv_style_t style_flat;        /*flatten a base object's card: no pad/border/radius*/
static lv_style_t style_header;
static lv_style_t style_header_btn;
static lv_style_t style_item;
static lv_style_t style_pressed;

static void styles_init(void)
{
    /*Menu background: a flat white card.*/
    lv_style_init(&style_menu);
    lv_style_set_bg_opa(&style_menu, LV_OPA_COVER);
    lv_style_set_bg_color(&style_menu, lv_color_white());
    lv_style_set_pad_all(&style_menu, 0);
    lv_style_set_pad_gap(&style_menu, 0);
    lv_style_set_radius(&style_menu, 0);
    lv_style_set_border_width(&style_menu, 0);

    /*Inner containers (page area and pages) are transparent with no padding so the
     *menu card shows through and rows reach the edges.*/
    lv_style_init(&style_flat);
    lv_style_set_bg_opa(&style_flat, LV_OPA_TRANSP);
    lv_style_set_pad_all(&style_flat, 0);
    lv_style_set_pad_gap(&style_flat, 0);
    lv_style_set_radius(&style_flat, 0);
    lv_style_set_border_width(&style_flat, 0);

    /*Header: a transparent row with a little padding.*/
    lv_style_init(&style_header);
    lv_style_set_bg_opa(&style_header, LV_OPA_TRANSP);
    lv_style_set_pad_hor(&style_header, 10);
    lv_style_set_pad_ver(&style_header, 4);
    lv_style_set_pad_gap(&style_header, 10);
    lv_style_set_radius(&style_header, 0);
    lv_style_set_border_width(&style_header, 0);

    /*Back button: flat, just the dark icon, no fill or shadow.*/
    lv_style_init(&style_header_btn);
    lv_style_set_bg_opa(&style_header_btn, LV_OPA_TRANSP);
    lv_style_set_shadow_opa(&style_header_btn, LV_OPA_TRANSP);
    lv_style_set_pad_all(&style_header_btn, 4);
    lv_style_set_text_color(&style_header_btn, lv_palette_darken(LV_PALETTE_GREY, 4));

    /*Item: a flat clickable row, not a filled button.*/
    lv_style_init(&style_item);
    lv_style_set_bg_opa(&style_item, LV_OPA_TRANSP);
    lv_style_set_shadow_opa(&style_item, LV_OPA_TRANSP);
    lv_style_set_pad_all(&style_item, 10);
    lv_style_set_pad_gap(&style_item, 10);
    lv_style_set_radius(&style_item, 0);
    lv_style_set_border_width(&style_item, 0);
    lv_style_set_text_color(&style_item, lv_palette_darken(LV_PALETTE_GREY, 4));

    /*Pressed feedback: a faint grey overlay.*/
    lv_style_init(&style_pressed);
    lv_style_set_bg_opa(&style_pressed, LV_OPA_20);
    lv_style_set_bg_color(&style_pressed, lv_palette_main(LV_PALETTE_GREY));
}

static void show_page(lv_obj_t * page)
{
    if(menu_cur_page) lv_obj_add_flag(menu_cur_page, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(page, LV_OBJ_FLAG_HIDDEN);
    menu_cur_page = page;

    /*The back button is only useful when we left the root page*/
    if(page == menu_root_page) lv_obj_add_flag(menu_back_btn, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_remove_flag(menu_back_btn, LV_OBJ_FLAG_HIDDEN);
}

static void load_page_event(lv_event_t * e)
{
    lv_obj_t * target = lv_event_get_user_data(e);
    show_page(target);
}

static void back_event(lv_event_t * e)
{
    LV_UNUSED(e);
    show_page(menu_root_page);
}

/**
 * Create a menu: a column with a header (back button + title) and a page area.
 */
static lv_obj_t * menu_create(lv_obj_t * parent)
{
    lv_obj_t * menu = lv_obj_create(parent);
    lv_obj_set_flex_flow(menu, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(menu, &style_menu, 0);

    lv_obj_t * header = lv_obj_create(menu);
    lv_obj_set_width(header, lv_pct(100));
    lv_obj_set_height(header, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_style(header, &style_header, 0);

    menu_back_btn = lv_button_create(header);
    lv_obj_add_style(menu_back_btn, &style_header_btn, 0);
    lv_obj_add_style(menu_back_btn, &style_pressed, LV_STATE_PRESSED);
#if LV_USE_IMAGE == 1
    lv_obj_t * back_img = lv_image_create(menu_back_btn);
    lv_image_set_src(back_img, LV_SYMBOL_LEFT);
    lv_obj_center(back_img);
#endif
    lv_obj_add_event_cb(menu_back_btn, back_event, LV_EVENT_CLICKED, NULL);

    lv_obj_t * title = lv_label_create(header);
    lv_label_set_text(title, "Menu");

    menu_content = lv_obj_create(menu);
    lv_obj_set_width(menu_content, lv_pct(100));
    lv_obj_set_flex_grow(menu_content, 1);
    lv_obj_add_style(menu_content, &style_flat, 0);

    return menu;
}

/**
 * Create a page: a full-size column inside the page area, hidden until shown.
 */
static lv_obj_t * menu_page_create(void)
{
    lv_obj_t * page = lv_obj_create(menu_content);
    lv_obj_set_size(page, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(page, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(page, &style_flat, 0);
    lv_obj_add_flag(page, LV_OBJ_FLAG_HIDDEN);
    return page;
}

/**
 * Add a full-width clickable item (a button with a label) to a page.
 */
static lv_obj_t * menu_add_item(lv_obj_t * page, const char * txt)
{
    lv_obj_t * item = lv_button_create(page);
    lv_obj_set_width(item, lv_pct(100));
    lv_obj_add_style(item, &style_item, 0);
    lv_obj_add_style(item, &style_pressed, LV_STATE_PRESSED);
    lv_obj_t * label = lv_label_create(item);
    lv_label_set_text(label, txt);
    return item;
}

/**
 * Make an item open another page when clicked.
 */
static void menu_set_load_page_event(lv_obj_t * item, lv_obj_t * page)
{
    lv_obj_add_event_cb(item, load_page_event, LV_EVENT_CLICKED, page);
}

/**
 * @title Menu built from base widgets
 * @brief Build a menu with pages and a back button.
 *
 * The static `menu_*` helpers build the menu: `menu_page_create`
 * makes a hidden full-size page, `menu_add_item` adds clickable rows, and
 * `menu_set_load_page_event` wires an item to reveal another page. Navigation is
 * just toggling `LV_OBJ_FLAG_HIDDEN` on the pages and showing the header back
 * button whenever we are off the root page.
 */
void lv_example_menu_navigation(void)
{
    styles_init();

    lv_obj_t * menu = menu_create(lv_screen_active());
    lv_obj_set_size(menu, lv_display_get_horizontal_resolution(NULL), lv_display_get_vertical_resolution(NULL));
    lv_obj_center(menu);

    /*Create a sub page*/
    lv_obj_t * sub_page = menu_page_create();
    lv_obj_t * label = lv_label_create(sub_page);
    lv_label_set_text(label, "Hello, I am hiding here");

    /*Create the main page*/
    lv_obj_t * main_page = menu_page_create();
    menu_add_item(main_page, "Item 1");
    menu_add_item(main_page, "Item 2");
    lv_obj_t * item3 = menu_add_item(main_page, "Item 3 (Click me!)");
    menu_set_load_page_event(item3, sub_page);

    /*Show the main page first*/
    menu_root_page = main_page;
    show_page(main_page);
}

#endif
