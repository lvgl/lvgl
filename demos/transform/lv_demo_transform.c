/**
 * @file lv_demo_transform.c
 * Recommended resolution 400x400
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_transform.h"

#if LV_USE_DEMO_TRANSFORM

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    const void * image;
    const char * name;
    const char * description;
} card_info_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void arc_event_cb(lv_event_t * e);
static void slider_event_cb(lv_event_t * e);
static lv_obj_t * card_create(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_style_t style_card;
static lv_style_t style_avatar;
static lv_style_t style_btn;
static lv_obj_t * card_to_transform;
LV_IMAGE_DECLARE(img_transform_avatar_15);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_demo_transform(void)
{
    static const int32_t grid_cols[] = {LV_GRID_CONTENT, 4, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static const int32_t grid_rows[] = {LV_GRID_CONTENT, -10, LV_GRID_FR(1), LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

    lv_style_init(&style_card);
    lv_style_set_width(&style_card, 300);
    lv_style_set_height(&style_card, 120);
    lv_style_set_layout(&style_card, LV_LAYOUT_GRID);
    lv_style_set_grid_column_dsc_array(&style_card, grid_cols);
    lv_style_set_grid_row_dsc_array(&style_card, grid_rows);
    lv_style_set_shadow_width(&style_card, 20);
    lv_style_set_shadow_offset_y(&style_card, 5);
    lv_style_set_shadow_color(&style_card, lv_color_hex3(0xccc));
    lv_style_set_border_width(&style_card, 0);
    lv_style_set_radius(&style_card, 12);
    lv_style_set_base_dir(&style_card, LV_BASE_DIR_AUTO);

    lv_style_init(&style_avatar);
    lv_style_set_shadow_width(&style_avatar, 20);
    lv_style_set_shadow_offset_y(&style_avatar, 5);
    lv_style_set_shadow_color(&style_avatar, lv_color_hex3(0xbbb));
    lv_style_set_radius(&style_avatar, LV_RADIUS_CIRCLE);

    lv_style_init(&style_btn);
    lv_style_set_width(&style_btn, 140);
    lv_style_set_height(&style_btn, 37);
    lv_style_set_bg_color(&style_btn, lv_color_hex(0x759efe));
    lv_style_set_bg_grad_color(&style_btn, lv_color_hex(0x4173ff));
    lv_style_set_bg_grad_dir(&style_btn, LV_GRAD_DIR_HOR);
    lv_style_set_radius(&style_btn, LV_RADIUS_CIRCLE);
    lv_style_set_shadow_width(&style_btn, 0);

    lv_obj_t * card;
    card = card_create();
    lv_obj_set_style_opa(card, LV_OPA_50, 0);
    lv_obj_center(card);

    card_to_transform = card_create();
    lv_obj_center(card_to_transform);

    int32_t disp_w = lv_display_get_horizontal_resolution(NULL);
    lv_obj_t * arc = lv_arc_create(lv_screen_active());
    lv_obj_set_size(arc, disp_w - 20, disp_w - 20);
    lv_arc_set_range(arc, 0, 270);
    lv_arc_set_value(arc, 225);
    lv_obj_add_event_cb(arc, arc_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_flag(arc, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_center(arc);

    lv_obj_t * slider = lv_slider_create(lv_screen_active());
    lv_obj_set_width(slider, lv_pct(70));
    lv_obj_align(slider, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_slider_set_range(slider, 128, 300);
    lv_slider_set_value(slider, 256, LV_ANIM_OFF);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_obj_t * card_create(void)
{
    lv_obj_t * card = lv_obj_create(lv_screen_active());
    lv_obj_add_style(card, &style_card, 0);
    lv_obj_remove_flag(card, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_CHAIN_HOR);

    lv_obj_t * avatar = lv_image_create(card);
    lv_image_set_src(avatar, &img_transform_avatar_15);
    lv_obj_set_grid_cell(avatar, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 4);
    lv_obj_add_style(avatar, &style_avatar, 0);

    lv_obj_t * name = lv_label_create(card);
    lv_label_set_text(name, "Pavel Svoboda");
    lv_obj_set_grid_cell(name, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_set_style_text_font(name, &lv_font_montserrat_18, 0);

    lv_obj_t * btn = lv_button_create(card);
    lv_obj_remove_flag(card, LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
    lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_add_style(btn, &style_btn, 0);

    LV_IMAGE_DECLARE(img_multilang_like);
    lv_obj_t * btn_img = lv_image_create(btn);
    lv_image_set_src(btn_img, &img_multilang_like);
    lv_obj_align(btn_img, LV_ALIGN_LEFT_MID, 30, 0);

    lv_obj_t * btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Like");
    lv_obj_align(btn_label, LV_ALIGN_LEFT_MID, 60, 1);

    lv_obj_scroll_to_view(card, LV_ANIM_OFF);

    return card;
}

static void arc_event_cb(lv_event_t * e)
{
    lv_obj_t * arc = lv_event_get_target(e);

    int32_t v = (int32_t)lv_arc_get_angle_end(arc);
    lv_obj_set_style_transform_rotation(card_to_transform, v * 10, 0);
}

static void slider_event_cb(lv_event_t * e)
{
    lv_obj_t * slider = lv_event_get_target(e);
    int32_t v = lv_slider_get_value(slider);
    lv_obj_set_style_transform_scale_x(card_to_transform, v, 0);
    lv_obj_set_style_transform_scale_y(card_to_transform, v, 0);
}

#endif
