/**
 * @file lv_demo_widgets_shop.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_widgets_shop.h"
#if LV_USE_DEMO_WIDGETS

#include "lv_demo_widgets_components.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_obj_t * create_shop_item(lv_obj_t * parent, const void * img_src, const char * name, const char * category,
                                   const char * price);
static void shop_chart_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * chart3;

static lv_chart_series_t * ser4;

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/


void lv_demo_widgets_shop_create(lv_obj_t * parent)
{
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW_WRAP);

    lv_obj_t * panel1 = lv_obj_create(parent);
    lv_obj_set_size(panel1, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_bottom(panel1, 30, 0);

    lv_obj_t * title = lv_demo_widgets_title_create(panel1, "Monthly Summary");

    lv_obj_t * date = lv_demo_widgets_title_create(panel1, "8-15 July, 2021");
    lv_obj_add_style(date, &style_text_muted, 0);

    lv_obj_t * amount = lv_demo_widgets_title_create(panel1, "$27,123.25");

    lv_obj_t * hint = lv_label_create(panel1);
    lv_label_set_text(hint, LV_SYMBOL_UP" 17% growth this week");
    lv_obj_set_style_text_color(hint, lv_palette_main(LV_PALETTE_GREEN), 0);

    chart3 = lv_chart_create(panel1);
    lv_chart_set_type(chart3, LV_CHART_TYPE_BAR);
    lv_chart_set_div_line_count(chart3, 6, 0);
    lv_chart_set_point_count(chart3, 7);
    lv_obj_add_event_cb(chart3, shop_chart_event_cb, LV_EVENT_ALL, NULL);

    ser4 = lv_chart_add_series(chart3, lv_theme_get_color_primary(chart3), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_next_value(chart3, ser4, lv_rand(60, 90));
    lv_chart_set_next_value(chart3, ser4, lv_rand(60, 90));
    lv_chart_set_next_value(chart3, ser4, lv_rand(60, 90));
    lv_chart_set_next_value(chart3, ser4, lv_rand(60, 90));
    lv_chart_set_next_value(chart3, ser4, lv_rand(60, 90));
    lv_chart_set_next_value(chart3, ser4, lv_rand(60, 90));
    lv_chart_set_next_value(chart3, ser4, lv_rand(60, 90));

    if(disp_size == DISP_LARGE) {
        static int32_t grid1_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static int32_t grid1_row_dsc[] = {
            LV_GRID_CONTENT,  /*Title*/
            LV_GRID_CONTENT,  /*Sub title*/
            20,               /*Spacer*/
            LV_GRID_CONTENT,  /*Amount*/
            LV_GRID_CONTENT,  /*Hint*/
            LV_GRID_TEMPLATE_LAST
        };

        lv_obj_set_size(chart3, lv_pct(100), lv_pct(100));
        lv_obj_set_style_pad_column(chart3, LV_DPX(30), 0);

        lv_obj_set_grid_dsc_array(panel1, grid1_col_dsc, grid1_row_dsc);
        lv_obj_set_grid_cell(title, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);
        lv_obj_set_grid_cell(date, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_cell(amount, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 3, 1);
        lv_obj_set_grid_cell(hint, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 4, 1);
        lv_obj_set_grid_cell(chart3, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 5);
    }
    else if(disp_size == DISP_MEDIUM) {
        static int32_t grid1_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static int32_t grid1_row_dsc[] = {
            LV_GRID_CONTENT,  /*Title + Date*/
            LV_GRID_CONTENT,  /*Amount + Hint*/
            200,              /*Chart*/
            LV_GRID_TEMPLATE_LAST
        };

        lv_obj_update_layout(panel1);
        lv_obj_set_width(chart3, lv_obj_get_content_width(panel1) - 20);
        lv_obj_set_style_pad_column(chart3, LV_DPX(30), 0);

        lv_obj_set_grid_dsc_array(panel1, grid1_col_dsc, grid1_row_dsc);
        lv_obj_set_grid_cell(title, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(date, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
        lv_obj_set_grid_cell(amount, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
        lv_obj_set_grid_cell(hint, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
        lv_obj_set_grid_cell(chart3, LV_GRID_ALIGN_END, 0, 2, LV_GRID_ALIGN_STRETCH, 2, 1);
    }
    else if(disp_size == DISP_SMALL) {
        static int32_t grid1_col_dsc[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static int32_t grid1_row_dsc[] = {
            LV_GRID_CONTENT,  /*Title*/
            LV_GRID_CONTENT,  /*Date*/
            LV_GRID_CONTENT,  /*Amount*/
            LV_GRID_CONTENT,  /*Hint*/
            LV_GRID_CONTENT,  /*Chart*/
            LV_GRID_TEMPLATE_LAST
        };

        lv_obj_set_width(chart3, LV_PCT(95));
        lv_obj_set_height(chart3, LV_VER_RES - 70);
        lv_obj_set_style_max_height(chart3, 300, 0);

        lv_obj_set_grid_dsc_array(panel1, grid1_col_dsc, grid1_row_dsc);
        lv_obj_set_grid_cell(title, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);
        lv_obj_set_grid_cell(date, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 1, 1);
        lv_obj_set_grid_cell(amount, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 2, 1);
        lv_obj_set_grid_cell(hint, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 3, 1);
        lv_obj_set_grid_cell(chart3, LV_GRID_ALIGN_END, 0, 1, LV_GRID_ALIGN_START, 4, 1);
    }

    lv_obj_t * list = lv_obj_create(parent);
    if(disp_size == DISP_SMALL) {
        lv_obj_add_flag(list, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        lv_obj_set_height(list, LV_PCT(100));
    }
    else {
        lv_obj_set_height(list, LV_PCT(100));
        lv_obj_set_style_max_height(list, 300, 0);
    }

    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_grow(list, 1);
    lv_obj_add_flag(list, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

    title = lv_demo_widgets_title_create(list, "Top products");

    LV_IMAGE_DECLARE(img_clothes);
    create_shop_item(list, &img_clothes, "Blue T-shirt", "Clothes", "$722");
    create_shop_item(list, &img_clothes, "Blue T-shirt", "Clothes", "$411");
    create_shop_item(list, &img_clothes, "Blue T-shirt", "Clothes", "$917");
    create_shop_item(list, &img_clothes, "Blue T-shirt", "Clothes", "$64");
    create_shop_item(list, &img_clothes, "Blue T-shirt", "Clothes", "$805");

    lv_obj_t * notifications = lv_obj_create(parent);
    if(disp_size == DISP_SMALL) {
        lv_obj_add_flag(notifications, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        lv_obj_set_height(notifications, LV_PCT(100));
    }
    else  {
        lv_obj_set_height(notifications, LV_PCT(100));
        lv_obj_set_style_max_height(notifications, 300, 0);
    }

    lv_obj_set_flex_flow(notifications, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_grow(notifications, 1);

    title = lv_demo_widgets_title_create(notifications, "Notification");

    lv_obj_t * cb;
    cb = lv_checkbox_create(notifications);
    lv_checkbox_set_text_static(cb, "Item purchased");

    cb = lv_checkbox_create(notifications);
    lv_checkbox_set_text_static(cb, "New connection");

    cb = lv_checkbox_create(notifications);
    lv_checkbox_set_text_static(cb, "New subscriber");
    lv_obj_add_state(cb, LV_STATE_CHECKED);

    cb = lv_checkbox_create(notifications);
    lv_checkbox_set_text_static(cb, "New message");
    lv_obj_add_state(cb, LV_STATE_DISABLED);

    cb = lv_checkbox_create(notifications);
    lv_checkbox_set_text_static(cb, "Milestone reached");
    lv_obj_add_state(cb, LV_STATE_CHECKED | LV_STATE_DISABLED);

    cb = lv_checkbox_create(notifications);
    lv_checkbox_set_text_static(cb, "Out of stock");

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_obj_t * create_shop_item(lv_obj_t * parent, const void * img_src, const char * name, const char * category,
                                   const char * price)
{
    static int32_t grid_col_dsc[] = {LV_GRID_CONTENT, 5, LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t grid_row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_remove_style_all(cont);
    lv_obj_set_size(cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_grid_dsc_array(cont, grid_col_dsc, grid_row_dsc);

    lv_obj_t * img = lv_image_create(cont);
    lv_image_set_src(img, img_src);
    lv_obj_set_grid_cell(img, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 2);

    lv_obj_t * label;
    label = lv_label_create(cont);
    lv_label_set_text_static(label, name);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_END, 0, 1);

    label = lv_label_create(cont);
    lv_label_set_text_static(label, category);
    lv_obj_add_style(label, &style_text_muted, 0);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_START, 1, 1);

    label = lv_label_create(cont);
    lv_label_set_text_static(label, price);
    lv_obj_set_grid_cell(label, LV_GRID_ALIGN_END, 3, 1, LV_GRID_ALIGN_END, 0, 1);

    return cont;
}

static void shop_chart_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
}

#endif /* LV_USE_DEMO_WIDGETS */
