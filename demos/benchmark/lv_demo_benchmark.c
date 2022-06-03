/**
 * @file lv_demo_benchmark.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_demo_benchmark.h"

#if LV_USE_DEMO_BENCHMARK

/*********************
 *      DEFINES
 *********************/
#define RND_NUM         64
#define SCENE_TIME      1000      /*ms*/
#define ANIM_TIME_MIN   ((2 * SCENE_TIME) / 10)
#define ANIM_TIME_MAX   (SCENE_TIME)
#define OBJ_NUM         8
#define OBJ_SIZE_MIN    (LV_MAX(LV_DPI_DEF / 20, 5))
#define OBJ_SIZE_MAX    (LV_HOR_RES / 2)
#define RADIUS          LV_MAX(LV_DPI_DEF / 15, 2)
#define BORDER_WIDTH    LV_MAX(LV_DPI_DEF / 40, 1)
#define SHADOW_WIDTH_SMALL  LV_MAX(LV_DPI_DEF / 15, 5)
#define SHADOW_OFS_X_SMALL  LV_MAX(LV_DPI_DEF / 20, 2)
#define SHADOW_OFS_Y_SMALL  LV_MAX(LV_DPI_DEF / 20, 2)
#define SHADOW_SPREAD_SMALL LV_MAX(LV_DPI_DEF / 30, 2)
#define SHADOW_WIDTH_LARGE  LV_MAX(LV_DPI_DEF / 5,  10)
#define SHADOW_OFS_X_LARGE  LV_MAX(LV_DPI_DEF / 10, 5)
#define SHADOW_OFS_Y_LARGE  LV_MAX(LV_DPI_DEF / 10, 5)
#define SHADOW_SPREAD_LARGE LV_MAX(LV_DPI_DEF / 30, 2)
#define IMG_WIDH        100
#define IMG_HEIGHT      100
#define IMG_NUM         LV_MAX((LV_HOR_RES * LV_VER_RES) / 5 / IMG_WIDH / IMG_HEIGHT, 1)
#define IMG_ZOOM_MIN    128
#define IMG_ZOOM_MAX    (256 + 64)
#define TXT "hello world\nit is a multi line text to test\nthe performance of text rendering"
#define LINE_WIDTH  LV_MAX(LV_DPI_DEF / 50, 2)
#define LINE_POINT_NUM  16
#define LINE_POINT_DIFF_MIN (LV_DPI_DEF / 10)
#define LINE_POINT_DIFF_MAX LV_MAX(LV_HOR_RES / (LINE_POINT_NUM + 2), LINE_POINT_DIFF_MIN * 2)
#define ARC_WIDTH_THIN LV_MAX(LV_DPI_DEF / 50, 2)
#define ARC_WIDTH_THICK LV_MAX(LV_DPI_DEF / 10, 5)

#ifndef dimof
    #define dimof(__array)     (sizeof(__array) / sizeof(__array[0]))
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const char * name;
    void (*create_cb)(void);
    uint32_t time_sum_normal;
    uint32_t time_sum_opa;
    uint32_t refr_cnt_normal;
    uint32_t refr_cnt_opa;
    uint32_t fps_normal;
    uint32_t fps_opa;
    uint8_t weight;
} scene_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_style_t style_common;
static bool opa_mode = true;
static bool run_max_speed = false;
static finished_cb_t * benchmark_finished_cb = NULL;
static uint32_t disp_ori_timer_period;
static uint32_t anim_ori_timer_period;

LV_IMG_DECLARE(img_benchmark_cogwheel_argb);
LV_IMG_DECLARE(img_benchmark_cogwheel_rgb);
LV_IMG_DECLARE(img_benchmark_cogwheel_chroma_keyed);
LV_IMG_DECLARE(img_benchmark_cogwheel_indexed16);
LV_IMG_DECLARE(img_benchmark_cogwheel_alpha16);

LV_FONT_DECLARE(lv_font_benchmark_montserrat_12_compr_az);
LV_FONT_DECLARE(lv_font_benchmark_montserrat_16_compr_az);
LV_FONT_DECLARE(lv_font_benchmark_montserrat_28_compr_az);

static void monitor_cb(lv_disp_drv_t * drv, uint32_t time, uint32_t px);
static void scene_next_task_cb(lv_timer_t * timer);
static void rect_create(lv_style_t * style);
static void img_create(lv_style_t * style, const void * src, bool rotate, bool zoom, bool aa);
static void txt_create(lv_style_t * style);
static void line_create(lv_style_t * style);
static void arc_create(lv_style_t * style);
static void fall_anim(lv_obj_t * obj);
static void rnd_reset(void);
static int32_t rnd_next(int32_t min, int32_t max);
static void report_cb(lv_timer_t * timer);

static void rectangle_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_bg_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    rect_create(&style_common);
}

static void rectangle_rounded_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_bg_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    rect_create(&style_common);
}

static void rectangle_circle_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, LV_RADIUS_CIRCLE);
    lv_style_set_bg_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    rect_create(&style_common);
}

static void border_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_border_width(&style_common, BORDER_WIDTH);
    lv_style_set_border_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    rect_create(&style_common);
}

static void border_rounded_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_border_width(&style_common, BORDER_WIDTH);
    lv_style_set_border_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    rect_create(&style_common);

}

static void border_circle_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, LV_RADIUS_CIRCLE);
    lv_style_set_border_width(&style_common, BORDER_WIDTH);
    lv_style_set_border_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    rect_create(&style_common);
}

static void border_top_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_border_width(&style_common, BORDER_WIDTH);
    lv_style_set_border_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_border_side(&style_common, LV_BORDER_SIDE_TOP);
    rect_create(&style_common);

}

static void border_left_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_border_width(&style_common, BORDER_WIDTH);
    lv_style_set_border_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_border_side(&style_common, LV_BORDER_SIDE_LEFT);
    rect_create(&style_common);
}

static void border_top_left_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_border_width(&style_common, BORDER_WIDTH);
    lv_style_set_border_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_border_side(&style_common, LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_TOP);
    rect_create(&style_common);
}

static void border_left_right_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_border_width(&style_common, BORDER_WIDTH);
    lv_style_set_border_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_border_side(&style_common, LV_BORDER_SIDE_LEFT | LV_BORDER_SIDE_RIGHT);
    rect_create(&style_common);
}

static void border_top_bottom_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_border_width(&style_common, BORDER_WIDTH);
    lv_style_set_border_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_border_side(&style_common, LV_BORDER_SIDE_TOP | LV_BORDER_SIDE_BOTTOM);
    rect_create(&style_common);

}

static void shadow_small_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_bg_opa(&style_common, LV_OPA_COVER);
    lv_style_set_shadow_opa(&style_common, opa_mode ? LV_OPA_80 : LV_OPA_COVER);
    lv_style_set_shadow_width(&style_common, SHADOW_WIDTH_SMALL);
    rect_create(&style_common);

}

static void shadow_small_ofs_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_bg_opa(&style_common, LV_OPA_COVER);
    lv_style_set_shadow_opa(&style_common, opa_mode ? LV_OPA_80 : LV_OPA_COVER);
    lv_style_set_shadow_width(&style_common, SHADOW_WIDTH_SMALL);
    lv_style_set_shadow_ofs_x(&style_common, SHADOW_OFS_X_SMALL);
    lv_style_set_shadow_ofs_y(&style_common, SHADOW_OFS_Y_SMALL);
    lv_style_set_shadow_spread(&style_common, SHADOW_SPREAD_SMALL);
    rect_create(&style_common);
}


static void shadow_large_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_bg_opa(&style_common, LV_OPA_COVER);
    lv_style_set_shadow_opa(&style_common, opa_mode ? LV_OPA_80 : LV_OPA_COVER);
    lv_style_set_shadow_width(&style_common, SHADOW_WIDTH_LARGE);
    rect_create(&style_common);
}

static void shadow_large_ofs_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_bg_opa(&style_common, LV_OPA_COVER);
    lv_style_set_shadow_opa(&style_common, opa_mode ? LV_OPA_80 : LV_OPA_COVER);
    lv_style_set_shadow_width(&style_common, SHADOW_WIDTH_LARGE);
    lv_style_set_shadow_ofs_x(&style_common, SHADOW_OFS_X_LARGE);
    lv_style_set_shadow_ofs_y(&style_common, SHADOW_OFS_Y_LARGE);
    lv_style_set_shadow_spread(&style_common, SHADOW_SPREAD_LARGE);
    rect_create(&style_common);
}


static void img_rgb_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_rgb, false, false, false);
}

static void img_argb_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_argb, false, false, false);

}

static void img_ckey_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_chroma_keyed, false, false, false);

}

static void img_index_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_indexed16, false, false, false);

}

static void img_alpha_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_alpha16, false, false, false);
}


static void img_rgb_recolor_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_img_recolor_opa(&style_common, LV_OPA_50);
    img_create(&style_common, &img_benchmark_cogwheel_rgb, false, false, false);

}

static void img_argb_recolor_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_img_recolor_opa(&style_common, LV_OPA_50);
    img_create(&style_common, &img_benchmark_cogwheel_argb, false, false, false);
}

static void img_ckey_recolor_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_img_recolor_opa(&style_common, LV_OPA_50);
    img_create(&style_common, &img_benchmark_cogwheel_chroma_keyed, false, false, false);
}

static void img_index_recolor_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_img_recolor_opa(&style_common, LV_OPA_50);
    img_create(&style_common, &img_benchmark_cogwheel_indexed16, false, false, false);

}

static void img_rgb_rot_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_rgb, true, false, false);
}

static void img_rgb_rot_aa_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_rgb, true, false, true);
}

static void img_argb_rot_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_argb, true, false, false);
}

static void img_argb_rot_aa_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_argb, true, false, true);
}

static void img_rgb_zoom_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_rgb, false, true, false);

}

static void img_rgb_zoom_aa_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_rgb, false, true, true);


}

static void img_argb_zoom_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_argb, false, true, false);
}


static void img_argb_zoom_aa_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    img_create(&style_common, &img_benchmark_cogwheel_argb, false, true, true);
}

static void txt_small_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_text_font(&style_common, lv_theme_get_font_small(NULL));
    lv_style_set_text_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    txt_create(&style_common);

}

static void txt_medium_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_text_font(&style_common, lv_theme_get_font_normal(NULL));
    lv_style_set_text_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    txt_create(&style_common);

}

static void txt_large_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_text_font(&style_common, lv_theme_get_font_large(NULL));
    lv_style_set_text_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    txt_create(&style_common);

}

static void txt_small_compr_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_text_font(&style_common, &lv_font_benchmark_montserrat_12_compr_az);
    lv_style_set_text_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    txt_create(&style_common);

}

static void txt_medium_compr_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_text_font(&style_common, &lv_font_benchmark_montserrat_16_compr_az);
    lv_style_set_text_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    txt_create(&style_common);

}

static void txt_large_compr_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_text_font(&style_common, &lv_font_benchmark_montserrat_28_compr_az);
    lv_style_set_text_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    txt_create(&style_common);

}


static void line_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_line_width(&style_common, LINE_WIDTH);
    lv_style_set_line_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    line_create(&style_common);

}

static void arc_think_cb(void)
{

    lv_style_reset(&style_common);
    lv_style_set_arc_width(&style_common, ARC_WIDTH_THIN);
    lv_style_set_arc_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    arc_create(&style_common);
}

static void arc_thick_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_arc_width(&style_common, ARC_WIDTH_THICK);
    lv_style_set_arc_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    arc_create(&style_common);

}


static void sub_rectangle_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_bg_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_blend_mode(&style_common, LV_BLEND_MODE_SUBTRACTIVE);
    rect_create(&style_common);
}

static void sub_border_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_border_width(&style_common, BORDER_WIDTH);
    lv_style_set_border_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_blend_mode(&style_common, LV_BLEND_MODE_SUBTRACTIVE);
    rect_create(&style_common);

}

static void sub_shadow_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_radius(&style_common, RADIUS);
    lv_style_set_bg_opa(&style_common, LV_OPA_COVER);
    lv_style_set_shadow_opa(&style_common, opa_mode ? LV_OPA_80 : LV_OPA_COVER);
    lv_style_set_shadow_width(&style_common, SHADOW_WIDTH_SMALL);
    lv_style_set_shadow_spread(&style_common, SHADOW_WIDTH_SMALL);
    lv_style_set_blend_mode(&style_common, LV_BLEND_MODE_SUBTRACTIVE);
    rect_create(&style_common);

}

static void sub_img_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_img_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_blend_mode(&style_common, LV_BLEND_MODE_SUBTRACTIVE);
    img_create(&style_common, &img_benchmark_cogwheel_argb, false, false, false);

}
static void sub_line_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_line_width(&style_common, LINE_WIDTH);
    lv_style_set_line_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_blend_mode(&style_common, LV_BLEND_MODE_SUBTRACTIVE);
    line_create(&style_common);

}

static void sub_arc_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_arc_width(&style_common, ARC_WIDTH_THICK);
    lv_style_set_arc_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_blend_mode(&style_common, LV_BLEND_MODE_SUBTRACTIVE);
    arc_create(&style_common);
}

static void sub_text_cb(void)
{
    lv_style_reset(&style_common);
    lv_style_set_text_font(&style_common, lv_theme_get_font_normal(NULL));
    lv_style_set_text_opa(&style_common, opa_mode ? LV_OPA_50 : LV_OPA_COVER);
    lv_style_set_blend_mode(&style_common, LV_BLEND_MODE_SUBTRACTIVE);
    txt_create(&style_common);
}



/**********************
 *  STATIC VARIABLES
 **********************/

static scene_dsc_t scenes[] = {
    {.name = "Rectangle",                    .weight = 30, .create_cb = rectangle_cb},
    {.name = "Rectangle rounded",            .weight = 20, .create_cb = rectangle_rounded_cb},
    {.name = "Circle",                       .weight = 10, .create_cb = rectangle_circle_cb},
    {.name = "Border",                       .weight = 20, .create_cb = border_cb},
    {.name = "Border rounded",               .weight = 30, .create_cb = border_rounded_cb},
    {.name = "Circle border",                .weight = 10, .create_cb = border_circle_cb},
    {.name = "Border top",                   .weight = 3, .create_cb = border_top_cb},
    {.name = "Border left",                  .weight = 3, .create_cb = border_left_cb},
    {.name = "Border top + left",            .weight = 3, .create_cb = border_top_left_cb},
    {.name = "Border left + right",          .weight = 3, .create_cb = border_left_right_cb},
    {.name = "Border top + bottom",          .weight = 3, .create_cb = border_top_bottom_cb},

    {.name = "Shadow small",                 .weight = 3, .create_cb = shadow_small_cb},
    {.name = "Shadow small offset",          .weight = 5, .create_cb = shadow_small_ofs_cb},
    {.name = "Shadow large",                 .weight = 5, .create_cb = shadow_large_cb},
    {.name = "Shadow large offset",          .weight = 3, .create_cb = shadow_large_ofs_cb},

    {.name = "Image RGB",                    .weight = 20, .create_cb = img_rgb_cb},
    {.name = "Image ARGB",                   .weight = 20, .create_cb = img_argb_cb},
    {.name = "Image chorma keyed",           .weight = 5, .create_cb = img_ckey_cb},
    {.name = "Image indexed",                .weight = 5, .create_cb = img_index_cb},
    {.name = "Image alpha only",             .weight = 5, .create_cb = img_alpha_cb},

    {.name = "Image RGB recolor",            .weight = 5, .create_cb = img_rgb_recolor_cb},
    {.name = "Image ARGB recolor",           .weight = 20, .create_cb = img_argb_recolor_cb},
    {.name = "Image chorma keyed recolor",   .weight = 3, .create_cb = img_ckey_recolor_cb},
    {.name = "Image indexed recolor",        .weight = 3, .create_cb = img_index_recolor_cb},

    {.name = "Image RGB rotate",             .weight = 3, .create_cb = img_rgb_rot_cb},
    {.name = "Image RGB rotate anti aliased", .weight = 3, .create_cb = img_rgb_rot_aa_cb},
    {.name = "Image ARGB rotate",            .weight = 5, .create_cb = img_argb_rot_cb},
    {.name = "Image ARGB rotate anti aliased", .weight = 5, .create_cb = img_argb_rot_aa_cb},
    {.name = "Image RGB zoom",               .weight = 3, .create_cb = img_rgb_zoom_cb},
    {.name = "Image RGB zoom anti aliased",  .weight = 3, .create_cb = img_rgb_zoom_aa_cb},
    {.name = "Image ARGB zoom",              .weight = 5, .create_cb = img_argb_zoom_cb},
    {.name = "Image ARGB zoom anti aliased", .weight = 5, .create_cb = img_argb_zoom_aa_cb},

    {.name = "Text small",                   .weight = 20, .create_cb = txt_small_cb},
    {.name = "Text medium",                  .weight = 30, .create_cb = txt_medium_cb},
    {.name = "Text large",                   .weight = 20, .create_cb = txt_large_cb},

    {.name = "Text small compressed",        .weight = 3, .create_cb = txt_small_compr_cb},
    {.name = "Text medium compressed",       .weight = 5, .create_cb = txt_medium_compr_cb},
    {.name = "Text large compressed",        .weight = 10, .create_cb = txt_large_compr_cb},

    {.name = "Line",                         .weight = 10, .create_cb = line_cb},

    {.name = "Arc think",                    .weight = 10, .create_cb = arc_think_cb},
    {.name = "Arc thick",                    .weight = 10, .create_cb = arc_thick_cb},

    {.name = "Substr. rectangle",            .weight = 10, .create_cb = sub_rectangle_cb},
    {.name = "Substr. border",               .weight = 10, .create_cb = sub_border_cb},
    {.name = "Substr. shadow",               .weight = 10, .create_cb = sub_shadow_cb},
    {.name = "Substr. image",                .weight = 10, .create_cb = sub_img_cb},
    {.name = "Substr. line",                 .weight = 10, .create_cb = sub_line_cb},
    {.name = "Substr. arc",                  .weight = 10, .create_cb = sub_arc_cb},
    {.name = "Substr. text",                 .weight = 10, .create_cb = sub_text_cb},

    {.name = "", .create_cb = NULL}
};

static int32_t scene_act = -1;
static lv_obj_t * scene_bg;
static lv_obj_t * title;
static lv_obj_t * subtitle;
static uint32_t rnd_act;


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

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static void benchmark_init(void)
{
    lv_disp_t * disp = lv_disp_get_default();
    disp->driver->monitor_cb = monitor_cb;

    /*Force to run at maximum frame rate*/
    if(run_max_speed) {
        if(disp->refr_timer) {
            disp_ori_timer_period = disp->refr_timer->period;
            lv_timer_set_period(disp->refr_timer, 1);
        }

        lv_timer_t * anim_timer = lv_anim_get_timer();
        anim_ori_timer_period = anim_timer->period;
        lv_timer_set_period(anim_timer, 1);
    }

    lv_obj_t * scr = lv_scr_act();
    lv_obj_remove_style_all(scr);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

    title = lv_label_create(scr);
    lv_obj_set_pos(title, LV_DPI_DEF / 30, LV_DPI_DEF / 30);

    subtitle = lv_label_create(scr);
    lv_obj_align_to(subtitle, title, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    scene_bg = lv_obj_create(scr);
    lv_obj_remove_style_all(scene_bg);
    lv_obj_set_size(scene_bg, lv_obj_get_width(scr), lv_obj_get_height(scr) - subtitle->coords.y2 - LV_DPI_DEF / 30);
    lv_obj_align(scene_bg, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_style_init(&style_common);

    lv_obj_update_layout(scr);
}


void lv_demo_benchmark(void)
{
    benchmark_init();

    /*Manually start scenes*/
    scene_next_task_cb(NULL);
}


void lv_demo_benchmark_run_scene(int_fast16_t scene_no)
{
    benchmark_init();

    if(((scene_no >> 1) >= dimof(scenes))) {
        /* invalid scene number */
        return ;
    }

    opa_mode = scene_no & 0x01;
    scene_act = scene_no >> 1;

    if(scenes[scene_act].create_cb) {
        lv_label_set_text_fmt(title, "%"LV_PRId32"/%d: %s%s", scene_act * 2 + (opa_mode ? 1 : 0), (int)(dimof(scenes) * 2) - 2,
                              scenes[scene_act].name, opa_mode ? " + opa" : "");
        lv_label_set_text(subtitle, "");

        rnd_reset();
        scenes[scene_act].create_cb();

        lv_timer_t * t = lv_timer_create(report_cb, SCENE_TIME, NULL);
        lv_timer_set_repeat_count(t, 1);
    }
}


void lv_demo_benchmark_set_finished_cb(finished_cb_t * finished_cb)
{
    benchmark_finished_cb = finished_cb;
}

void lv_demo_benchmark_set_max_speed(bool en)
{
    run_max_speed = en;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void monitor_cb(lv_disp_drv_t * drv, uint32_t time, uint32_t px)
{
    LV_UNUSED(drv);
    LV_UNUSED(px);
    if(opa_mode) {
        scenes[scene_act].refr_cnt_opa ++;
        scenes[scene_act].time_sum_opa += time;
    }
    else {
        scenes[scene_act].refr_cnt_normal ++;
        scenes[scene_act].time_sum_normal += time;
    }

    //    lv_obj_invalidate(lv_scr_act());
}

static void generate_report(void)
{
    uint32_t weight_sum = 0;
    uint32_t weight_normal_sum = 0;
    uint32_t weight_opa_sum = 0;
    uint32_t fps_sum = 0;
    uint32_t fps_normal_sum = 0;
    uint32_t fps_opa_sum = 0;
    uint32_t i;
    for(i = 0; scenes[i].create_cb; i++) {
        fps_normal_sum += scenes[i].fps_normal * scenes[i].weight;
        weight_normal_sum += scenes[i].weight;

        uint32_t w = LV_MAX(scenes[i].weight / 2, 1);
        fps_opa_sum += scenes[i].fps_opa * w;
        weight_opa_sum += w;
    }


    fps_sum = fps_normal_sum + fps_opa_sum;
    weight_sum = weight_normal_sum + weight_opa_sum;

    uint32_t fps_weighted = fps_sum / weight_sum;
    uint32_t fps_normal_unweighted = fps_normal_sum / weight_normal_sum;
    uint32_t fps_opa_unweighted = fps_opa_sum / weight_opa_sum;

    uint32_t opa_speed_pct = (fps_opa_unweighted * 100) / fps_normal_unweighted;

    if(NULL != benchmark_finished_cb) {
        (*benchmark_finished_cb)();
    }

    lv_obj_clean(lv_scr_act());
    scene_bg = NULL;


    lv_obj_set_flex_flow(lv_scr_act(), LV_FLEX_FLOW_COLUMN);

    title = lv_label_create(lv_scr_act());
    lv_label_set_text_fmt(title, "Weighted FPS: %"LV_PRIu32, fps_weighted);

    subtitle = lv_label_create(lv_scr_act());
    lv_label_set_text_fmt(subtitle, "Opa. speed: %"LV_PRIu32"%%", opa_speed_pct);

    lv_coord_t w = lv_obj_get_content_width(lv_scr_act());
    lv_obj_t * table = lv_table_create(lv_scr_act());
    //        lv_obj_clean_style_list(table, LV_PART_MAIN);
    lv_table_set_col_cnt(table, 2);

    lv_table_set_col_width(table, 0, (w * 3) / 4 - 3);
    lv_table_set_col_width(table, 1, w  / 4 - 3);
    lv_obj_set_width(table, lv_pct(100));

    //        static lv_style_t style_cell_slow;
    //        static lv_style_t style_cell_very_slow;
    //        static lv_style_t style_cell_title;
    //
    //        lv_style_init(&style_cell_title);
    //        lv_style_set_bg_color(&style_cell_title, LV_STATE_DEFAULT, lv_palette_main(LV_PALETTE_GREY));
    //        lv_style_set_bg_opa(&style_cell_title, LV_STATE_DEFAULT, LV_OPA_50);
    //
    //        lv_style_init(&style_cell_slow);
    //        lv_style_set_text_color(&style_cell_slow, LV_STATE_DEFAULT, LV_COLOR_ORANGE);
    //
    //        lv_style_init(&style_cell_very_slow);
    //        lv_style_set_text_color(&style_cell_very_slow, LV_STATE_DEFAULT, lv_palette_main(LV_PALETTE_RED));

    //        lv_obj_add_style(table, LV_TABLE_PART_CELL2, &style_cell_slow);
    //        lv_obj_add_style(table, LV_TABLE_PART_CELL3, &style_cell_very_slow);
    //        lv_obj_add_style(table, LV_TABLE_PART_CELL4, &style_cell_title);


    uint16_t row = 0;
    lv_table_add_cell_ctrl(table, row, 0, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    lv_table_set_cell_value(table, row, 0, "Slow but common cases");
    //        lv_table_set_cell_type(table, row, 0, 4);

    LV_LOG("\r\n"
           "LVGL v%d.%d.%d " LVGL_VERSION_INFO
           " Benchmark (in csv format)\r\n",
           LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH);
    LV_LOG("Weighted FPS: %"LV_PRIu32"\r\n", fps_weighted);
    LV_LOG("Opa. speed: %"LV_PRIu32"%%\r\n", opa_speed_pct);

    row++;
    char buf[256];
    for(i = 0; scenes[i].create_cb; i++) {

        if(scenes[i].fps_normal < 20 && scenes[i].weight >= 10) {
            lv_table_set_cell_value(table, row, 0, scenes[i].name);

            lv_snprintf(buf, sizeof(buf), "%"LV_PRIu32, scenes[i].fps_normal);
            lv_table_set_cell_value(table, row, 1, buf);

            //                lv_table_set_cell_type(table, row, 0, 2);
            //                lv_table_set_cell_type(table, row, 1, 2);

            LV_LOG("%s,%s\r\n", scenes[i].name, buf);

            row++;
        }

        if(scenes[i].fps_opa < 20 && LV_MAX(scenes[i].weight / 2, 1) >= 10) {
            lv_snprintf(buf, sizeof(buf), "%s + opa", scenes[i].name);
            lv_table_set_cell_value(table, row, 0, buf);

            LV_LOG("%s,", buf);

            lv_snprintf(buf, sizeof(buf), "%"LV_PRIu32, scenes[i].fps_opa);
            lv_table_set_cell_value(table, row, 1, buf);

            //                lv_table_set_cell_type(table, row, 0, 2);
            //                lv_table_set_cell_type(table, row, 1, 2);
            LV_LOG("%s\r\n", buf);

            row++;
        }
    }

    /*No 'slow but common cases'*/
    if(row == 1) {
        lv_table_add_cell_ctrl(table, row, 0, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
        lv_table_set_cell_value(table, row, 0, "All good");
        row++;
    }

    lv_table_add_cell_ctrl(table, row, 0, LV_TABLE_CELL_CTRL_MERGE_RIGHT);
    lv_table_set_cell_value(table, row, 0, "All cases");
    //        lv_table_set_cell_type(table, row, 0, 4);
    row++;

    for(i = 0; scenes[i].create_cb; i++) {
        lv_table_set_cell_value(table, row, 0, scenes[i].name);

        lv_snprintf(buf, sizeof(buf), "%"LV_PRIu32, scenes[i].fps_normal);
        lv_table_set_cell_value(table, row, 1, buf);

        if(scenes[i].fps_normal < 10) {
            //                lv_table_set_cell_type(table, row, 0, 3);
            //                lv_table_set_cell_type(table, row, 1, 3);
        }
        else if(scenes[i].fps_normal < 20) {
            //                lv_table_set_cell_type(table, row, 0, 2);
            //                lv_table_set_cell_type(table, row, 1, 2);
        }

        LV_LOG("%s,%s\r\n", scenes[i].name, buf);

        row++;

        lv_snprintf(buf, sizeof(buf), "%s + opa", scenes[i].name);
        lv_table_set_cell_value(table, row, 0, buf);

        LV_LOG("%s,", buf);

        lv_snprintf(buf, sizeof(buf), "%"LV_PRIu32, scenes[i].fps_opa);
        lv_table_set_cell_value(table, row, 1, buf);


        if(scenes[i].fps_opa < 10) {
            //                lv_table_set_cell_type(table, row, 0, 3);
            //                lv_table_set_cell_type(table, row, 1, 3);
        }
        else if(scenes[i].fps_opa < 20) {
            //                lv_table_set_cell_type(table, row, 0, 2);
            //                lv_table_set_cell_type(table, row, 1, 2);
        }

        LV_LOG("%s\r\n", buf);

        row++;
    }

    //        lv_page_set_scrl_layout(page, LV_LAYOUT_COLUMN_LEFT);
}

static void report_cb(lv_timer_t * timer)
{
    if(NULL != benchmark_finished_cb) {
        (*benchmark_finished_cb)();
    }

    if(opa_mode) {
        if(scene_act >= 0) {
            if(scenes[scene_act].time_sum_opa == 0) scenes[scene_act].time_sum_opa = 1;
            scenes[scene_act].fps_opa = (1000 * scenes[scene_act].refr_cnt_opa) / scenes[scene_act].time_sum_opa;
        }

        lv_label_set_text_fmt(subtitle, "Result : %"LV_PRId32" FPS",
                              scenes[scene_act].fps_opa);
        LV_LOG("Result of \"%s + opa\": %"LV_PRId32" FPS", scenes[scene_act].name,
               scenes[scene_act].fps_opa);
    }
    else {
        if(scenes[scene_act].time_sum_normal == 0) scenes[scene_act].time_sum_normal = 1;
        scenes[scene_act].fps_normal = (1000 * scenes[scene_act].refr_cnt_normal) / scenes[scene_act].time_sum_normal;

        lv_label_set_text_fmt(subtitle, "Result : %"LV_PRId32" FPS",
                              scenes[scene_act].fps_normal);
        LV_LOG("Result of \"%s\": %"LV_PRId32" FPS", scenes[scene_act].name,
               scenes[scene_act].fps_normal);
    }
}

static void scene_next_task_cb(lv_timer_t * timer)
{
    LV_UNUSED(timer);
    lv_obj_clean(scene_bg);

    if(opa_mode) {
        if(scene_act >= 0) {
            if(scenes[scene_act].time_sum_opa == 0) scenes[scene_act].time_sum_opa = 1;
            scenes[scene_act].fps_opa = (1000 * scenes[scene_act].refr_cnt_opa) / scenes[scene_act].time_sum_opa;
            if(scenes[scene_act].create_cb) scene_act++;    /*If still there are scenes go to the next*/
        }
        else {
            scene_act++;
        }
        opa_mode = false;
    }
    else {
        if(scenes[scene_act].time_sum_normal == 0) scenes[scene_act].time_sum_normal = 1;
        scenes[scene_act].fps_normal = (1000 * scenes[scene_act].refr_cnt_normal) / scenes[scene_act].time_sum_normal;
        opa_mode = true;
    }

    if(scenes[scene_act].create_cb) {
        lv_label_set_text_fmt(title, "%"LV_PRId32"/%d: %s%s", scene_act * 2 + (opa_mode ? 1 : 0),
                              (int)(dimof(scenes) * 2) - 2,  scenes[scene_act].name, opa_mode ? " + opa" : "");
        if(opa_mode) {
            lv_label_set_text_fmt(subtitle, "Result of \"%s\": %"LV_PRId32" FPS", scenes[scene_act].name,
                                  scenes[scene_act].fps_normal);
        }
        else {
            if(scene_act > 0) {
                lv_label_set_text_fmt(subtitle, "Result of \"%s + opa\": %"LV_PRId32" FPS", scenes[scene_act - 1].name,
                                      scenes[scene_act - 1].fps_opa);
            }
            else {
                lv_label_set_text(subtitle, "");
            }
        }

        rnd_reset();
        scenes[scene_act].create_cb();
        lv_timer_t * t = lv_timer_create(scene_next_task_cb, SCENE_TIME, NULL);
        lv_timer_set_repeat_count(t, 1);

    }
    /*Ready*/
    else {

        /*Restore original frame rate*/
        if(run_max_speed) {
            lv_timer_t * anim_timer = lv_anim_get_timer();
            lv_timer_set_period(anim_timer, anim_ori_timer_period);

            lv_disp_t * disp = lv_disp_get_default();
            lv_timer_t * refr_timer = _lv_disp_get_refr_timer(disp);
            if(refr_timer) {
                lv_timer_set_period(refr_timer, disp_ori_timer_period);
            }
        }

        generate_report();      /* generate report */
    }
}


static void rect_create(lv_style_t * style)
{
    uint32_t i;
    for(i = 0; i < OBJ_NUM; i++) {
        lv_obj_t * obj = lv_obj_create(scene_bg);
        lv_obj_remove_style_all(obj);
        lv_obj_add_style(obj, style, 0);
        lv_obj_set_style_bg_color(obj, lv_color_hex(rnd_next(0, 0xFFFFF0)), 0);
        lv_obj_set_style_border_color(obj, lv_color_hex(rnd_next(0, 0xFFFFF0)), 0);
        lv_obj_set_style_shadow_color(obj, lv_color_hex(rnd_next(0, 0xFFFFF0)), 0);

        lv_obj_set_size(obj, rnd_next(OBJ_SIZE_MIN, OBJ_SIZE_MAX), rnd_next(OBJ_SIZE_MIN, OBJ_SIZE_MAX));

        fall_anim(obj);
    }
}


static void img_create(lv_style_t * style, const void * src, bool rotate, bool zoom, bool aa)
{
    uint32_t i;
    for(i = 0; i < (uint32_t)IMG_NUM; i++) {
        lv_obj_t * obj = lv_img_create(scene_bg);
        lv_obj_remove_style_all(obj);
        lv_obj_add_style(obj, style, 0);
        lv_img_set_src(obj, src);
        lv_obj_set_style_img_recolor(obj, lv_color_hex(rnd_next(0, 0xFFFFF0)), 0);

        if(rotate) lv_img_set_angle(obj, rnd_next(0, 3599));
        if(zoom) lv_img_set_zoom(obj, rnd_next(IMG_ZOOM_MIN, IMG_ZOOM_MAX));
        lv_img_set_antialias(obj, aa);

        fall_anim(obj);
    }
}


static void txt_create(lv_style_t * style)
{
    uint32_t i;
    for(i = 0; i < OBJ_NUM; i++) {
        lv_obj_t * obj = lv_label_create(scene_bg);
        lv_obj_remove_style_all(obj);
        lv_obj_add_style(obj, style, 0);
        lv_obj_set_style_text_color(obj, lv_color_hex(rnd_next(0, 0xFFFFF0)), 0);

        lv_label_set_text(obj, TXT);

        fall_anim(obj);
    }
}


static void line_create(lv_style_t * style)
{
    static lv_point_t points[OBJ_NUM][LINE_POINT_NUM];

    uint32_t i;
    for(i = 0; i < OBJ_NUM; i++) {
        points[i][0].x = 0;
        points[i][0].y = 0;
        uint32_t j;
        for(j = 1; j < LINE_POINT_NUM; j++) {
            points[i][j].x = points[i][j - 1].x + rnd_next(LINE_POINT_DIFF_MIN, LINE_POINT_DIFF_MAX);
            points[i][j].y = rnd_next(LINE_POINT_DIFF_MIN, LINE_POINT_DIFF_MAX);
        }


        lv_obj_t * obj = lv_line_create(scene_bg);
        lv_obj_remove_style_all(obj);
        lv_obj_add_style(obj, style, 0);
        lv_obj_set_style_line_color(obj, lv_color_hex(rnd_next(0, 0xFFFFF0)), 0);

        lv_line_set_points(obj, points[i], LINE_POINT_NUM);

        fall_anim(obj);

    }
}


static void arc_anim_end_angle_cb(void * var, int32_t v)
{
    lv_arc_set_end_angle(var, v);
}

static void arc_create(lv_style_t * style)
{
    uint32_t i;
    for(i = 0; i < OBJ_NUM; i++) {
        lv_obj_t * obj = lv_arc_create(scene_bg);
        lv_obj_remove_style_all(obj);
        lv_obj_set_size(obj, rnd_next(OBJ_SIZE_MIN, OBJ_SIZE_MAX), rnd_next(OBJ_SIZE_MIN, OBJ_SIZE_MAX));
        lv_obj_add_style(obj, style, LV_PART_INDICATOR);
        lv_obj_set_style_arc_color(obj, lv_color_hex(rnd_next(0, 0xFFFFF0)), LV_PART_INDICATOR);

        lv_arc_set_start_angle(obj, 0);

        uint32_t t = rnd_next(ANIM_TIME_MIN / 4, ANIM_TIME_MAX / 4);

        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, obj);
        lv_anim_set_exec_cb(&a, arc_anim_end_angle_cb);
        lv_anim_set_values(&a, 0, 359);
        lv_anim_set_time(&a, t);
        lv_anim_set_playback_time(&a, t);
        lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
        lv_anim_start(&a);

        fall_anim(obj);
    }
}


static void fall_anim_y_cb(void * var, int32_t v)
{
    lv_obj_set_y(var, v);
}

static void fall_anim(lv_obj_t * obj)
{
    lv_obj_set_x(obj, rnd_next(0, lv_obj_get_width(scene_bg) - lv_obj_get_width(obj)));

    uint32_t t = rnd_next(ANIM_TIME_MIN, ANIM_TIME_MAX);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_exec_cb(&a, fall_anim_y_cb);
    lv_anim_set_values(&a, 0, lv_obj_get_height(scene_bg) - lv_obj_get_height(obj));
    lv_anim_set_time(&a, t);
    lv_anim_set_playback_time(&a, t);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    a.act_time = a.time / 2;    /*To start fro mteh middle*/
    lv_anim_start(&a);

}

static void rnd_reset(void)
{
    rnd_act = 0;
}

static int32_t rnd_next(int32_t min, int32_t max)
{
    if(min == max)
        return min;

    if(min > max) {
        int32_t t = min;
        min = max;
        max = t;
    }

    int32_t d = max - min;
    int32_t r = (rnd_map[rnd_act] % d) + min;

    rnd_act++;
    if(rnd_act >= RND_NUM) rnd_act = 0;

    return r;

}

#endif
