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

#if LV_FONT_MONTSERRAT_20 == 0
    #error "LV_FONT_MONTSERRAT_20 is required for lv_demo_benchmark. Enable it in lv_conf.h."
#endif

#if LV_FONT_MONTSERRAT_24 == 0
    #error "LV_FONT_MONTSERRAT_24 is required for lv_demo_benchmark. Enable it in lv_conf.h."
#endif

#if LV_FONT_MONTSERRAT_26 == 0
    #error "LV_FONT_MONTSERRAT_26 is required for lv_demo_benchmark. Enable it in lv_conf.h."
#endif

#if LV_USE_DEMO_WIDGETS == 0
    #error "LV_USE_DEMO_WIDGETS needs to be enabled"
#endif

#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN && LV_MEM_SIZE < 128 * 1024
    #warning "It's recommended to have at least 128kB RAM for the benchmark"
#endif

#if LV_USE_TINY_TTF
    #if LV_TINY_TTF_FILE_SUPPORT == 0
        #pragma message("WARNING: You choose to test tinyTTF, but LV_TINY_TTF_FILE_SUPPORT is not enabled. TinyTTF will not be tested.")
        #define TEST_TINY_TTF 0
    #else
        #define TEST_TINY_TTF 1
    #endif
#endif

#include "../../lvgl_private.h"

/**********************
 *      DEFINES
 **********************/
#if LV_USE_PERF_MONITOR_LOG_MODE == 1
    #define HEADER_HEIGHT   0
#else
    #define HEADER_HEIGHT   48
#endif
#define FALL_HEIGHT     80
#define PAD_BASIC       8

#define LV_TEST_FONT_STRING_CHINESE "法律之前人人平等,并有权享受法律的平等保护,不受任何歧视。人人有权享受平等保护,以免受违反本宣言的任何歧视行为以及煽动这种歧视的任何行为之害。"
#define LV_TEST_FONT_STRING_ENGLISH "No one shall be subjected to arbitrary arrest, detention or exile. Everyone is entitled in full equality to a fair and public hearing by an independent and impartial tribunal, in the determination of his rights and obligations and of any criminal charge against him. No one shall be subjected to arbitrary interference with his privacy, family, home or correspondence, nor to attacks upon his honour and reputation. Everyone has the right to the protection of the law against such interference or attacks."
#define LV_TEST_FONT_SIZE 24

/**********************
 *      TYPEDEFS
 **********************/

typedef struct benchmark_context {
    lv_font_t * font_bitmap;
    lv_font_t * font_outline;
    lv_font_t * font_tinyttf;
    lv_obj_t * label_perf;
    uint32_t scene_act;
    uint32_t rnd_act;
} benchmark_context_t;

typedef struct scene_dsc {
    const char * name;
    void (*create_cb)(benchmark_context_t *);
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

static void load_scene(benchmark_context_t * context);
static void next_scene_timer_cb(lv_timer_t * timer);

#if LV_USE_PERF_MONITOR
    static void sysmon_perf_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
#endif

static void summary_create(void);

static void rnd_reset(benchmark_context_t * context);
static int32_t rnd_next(benchmark_context_t * context, int32_t min, int32_t max);
static void shake_anim_y_cb(void * var, int32_t v);
static void fall_anim(benchmark_context_t * context, lv_obj_t * obj, int32_t y_max);
static void scroll_anim(lv_obj_t * obj, int32_t y_max);
static void scroll_anim_y_cb(void * var, int32_t v);
static void color_anim_cb(void * var, int32_t v);
static void color_anim(lv_obj_t * obj);
static void arc_anim(benchmark_context_t * context, lv_obj_t * obj);

static lv_obj_t * card_create(void);

#if LV_USE_FREETYPE
static void span_text_add(lv_obj_t * spans, lv_font_t * font, const char * text);

static void span_text_bitmap_cb(benchmark_context_t * context)
{
    lv_obj_t * spans = lv_spangroup_create(lv_screen_active());
    lv_obj_center(spans);
    lv_obj_set_width(spans, lv_display_get_horizontal_resolution(NULL));
    lv_obj_set_style_text_align(spans, LV_TEXT_ALIGN_LEFT, 0);
    lv_spangroup_set_overflow(spans, LV_SPAN_OVERFLOW_CLIP);
    lv_spangroup_set_mode(spans, LV_SPAN_MODE_BREAK);

    span_text_add(spans, context->font_bitmap, LV_TEST_FONT_STRING_CHINESE);
}

static void span_text_outline_cb(benchmark_context_t * context)
{
    lv_obj_t * spans = lv_spangroup_create(lv_screen_active());
    lv_obj_center(spans);
    lv_obj_set_width(spans, lv_display_get_horizontal_resolution(NULL));
    lv_obj_set_style_text_align(spans, LV_TEXT_ALIGN_LEFT, 0);
    lv_spangroup_set_overflow(spans, LV_SPAN_OVERFLOW_CLIP);
    lv_spangroup_set_mode(spans, LV_SPAN_MODE_BREAK);

    span_text_add(spans, context->font_outline, LV_TEST_FONT_STRING_CHINESE);
    span_text_add(spans, context->font_outline, LV_TEST_FONT_STRING_ENGLISH);
}

static void ttf_text_bitmap_cb(benchmark_context_t * context)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    if(context->font_bitmap) {
        lv_obj_set_style_text_font(label, context->font_bitmap, 0);
    }
    lv_label_set_text(label, LV_TEST_FONT_STRING_CHINESE);
    lv_obj_set_width(label, lv_pct(100));
}

static void ttf_text_outline_cb(benchmark_context_t * context)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    if(context->font_outline) {
        lv_obj_set_style_text_font(label, context->font_outline, 0);
    }
    lv_label_set_text(label, LV_TEST_FONT_STRING_ENGLISH);
    lv_obj_set_width(label, lv_pct(100));
}
#endif

#if TEST_TINY_TTF
static void tiny_ttf_span_text_cb(benchmark_context_t * context)
{
    lv_obj_t * spans = lv_spangroup_create(lv_screen_active());
    lv_obj_center(spans);
    lv_obj_set_width(spans, lv_display_get_horizontal_resolution(NULL));
    lv_obj_set_style_text_align(spans, LV_TEXT_ALIGN_LEFT, 0);
    lv_spangroup_set_overflow(spans, LV_SPAN_OVERFLOW_CLIP);
    lv_spangroup_set_mode(spans, LV_SPAN_MODE_BREAK);

    span_text_add(spans, context->font_tinyttf, LV_TEST_FONT_STRING_CHINESE);
}

static void tiny_ttf_text_cb(benchmark_context_t * context)
{
    lv_obj_t * label = lv_label_create(lv_screen_active());
    if(context->font_tinyttf) {
        lv_obj_set_style_text_font(label, context->font_tinyttf, 0);
    }
    lv_label_set_text(label, LV_TEST_FONT_STRING_ENGLISH);
    lv_obj_set_width(label, lv_pct(100));
}
#endif

static void empty_screen_cb(benchmark_context_t * context)
{
    LV_UNUSED(context);
    color_anim(lv_screen_active());
}

static void moving_wallpaper_cb(benchmark_context_t * context)
{
    lv_obj_set_style_pad_all(lv_screen_active(), 0, 0);
    LV_IMAGE_DECLARE(img_benchmark_lvgl_logo_rgb);

    lv_obj_t * img = lv_image_create(lv_screen_active());
    lv_obj_set_size(img, lv_pct(150), lv_pct(150));
    lv_image_set_src(img, &img_benchmark_lvgl_logo_rgb);
    lv_image_set_inner_align(img, LV_IMAGE_ALIGN_TILE);
    fall_anim(context, img, - lv_display_get_vertical_resolution(NULL) / 3);
}

static void single_rectangle_cb(benchmark_context_t * context)
{
    LV_UNUSED(context);

    lv_obj_t * obj = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_center(obj);
    lv_obj_set_size(obj, lv_pct(30), lv_pct(30));

    color_anim(obj);
}

static void multiple_rectangles_cb(benchmark_context_t * context)
{
    LV_UNUSED(context);

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

static void multiple_rgb_images_cb(benchmark_context_t * context)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);
    lv_obj_set_style_pad_bottom(scr, FALL_HEIGHT + PAD_BASIC, 0);

    LV_IMAGE_DECLARE(img_benchmark_lvgl_logo_rgb);
    int32_t hor_cnt = ((int32_t)lv_obj_get_content_width(scr)) / 160;
    int32_t ver_cnt = ((int32_t)lv_obj_get_content_height(scr)) / 160;

    if(hor_cnt < 1) hor_cnt = 1;
    if(ver_cnt < 1) ver_cnt = 1;

    int32_t y;
    for(y = 0; y < ver_cnt; y++) {
        int32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * obj = lv_image_create(lv_screen_active());
            lv_image_set_src(obj, &img_benchmark_lvgl_logo_rgb);
            if(x == 0) lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

            fall_anim(context, obj, 80);
        }
    }
}

static void multiple_argb_images_cb(benchmark_context_t * context)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);
    lv_obj_set_style_pad_bottom(scr, FALL_HEIGHT + PAD_BASIC, 0);

    LV_IMAGE_DECLARE(img_benchmark_lvgl_logo_argb);
    int32_t hor_cnt = ((int32_t)lv_obj_get_content_width(scr)) / 160;
    int32_t ver_cnt = ((int32_t)lv_obj_get_content_height(scr)) / 160;

    if(hor_cnt < 1) hor_cnt = 1;
    if(ver_cnt < 1) ver_cnt = 1;

    int32_t y;
    for(y = 0; y < ver_cnt; y++) {
        int32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * obj = lv_image_create(lv_screen_active());
            lv_image_set_src(obj, &img_benchmark_lvgl_logo_argb);
            if(x == 0) lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

            fall_anim(context, obj, 80);
        }
    }
}

static void rotated_argb_image_cb(benchmark_context_t * context)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);
    lv_obj_set_style_pad_bottom(scr, FALL_HEIGHT + PAD_BASIC, 0);

    LV_IMAGE_DECLARE(img_benchmark_lvgl_logo_argb);
    int32_t hor_cnt = ((int32_t)lv_obj_get_content_width(scr)) / 240;   /*240 instead of 160 to have less rotated images*/
    int32_t ver_cnt = ((int32_t)lv_obj_get_content_height(scr)) / 240;

    if(hor_cnt < 1) hor_cnt = 1;
    if(ver_cnt < 1) ver_cnt = 1;

    int32_t y;
    for(y = 0; y < ver_cnt; y++) {
        int32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * obj = lv_image_create(lv_screen_active());
            lv_image_set_src(obj, &img_benchmark_lvgl_logo_argb);
            if(x == 0) lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);

            lv_image_set_rotation(obj, lv_rand(100, 3500));
            fall_anim(context, obj, 80);
        }
    }
}

static void multiple_labels_cb(benchmark_context_t * context)
{
    LV_UNUSED(context);
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);

    int32_t hor_res = lv_display_get_horizontal_resolution(NULL);
    int32_t ver_res = lv_display_get_vertical_resolution(NULL);
    if(hor_res * ver_res > 800 * 480) lv_obj_set_style_text_font(scr, &lv_font_montserrat_26, 0);
    else if(hor_res * ver_res > 320 * 240) lv_obj_set_style_text_font(scr, &lv_font_montserrat_20, 0);
    else lv_obj_set_style_text_font(scr, &lv_font_montserrat_14, 0);

    lv_point_t s;
    lv_text_get_size(&s, "Hello LVGL!", lv_obj_get_style_text_font(scr, 0), 0, 0, LV_COORD_MAX,
                     LV_TEXT_FLAG_NONE);

    int32_t hor_cnt = ((int32_t)lv_obj_get_content_width(scr)) / (s.x * 3 / 2);
    int32_t ver_cnt = ((int32_t)lv_obj_get_content_height(scr)) / (s.y  * 3);

    if(hor_cnt < 1) hor_cnt = 1;
    if(ver_cnt < 1) ver_cnt = 1;

    int32_t y;
    for(y = 0; y < ver_cnt; y++) {
        int32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * obj = lv_label_create(lv_screen_active());
            if(x == 0) lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
            lv_label_set_text_static(obj, "Hello LVGL!");
            color_anim(obj);
        }
    }
}

static void screen_sized_text_cb(benchmark_context_t * context)
{
    LV_UNUSED(context);

    const char * txt =
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque fringilla, lorem dapibus fringilla feugiat, justo arcu volutpat magna, vitae ultricies metus tortor nec est. Fusce ut tellus arcu. Fusce eu rutrum metus, nec porta felis. Sed sed ligula laoreet, sodales lacus blandit, elementum justo. Sed posuere quam ut pellentesque ullamcorper. In quis consequat magna. Etiam quis turpis nec lorem dictum finibus. Donec mattis enim dolor, consequat lacinia nisi scelerisque id. Nulla euismod, purus sit amet accumsan tempus, lorem lectus euismod dolor, sit amet facilisis nisl quam elementum nisi. Curabitur et massa eget lorem lacinia scelerisque eget vitae felis. Nulla facilisi.\n\n"
        "Vivamus auctor sit amet ante id rhoncus. Duis a dolor neque. Mauris eu ornare tortor. Vivamus consequat, ipsum a volutpat congue, sem libero laoreet nulla, malesuada efficitur leo orci a est. Donec tincidunt nulla nibh, quis pretium mi fermentum quis. Fusce a mattis libero. Curabitur in felis suscipit, ultrices diam imperdiet, vestibulum arcu. Praesent id faucibus turpis. Pellentesque sed massa tincidunt, interdum purus tempus, pellentesque risus. Fusce feugiat magna eget nisl eleifend efficitur. Mauris ut convallis justo. Integer malesuada rutrum orci non tincidunt.\n\n"
        "Nullam aliquet leo sit amet volutpat tincidunt. Mauris ac accumsan nibh. Morbi accumsan commodo leo, at hendrerit massa hendrerit et. Aliquam nec sodales ex. Morbi at aliquet sem. Sed at magna ut felis mollis dictum ut ac orci. Nunc id lorem lacus. Vivamus id accumsan dolor, sed suscipit nulla. Pellentesque dictum erat non bibendum tempor. Fusce arcu risus, eleifend in lacus a, iaculis fermentum sapien. Praesent sodales libero vitae massa suscipit tincidunt. Aliquam quis arcu urna. Nunc sit amet mi leo.\n\n"
        "Aliquam erat volutpat. Sed viverra pharetra ipsum, sed various arcu various nec. Curabitur rutrum odio et pretium fermentum. Maecenas vitae ligula nisi. Maecenas nec dapibus erat. Suspendisse vel accumsan erat. Proin congue diam at turpis accumsan eleifend.\n\n"
        "Etiam suscipit metus magna, in vulputate magna cursus eget. Donec vel rhoncus turpis. Phasellus vitae enim quis sapien pharetra aliquam quis a quam. In mauris nulla, euismod quis orci et, interdum finibus lorem. Aenean quis dolor eget est ultricies consectetur eu nec metus. Nullam at pulvinar elit. Aenean blandit faucibus sodales. Vivamus vel porta enim, et pharetra libero. Donec aliquet pretium erat viverra fermentum. Fusce sit amet porta mi. Nullam non elit ex. In luctus, nunc id iaculis ullamcorper, eros quam eleifend elit, quis dictum sem justo eu eros. Nulla vitae faucibus lectus. Nunc blandit, mi eget suscipit scelerisque, lorem nunc tincidunt tellus, eget gravida libero metus sed nunc.\n\n"
        "Morbi erat libero, commodo sit amet turpis eget, efficitur pulvinar dolor. Pellentesque vehicula, velit eget auctor scelerisque, sem risus aliquam lectus, sit amet dapibus massa ex non magna. Donec magna leo, laoreet quis erat vitae, consequat aliquet tellus. Etiam vitae lectus erat. Mauris interdum feugiat aliquet. Nunc justo augue, mattis id finibus eu, sagittis id enim. Vivamus malesuada mauris sed nibh luctus, porta bibendum quam ornare. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Vivamus malesuada magna nec diam tempus, laoreet imperdiet magna faucibus. Aliquam erat volutpat.\n\n"
        "Aenean mattis lobortis quam in venenatis. Sed euismod convallis lectus vel euismod. Vestibulum consequat luctus neque. Quisque consequat bibendum neque eget mollis. Vivamus viverra vehicula eros vel dapibus. Nullam id lectus aliquam, sagittis mi efficitur, interdum mauris. Nunc at felis lobortis, lobortis erat a, euismod augue. In id purus malesuada, tempus magna at, porta mi. Sed tristique nunc eget placerat luctus. Pellentesque posuere non purus vitae malesuada. Curabitur hendrerit dolor metus, nec posuere orci placerat ac.\n\n";

    lv_obj_t * scr = lv_screen_active();
    int32_t hor_res = lv_display_get_horizontal_resolution(NULL);
    int32_t ver_res = lv_display_get_vertical_resolution(NULL);
    if(hor_res * ver_res > 800 * 480) lv_obj_set_style_text_font(scr, &lv_font_montserrat_26, 0);
    else if(hor_res * ver_res > 320 * 240) lv_obj_set_style_text_font(scr, &lv_font_montserrat_20, 0);
    else lv_obj_set_style_text_font(scr, &lv_font_montserrat_14, 0);

    lv_obj_t * obj = lv_label_create(scr);
    lv_obj_set_width(obj, lv_pct(100));
    lv_label_set_text_static(obj, txt);

    lv_obj_update_layout(obj);

    scroll_anim(scr, lv_obj_get_scroll_bottom(scr));
}

static void multiple_arcs_cb(benchmark_context_t * context)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);

    LV_IMAGE_DECLARE(img_benchmark_lvgl_logo_argb);

    int32_t hor_cnt = ((int32_t)lv_obj_get_content_width(scr)) / 160;
    int32_t ver_cnt = ((int32_t)lv_obj_get_content_height(scr)) / 160;

    if(hor_cnt < 1) hor_cnt = 1;
    if(ver_cnt < 1) ver_cnt = 1;

    int32_t y;
    for(y = 0; y < ver_cnt; y++) {
        int32_t x;
        for(x = 0; x < hor_cnt; x++) {

            lv_obj_t * obj = lv_arc_create(lv_screen_active());
            if(x == 0) lv_obj_add_flag(obj, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
            lv_obj_set_size(obj, 100, 100);
            lv_obj_center(obj);

            lv_arc_set_bg_angles(obj, 0, 360);

            lv_obj_set_style_arc_opa(obj, 0, LV_PART_MAIN);
            lv_obj_set_style_bg_opa(obj, 0, LV_PART_KNOB);
            lv_obj_set_style_arc_width(obj, 10, LV_PART_INDICATOR);
            lv_obj_set_style_arc_rounded(obj, false, LV_PART_INDICATOR);
            lv_obj_set_style_arc_color(obj, lv_color_hex3(lv_rand(0x00f, 0xff0)), LV_PART_INDICATOR);
            arc_anim(context, obj);
        }
    }
}

static void containers_cb(benchmark_context_t * context)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);
    lv_obj_set_style_pad_bottom(scr, FALL_HEIGHT + PAD_BASIC, 0);

    int32_t hor_cnt = ((int32_t)lv_obj_get_content_width(scr)) / 350;
    int32_t ver_cnt = ((int32_t)lv_obj_get_content_height(scr)) / 170;

    if(hor_cnt < 1) hor_cnt = 1;
    if(ver_cnt < 1) ver_cnt = 1;

    int32_t y;
    for(y = 0; y < ver_cnt; y++) {
        int32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * card = card_create();
            if(x == 0) lv_obj_add_flag(card, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
            fall_anim(context, card, 30);
        }
    }
}

static void containers_with_overlay_cb(benchmark_context_t * context)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);
    lv_obj_set_style_pad_bottom(scr, FALL_HEIGHT + PAD_BASIC, 0);

    int32_t hor_cnt = ((int32_t)lv_obj_get_content_width(scr)) / 350;
    int32_t ver_cnt = ((int32_t)lv_obj_get_content_height(scr)) / 170;

    if(hor_cnt < 1) hor_cnt = 1;
    if(ver_cnt < 1) ver_cnt = 1;

    int32_t y;
    for(y = 0; y < ver_cnt; y++) {
        int32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * card = card_create();
            if(x == 0) lv_obj_add_flag(card, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
            fall_anim(context, card, 30);
        }
    }

    lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_50, 0);
    color_anim(lv_layer_top());
}

static void containers_with_opa_cb(benchmark_context_t * context)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);
    lv_obj_set_style_pad_bottom(scr, FALL_HEIGHT + PAD_BASIC, 0);

    int32_t hor_cnt = ((int32_t)lv_obj_get_content_width(scr)) / 350;
    int32_t ver_cnt = ((int32_t)lv_obj_get_content_height(scr)) / 170;

    if(hor_cnt < 1) hor_cnt = 1;
    if(ver_cnt < 1) ver_cnt = 1;

    int32_t y;
    for(y = 0; y < ver_cnt; y++) {
        int32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * card = card_create();
            if(x == 0) lv_obj_add_flag(card, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
            lv_obj_set_style_opa(card, LV_OPA_50, 0);
            fall_anim(context, card, 30);
        }
    }
}

static void containers_with_opa_layer_cb(benchmark_context_t * context)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_SPACE_EVENLY);
    lv_obj_set_style_pad_bottom(scr, FALL_HEIGHT + PAD_BASIC, 0);

    int32_t hor_cnt = ((int32_t)lv_obj_get_content_width(scr)) / 350;
    int32_t ver_cnt = ((int32_t)lv_obj_get_content_height(scr)) / 170;

    if(hor_cnt < 1) hor_cnt = 1;
    if(ver_cnt < 1) ver_cnt = 1;

    int32_t y;
    for(y = 0; y < ver_cnt; y++) {
        int32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * card = card_create();
            lv_obj_set_style_opa_layered(card, LV_OPA_50, 0);
            if(x == 0) lv_obj_add_flag(card, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
            fall_anim(context, card, 30);
        }
    }
}

static void containers_with_scrolling_cb(benchmark_context_t * context)
{
    LV_UNUSED(context);

    lv_obj_t * scr = lv_screen_active();
    lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_row(scr, 32, 0);

    int32_t hor_cnt = ((int32_t)lv_obj_get_content_width(scr)) / 400;
    int32_t ver_cnt = ((int32_t)lv_obj_get_content_height(scr)) / (120 + 32);

    if(hor_cnt < 1) hor_cnt = 1;
    if(ver_cnt < 1) ver_cnt = 1;

    ver_cnt *= 2; /*To make it scroll*/
    if(ver_cnt < 20) ver_cnt = 20; /*The test with many widgets*/

    int32_t y;
    for(y = 0; y < ver_cnt; y++) {
        int32_t x;
        for(x = 0; x < hor_cnt; x++) {
            lv_obj_t * card = card_create();
            if(x == 0) lv_obj_add_flag(card, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
        }
    }

    lv_obj_update_layout(scr);
    scroll_anim(scr, lv_obj_get_scroll_bottom(scr));
}

static void widgets_demo_cb(benchmark_context_t * context)
{
    LV_UNUSED(context);

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
    {.name = "Empty screen",               .scene_time = 3000,  .create_cb = empty_screen_cb},
    {.name = "Moving wallpaper",           .scene_time = 3000,  .create_cb = moving_wallpaper_cb},
    {.name = "Single rectangle",           .scene_time = 3000,  .create_cb = single_rectangle_cb},
    {.name = "Multiple rectangles",        .scene_time = 3000,  .create_cb = multiple_rectangles_cb},
    {.name = "Multiple RGB images",        .scene_time = 3000,  .create_cb = multiple_rgb_images_cb},
    {.name = "Multiple ARGB images",       .scene_time = 3000,  .create_cb = multiple_argb_images_cb},
    {.name = "Rotated ARGB images",        .scene_time = 3000,  .create_cb = rotated_argb_image_cb},
    {.name = "Multiple labels",            .scene_time = 3000,  .create_cb = multiple_labels_cb},
    {.name = "Screen sized text",          .scene_time = 5000,  .create_cb = screen_sized_text_cb},
    {.name = "Multiple arcs",              .scene_time = 3000,  .create_cb = multiple_arcs_cb},

#if LV_USE_FREETYPE
    {.name = "Span text(bitmap)",          .scene_time = 3000,  .create_cb = span_text_bitmap_cb},
    {.name = "Span text(outline)",         .scene_time = 3000,  .create_cb = span_text_outline_cb},
    {.name = "TTF text(bitmap)",           .scene_time = 3000,  .create_cb = ttf_text_bitmap_cb},
    {.name = "TTF text(outline)",          .scene_time = 3000,  .create_cb = ttf_text_outline_cb},
#endif

#if TEST_TINY_TTF
    {.name = "TinyTTF span text",          .scene_time = 3000,  .create_cb = tiny_ttf_span_text_cb},
    {.name = "TinyTTF text",               .scene_time = 3000,  .create_cb = tiny_ttf_text_cb},
#endif

    {.name = "Containers",                 .scene_time = 3000,  .create_cb = containers_cb},
    {.name = "Containers with overlay",    .scene_time = 3000,  .create_cb = containers_with_overlay_cb},
    {.name = "Containers with opa",        .scene_time = 3000,  .create_cb = containers_with_opa_cb},
    {.name = "Containers with opa_layer",  .scene_time = 3000,  .create_cb = containers_with_opa_layer_cb},
    {.name = "Containers with scrolling",  .scene_time = 5000,  .create_cb = containers_with_scrolling_cb},

    {.name = "Widgets demo",               .scene_time = 20000, .create_cb = widgets_demo_cb},

    {.name = "", .create_cb = NULL}
};

#define sysmon_perf LV_GLOBAL_DEFAULT()->sysmon_perf

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static benchmark_context_t * benchmark_context_init(void)
{
    benchmark_context_t * context = (benchmark_context_t *)lv_malloc_zeroed(sizeof(benchmark_context_t));
    LV_ASSERT_NULL(context);
    lv_fs_drv_t ** drv = lv_ll_get_head(&(LV_GLOBAL_DEFAULT()->fsdrv_ll));

#if LV_USE_FREETYPE
    char FREETYPE_FULL_PATH[128];
#if LV_FREETYPE_USE_LVGL_PORT
    lv_snprintf(FREETYPE_FULL_PATH, sizeof(FREETYPE_FULL_PATH), "%c:%s", (*drv)->letter, LV_DEMO_BENCHMARK_FONT_PATH);
#else
    LV_UNUSED(drv);
    lv_snprintf(FREETYPE_FULL_PATH, sizeof(FREETYPE_FULL_PATH), "./%s", LV_DEMO_BENCHMARK_FONT_PATH);
#endif
    context->font_bitmap = lv_freetype_font_create(FREETYPE_FULL_PATH,
                                                   LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                   LV_TEST_FONT_SIZE,
                                                   LV_FREETYPE_FONT_STYLE_NORMAL);
    if(context->font_bitmap == NULL) {
        LV_LOG_ERROR("freetype font creation failed!");
    }
    context->font_outline = lv_freetype_font_create(FREETYPE_FULL_PATH,
                                                    LV_FREETYPE_FONT_RENDER_MODE_OUTLINE,
                                                    LV_TEST_FONT_SIZE,
                                                    LV_FREETYPE_FONT_STYLE_NORMAL);
    if(context->font_outline == NULL) {
        LV_LOG_ERROR("freetype font creation failed!");
    }
#endif

#if TEST_TINY_TTF
    char TINYTTF_FULL_PATH[128];
    lv_snprintf(TINYTTF_FULL_PATH, sizeof(TINYTTF_FULL_PATH), "%c:%s", (*drv)->letter, LV_DEMO_BENCHMARK_FONT_PATH);
    context->font_tinyttf = lv_tiny_ttf_create_file(TINYTTF_FULL_PATH, LV_TEST_FONT_SIZE);
    if(context->font_tinyttf == NULL) {
        LV_LOG_ERROR("TinyTTF font creation failed!");
    }
#endif
    context->scene_act = 0;
    context->label_perf = lv_label_create(lv_layer_top());
    lv_obj_set_style_bg_opa(context->label_perf, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(context->label_perf, lv_color_white(), 0);
    lv_obj_set_style_text_color(context->label_perf, lv_color_black(), 0);
    lv_obj_set_width(context->label_perf, lv_pct(100));
#if LV_USE_PERF_MONITOR_LOG_MODE
    lv_obj_add_flag(context->label_perf, LV_OBJ_FLAG_HIDDEN);
#endif
    return context;
}

static void benchmark_context_deinit(benchmark_context_t * context)
{
#if LV_USE_FREETYPE
    if(context->font_bitmap != NULL) {
        lv_freetype_font_delete(context->font_bitmap);
    }
    if(context->font_outline != NULL) {
        lv_freetype_font_delete(context->font_outline);
    }
#endif
#if TEST_TINY_TTF
    if(context->font_bitmap != NULL) {
        lv_tiny_ttf_destroy(context->font_tinyttf);
    }
#endif
    lv_obj_delete(context->label_perf);
    lv_free(context);
}

void lv_demo_benchmark(void)
{
    benchmark_context_t * context = benchmark_context_init();

    lv_obj_t * scr = lv_screen_active();
    lv_obj_remove_style_all(scr);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(scr, lv_color_black(), 0);
    lv_obj_set_style_bg_color(scr, lv_palette_lighten(LV_PALETTE_GREY, 4), 0);
    lv_obj_set_style_pad_all(lv_screen_active(), 8, 0);
    lv_obj_set_style_pad_top(lv_screen_active(), HEADER_HEIGHT, 0);
    lv_obj_set_style_pad_gap(lv_screen_active(), 8, 0);

    load_scene(context);

    lv_timer_create(next_scene_timer_cb, scenes[0].scene_time, context);

#if LV_USE_PERF_MONITOR
    lv_display_t * disp = lv_display_get_default();
    lv_subject_add_observer_obj(&disp->perf_sysmon_backend.subject, sysmon_perf_observer_cb, context->label_perf, context);
#else
    lv_label_set_text(context->label_perf, "LV_USE_PERF_MONITOR is not enabled");
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void load_scene(benchmark_context_t * context)
{
    lv_obj_t * scr = lv_screen_active();
    lv_obj_clean(scr);
    lv_obj_set_style_bg_color(scr, lv_palette_lighten(LV_PALETTE_GREY, 4), 0);
    lv_obj_set_style_text_color(scr, lv_color_black(), 0);
    lv_obj_set_style_text_font(scr, LV_FONT_DEFAULT, 0);
    lv_obj_set_style_pad_all(scr, PAD_BASIC, 0);
    lv_obj_set_style_pad_gap(scr, PAD_BASIC, 0);
    lv_obj_set_style_pad_top(scr, HEADER_HEIGHT, 0);
    lv_obj_set_layout(scr, LV_LAYOUT_NONE);
    lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    lv_anim_delete(scr, scroll_anim_y_cb);
    lv_anim_delete(scr, shake_anim_y_cb);
    lv_anim_delete(scr, color_anim_cb);

    lv_anim_delete(lv_layer_top(), color_anim_cb);
    lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_TRANSP, 0);

    rnd_reset(context);
    if(scenes[context->scene_act].create_cb) {
        scenes[context->scene_act].create_cb(context);
    }
}

static void next_scene_timer_cb(lv_timer_t * timer)
{
    benchmark_context_t * context = lv_timer_get_user_data(timer);

    context->scene_act++;

    load_scene(context);
    if(scenes[context->scene_act].scene_time == 0) {
        benchmark_context_deinit(context);
        lv_timer_delete(timer);
        summary_create();
    }
    else {
        lv_timer_set_period(timer, scenes[context->scene_act].scene_time);
    }
}

#if LV_USE_PERF_MONITOR
static void sysmon_perf_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    const lv_sysmon_perf_info_t * info = lv_subject_get_pointer(subject);
    benchmark_context_t * context = lv_observer_get_user_data(observer);

    char scene_name[64];
    if(scenes[context->scene_act].name[0] != '\0') {
        lv_snprintf(scene_name, sizeof(scene_name), "%s: ", scenes[context->scene_act].name);
    }
    else {
        scene_name[0] = '\0';
    }

#if !LV_USE_PERF_MONITOR_LOG_MODE
    lv_obj_t * label = lv_observer_get_target(observer);
    lv_label_set_text_fmt(label,
                          "%s"
                          "%" LV_PRIu32" FPS, %" LV_PRIu32 "%% CPU\n"
                          "refr. %" LV_PRIu32" ms = %" LV_PRIu32 "ms render + %" LV_PRIu32" ms flush",
                          scene_name,
                          info->calculated.fps, info->calculated.cpu,
                          info->calculated.render_avg_time + info->calculated.flush_avg_time,
                          info->calculated.render_avg_time, info->calculated.flush_avg_time);
#endif

    /*Ignore the first call as it contains data from the previous scene*/
    if(scenes[context->scene_act].measurement_cnt != 0) {
        scenes[context->scene_act].cpu_avg_usage += info->calculated.cpu;
        scenes[context->scene_act].fps_avg += info->calculated.fps;
        scenes[context->scene_act].render_avg_time += info->calculated.render_avg_time;
        scenes[context->scene_act].flush_avg_time += info->calculated.flush_avg_time;
    }
    scenes[context->scene_act].measurement_cnt++;
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
    LV_LOG("Benchmark Summary (%d.%d.%d %s)\r\n",
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

static void arc_anim(benchmark_context_t * context, lv_obj_t * obj)
{
    uint32_t t1 = rnd_next(context, 1000, 3000);
    uint32_t t2 = rnd_next(context, 1000, 3000);
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, arc_anim_cb);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_duration(&a, t1);
    lv_anim_set_reverse_duration(&a, t2);
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
    lv_anim_set_reverse_duration(&a, t);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}

static void shake_anim_y_cb(void * var, int32_t v)
{
    lv_obj_set_style_translate_y(var, v, 0);
}

static void fall_anim(benchmark_context_t * context, lv_obj_t * obj, int32_t y_max)
{
    uint32_t t1 = rnd_next(context, 300, 3000);
    uint32_t t2 = rnd_next(context, 300, 3000);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, shake_anim_y_cb);
    lv_anim_set_values(&a, 0, y_max);
    lv_anim_set_duration(&a, t1);
    lv_anim_set_reverse_duration(&a, t2);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}

static lv_obj_t * card_create(void)
{
    lv_obj_t * panel = lv_obj_create(lv_screen_active());
    lv_obj_set_size(panel, 270, 120);
    lv_obj_set_style_pad_all(panel, 8, 0);

    LV_IMAGE_DECLARE(img_benchmark_avatar);
    lv_obj_t * child = lv_image_create(panel);
    lv_obj_align(child, LV_ALIGN_LEFT_MID, 0, 0);
    lv_image_set_src(child, &img_benchmark_avatar);

    child = lv_label_create(panel);
    lv_label_set_text_static(child, "John Smith");
    lv_obj_set_style_text_font(child, &lv_font_montserrat_24, 0);
    lv_obj_set_pos(child, 100, 0);

    child = lv_label_create(panel);
    lv_label_set_text_static(child, "A DIY enthusiast");
    lv_obj_set_style_text_font(child, &lv_font_montserrat_14, 0);
    lv_obj_set_pos(child, 100, 30);

    child = lv_button_create(panel);
    lv_obj_set_pos(child, 100, 50);

    child = lv_label_create(child);
    lv_label_set_text_static(child, "Connect");

    return panel;
}

#if LV_USE_FREETYPE || TEST_TINY_TTF
static void span_text_add(lv_obj_t * spans, lv_font_t * font, const char * text)
{
    lv_span_t * span = lv_spangroup_new_span(spans);
    lv_span_set_text(span, text);
    if(font) {
        lv_style_set_text_font(lv_span_get_style(span), font);
    }
}
#endif

static void rnd_reset(benchmark_context_t * context)
{
    context->rnd_act = 0;
}

static int32_t rnd_next(benchmark_context_t * context, int32_t min, int32_t max)
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
        0xd2c5eedd, 0x24f0cc9b, 0x3aa7b571, 0xd289a1c9,
        0x79c7dc3,  0x5bf68c86, 0xc9f55239, 0x42052cfb,
        0x63dae9df, 0x75c9e11f, 0x407f9151, 0x104ebc63,
        0xb4b52591, 0x53a46b7a, 0x9398d144, 0x9a7c6c3d,
        0x76b35b78, 0xa028e33e, 0xbfe586e4, 0xf3f79731,
        0x99591738, 0xd7b0a847, 0x1ffb1936, 0xfeeea2e4,
        0xbc896279, 0xa8241a72, 0x871124fa, 0x27bb9866,
        0x41794272, 0x92f5dc59, 0x98c9d185, 0x6fc5905b,
        0xf0ba9f1a, 0x771cad1b, 0xf6285752, 0xb5ffcbc5,
        0x6fd2b63c, 0x2c404190, 0x209469e6, 0x628531b1,
        0x98a726bc, 0xcc6c0d97, 0x86c2e7b9, 0x7bc12e1f,
        0xf9a67e10, 0xd5bf101f, 0xa1aaaf35, 0x69b078fc,
        0x71d698b2, 0x9a954baa, 0xe7423a82, 0xdd9898e1,
        0xf4980e5c, 0x4f3607b9, 0x9ce35d27, 0xb4b764e0,
        0xa1fa3ad3, 0x220ad165, 0x282216b4, 0x7e583888,
        0xf8315b2b, 0x81c27062, 0x8eb89a85,     /*Intentionally incomplete line to make the length of array more arbitrary*/

    };

    if(min == max) return min;

    if(min > max) {
        int32_t t = min;
        min = max;
        max = t;
    }

    int32_t d = max - min;
    int32_t r = (rnd_map[context->rnd_act] % d) + min;

    context->rnd_act++;
    if(context->rnd_act >= sizeof(rnd_map) / sizeof(rnd_map[0])) context->rnd_act = 0;

    return r;
}

#endif
