/**
 * @file lv_demo_ebike_stats.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_ebike.h"
#if LV_USE_DEMO_EBIKE

#include "../../lvgl_private.h"
#include "translations/lv_i18n.h"
#include "lv_demo_ebike_stats.h"
#include "lv_demo_ebike_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    MODE_AVG_SPEED,
    MODE_DISTANCE,
    MODE_TOP_SPEED,
} stat_mode_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_obj_t * left_cont_create(lv_obj_t * parent);
static lv_obj_t * right_cont_create(lv_obj_t * parent);

/**********************
 *  STATIC VARIABLES
 **********************/
/*Subjects used only by the statistics page to it easier to sync widgets*/
static lv_subject_t subject_week;
static lv_subject_t subject_day;
static lv_subject_t subject_avg_speed;
static lv_subject_t subject_distance;
static lv_subject_t subject_top_speed;
static lv_subject_t subject_mode;
static lv_obj_t * left_arrow;
static lv_obj_t * right_arrow;

static int32_t top_speed_values[] = {46, 28, 42, 39, 41, 25, 49, 37, 35, 40, 33, 40, 31, 27, 45, 38, 41, 40, 27, 25, 30, 45, 31, 43, 41, 34, 47, 32, 30, 33};
static int32_t avg_speed_values[] = {21, 24, 27, 29, 23, 28, 28, 22, 29, 28, 24, 26, 24, 30, 25, 25, 20, 28, 24, 27, 25, 27, 20, 29, 25, 24, 23, 26, 27, 27};
static int32_t distance_values[] =  {87, 63, 29, 84, 27, 84, 33, 76, 77, 49, 46, 29, 67, 21, 87, 75, 40, 19, 12, 67, 66, 11, 59, 33, 51, 75, 44, 61, 53, 63};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_ebike_stats_init(void)
{
    lv_subject_init_int(&subject_mode, MODE_DISTANCE);
    lv_subject_init_int(&subject_week, 0);
    lv_subject_init_int(&subject_day, 0);
    lv_subject_init_int(&subject_top_speed, 0);
    lv_subject_init_int(&subject_avg_speed, 0);
    lv_subject_init_int(&subject_distance, 0);
}

void lv_demo_ebike_stats_deinit(void)
{
    lv_subject_deinit(&subject_mode);
    lv_subject_deinit(&subject_week);
    lv_subject_deinit(&subject_day);
    lv_subject_deinit(&subject_top_speed);
    lv_subject_deinit(&subject_avg_speed);
    lv_subject_deinit(&subject_distance);
}

void lv_demo_ebike_stats_create(lv_obj_t * parent)
{
    lv_obj_t * main_cont = lv_obj_create(parent);
    lv_obj_set_style_bg_opa(main_cont, 0, 0);
    lv_obj_set_size(main_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(main_cont, LV_DEMO_EBIKE_PORTRAIT ? LV_FLEX_FLOW_COLUMN : LV_FLEX_FLOW_ROW);

    lv_obj_t * left_cont = left_cont_create(main_cont);
#if LV_DEMO_EBIKE_PORTRAIT
    lv_obj_set_size(left_cont, lv_pct(100), 120);
#else
    lv_obj_set_size(left_cont, 164, lv_pct(100));
#endif

    lv_obj_t * right_cont = right_cont_create(main_cont);
    lv_obj_set_size(right_cont, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_grow(right_cont, 1);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_obj_t * left_cont_create(lv_obj_t * parent)
{
    lv_obj_t * left_cont = lv_obj_create(parent);
    lv_obj_set_style_bg_opa(left_cont, 0, 0);
    lv_obj_remove_flag(left_cont, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * label = lv_label_create(left_cont);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 24, 16);
    lv_label_set_text(label, _("STATS"));
    lv_obj_set_style_text_font(label, EBIKE_FONT_MEDIUM, 0);


    lv_obj_t * stats_img;
#if LV_USE_LOTTIE
    extern const uint8_t lottie_ebike_stats[];
    extern const size_t lottie_ebike_stats_size;
    stats_img = lv_lottie_create(left_cont);
    lv_lottie_set_src_data(stats_img, lottie_ebike_stats, lottie_ebike_stats_size);
    lv_lottie_set_draw_buf(stats_img, lv_demo_ebike_get_lottie_draw_buf());
#else
    stats_img = lv_image_create(left_cont);
    LV_IMAGE_DECLARE(img_ebike_stats_large);
    lv_image_set_src(stats_img, &img_ebike_stats_large);
#endif

#if LV_DEMO_EBIKE_PORTRAIT
    lv_obj_align(stats_img, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
#else
    lv_obj_align(stats_img, LV_ALIGN_BOTTOM_MID, 0, 0);
#endif
    return left_cont;
}
static void tabs_click_event_cb(lv_event_t * e)
{
    lv_obj_t * btnm = lv_event_get_target(e);
    lv_subject_set_int(&subject_mode, lv_buttonmatrix_get_selected_button(btnm));
}

static lv_obj_t * tabs_create(lv_obj_t * parent)
{
    lv_obj_t * btnm = lv_buttonmatrix_create(parent);
#if LV_DEMO_EBIKE_PORTRAIT
    lv_obj_set_size(btnm, lv_pct(100), 40);
#else
    lv_obj_set_size(btnm, lv_pct(100), 24);
#endif
    lv_obj_set_style_bg_opa(btnm, 0, 0);
    lv_obj_set_style_bg_opa(btnm, 0, LV_PART_ITEMS);
    lv_obj_set_style_border_width(btnm, 1, LV_PART_ITEMS);
    lv_obj_set_style_border_width(btnm, 2, LV_PART_ITEMS | LV_STATE_FOCUSED);
    lv_obj_set_style_border_side(btnm, LV_BORDER_SIDE_BOTTOM, LV_PART_ITEMS);
    lv_obj_set_style_border_color(btnm, EBIKE_COLOR_TURQUOISE, LV_PART_ITEMS);
    lv_obj_set_style_border_opa(btnm, LV_OPA_20, LV_PART_ITEMS);
    lv_obj_set_style_border_opa(btnm, LV_OPA_COVER, LV_PART_ITEMS | LV_STATE_FOCUSED);
    lv_obj_set_style_text_font(btnm, EBIKE_FONT_SMALL, LV_PART_ITEMS);
    lv_obj_set_style_text_color(btnm, lv_color_white(), LV_PART_ITEMS);
    lv_obj_set_style_text_color(btnm, EBIKE_COLOR_TURQUOISE, LV_PART_ITEMS | LV_STATE_FOCUSED);
    static const char * texts[4];
    texts[0] = _("Avg. speed");
    texts[1] = _("Distance");
    texts[2] = _("Top speed");

    lv_buttonmatrix_set_map(btnm, texts);

    lv_obj_add_event_cb(btnm, tabs_click_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_buttonmatrix_set_selected_button(btnm, lv_subject_get_int(&subject_mode));

    return btnm;
}

static void left_click_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    int32_t week = lv_subject_get_int(&subject_week);
    if(week > 0) {
        week--;
        lv_subject_set_int(&subject_week, week);
    }
}

static void right_click_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    int32_t week = lv_subject_get_int(&subject_week);
    if(week < 3) {
        week++;
        lv_subject_set_int(&subject_week, week);
    }

}

static void current_week_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * label = lv_observer_get_target_obj(observer);
    int32_t week = lv_subject_get_int(&subject_week);
    lv_label_set_text_fmt(label, _("March %d - March %d"), week * 7 + 1, week * 7 + 7);


    if(week == 0) lv_obj_set_style_image_opa(left_arrow, LV_OPA_50, 0);
    else lv_obj_set_style_image_opa(left_arrow, LV_OPA_100, 0);

    if(week == 3) lv_obj_set_style_image_opa(right_arrow, LV_OPA_50, 0);
    else lv_obj_set_style_image_opa(right_arrow, LV_OPA_100, 0);
}

static void current_data_objserver_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * label = lv_observer_get_target_obj(observer);
    stat_mode_t mode = lv_subject_get_int(&subject_mode);
    int32_t day = lv_subject_get_int(&subject_day);
    switch(mode) {
        case MODE_AVG_SPEED:
            lv_label_set_text_fmt(label, "%dkm/h", avg_speed_values[day]);
            break;
        case MODE_TOP_SPEED:
            lv_label_set_text_fmt(label, "%dkm/h", top_speed_values[day]);
            break;
        case MODE_DISTANCE:
            lv_label_set_text_fmt(label, "%dkm", distance_values[day]);
            break;
    }
}

static lv_obj_t * data_cont_create(lv_obj_t * parent)
{
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_size(cont, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_text_color(cont, lv_color_white(), 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(cont, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_cross_place(cont, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_style_flex_track_place(cont, LV_FLEX_ALIGN_SPACE_BETWEEN, 0);

    LV_IMAGE_DECLARE(img_ebike_arrow_left_2);
    LV_IMAGE_DECLARE(img_ebike_arrow_right_2);

    left_arrow = lv_image_create(cont);
    lv_image_set_src(left_arrow, &img_ebike_arrow_left_2);
    lv_obj_set_ext_click_area(left_arrow, 32);
    lv_obj_set_size(left_arrow, 40, 40);
    lv_obj_add_flag(left_arrow, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(left_arrow, left_click_event_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, "138km");
    lv_obj_set_style_text_font(label, EBIKE_FONT_LARGE, 0);
    lv_obj_add_flag(label, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    lv_subject_add_observer_obj(&subject_mode, current_data_objserver_cb, label, NULL);
    lv_subject_add_observer_obj(&subject_day, current_data_objserver_cb, label, NULL);

    label = lv_label_create(cont);
    lv_label_set_text(label, "March 18 - March 25");
    lv_obj_set_style_text_font(label, EBIKE_FONT_SMALL, 0);

    right_arrow = lv_image_create(cont);
    lv_image_set_src(right_arrow, &img_ebike_arrow_right_2);
    lv_obj_set_ext_click_area(right_arrow, 32);
    lv_obj_set_size(right_arrow, 40, 40);
    lv_obj_add_flag(right_arrow, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
    lv_obj_add_flag(right_arrow, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(right_arrow, right_click_event_cb, LV_EVENT_CLICKED, NULL);

    lv_subject_add_observer_obj(&subject_week, current_week_observer_cb, label, NULL);

    return cont;
}

static uint32_t day_pressed;
static void chart_value_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * chart = lv_event_get_target(e);
    day_pressed = lv_chart_get_pressed_point(chart);
    if(day_pressed == 0) return;


}
static void chart_released_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_subject_set_int(&subject_day, day_pressed);
}


static void chart_draw_event_cb(lv_event_t * e)
{
    lv_obj_t * chart = lv_event_get_target(e);
    lv_obj_t * cont = lv_obj_get_parent(chart);
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_grad.dir = LV_GRAD_DIR_VER;
    rect_dsc.bg_grad.stops_count = 2;
    rect_dsc.bg_grad.stops[0].color = lv_color_hex(0x00C3BC);
    rect_dsc.bg_grad.stops[0].opa = LV_OPA_0;
    rect_dsc.bg_grad.stops[0].frac = 50;
    rect_dsc.bg_grad.stops[1].color = lv_color_hex(0x8968B6);
    rect_dsc.bg_grad.stops[1].opa = LV_OPA_100;
    rect_dsc.bg_grad.stops[1].frac = 200;

    uint32_t day = lv_subject_get_int(&subject_day);

    lv_point_t p;
    lv_chart_get_point_pos_by_id(chart, lv_chart_get_series_next(chart, NULL), day, &p);
    lv_coord_t w = lv_obj_get_width(cont) / 7;
    lv_area_t a;
    a.x1 = chart->coords.x1 + p.x - w / 2;
    a.x2 = chart->coords.x1 + p.x + w / 2;
    a.y1 = chart->coords.y1;
    a.y2 = chart->coords.y2;
    lv_draw_rect(lv_event_get_layer(e), &rect_dsc, &a);

    char buf[32];
    lv_snprintf(buf, sizeof(buf), _("March %d"), lv_subject_get_int(&subject_day));
    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.font = EBIKE_FONT_SMALL;
    label_dsc.color = lv_color_white();
    label_dsc.text = buf;
    label_dsc.text_local = 1;
    label_dsc.align = LV_TEXT_ALIGN_CENTER;

    a.x1 = chart->coords.x1 + p.x - 100;
    a.x2 = chart->coords.x1 + p.x + 100;
    a.y1 = chart->coords.y2 + 5;
    a.y2 = chart->coords.y2 + 20;
    lv_draw_label(lv_event_get_layer(e), &label_dsc, &a);
}

static void chart_refr_ext_draw(lv_event_t * e)
{
#if LV_DEMO_EBIKE_PORTRAIT
    lv_event_set_ext_draw_size(e, 48);
#else
    lv_event_set_ext_draw_size(e, 32);
#endif
}

static void chart_draw_task_event_cb(lv_event_t * e)
{
    lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t * base_dsc = draw_task->draw_dsc;

    if(base_dsc->part != LV_PART_ITEMS || draw_task->type != LV_DRAW_TASK_TYPE_LINE) return;

    lv_obj_t * obj = lv_event_get_target(e);

    /*Draw a triangle below the line witch some opacity gradient*/
    lv_draw_line_dsc_t * draw_line_dsc = draw_task->draw_dsc;
    lv_draw_triangle_dsc_t tri_dsc;

    lv_draw_triangle_dsc_init(&tri_dsc);
    tri_dsc.p[0].x = draw_line_dsc->p1.x;
    tri_dsc.p[0].y = draw_line_dsc->p1.y;
    tri_dsc.p[1].x = draw_line_dsc->p2.x;
    tri_dsc.p[1].y = draw_line_dsc->p2.y;
    tri_dsc.p[2].x = draw_line_dsc->p1.y < draw_line_dsc->p2.y ? draw_line_dsc->p1.x : draw_line_dsc->p2.x;
    tri_dsc.p[2].y = LV_MAX(draw_line_dsc->p1.y, draw_line_dsc->p2.y);
    tri_dsc.bg_grad.dir = LV_GRAD_DIR_VER;

    int32_t full_h = lv_obj_get_height(obj);
    int32_t fract_upper = (int32_t)(LV_MIN(draw_line_dsc->p1.y, draw_line_dsc->p2.y) - obj->coords.y1) * 255 / full_h;
    int32_t fract_lower = (int32_t)(LV_MAX(draw_line_dsc->p1.y, draw_line_dsc->p2.y) - obj->coords.y1) * 255 / full_h;
    tri_dsc.bg_grad.stops[0].color = lv_color_hex(0x3987CF);
    tri_dsc.bg_grad.stops[0].opa = 200 * (255 - fract_upper) / 256;
    tri_dsc.bg_grad.stops[0].frac = 0;
    tri_dsc.bg_grad.stops[1].color = lv_color_hex(0x3987CF);
    tri_dsc.bg_grad.stops[1].opa = 200 * (255 - fract_lower) / 256;
    tri_dsc.bg_grad.stops[1].frac = 255;

    lv_draw_triangle(base_dsc->layer, &tri_dsc);

    /*Draw rectangle below the triangle*/
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_grad.dir = LV_GRAD_DIR_VER;
    rect_dsc.bg_grad.stops[0].color = lv_color_hex(0x3987CF);
    rect_dsc.bg_grad.stops[0].frac = 0;
    rect_dsc.bg_grad.stops[0].opa = 200 * (255 - fract_lower) / 256;
    rect_dsc.bg_grad.stops[1].color = lv_color_hex(0x3987CF);
    rect_dsc.bg_grad.stops[1].frac = 255;
    rect_dsc.bg_grad.stops[1].opa = 0;

    lv_area_t rect_area;
    rect_area.x1 = (int32_t)draw_line_dsc->p1.x;
    rect_area.x2 = (int32_t)draw_line_dsc->p2.x - 1;
    rect_area.y1 = (int32_t)LV_MAX(draw_line_dsc->p1.y, draw_line_dsc->p2.y);
    rect_area.y2 = (int32_t)obj->coords.y2;
    lv_draw_rect(base_dsc->layer, &rect_dsc, &rect_area);
}

static void chart_gesture_event_cb(lv_event_t * e)
{
    LV_UNUSED(e);
    lv_dir_t d = lv_indev_get_gesture_dir(lv_indev_active());

    int32_t week = lv_subject_get_int(&subject_week);
    if(d == LV_DIR_RIGHT) {
        if(week > 0) week--;
    }
    else if(d == LV_DIR_LEFT) {
        if(week < 3) week++;
    }
    else {
        return;
    }

    lv_indev_wait_release(lv_indev_active());
    lv_subject_set_int(&subject_week, week);
}

static void chart_week_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    int32_t week = lv_subject_get_int(&subject_week);
    lv_subject_set_int(&subject_day, week * 7 + 1);

    lv_obj_t * chart = lv_observer_get_target_obj(observer);
    lv_obj_t * cont = lv_obj_get_parent(chart);

    lv_point_t p;
    lv_chart_get_point_pos_by_id(chart, lv_chart_get_series_next(chart, NULL), week * 7, &p);
    lv_obj_scroll_to_x(cont,  p.x, LV_ANIM_ON);
}

static void chart_mode_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * chart = lv_observer_get_target_obj(observer);
    lv_chart_series_t * ser = lv_chart_get_series_next(chart, NULL);

    switch(lv_subject_get_int(&subject_mode)) {
        case MODE_AVG_SPEED:
            lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 30);
            lv_chart_set_ext_y_array(chart, ser, avg_speed_values);
            break;
        case MODE_TOP_SPEED:
            lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 50);
            lv_chart_set_ext_y_array(chart, ser, top_speed_values);
            break;
        case MODE_DISTANCE:
            lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 90);
            lv_chart_set_ext_y_array(chart, ser, distance_values);
            break;
        default:
            break;
    }
}

static void chart_day_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    LV_UNUSED(subject);
    lv_obj_t * chart = lv_observer_get_target_obj(observer);

    int32_t day = lv_subject_get_int(&subject_day);

    lv_subject_set_int(&subject_avg_speed, avg_speed_values[day]);
    lv_subject_set_int(&subject_distance, distance_values[day]);
    lv_subject_set_int(&subject_top_speed, top_speed_values[day]);

    lv_obj_invalidate(chart);
}

static lv_obj_t * chart_create(lv_obj_t * parent)
{
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_width(cont, lv_pct(100));
    lv_obj_set_flex_grow(cont, 1);
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_remove_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * chart = lv_chart_create(cont);
    lv_obj_set_flex_grow(chart, 1);
    lv_obj_set_size(chart, lv_pct(370), lv_pct(100));
    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_CIRCULAR);
    lv_chart_set_point_count(chart, 30);
    lv_chart_set_div_line_count(chart, 0, 0);
    lv_obj_remove_flag(chart, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_line_width(chart, 3, LV_PART_ITEMS);
    lv_obj_set_style_size(chart, 10, 10, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(chart, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(chart, lv_color_black(), LV_PART_INDICATOR);
    lv_obj_set_style_radius(chart, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
    lv_obj_set_style_border_color(chart, lv_color_black(), LV_PART_INDICATOR);
    lv_obj_set_style_border_width(chart, 2, LV_PART_INDICATOR);
    lv_obj_set_style_border_width(chart, 1, 0);
    lv_obj_set_style_border_side(chart, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_bg_opa(chart, 0, 0);
    lv_obj_set_style_margin_bottom(chart, 24, 0);
    lv_obj_set_style_max_height(chart, 260, 0);

    lv_obj_add_event_cb(chart, chart_value_changed_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(chart, chart_released_event_cb, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(chart, chart_draw_event_cb, LV_EVENT_DRAW_MAIN_BEGIN, NULL);
    lv_obj_add_event_cb(chart, chart_refr_ext_draw, LV_EVENT_REFR_EXT_DRAW_SIZE, NULL);
    lv_obj_add_event_cb(chart, chart_draw_task_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
    lv_obj_add_event_cb(chart, chart_gesture_event_cb, LV_EVENT_GESTURE, NULL);
    lv_obj_add_flag(chart, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

    lv_chart_series_t * ser = lv_chart_add_series(chart, lv_color_white(), 0);
    lv_chart_set_next_value(chart, ser, 30);
    lv_chart_set_next_value(chart, ser, 60);
    lv_chart_set_next_value(chart, ser, 22);
    lv_chart_set_next_value(chart, ser, 40);
    lv_chart_set_next_value(chart, ser, 48);
    lv_chart_set_next_value(chart, ser, 30);
    lv_chart_set_next_value(chart, ser, 69);
    lv_chart_set_next_value(chart, ser, 21);
    lv_chart_set_next_value(chart, ser, 60);

    lv_subject_add_observer_obj(&subject_week, chart_week_observer_cb, chart, NULL);
    lv_subject_add_observer_obj(&subject_mode, chart_mode_observer_cb, chart, NULL);
    lv_subject_add_observer_obj(&subject_day, chart_day_observer_cb, chart, NULL);

    return chart;
}

static lv_obj_t * stat_card_create(lv_obj_t * parent, const char * name, lv_subject_t * subject, const char * fmt)
{
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_track_place(cont, LV_FLEX_ALIGN_CENTER, 0);
    lv_obj_set_flex_grow(cont, 1);
    lv_obj_set_height(cont, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(cont, 0, 0);

    lv_obj_t * label = lv_label_create(cont);
    lv_label_set_text(label, name);
    lv_obj_set_style_text_font(label, EBIKE_FONT_SMALL, 0);

    label = lv_label_create(cont);
    lv_obj_set_style_text_font(label, EBIKE_FONT_MEDIUM, 0);
    lv_label_bind_text(label, subject, fmt);

    return cont;
}

static lv_obj_t * stat_cont_create(lv_obj_t * parent)
{
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_style_bg_opa(cont, 0, 0);
    lv_obj_set_size(cont, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW);

    stat_card_create(cont, _("Avg. speed"), &subject_avg_speed, "%dkm/h");
    stat_card_create(cont, _("Distance"), &subject_distance, "%dkm");
    stat_card_create(cont, _("Top speed"), &subject_top_speed, "%dkm/h");

    return cont;
}

static lv_obj_t * right_cont_create(lv_obj_t * parent)
{
    lv_obj_t * right_cont = lv_obj_create(parent);
    lv_obj_set_style_bg_opa(right_cont, 0, 0);
    lv_obj_set_flex_flow(right_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_flex_main_place(right_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, 0);
    lv_obj_set_style_pad_ver(right_cont, 12, 0);
    lv_obj_set_style_pad_right(right_cont, 8, 0);
    lv_obj_set_style_pad_row(right_cont, 8, 0);
    lv_obj_set_height(right_cont, lv_pct(100));

    tabs_create(right_cont);
    data_cont_create(right_cont);
    chart_create(right_cont);
    stat_cont_create(right_cont);

    return right_cont;
}

#endif /*LV_USE_DEMO_EBIKE*/
