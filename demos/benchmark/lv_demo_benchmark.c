/**
 * @file lv_demo_benchmark.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_benchmark.h"

#if LV_USE_DEMO_BENCHMARK

#if LV_FONT_MONTSERRAT_14 == 0
    #error "LV_FONT_MONTSERRAT_14 is required for lv_demo_benchmark. Enable it in lv_conf.h."
#endif

#if LV_FONT_MONTSERRAT_24 == 0
    #error "LV_FONT_MONTSERRAT_24 is required for lv_demo_benchmark. Enable it in lv_conf.h."
#endif

#if LV_USE_DEMO_WIDGETS == 0
    #error "LV_USE_DEMO_WIDGETS needs to be enabled"
#endif

#include "../../src/display/lv_display_private.h"
#include "../../src/core/lv_global.h"

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const char * name;
    void (*create_cb)(void);
    uint32_t scene_time;
    uint32_t cpu_avg_usage;
    uint32_t fps_avg;
    uint32_t render_avg_time;
    uint32_t flush_avg_time;
    uint32_t measurement_cnt;
} scene_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void load_scene(uint32_t scene);
static void next_scene_timer_cb(lv_timer_t * timer);

#if LV_USE_PERF_MONITOR
    static void sysmon_perf_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
#endif

static void summary_create(void);

static void rnd_reset(void);
static int32_t rnd_next(int32_t min, int32_t max);
static void shake_anim_y_cb(void * var, int32_t v);
static void shake_anim(lv_obj_t * obj, int32_t y_max);
static void scroll_anim(lv_obj_t * obj, int32_t y_max);
static void scroll_anim_y_cb(void * var, int32_t v);
static void color_anim_cb(void * var, int32_t v);
static void color_anim(lv_obj_t * obj);
static void arc_anim(lv_obj_t * obj);

static lv_obj_t * card_create(void);

static void empty_screen_cb(void)
{
    color_anim(lv_screen_active());
}

static void moving_wallpaper_cb(void)
{
    lv_obj_set_style_pad_all(lv_screen_active(), 0, 0);
    LV_IMG_DECLARE(img_benchmark_cogwheel_rgb);

    lv_obj_t * img = lv_img_create(lv_screen_active());
    lv_obj_set_size(img, lv_pct(150), lv_pct(150));
    lv_image_set_src(img, &img_benchmark_cogwheel_rgb);
    lv_image_set_align(img, LV_IMAGE_ALIGN_TILE);
    shake_anim(img, - lv_display_get_vertical_resolution(NULL) / 3);
}

static void single_rectangle_cb(void)
{
    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_center(obj);
    lv_obj_set_size(obj, lv_pct(30), lv_pct(30));

    color_anim(obj);

}

static void multiple_rectangles_cb(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_SPACE_EVENLY);

    uint32_t i;
    for(i = 0; i < 9; i++) {
        lv_obj_t * obj = lv_obj_create(lv_screen_active());
        lv_obj_remove_style_all(obj);
        lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
        lv_obj_set_size(obj, lv_pct(25), lv_pct(25));

        color_anim(obj);
    }
}

static void multiple_rgb_images_cb(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(lv_screen_active(), 20, 0);

    LV_IMG_DECLARE(img_benchmark_cogwheel_rgb);
    uint32_t hor_cnt = (lv_display_get_horizontal_resolution(NULL) - 16) / 116;
    uint32_t ver_cnt = (lv_display_get_vertical_resolution(NULL) - 116) / 116;

    uint32_t y;
    for(y = 0; y < ver_cnt; y++) {
        uint32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * obj = lv_img_create(lv_screen_active());
            lv_image_set_src(obj, &img_benchmark_cogwheel_rgb);
            if(x == 0) lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

            shake_anim(obj, 80);
        }
    }
}

static void multiple_argb_images_cb(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(lv_screen_active(), 20, 0);

    LV_IMG_DECLARE(img_benchmark_cogwheel_argb);
    uint32_t hor_cnt = (lv_display_get_horizontal_resolution(NULL) - 16) / 116;
    uint32_t ver_cnt = (lv_display_get_vertical_resolution(NULL) - 116) / 116;

    uint32_t y;
    for(y = 0; y < ver_cnt; y++) {
        uint32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * obj = lv_img_create(lv_screen_active());
            lv_image_set_src(obj, &img_benchmark_cogwheel_argb);
            if(x == 0) lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

            shake_anim(obj, 80);
        }
    }
}

static void rotated_argb_image_cb(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(lv_screen_active(), 20, 0);

    LV_IMG_DECLARE(img_benchmark_cogwheel_argb);
    uint32_t hor_cnt = (lv_display_get_horizontal_resolution(NULL) - 16) / 116;
    uint32_t ver_cnt = (lv_display_get_vertical_resolution(NULL) - 116) / 116;

    uint32_t y;
    for(y = 0; y < ver_cnt; y++) {
        uint32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * obj = lv_img_create(lv_screen_active());
            lv_image_set_src(obj, &img_benchmark_cogwheel_argb);
            if(x == 0) lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

            lv_image_set_rotation(obj, lv_rand(100, 3500));
            shake_anim(obj, 80);
        }
    }
}

static void multiple_labels_cb(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(lv_screen_active(), 80, 0);

    lv_point_t s;
    lv_text_get_size(&s, "Hello LVGL!", lv_obj_get_style_text_font(lv_screen_active(), 0), 0, 0, LV_COORD_MAX,
                     LV_TEXT_FLAG_NONE);

    uint32_t cnt = (lv_display_get_horizontal_resolution(NULL) - 16) / (s.x + 30);
    cnt = cnt * ((lv_display_get_vertical_resolution(NULL) - 200) / (s.y + 50));

    uint32_t i;
    for(i = 0; i < cnt; i++) {
        lv_obj_t * obj = lv_label_create(lv_screen_active());
        lv_label_set_text(obj, "Hello LVGL!");
        color_anim(obj);
    }
}

static void screen_sized_text_cb(void)
{
    const char * txt =
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla nec rhoncus arcu, in consectetur orci. Sed vitae dolor sed nisi ultrices vehicula quis ac dolor. Vivamus hendrerit hendrerit lectus, sed tempus velit suscipit in. Fusce eu tristique arcu. Sed et molestie leo, in lacinia nunc. Quisque semper lorem sed ante feugiat, at molestie risus blandit. Maecenas lobortis urna in diam feugiat porta. Ut facilisis mauris eget nibh posuere aliquet. Proin facilisis egestas magna, id vulputate massa bibendum a. Etiam gravida metus non egestas suscipit. Sed sollicitudin mollis nisi, eu fringilla leo vestibulum posuere. Donec et ex nulla. Phasellus et ornare justo, vel hendrerit justo. Curabitur pulvinar nunc sed tincidunt dignissim. Praesent eleifend lectus velit, id malesuada ante placerat id. Fusce massa erat, egestas vel venenatis eu, tempus nec est.\n\n"
        "Phasellus iaculis malesuada molestie. Cras ullamcorper justo a dolor dignissim tincidunt. Mauris euismod risus quis lobortis mollis. Ut vitae placerat massa, aliquet various lectus. Nulla ac ornare purus, quis auctor velit. Donec posuere dolor rhoncus efficitur dictum. Integer venenatis aliquet nunc eu convallis. Nunc quis various velit. Suspendisse enim metus, molestie eget mauris sit amet, euismod volutpat turpis. Duis rhoncus commodo gravida. Pellentesque velit mi, dictum id consequat placerat, condimentum ac elit. Duis aliquet leo eu dolor cursus rhoncus. Quisque aliquam sapien ut purus hendrerit laoreet. Ut venenatis venenatis risus, a vestibulum enim lobortis a. Maecenas auctor tortor lorem, quis laoreet nulla aliquet a. Sed ipsum lorem, facilisis in congue a, dictum ut ligula.\n\n"
        "Aliquam id tellus in enim hendrerit mattis. Sed ipsum arcu, feugiat sed eros quis, vulputate facilisis turpis. Quisque venenatis risus massa. Proin lacinia, nunc non ultrices commodo, ligula dolor lobortis lectus, iaculis pulvinar metus orci eu elit. Donec tincidunt lacinia semper. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Donec vitae odio risus. Donec sodales sed nulla sit amet iaculis. Duis lacinia mauris dictum, fermentum nibh eget, convallis tellus. Sed congue luctus purus non scelerisque. Etiam fermentum lacus mauris, at bibendum nunc aliquam at. Vivamus accumsan vestibulum pharetra. Proin rhoncus nisi purus, vel blandit metus auctor eget. Fusce dictum sed lectus sed aliquam. Praesent lobortis quam sed pretium tincidunt.\n\n"
        "Integer vehicula vestibulum eros. Donec facilisis magna a est cursus, sed posuere velit faucibus. In et ultrices lorem. Sed et lacus finibus, vulputate odio et, finibus tellus. Aenean finibus nibh vehicula elementum maximus. Maecenas in luctus tortor, vitae ullamcorper lacus. Ut nulla elit, aliquam at vestibulum ut, pellentesque non justo.\n\n"
        "Fusce dignissim turpis massa, eget semper purus semper at. Ut et augue vitae metus laoreet auctor. Morbi tincidunt, neque vel tincidunt interdum, sapien nibh finibus lorem, eu eleifend diam ipsum et eros. Duis iaculis vulputate lacinia. Phasellus id mauris sed magna gravida suscipit. Sed aliquet tincidunt ante ac posuere. In vestibulum quam ultricies, ultricies arcu eu, aliquam sapien. Phasellus sollicitudin velit facilisis, dignissim nisi sed, pellentesque magna.";

    lv_obj_t * scr = lv_screen_active();

    lv_obj_t * obj = lv_label_create(scr);
    lv_obj_set_width(obj, lv_pct(100));
    lv_label_set_text(obj, txt);

    lv_obj_update_layout(obj);

    scroll_anim(scr, lv_obj_get_scroll_bottom(scr));
}

static void multiple_arcs_cb(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    LV_IMG_DECLARE(img_benchmark_cogwheel_argb);
    uint32_t hor_cnt = (lv_display_get_horizontal_resolution(NULL) - 16) / lv_dpx(160);
    uint32_t ver_cnt = (lv_display_get_vertical_resolution(NULL) - 16) / lv_dpx(160);

    uint32_t y;
    for(y = 0; y < ver_cnt; y++) {
        uint32_t x;
        for(x = 0; x < hor_cnt; x++) {

            lv_obj_t * obj = lv_arc_create(lv_screen_active());
            if(x == 0) lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
            lv_obj_set_size(obj, lv_dpx(100), lv_dpx(100));
            lv_obj_center(obj);

            lv_arc_set_bg_angles(obj, 0, 360);

            lv_obj_set_style_margin_all(obj, lv_dpx(20), 0);
            lv_obj_set_style_arc_opa(obj, 0, LV_PART_MAIN);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_KNOB);
            lv_obj_set_style_arc_width(obj, 10, LV_PART_INDICATOR);
            lv_obj_set_style_arc_rounded(obj, false, LV_PART_INDICATOR);
            lv_obj_set_style_arc_color(obj, lv_color_hex3(lv_rand(0x00f, 0xff0)), LV_PART_INDICATOR);
            arc_anim(obj);
        }
    }
}

static void containers_cb(void)
{

    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    uint32_t hor_cnt = (lv_display_get_horizontal_resolution(NULL) - 16) / 300;
    uint32_t ver_cnt = (lv_display_get_vertical_resolution(NULL) - 16) / 150;

    uint32_t y;
    for(y = 0; y < ver_cnt; y++) {
        uint32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * card = card_create();
            if(x == 0) lv_obj_add_flag(card, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
            shake_anim(card, 30);
        }
    }
}

static void containers_with_overlay_cb(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    uint32_t hor_cnt = (lv_display_get_horizontal_resolution(NULL) - 16) / 300;
    uint32_t ver_cnt = (lv_display_get_vertical_resolution(NULL) - 16) / 150;

    uint32_t y;
    for(y = 0; y < ver_cnt; y++) {
        uint32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * card = card_create();
            if(x == 0) lv_obj_add_flag(card, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
            shake_anim(card, 30);
        }
    }

    lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_50, 0);
    color_anim(lv_layer_top());
}

static void containers_with_opa_cb(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    uint32_t hor_cnt = (lv_display_get_horizontal_resolution(NULL) - 16) / 300;
    uint32_t ver_cnt = (lv_display_get_vertical_resolution(NULL) - 16) / 150;

    uint32_t y;
    for(y = 0; y < ver_cnt; y++) {
        uint32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * card = card_create();
            if(x == 0) lv_obj_add_flag(card, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
            lv_obj_set_style_opa(card, LV_OPA_50, 0);
            shake_anim(card, 30);
        }
    }
}

static void containers_with_opa_layer_cb(void)
{
    lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    uint32_t hor_cnt = (lv_display_get_horizontal_resolution(NULL) - 16) / 300;
    uint32_t ver_cnt = (lv_display_get_vertical_resolution(NULL) - 16) / 150;

    uint32_t y;
    for(y = 0; y < ver_cnt; y++) {
        uint32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * card = card_create();
            lv_obj_set_style_opa_layered(card, LV_OPA_50, 0);
            if(x == 0) lv_obj_add_flag(card, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
            shake_anim(card, 30);
        }
    }
}

static void containers_with_scrolling_cb(void)
{
    lv_obj_t * scr = lv_screen_active();

    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    uint32_t i;
    for(i = 0; i < 50; i++) {
        card_create();
    }

    lv_obj_update_layout(scr);
    scroll_anim(scr, lv_obj_get_scroll_bottom(scr));
}

static void widgets_demo_cb(void)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_style_pad_hor(scr, 0, 0);
    lv_obj_set_style_pad_bottom(scr, 0, 0);
    lv_demo_widgets();
    lv_demo_widgets_start_slideshow();

}

/**********************
 *  STATIC VARIABLES
 **********************/

static scene_dsc_t scenes[] = {
    {.name = "Empty screen",               .scene_time = 3000, .create_cb = empty_screen_cb},
    {.name = "Moving wallpaper",           .scene_time = 3000, .create_cb = moving_wallpaper_cb},
    {.name = "Single rectangle",           .scene_time = 3000, .create_cb = single_rectangle_cb},
    {.name = "Multiple rectangles",        .scene_time = 3000, .create_cb = multiple_rectangles_cb},
    {.name = "Multiple RGB images",        .scene_time = 3000, .create_cb = multiple_rgb_images_cb},
    {.name = "Multiple ARGB images",       .scene_time = 3000, .create_cb = multiple_argb_images_cb},
    {.name = "Rotated ARGB images",        .scene_time = 3000, .create_cb = rotated_argb_image_cb},
    {.name = "Multiple labels",            .scene_time = 3000, .create_cb = multiple_labels_cb},
    {.name = "Screen sized text",          .scene_time = 5000, .create_cb = screen_sized_text_cb},
    {.name = "Multiple arcs",              .scene_time = 3000, .create_cb = multiple_arcs_cb},

    {.name = "Containers",                 .scene_time = 3000, .create_cb = containers_cb},
    {.name = "Containers with overlay",    .scene_time = 3000, .create_cb = containers_with_overlay_cb},
    {.name = "Containers with opa",        .scene_time = 3000, .create_cb = containers_with_opa_cb},
    {.name = "Containers with opa_layer",  .scene_time = 3000, .create_cb = containers_with_opa_layer_cb},
    {.name = "Containers with scrolling",  .scene_time = 5000, .create_cb = containers_with_scrolling_cb},

    {.name = "Widgets demo",               .scene_time = 20000,           .create_cb = widgets_demo_cb},

    {.name = "", .create_cb = NULL}
};

#define sysmon_perf LV_GLOBAL_DEFAULT()->sysmon_perf

static uint32_t scene_act;
static uint32_t rnd_act;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_demo_benchmark(void)
{
    scene_act = 0;

    lv_obj_t * scr = lv_screen_active();
    lv_obj_remove_style_all(scr);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(scr, lv_color_black(), 0);
    lv_obj_set_style_bg_color(scr, lv_palette_lighten(LV_PALETTE_GREY, 4), 0);
    lv_obj_set_style_pad_all(lv_screen_active(), 8, 0);
    lv_obj_set_style_pad_top(lv_screen_active(), 48, 0);
    lv_obj_set_style_pad_gap(lv_screen_active(), 8, 0);

    lv_obj_t * title = lv_label_create(lv_layer_top());
    lv_obj_set_style_bg_opa(title, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(title, lv_color_white(), 0);
    lv_obj_set_style_text_color(title, lv_color_black(), 0);
    lv_obj_set_width(title, lv_pct(100));

    load_scene(scene_act);

    lv_timer_create(next_scene_timer_cb, scenes[0].scene_time, NULL);

#if LV_USE_PERF_MONITOR
    lv_subject_add_observer_obj(&sysmon_perf.subject, sysmon_perf_observer_cb, title, NULL);
#else
    lv_label_set_text(title, "LV_USE_PERF_MONITOR is not enabled");
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void load_scene(uint32_t scene)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_clean(scr);
    lv_obj_set_style_bg_color(scr, lv_palette_lighten(LV_PALETTE_GREY, 4), 0);
    lv_obj_set_style_text_color(scr, lv_color_black(), 0);
    lv_obj_set_style_pad_all(lv_screen_active(), 8, 0);
    lv_obj_set_style_pad_top(lv_screen_active(), 48, 0);
    lv_obj_set_style_pad_gap(lv_screen_active(), 8, 0);
    lv_obj_set_layout(scr, LV_LAYOUT_NONE);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    lv_anim_delete(scr, scroll_anim_y_cb);
    lv_anim_delete(scr, shake_anim_y_cb);
    lv_anim_delete(scr, color_anim_cb);

    lv_anim_delete(lv_layer_top(), color_anim_cb);
    lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_TRANSP, 0);

    rnd_reset();
    if(scenes[scene].create_cb) scenes[scene].create_cb();
}

static void next_scene_timer_cb(lv_timer_t * timer)
{
    LV_UNUSED(timer);

    scene_act++;

    load_scene(scene_act);
    if(scenes[scene_act].scene_time == 0) {
        lv_timer_delete(timer);
        summary_create();
    }
    else {
        lv_timer_set_period(timer, scenes[scene_act].scene_time);
    }
}

#if LV_USE_PERF_MONITOR
static void sysmon_perf_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    const lv_sysmon_perf_info_t * info = lv_subject_get_pointer(subject);
    lv_obj_t * label = lv_observer_get_target(observer);

    char scene_name[64];
    if(scenes[scene_act].name[0] != '\0') {
        lv_snprintf(scene_name, sizeof(scene_name), "%s: ", scenes[scene_act].name);
    }
    else {
        scene_name[0] = '\0';
    }

    lv_label_set_text_fmt(label,
                          "%s"
                          "%" LV_PRIu32" FPS, %" LV_PRIu32 "%% CPU\n"
                          "refr. %" LV_PRIu32" ms = %" LV_PRIu32 "ms render + %" LV_PRIu32" ms flush",
                          scene_name,
                          info->calculated.fps, info->calculated.cpu,
                          info->calculated.render_avg_time + info->calculated.flush_avg_time,
                          info->calculated.render_avg_time, info->calculated.flush_avg_time);

    /*Ignore the first call as it contains data from the previous scene*/
    if(scenes[scene_act].measurement_cnt != 0) {
        scenes[scene_act].cpu_avg_usage += info->calculated.cpu;
        scenes[scene_act].fps_avg += info->calculated.fps;
        scenes[scene_act].render_avg_time += info->calculated.render_avg_time;
        scenes[scene_act].flush_avg_time += info->calculated.flush_avg_time;
    }
    scenes[scene_act].measurement_cnt++;

}
#endif

static void table_draw_task_event_cb(lv_event_t * e)
{
    lv_draw_task_t * t = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t * draw_dsc_base = t->draw_dsc;
    if(draw_dsc_base->part != LV_PART_ITEMS) return;

    int32_t row = draw_dsc_base->id1;
    if(row == 0) {
        lv_draw_fill_dsc_t * draw_dsc_fill = lv_draw_task_get_fill_dsc(t);
        if(draw_dsc_fill) {
            draw_dsc_fill->color = lv_palette_darken(LV_PALETTE_BLUE_GREY, 4);
        }
        lv_draw_label_dsc_t * draw_dsc_label = lv_draw_task_get_label_dsc(t);
        if(draw_dsc_label) {
            draw_dsc_label->color = lv_color_white();
        }
    }
    else if(row == 1) {
        lv_draw_border_dsc_t * draw_dsc_border = lv_draw_task_get_border_dsc(t);
        if(draw_dsc_border) {
            draw_dsc_border->color = lv_palette_darken(LV_PALETTE_BLUE_GREY, 4);
            draw_dsc_border->width = 2;
            draw_dsc_border->side = LV_BORDER_SIDE_BOTTOM;
        }
        lv_draw_label_dsc_t * draw_dsc_label = lv_draw_task_get_label_dsc(t);
        if(draw_dsc_label) {
            draw_dsc_label->color = lv_palette_darken(LV_PALETTE_BLUE_GREY, 4);
        }
    }

}

static void summary_create(void)
{
    lv_obj_clean(lv_screen_active());
    lv_obj_set_style_pad_hor(lv_screen_active(), 0, 0);
    lv_obj_t * table = lv_table_create(lv_screen_active());
    lv_obj_set_width(table, lv_pct(100));
    lv_obj_set_style_max_height(table, lv_pct(100), 0);
    lv_obj_add_flag(table, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    lv_obj_set_style_text_color(table, lv_palette_darken(LV_PALETTE_BLUE_GREY, 2), LV_PART_ITEMS);
    lv_obj_set_style_border_color(table, lv_palette_darken(LV_PALETTE_BLUE_GREY, 2), LV_PART_ITEMS);
    lv_obj_add_event_cb(table, table_draw_task_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);

    lv_table_set_cell_value(table, 0, 0, "Name");
    lv_table_set_cell_value(table, 0, 1, "Avg. CPU");
    lv_table_set_cell_value(table, 0, 2, "Avg. FPS");
    lv_table_set_cell_value(table, 0, 3, "Avg. time (render + flush)");

    /* csv log */
    LV_LOG("Benchmark Summary (%"LV_PRIu32".%"LV_PRIu32".%"LV_PRIu32" %s)\r\n",
           LVGL_VERSION_MAJOR,
           LVGL_VERSION_MINOR,
           LVGL_VERSION_PATCH,
           LVGL_VERSION_INFO);
    LV_LOG("Name, Avg. CPU, Avg. FPS, Avg. time, render time, flush time\r\n");

    lv_obj_update_layout(table);
    int32_t col_w = lv_obj_get_content_width(table) / 4;

    lv_table_set_column_width(table, 0, col_w);
    lv_table_set_column_width(table, 1, col_w);
    lv_table_set_column_width(table, 2, col_w);
    lv_table_set_column_width(table, 3, col_w);

    uint32_t i;
    int32_t total_avg_fps = 0;
    int32_t total_avg_cpu = 0;
    int32_t total_avg_render_time = 0;
    int32_t total_avg_flush_time = 0;
    int32_t valid_scene_cnt = 0;
    for(i = 0; scenes[i].create_cb; i++) {
        lv_table_set_cell_value(table, i + 2, 0, scenes[i].name);

        /*the first measurement was ignored as it contains data from the previous scene*/
        if(scenes[i].measurement_cnt <= 1) {
            lv_table_set_cell_value(table, i + 2, 1, "N/A");
            lv_table_set_cell_value(table, i + 2, 2, "N/A");
            lv_table_set_cell_value(table, i + 2, 3, "N/A");
        }
        else {
            int32_t cnt = scenes[i].measurement_cnt - 1;
            lv_table_set_cell_value_fmt(table, i + 2, 1, "%"LV_PRIu32" %%", scenes[i].cpu_avg_usage / cnt);
            lv_table_set_cell_value_fmt(table, i + 2, 2, "%"LV_PRIu32" FPS", scenes[i].fps_avg / cnt);

            uint32_t render_time = scenes[i].render_avg_time / cnt;
            uint32_t flush_time = scenes[i].flush_avg_time / cnt;
            lv_table_set_cell_value_fmt(table, i + 2, 3, "%"LV_PRIu32" ms (%"LV_PRIu32" + %"LV_PRIu32")",
                                        render_time + flush_time, render_time, flush_time);

            /* csv log */
            LV_LOG("%s, %"LV_PRIu32"%%, %"LV_PRIu32", %"LV_PRIu32", %"LV_PRIu32", %"LV_PRIu32"\r\n",
                   scenes[i].name,
                   scenes[i].cpu_avg_usage / cnt,
                   scenes[i].fps_avg / cnt,
                   render_time + flush_time,
                   render_time,
                   flush_time);

            valid_scene_cnt++;
            total_avg_cpu += scenes[i].cpu_avg_usage / cnt;
            total_avg_fps += scenes[i].fps_avg / cnt;
            total_avg_render_time += scenes[i].render_avg_time / cnt;
            total_avg_flush_time += scenes[i].flush_avg_time / cnt;
        }
    }

    /*Add the average*/
    lv_table_set_cell_value(table, 1, 0, "All scenes avg.");
    if(valid_scene_cnt < 1) {
        lv_table_set_cell_value(table, 1, 1, "N/A");
        lv_table_set_cell_value(table, 1, 2, "N/A");
        lv_table_set_cell_value(table, 1, 3, "N/A");
    }
    else {
        lv_table_set_cell_value_fmt(table, 1, 1, "%"LV_PRIu32" %%", total_avg_cpu / valid_scene_cnt);
        lv_table_set_cell_value_fmt(table, 1, 2, "%"LV_PRIu32" FPS", total_avg_fps / valid_scene_cnt);

        uint32_t render_time = total_avg_render_time / valid_scene_cnt;
        uint32_t flush_time = total_avg_flush_time / valid_scene_cnt;
        lv_table_set_cell_value_fmt(table, 1, 3, "%"LV_PRIu32" ms (%"LV_PRIu32" + %"LV_PRIu32")",
                                    render_time + flush_time, render_time, flush_time);
        /* csv log */
        LV_LOG("All scenes avg.,%"LV_PRIu32"%%, %"LV_PRIu32", %"LV_PRIu32", %"LV_PRIu32", %"LV_PRIu32"\r\n",
               total_avg_cpu / valid_scene_cnt,
               total_avg_fps / valid_scene_cnt,
               render_time + flush_time,
               render_time,
               flush_time);
    }
}

/*----------------
 * SCENE HELPERS
 *----------------*/

static void color_anim_cb(void * var, int32_t v)
{
    LV_UNUSED(v);
    lv_obj_set_style_bg_color(var, lv_color_hex3(lv_rand(0x00f, 0xff0)), 0);
    lv_obj_set_style_text_color(var, lv_color_hex3(lv_rand(0x00f, 0xff0)), 0);
}

static void color_anim(lv_obj_t * obj)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, color_anim_cb);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_duration(&a, 100);      /*New value in each ms*/
    lv_anim_set_var(&a, obj);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}

static void arc_anim_cb(void * var, int32_t v)
{
    lv_arc_set_value(var, v);
}

static void arc_anim(lv_obj_t * obj)
{
    uint32_t t1 = rnd_next(1000, 3000);
    uint32_t t2 = rnd_next(1000, 3000);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, arc_anim_cb);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_duration(&a, t1);
    lv_anim_set_playback_duration(&a, t2);
    lv_anim_set_var(&a, obj);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}

static void scroll_anim_y_cb(void * var, int32_t v)
{
    lv_obj_scroll_to_y(var, v, LV_ANIM_OFF);
}

static void scroll_anim(lv_obj_t * obj, int32_t y_max)
{
    uint32_t t = lv_anim_speed(lv_display_get_dpi(NULL));

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, scroll_anim_y_cb);
    lv_anim_set_values(&a, 0, y_max);
    lv_anim_set_duration(&a, t);
    lv_anim_set_playback_duration(&a, t);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);

}
static void shake_anim_y_cb(void * var, int32_t v)
{
    lv_obj_set_style_translate_y(var, v, 0);
}

static void shake_anim(lv_obj_t * obj, int32_t y_max)
{
    uint32_t t1 = rnd_next(300, 3000);
    uint32_t t2 = rnd_next(300, 3000);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, shake_anim_y_cb);
    lv_anim_set_values(&a, 0, y_max);
    lv_anim_set_duration(&a, t1);
    lv_anim_set_playback_duration(&a, t2);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}

static lv_obj_t * card_create(void)
{
    lv_obj_t * panel = lv_obj_create(lv_screen_active());
    lv_obj_set_size(panel, 270, 120);
    lv_obj_set_style_pad_all(panel, 8, 0);

    LV_IMG_DECLARE(img_transform_avatar_15);
    lv_obj_t * child = lv_image_create(panel);
    lv_obj_align(child, LV_ALIGN_LEFT_MID, 0, 0);
    lv_image_set_src(child, &img_transform_avatar_15);

    child = lv_label_create(panel);
    lv_label_set_text(child, "John Smith");
    lv_obj_set_style_text_font(child, &lv_font_montserrat_24, 0);
    lv_obj_set_pos(child, 100, 0);

    child = lv_label_create(panel);
    lv_label_set_text(child, "A DIY enthusiast");
    lv_obj_set_style_text_font(child, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(child, 100, 30);

    child = lv_button_create(panel);
    lv_obj_set_pos(child, 100, 50);

    child = lv_label_create(child);
    lv_label_set_text(child, "Connect");

    return panel;
}

static void rnd_reset(void)
{
    rnd_act = 0;
}

static int32_t rnd_next(int32_t min, int32_t max)
{
    static const uint32_t rnd_map[] = {
        0xbd13204f, 0x67d8167f, 0x20211c99, 0xb0a7cc05,
        0x06d5c703, 0xeafb01a7, 0xd0473b5c, 0xc999aaa2,
        0x86f9d5d9, 0x294bdb29, 0x12a3c207, 0x78914d14,
        0x10a30006, 0x6134c7db, 0x194443af, 0x142d1099,
        0x376292d5, 0x20f433c5, 0x074d2a59, 0x4e74c293,
        0x072a0810, 0xdd0f136d, 0x5cca6dbc, 0x623bfdd8,
        0xb645eb2f, 0xbe50894a, 0xc9b56717, 0xe0f912c8,
        0x4f6b5e24, 0xfe44b128, 0xe12d57a8, 0x9b15c9cc,
        0xab2ae1d3, 0xb4dc5074, 0x67d457c8, 0x8e46b00c,
        0xa29a1871, 0xcee40332, 0x80f93aa1, 0x85286096,
        0x09bd6b49, 0x95072088, 0x2093924b, 0x6a27328f,
        0xa796079b, 0xc3b488bc, 0xe29bcce0, 0x07048a4c,
        0x7d81bd99, 0x27aacb30, 0x44fc7a0e, 0xa2382241,
        0x8357a17d, 0x97e9c9cc, 0xad10ff52, 0x9923fc5c,
        0x8f2c840a, 0x20356ba2, 0x7997a677, 0x9a7f1800,
        0x35c7562b, 0xd901fe51, 0x8f4e053d, 0xa5b94923,
    };

    if(min == max) return min;

    if(min > max) {
        int32_t t = min;
        min = max;
        max = t;
    }

    int32_t d = max - min;
    int32_t r = (rnd_map[rnd_act] % d) + min;

    rnd_act++;
    if(rnd_act >= sizeof(rnd_map) / sizeof(rnd_map[0])) rnd_act = 0;

    return r;
}

#endif
