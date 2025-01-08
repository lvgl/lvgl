/**
 * @file lv_demo_high_res_app_about.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_demo_high_res_private.h"
#if LV_USE_DEMO_HIGH_RES

#include "../../src/widgets/image/lv_image.h"
#include "../../src/widgets/label/lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void back_clicked_cb(lv_event_t * e);
static void slide_deck_scrolled_cb(lv_event_t * e);
static lv_obj_t * create_button(lv_obj_t * parent, const void * img_src, lv_demo_high_res_ctx_t * c);
static void left_clicked_cb(lv_event_t * e);
static void bg_cont_delete_cb(lv_event_t * e);
static bool advance_slides(lv_obj_t * slide_deck);
static void right_clicked_cb(lv_event_t * e);
static void play_pause_timer_cb(lv_timer_t * t);
static void play_pause_clicked_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_high_res_app_about(lv_obj_t * base_obj)
{
    lv_demo_high_res_ctx_t * c = lv_obj_get_user_data(base_obj);

    /* background */

    lv_obj_t * bg = base_obj;
    lv_obj_remove_style_all(bg);
    lv_obj_set_size(bg, LV_PCT(100), LV_PCT(100));

    lv_obj_t * bg_img = lv_image_create(bg);
    lv_subject_add_observer_obj(&c->th, lv_demo_high_res_theme_observer_image_src_cb, bg_img,
                                (void *)&c->imgs[IMG_LIGHT_BG_ABOUT]);

    lv_obj_t * bg_cont = lv_obj_create(bg);
    lv_obj_remove_style_all(bg_cont);
    lv_obj_set_size(bg_cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_top(bg_cont, c->sz->gap[7], 0);
    lv_obj_set_style_pad_bottom(bg_cont, c->sz->gap[10], 0);
    lv_obj_set_style_pad_hor(bg_cont, c->sz->gap[10], 0);

    /* top margin */

    lv_obj_t * top_margin = lv_demo_high_res_top_margin_create(bg_cont, 0, true, c);

    /* app info */

    lv_obj_t * app_info = lv_demo_high_res_simple_container_create(bg_cont, true, c->sz->gap[4], LV_FLEX_ALIGN_START);
    lv_obj_align_to(app_info, top_margin, LV_ALIGN_OUT_BOTTOM_LEFT, 0, c->sz->gap[7]);

    lv_obj_t * back = lv_demo_high_res_simple_container_create(app_info, false, c->sz->gap[2], LV_FLEX_ALIGN_CENTER);
    lv_obj_add_event_cb(back, back_clicked_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * back_icon = lv_image_create(back);
    lv_image_set_src(back_icon, c->imgs[IMG_ARROW_LEFT]);
    lv_obj_add_style(back_icon, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_A8_IMG], 0);
    lv_obj_add_flag(back_icon, LV_OBJ_FLAG_EVENT_BUBBLE);

    lv_obj_t * back_label = lv_label_create(back);
    lv_label_set_text_static(back_label, "Back");
    lv_obj_set_style_text_opa(back_label, LV_OPA_60, 0);
    lv_obj_add_style(back_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_add_style(back_label, &c->fonts[FONT_HEADING_MD], 0);

    lv_obj_t * app_label = lv_label_create(app_info);
    lv_label_set_text_static(app_label, "About");
    lv_obj_add_style(app_label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
    lv_obj_add_style(app_label, &c->fonts[FONT_HEADING_LG], 0);

    /* slides */

    if(lv_array_is_empty(&c->about_slides_array)) {
        lv_obj_t * label = lv_label_create(bg_cont);
        if(c->about_slides_dir_exists) {
            lv_label_set_text_fmt(label, "Couldn't find images named Slide1.png, Slide2.png, etc. in the '%s' folder",
                                  c->slides_path);
        }
        else {
            lv_label_set_text_fmt(label, "Couldn't open the '%s' folder to load the images", c->slides_path);
        }
        lv_obj_center(label);
        lv_obj_add_style(label, &c->styles[STYLE_COLOR_BASE][STYLE_TYPE_TEXT], 0);
        return;
    }

    lv_obj_t * slides_cont = lv_obj_create(bg_cont);
    lv_obj_remove_style_all(slides_cont);
    lv_obj_set_size(slides_cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(slides_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(slides_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_align(slides_cont, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_style_pad_row(slides_cont, c->sz->gap[3], 0);


    lv_obj_t * slide_deck_cont = lv_obj_create(slides_cont);
    lv_obj_remove_style_all(slide_deck_cont);
    lv_obj_set_size(slide_deck_cont, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_t * slide_deck = lv_demo_high_res_simple_container_create(slide_deck_cont, false, c->sz->gap[2],
                                                                     LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_left(slide_deck, lv_display_get_horizontal_resolution(NULL) * 2 / 5, 0);

    lv_obj_t * buttons = lv_demo_high_res_simple_container_create(slides_cont, false, c->sz->gap[2], LV_FLEX_ALIGN_START);
    lv_obj_t * left = create_button(buttons, c->imgs[IMG_PAGER_LEFT], c);
    lv_obj_set_width(left, c->sz->icon[3]);
    lv_obj_add_state(left, LV_STATE_DISABLED);
    lv_obj_t * play_pause = create_button(buttons, c->imgs[IMG_PAGER_PAUSE], c);
    lv_obj_set_width(play_pause, c->sz->icon[4]);
    lv_obj_set_user_data(play_pause, c);
    lv_obj_t * right = create_button(buttons, c->imgs[IMG_PAGER_RIGHT], c);
    lv_obj_set_width(right, c->sz->icon[3]);

    lv_obj_add_event_cb(slide_deck_cont, slide_deck_scrolled_cb, LV_EVENT_SCROLL, buttons);
    lv_obj_add_event_cb(left, left_clicked_cb, LV_EVENT_CLICKED, slide_deck);
    lv_obj_add_event_cb(right, right_clicked_cb, LV_EVENT_CLICKED, slide_deck);

    lv_timer_t * play_pause_timer = lv_timer_create(play_pause_timer_cb, 5000, slide_deck);
    lv_obj_add_event_cb(play_pause, play_pause_clicked_cb, LV_EVENT_CLICKED, play_pause_timer);
    lv_obj_add_event_cb(bg_cont, bg_cont_delete_cb, LV_EVENT_DELETE, play_pause_timer);

    lv_obj_t * slide;

    uint32_t about_slides_count = lv_array_size(&c->about_slides_array);
    for(uint32_t i = 0; i < about_slides_count; i++) {
        lv_image_dsc_t ** slide_src = lv_array_at(&c->about_slides_array, i);
        slide = lv_image_create(slide_deck);
        lv_image_set_src(slide, *slide_src);
    }

    slide = lv_obj_get_child(slide_deck, 0);
    if(slide) {
        lv_obj_scroll_to_view_recursive(slide, LV_ANIM_OFF);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void back_clicked_cb(lv_event_t * e)
{
    lv_obj_t * back = lv_event_get_target_obj(e);

    lv_obj_t * base_obj = lv_obj_get_parent(lv_obj_get_parent(lv_obj_get_parent(back)));
    lv_obj_clean(base_obj);
    lv_demo_high_res_home(base_obj);
}

static void slide_deck_scrolled_cb(lv_event_t * e)
{
    lv_obj_t * slide_deck_cont = lv_event_get_target_obj(e);
    lv_obj_t * slide_deck = lv_obj_get_child(slide_deck_cont, 0);
    lv_obj_t * buttons = lv_event_get_user_data(e);
    lv_obj_t * left_button = lv_obj_get_child(buttons, 0);
    lv_obj_t * right_button = lv_obj_get_child(buttons, 2);

    lv_obj_set_state(left_button, LV_STATE_DISABLED,
                     lv_obj_get_scroll_left(slide_deck_cont) - lv_obj_get_style_pad_left(slide_deck, 0) <= 0);
    lv_obj_set_state(right_button, LV_STATE_DISABLED, lv_obj_get_scroll_right(slide_deck_cont) <= 0);
}

static lv_obj_t * create_button(lv_obj_t * parent, const void * img_src, lv_demo_high_res_ctx_t * c)
{
    lv_obj_t * button = lv_obj_create(parent);
    lv_obj_remove_style_all(button);
    lv_obj_set_style_bg_opa(button, LV_OPA_COVER, 0);
    lv_obj_set_size(button, c->sz->icon[4], c->sz->icon[1]);
    lv_obj_set_style_radius(button, c->sz->gap[3], 0);
    lv_obj_add_style(button, &c->styles[STYLE_COLOR_ACCENT][STYLE_TYPE_OBJ], 0);
    lv_obj_set_style_opa(button, LV_OPA_20, LV_STATE_DISABLED);
    lv_obj_t * img = lv_image_create(button);
    lv_image_set_src(img, img_src);
    lv_obj_set_style_image_recolor(img, lv_color_white(), 0);
    lv_obj_set_style_image_recolor_opa(img, LV_OPA_COVER, 0);
    lv_obj_center(img);
    return button;
}

static void left_clicked_cb(lv_event_t * e)
{
    lv_obj_t * slide_deck = lv_event_get_user_data(e);
    lv_obj_t * slide_deck_cont = lv_obj_get_parent(slide_deck);
    lv_area_t slide_deck_cont_area;
    lv_obj_get_coords(slide_deck_cont, &slide_deck_cont_area);

    int32_t slide_count = lv_obj_get_child_count(slide_deck);
    for(int32_t i = slide_count - 1; i >= 0; i--) {
        lv_obj_t * slide = lv_obj_get_child(slide_deck, i);
        lv_area_t slide_area;
        lv_obj_get_coords(slide, &slide_area);
        if(slide_area.x1 < slide_deck_cont_area.x1 && slide_area.x2 < slide_deck_cont_area.x2) {
            lv_obj_scroll_to_view_recursive(slide, LV_ANIM_ON);
            break;
        }
    }
}

static void bg_cont_delete_cb(lv_event_t * e)
{
    lv_timer_t * play_pause_timer = lv_event_get_user_data(e);
    lv_timer_delete(play_pause_timer);
}

static bool advance_slides(lv_obj_t * slide_deck)
{
    lv_obj_t * slide_deck_cont = lv_obj_get_parent(slide_deck);
    lv_area_t slide_deck_cont_area;
    lv_obj_get_coords(slide_deck_cont, &slide_deck_cont_area);

    int32_t slide_count = lv_obj_get_child_count(slide_deck);
    for(int32_t i = 0; i < slide_count; i++) {
        lv_obj_t * slide = lv_obj_get_child(slide_deck, i);
        lv_area_t slide_area;
        lv_obj_get_coords(slide, &slide_area);
        if(slide_area.x1 > slide_deck_cont_area.x1 && slide_area.x2 > slide_deck_cont_area.x2) {
            lv_obj_scroll_to_view_recursive(slide, LV_ANIM_ON);
            return true;
        }
    }
    return false;
}

static void right_clicked_cb(lv_event_t * e)
{
    lv_obj_t * slide_deck = lv_event_get_user_data(e);
    advance_slides(slide_deck);
}

static void play_pause_timer_cb(lv_timer_t * t)
{
    lv_obj_t * slide_deck = lv_timer_get_user_data(t);
    bool scrolled = advance_slides(slide_deck);
    if(!scrolled) {
        lv_obj_t * first_slide = lv_obj_get_child(slide_deck, 0);
        lv_obj_scroll_to_view_recursive(first_slide, LV_ANIM_ON);
    }
}

static void play_pause_clicked_cb(lv_event_t * e)
{
    lv_obj_t * play_pause = lv_event_get_target_obj(e);
    lv_timer_t * play_pause_timer = lv_event_get_user_data(e);
    lv_demo_high_res_ctx_t * c = lv_obj_get_user_data(play_pause);
    lv_obj_t * play_pause_img = lv_obj_get_child(play_pause, 0);

    if(lv_image_get_src(play_pause_img) == c->imgs[IMG_PAGER_PAUSE]) {
        lv_image_set_src(play_pause_img, c->imgs[IMG_PAGER_PLAY]);
        lv_timer_pause(play_pause_timer);
    }
    else {
        lv_image_set_src(play_pause_img, c->imgs[IMG_PAGER_PAUSE]);
        lv_timer_reset(play_pause_timer);
        lv_timer_resume(play_pause_timer);
    }
}

#endif /*LV_USE_DEMO_HIGH_RES*/
