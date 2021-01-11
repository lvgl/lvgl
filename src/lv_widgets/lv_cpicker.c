/**
 * @file lv_cpicker.c
 *
 * From @AloyseTech and @paulpv.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_cpicker.h"
#if LV_USE_CPICKER != 0

#include "../lv_misc/lv_debug.h"
#include "../lv_draw/lv_draw_arc.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_core/lv_indev.h"
#include "../lv_core/lv_refr.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_cpicker"

#ifndef LV_CPICKER_DEF_TYPE
    #define LV_CPICKER_DEF_TYPE LV_CPICKER_TYPE_DISC
#endif

#ifndef LV_CPICKER_DEF_HUE
    #define LV_CPICKER_DEF_HUE 0
#endif

#ifndef LV_CPICKER_DEF_SATURATION
    #define LV_CPICKER_DEF_SATURATION 100
#endif

#ifndef LV_CPICKER_DEF_VALUE
    #define LV_CPICKER_DEF_VALUE 100
#endif

#ifndef LV_CPICKER_DEF_HSV
    #define LV_CPICKER_DEF_HSV ((lv_color_hsv_t){LV_CPICKER_DEF_HUE, LV_CPICKER_DEF_SATURATION, LV_CPICKER_DEF_VALUE})
#endif

#ifndef LV_CPICKER_DEF_QF /*quantization factor*/
    #define LV_CPICKER_DEF_QF 3
#endif

#define TRI_OFFSET 2

/* The OUTER_MASK_WIDTH define is required to assist with the placing of a mask over the outer ring of the widget as when the
 * multicoloured radial lines are calculated for the outer ring of the widget their lengths are jittering because of the
 * integer based arithmetic. From tests the maximum delta was found to be 2 so the current value is set to 3 to achieve
 * appropriate masking.
 */
#define OUTER_MASK_WIDTH 3

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_design_res_t lv_cpicker_design(lv_obj_t * cpicker, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_cpicker_signal(lv_obj_t * cpicker, lv_signal_t sign, void * param);
static lv_style_list_t * lv_cpicker_get_style(lv_obj_t * cpicker, uint8_t part);
static bool lv_cpicker_hit(lv_obj_t * cpicker, const lv_point_t * p);

static void draw_rect_grad(lv_obj_t * cpicker, const lv_area_t * mask);
static void draw_disc_grad(lv_obj_t * cpicker, const lv_area_t * mask);
static void draw_knob(lv_obj_t * cpicker, const lv_area_t * mask);
static void invalidate_knob(lv_obj_t * cpicker);
static lv_area_t get_knob_area(lv_obj_t * cpicker);

static void next_color_mode(lv_obj_t * cpicker);
static lv_res_t double_click_reset(lv_obj_t * cpicker);
static void refr_knob_pos(lv_obj_t * cpicker);
static lv_color_t angle_to_mode_color(lv_obj_t * cpicker, uint16_t angle);
static uint16_t get_angle(lv_obj_t * cpicker);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;
static lv_design_cb_t ancestor_design;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a color_picker object
 * @param par pointer to an object, it will be the parent of the new color_picker
 * @param copy pointer to a color_picker object, if not NULL then the new object will be copied from it
 * @return pointer to the created color_picker
 */
lv_obj_t * lv_cpicker_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("color_picker create started");

    lv_obj_t * cpicker = lv_obj_create(par, copy);
    LV_ASSERT_MEM(cpicker);
    if(cpicker == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(cpicker);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(cpicker);

    /*Allocate the extended data*/
    lv_cpicker_ext_t * ext = lv_obj_allocate_ext_attr(cpicker, sizeof(lv_cpicker_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(cpicker);
        return NULL;
    }

    /*Initialize the allocated 'ext' */
    ext->type = LV_CPICKER_DEF_TYPE;
    ext->hsv = LV_CPICKER_DEF_HSV;
    ext->knob.colored = 1;
    ext->color_mode = LV_CPICKER_COLOR_MODE_HUE;
    ext->color_mode_fixed = 0;
    ext->last_click_time = 0;
    ext->last_change_time = 0;

    lv_style_list_init(&ext->knob.style_list);

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(cpicker, lv_cpicker_signal);
    lv_obj_set_design_cb(cpicker, lv_cpicker_design);

    /*If no copy do the basic initialization*/
    if(copy == NULL) {
        lv_obj_set_size(cpicker, LV_DPI * 2, LV_DPI * 2);
        lv_obj_add_protect(cpicker, LV_PROTECT_PRESS_LOST);
        lv_obj_set_adv_hittest(cpicker, true);
        lv_theme_apply(cpicker, LV_THEME_CPICKER);
    }
    /*Copy 'copy'*/
    else {
        lv_cpicker_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->type = copy_ext->type;
        ext->color_mode = copy_ext->color_mode;
        ext->color_mode_fixed = copy_ext->color_mode_fixed;
        ext->hsv = copy_ext->hsv;
        ext->knob.colored = copy_ext->knob.colored;

        lv_style_list_copy(&ext->knob.style_list, &copy_ext->knob.style_list);
        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(cpicker, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
    }
    refr_knob_pos(cpicker);

    LV_LOG_INFO("color_picker created");

    return cpicker;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a new type for a cpicker
 * @param cpicker pointer to a cpicker object
 * @param type new type of the cpicker (from 'lv_cpicker_type_t' enum)
 */
void lv_cpicker_set_type(lv_obj_t * cpicker, lv_cpicker_type_t type)
{
    LV_ASSERT_OBJ(cpicker, LV_OBJX_NAME);

    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);
    if(ext->type == type) return;

    ext->type = type;
    lv_obj_refresh_ext_draw_pad(cpicker);
    refr_knob_pos(cpicker);

    lv_obj_invalidate(cpicker);
}

/**
 * Set the current hue of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param hue current selected hue [0..360]
 * @return true if changed, otherwise false
 */
bool lv_cpicker_set_hue(lv_obj_t * cpicker, uint16_t hue)
{
    lv_color_hsv_t hsv = lv_cpicker_get_hsv(cpicker);
    hsv.h = hue;
    return lv_cpicker_set_hsv(cpicker, hsv);
}

/**
 * Set the current saturation of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param saturation current selected saturation [0..100]
 * @return true if changed, otherwise false
 */
bool lv_cpicker_set_saturation(lv_obj_t * cpicker, uint8_t saturation)
{
    lv_color_hsv_t hsv = lv_cpicker_get_hsv(cpicker);
    hsv.s = saturation;
    return lv_cpicker_set_hsv(cpicker, hsv);
}

/**
 * Set the current value of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param val current selected value [0..100]
 * @return true if changed, otherwise false
 */
bool lv_cpicker_set_value(lv_obj_t * cpicker, uint8_t val)
{
    lv_color_hsv_t hsv = lv_cpicker_get_hsv(cpicker);
    hsv.v = val;
    return lv_cpicker_set_hsv(cpicker, hsv);
}

/**
 * Set the current hsv of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param color current selected hsv
 * @return true if changed, otherwise false
 */
bool lv_cpicker_set_hsv(lv_obj_t * cpicker, lv_color_hsv_t hsv)
{
    LV_ASSERT_OBJ(cpicker, LV_OBJX_NAME);

    if(hsv.h > 360) hsv.h %= 360;
    if(hsv.s > 100) hsv.s = 100;
    if(hsv.v > 100) hsv.v = 100;

    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    if(ext->hsv.h == hsv.h && ext->hsv.s == hsv.s && ext->hsv.v == hsv.v) return false;

    ext->hsv = hsv;

    refr_knob_pos(cpicker);

    lv_obj_invalidate(cpicker);

    return true;
}

/**
 * Set the current color of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @param color current selected color
 * @return true if changed, otherwise false
 */
bool lv_cpicker_set_color(lv_obj_t * cpicker, lv_color_t color)
{
    LV_ASSERT_OBJ(cpicker, LV_OBJX_NAME);

    lv_color32_t c32;
    c32.full = lv_color_to32(color);

    return lv_cpicker_set_hsv(cpicker,
                              lv_color_rgb_to_hsv(c32.ch.red, c32.ch.green, c32.ch.blue));
}

/**
 * Set the current color mode.
 * @param cpicker pointer to colorpicker object
 * @param mode color mode (hue/sat/val)
 */
void lv_cpicker_set_color_mode(lv_obj_t * cpicker, lv_cpicker_color_mode_t mode)
{
    LV_ASSERT_OBJ(cpicker, LV_OBJX_NAME);

    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    ext->color_mode = mode;
    refr_knob_pos(cpicker);
    lv_obj_invalidate(cpicker);
}

/**
 * Set if the color mode is changed on long press on center
 * @param cpicker pointer to colorpicker object
 * @param fixed color mode cannot be changed on long press
 */
void lv_cpicker_set_color_mode_fixed(lv_obj_t * cpicker, bool fixed)
{
    LV_ASSERT_OBJ(cpicker, LV_OBJX_NAME);

    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    ext->color_mode_fixed = fixed;
}

/**
 * Make the knob to be colored to the current color
 * @param cpicker pointer to colorpicker object
 * @param en true: color the knob; false: not color the knob
 */
void lv_cpicker_set_knob_colored(lv_obj_t * cpicker, bool en)
{
    LV_ASSERT_OBJ(cpicker, LV_OBJX_NAME);

    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);
    ext->knob.colored = en ? 1 : 0;
    invalidate_knob(cpicker);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the current color mode.
 * @param cpicker pointer to colorpicker object
 * @return color mode (hue/sat/val)
 */
lv_cpicker_color_mode_t lv_cpicker_get_color_mode(lv_obj_t * cpicker)
{
    LV_ASSERT_OBJ(cpicker, LV_OBJX_NAME);

    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return ext->color_mode;
}

/**
 * Get if the color mode is changed on long press on center
 * @param cpicker pointer to colorpicker object
 * @return mode cannot be changed on long press
 */
bool lv_cpicker_get_color_mode_fixed(lv_obj_t * cpicker)
{
    LV_ASSERT_OBJ(cpicker, LV_OBJX_NAME);

    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return ext->color_mode_fixed;
}

/**
 * Get the current selected hue of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return hue current selected hue
 */
uint16_t lv_cpicker_get_hue(lv_obj_t * cpicker)
{
    LV_ASSERT_OBJ(cpicker, LV_OBJX_NAME);

    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return ext->hsv.h;
}

/**
 * Get the current selected saturation of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return current selected saturation
 */
uint8_t lv_cpicker_get_saturation(lv_obj_t * cpicker)
{
    LV_ASSERT_OBJ(cpicker, LV_OBJX_NAME);

    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return ext->hsv.s;
}

/**
 * Get the current selected hue of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return current selected value
 */
uint8_t lv_cpicker_get_value(lv_obj_t * cpicker)
{
    LV_ASSERT_OBJ(cpicker, LV_OBJX_NAME);

    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return ext->hsv.v;
}

/**
 * Get the current selected hsv of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return current selected hsv
 */
lv_color_hsv_t lv_cpicker_get_hsv(lv_obj_t * cpicker)
{
    LV_ASSERT_OBJ(cpicker, LV_OBJX_NAME);

    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return ext->hsv;
}

/**
 * Get the current selected color of a colorpicker.
 * @param cpicker pointer to colorpicker object
 * @return color current selected color
 */
lv_color_t lv_cpicker_get_color(lv_obj_t * cpicker)
{
    LV_ASSERT_OBJ(cpicker, LV_OBJX_NAME);

    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return lv_color_hsv_to_rgb(ext->hsv.h, ext->hsv.s, ext->hsv.v);
}

/**
 * Whether the knob is colored to the current color or not
 * @param cpicker pointer to color picker object
 * @return true: color the knob; false: not color the knob
 */
bool lv_cpicker_get_knob_colored(lv_obj_t * cpicker)
{
    LV_ASSERT_OBJ(cpicker, LV_OBJX_NAME);

    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    return ext->knob.colored ? true : false;
}

/*=====================
 * Other functions
 *====================*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the color_picker
 * @param cpicker pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @return return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_cpicker_design(lv_obj_t * cpicker, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK)  {
        return LV_DESIGN_RES_NOT_COVER;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

        if(ext->type == LV_CPICKER_TYPE_DISC) {
            draw_disc_grad(cpicker, clip_area);
        }
        else if(ext->type == LV_CPICKER_TYPE_RECT) {
            draw_rect_grad(cpicker, clip_area);
        }

        draw_knob(cpicker, clip_area);
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
    }

    return LV_DESIGN_RES_OK;
}

static void draw_disc_grad(lv_obj_t * cpicker, const lv_area_t * mask)
{
    lv_coord_t w = lv_obj_get_width(cpicker);
    lv_coord_t h = lv_obj_get_height(cpicker);
    lv_coord_t cx = cpicker->coords.x1 + w / 2;
    lv_coord_t cy = cpicker->coords.y1 + h / 2;
    lv_coord_t r = w / 2;

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(cpicker, LV_CPICKER_PART_MAIN, &line_dsc);

    line_dsc.width = (r * 628 / (360 / LV_CPICKER_DEF_QF)) / 100;
    line_dsc.width += 2;
    uint16_t i;
    lv_coord_t cir_w = lv_obj_get_style_scale_width(cpicker, LV_CPICKER_PART_MAIN);

    /* Mask outer ring of widget to tidy up ragged edges of lines while drawing outer ring */
    lv_area_t mask_area_out;
    lv_area_copy(&mask_area_out, &cpicker->coords);
    mask_area_out.x1 += OUTER_MASK_WIDTH;
    mask_area_out.x2 -= OUTER_MASK_WIDTH;
    mask_area_out.y1 += OUTER_MASK_WIDTH;
    mask_area_out.y2 -= OUTER_MASK_WIDTH;
    lv_draw_mask_radius_param_t mask_out_param;
    lv_draw_mask_radius_init(&mask_out_param, &mask_area_out, LV_RADIUS_CIRCLE, false);
    int16_t mask_out_id = lv_draw_mask_add(&mask_out_param, 0);

    /* The inner line ends will be masked out.
     * So make lines a little bit longer because the masking makes a more even result */
    lv_coord_t cir_w_extra = cir_w + line_dsc.width;

    for(i = 0; i <= 360; i += LV_CPICKER_DEF_QF) {
        line_dsc.color = angle_to_mode_color(cpicker, i);

        lv_point_t p[2];
        p[0].x = cx + (r * _lv_trigo_sin(i) >> LV_TRIGO_SHIFT);
        p[0].y = cy + (r * _lv_trigo_sin(i + 90) >> LV_TRIGO_SHIFT);
        p[1].x = cx + ((r - cir_w_extra) * _lv_trigo_sin(i) >> LV_TRIGO_SHIFT);
        p[1].y = cy + ((r - cir_w_extra) * _lv_trigo_sin(i + 90) >> LV_TRIGO_SHIFT);

        lv_draw_line(&p[0], &p[1], mask, &line_dsc);
    }
    /* Now remove mask to continue with inner part */
    lv_draw_mask_remove_id(mask_out_id);

    /*Mask out the inner area*/
    lv_draw_rect_dsc_t bg_dsc;
    lv_draw_rect_dsc_init(&bg_dsc);
    lv_obj_init_draw_rect_dsc(cpicker, LV_CPICKER_PART_MAIN, &bg_dsc);
    bg_dsc.radius = LV_RADIUS_CIRCLE;

    lv_area_t area_mid;
    lv_area_copy(&area_mid, &cpicker->coords);
    area_mid.x1 += cir_w;
    area_mid.y1 += cir_w;
    area_mid.x2 -= cir_w;
    area_mid.y2 -= cir_w;

    lv_draw_rect(&area_mid, mask, &bg_dsc);

    lv_style_int_t inner = lv_obj_get_style_pad_inner(cpicker, LV_CPICKER_PART_MAIN);
    lv_color_t color = lv_cpicker_get_color(cpicker);
    bg_dsc.bg_color = color;
    area_mid.x1 += inner;
    area_mid.y1 += inner;
    area_mid.x2 -= inner;
    area_mid.y2 -= inner;

    lv_draw_rect(&area_mid, mask, &bg_dsc);
}

static void draw_rect_grad(lv_obj_t * cpicker, const lv_area_t * mask)
{
    lv_draw_rect_dsc_t bg_dsc;
    lv_draw_rect_dsc_init(&bg_dsc);
    lv_obj_init_draw_rect_dsc(cpicker, LV_CPICKER_PART_MAIN, &bg_dsc);

    lv_area_t grad_area;
    lv_obj_get_coords(cpicker, &grad_area);

    if(bg_dsc.radius) {
        lv_coord_t h = lv_obj_get_height(cpicker);
        lv_coord_t r = bg_dsc.radius;
        if(r > h / 2) r = h / 2;
        /*Make the gradient area smaller with a half circle on both ends*/
        grad_area.x1 += r;
        grad_area.x2 -= r;

        /*Draw the left rounded end*/
        lv_area_t rounded_edge_area;
        lv_obj_get_coords(cpicker, &rounded_edge_area);
        rounded_edge_area.x2 = rounded_edge_area.x1 + 2 * r;

        bg_dsc.bg_color = angle_to_mode_color(cpicker, 0);

        lv_draw_rect(&rounded_edge_area, mask, &bg_dsc);

        /*Draw the right rounded end*/
        lv_obj_get_coords(cpicker, &rounded_edge_area);
        rounded_edge_area.x1 = rounded_edge_area.x2 - 2 * r;

        bg_dsc.bg_color = angle_to_mode_color(cpicker, 359);

        lv_draw_rect(&rounded_edge_area, mask, &bg_dsc);
    }

    lv_coord_t grad_w = lv_area_get_width(&grad_area);
    if(grad_w < 1) return;
    uint16_t i_step = LV_MATH_MAX(LV_CPICKER_DEF_QF, 360 / grad_w);
    bg_dsc.radius = 0;
    bg_dsc.border_width = 0;
    bg_dsc.shadow_width = 0;

    uint16_t i;
    for(i = 0; i < 360; i += i_step) {
        bg_dsc.bg_color = angle_to_mode_color(cpicker, i);

        /*the following attribute might need changing between index to add border, shadow, radius etc*/
        lv_area_t rect_area;

        /*scale angle (hue/sat/val) to linear coordinate*/
        lv_coord_t xi = (i * grad_w) / 360;
        lv_coord_t xi2 = ((i + i_step) * grad_w) / 360;

        rect_area.x1 = LV_MATH_MIN(grad_area.x1 + xi, grad_area.x1 + grad_w - i_step);
        rect_area.y1 = grad_area.y1;
        rect_area.x2 = LV_MATH_MIN(grad_area.x1 + xi2, grad_area.x1 + grad_w - i_step);
        rect_area.y2 = grad_area.y2;

        lv_draw_rect(&rect_area, mask, &bg_dsc);
    }
}

static void draw_knob(lv_obj_t * cpicker, const lv_area_t * mask)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    lv_draw_rect_dsc_t cir_dsc;
    lv_draw_rect_dsc_init(&cir_dsc);
    lv_obj_init_draw_rect_dsc(cpicker, LV_CPICKER_PART_KNOB, &cir_dsc);

    cir_dsc.radius = LV_RADIUS_CIRCLE;

    if(ext->knob.colored) {
        cir_dsc.bg_color = lv_cpicker_get_color(cpicker);
    }

    lv_area_t knob_area = get_knob_area(cpicker);

    lv_draw_rect(&knob_area, mask, &cir_dsc);
}

static void invalidate_knob(lv_obj_t * cpicker)
{
    lv_area_t knob_area = get_knob_area(cpicker);

    lv_obj_invalidate_area(cpicker, &knob_area);
}

static lv_area_t get_knob_area(lv_obj_t * cpicker)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    /*Get knob's radius*/
    uint16_t r = 0;
    if(ext->type == LV_CPICKER_TYPE_DISC) {
        r = lv_obj_get_style_scale_width(cpicker, LV_CPICKER_PART_MAIN) / 2;
    }
    else if(ext->type == LV_CPICKER_TYPE_RECT) {
        lv_coord_t h = lv_obj_get_height(cpicker);
        r = h / 2;
    }

    lv_style_int_t left = lv_obj_get_style_pad_left(cpicker, LV_CPICKER_PART_KNOB);
    lv_style_int_t right = lv_obj_get_style_pad_right(cpicker, LV_CPICKER_PART_KNOB);
    lv_style_int_t top = lv_obj_get_style_pad_top(cpicker, LV_CPICKER_PART_KNOB);
    lv_style_int_t bottom = lv_obj_get_style_pad_bottom(cpicker, LV_CPICKER_PART_KNOB);

    lv_area_t knob_area;
    knob_area.x1 = cpicker->coords.x1 + ext->knob.pos.x - r - left;
    knob_area.y1 = cpicker->coords.y1 + ext->knob.pos.y - r - right;
    knob_area.x2 = cpicker->coords.x1 + ext->knob.pos.x + r + top;
    knob_area.y2 = cpicker->coords.y1 + ext->knob.pos.y + r + bottom;

    return knob_area;
}

/**
 * Signal function of the color_picker
 * @param cpicker pointer to a color_picker object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_cpicker_signal(lv_obj_t * cpicker, lv_signal_t sign, void * param)
{
    /* Include the ancient signal function */
    lv_res_t res;

    if(sign == LV_SIGNAL_GET_STYLE) {
        lv_get_style_info_t * info = param;
        info->result = lv_cpicker_get_style(cpicker, info->part);
        if(info->result != NULL) return LV_RES_OK;
        else return ancestor_signal(cpicker, sign, param);
    }

    res = ancestor_signal(cpicker, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);

    if(sign == LV_SIGNAL_CLEANUP) {
        lv_obj_clean_style_list(cpicker, LV_CPICKER_PART_KNOB);
    }
    else if(sign == LV_SIGNAL_REFR_EXT_DRAW_PAD) {
        lv_style_int_t left = lv_obj_get_style_pad_left(cpicker, LV_CPICKER_PART_KNOB);
        lv_style_int_t right = lv_obj_get_style_pad_right(cpicker, LV_CPICKER_PART_KNOB);
        lv_style_int_t top = lv_obj_get_style_pad_top(cpicker, LV_CPICKER_PART_KNOB);
        lv_style_int_t bottom = lv_obj_get_style_pad_bottom(cpicker, LV_CPICKER_PART_KNOB);

        lv_coord_t knob_pad = LV_MATH_MAX4(left, right, top, bottom) + 2;

        if(ext->type == LV_CPICKER_TYPE_DISC) {
            cpicker->ext_draw_pad = LV_MATH_MAX(cpicker->ext_draw_pad, knob_pad);
        }
        else {
            knob_pad += lv_obj_get_height(cpicker) / 2;
            cpicker->ext_draw_pad = LV_MATH_MAX(cpicker->ext_draw_pad, knob_pad);
        }
    }
    else if(sign == LV_SIGNAL_COORD_CHG) {
        /*Refresh extended draw area to make knob visible*/
        if(lv_obj_get_width(cpicker) != lv_area_get_width(param) ||
           lv_obj_get_height(cpicker) != lv_area_get_height(param)) {
            lv_obj_refresh_ext_draw_pad(cpicker);
            refr_knob_pos(cpicker);
            lv_obj_invalidate(cpicker);
        }
    }
    else if(sign == LV_SIGNAL_STYLE_CHG) {
        /*Refresh extended draw area to make knob visible*/
        lv_obj_refresh_ext_draw_pad(cpicker);
        refr_knob_pos(cpicker);
        lv_obj_invalidate(cpicker);
    }
    else if(sign == LV_SIGNAL_CONTROL) {
#if LV_USE_GROUP
        uint32_t c = *((uint32_t *)param); /*uint32_t because can be UTF-8*/

        if(c == LV_KEY_RIGHT || c == LV_KEY_UP) {
            lv_color_hsv_t hsv_cur;
            hsv_cur = ext->hsv;

            switch(ext->color_mode) {
                case LV_CPICKER_COLOR_MODE_HUE:
                    hsv_cur.h = (ext->hsv.h + 1) % 360;
                    break;
                case LV_CPICKER_COLOR_MODE_SATURATION:
                    hsv_cur.s = (ext->hsv.s + 1) % 100;
                    break;
                case LV_CPICKER_COLOR_MODE_VALUE:
                    hsv_cur.v = (ext->hsv.v + 1) % 100;
                    break;
            }

            if(lv_cpicker_set_hsv(cpicker, hsv_cur)) {
                res = lv_event_send(cpicker, LV_EVENT_VALUE_CHANGED, NULL);
                if(res != LV_RES_OK) return res;
            }
        }
        else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) {
            lv_color_hsv_t hsv_cur;
            hsv_cur = ext->hsv;

            switch(ext->color_mode) {
                case LV_CPICKER_COLOR_MODE_HUE:
                    hsv_cur.h = ext->hsv.h > 0 ? (ext->hsv.h - 1) : 360;
                    break;
                case LV_CPICKER_COLOR_MODE_SATURATION:
                    hsv_cur.s = ext->hsv.s > 0 ? (ext->hsv.s - 1) : 100;
                    break;
                case LV_CPICKER_COLOR_MODE_VALUE:
                    hsv_cur.v = ext->hsv.v > 0 ? (ext->hsv.v - 1) : 100;
                    break;
            }

            if(lv_cpicker_set_hsv(cpicker, hsv_cur)) {
                res = lv_event_send(cpicker, LV_EVENT_VALUE_CHANGED, NULL);
                if(res != LV_RES_OK) return res;
            }
        }
#endif
    }
    else if(sign == LV_SIGNAL_PRESSED) {
        ext->last_change_time = lv_tick_get();
        lv_indev_get_point(lv_indev_get_act(), &ext->last_press_point);
        res = double_click_reset(cpicker);
        if(res != LV_RES_OK) return res;
    }
    else if(sign == LV_SIGNAL_PRESSING) {
        lv_indev_t * indev = lv_indev_get_act();
        if(indev == NULL) return res;

        lv_indev_type_t indev_type = lv_indev_get_type(indev);
        lv_point_t p;
        if(indev_type == LV_INDEV_TYPE_ENCODER || indev_type == LV_INDEV_TYPE_KEYPAD) {
            p.x = cpicker->coords.x1 + lv_obj_get_width(cpicker) / 2;
            p.y = cpicker->coords.y1 + lv_obj_get_height(cpicker) / 2;
        }
        else {
            lv_indev_get_point(indev, &p);
        }

        if((LV_MATH_ABS(p.x - ext->last_press_point.x) > indev->driver.drag_limit / 2) ||
           (LV_MATH_ABS(p.y - ext->last_press_point.y) > indev->driver.drag_limit / 2)) {
            ext->last_change_time = lv_tick_get();
            ext->last_press_point.x = p.x;
            ext->last_press_point.y = p.y;
        }

        p.x -= cpicker->coords.x1;
        p.y -= cpicker->coords.y1;

        /*Ignore pressing in the inner area*/
        uint16_t w = lv_obj_get_width(cpicker);

        int16_t angle = 0;

        if(ext->type == LV_CPICKER_TYPE_RECT) {
            /*If pressed long enough without change go to next color mode*/
            uint32_t diff = lv_tick_elaps(ext->last_change_time);
            if(diff > (uint32_t)indev->driver.long_press_time * 2 && !ext->color_mode_fixed) {
                next_color_mode(cpicker);
                lv_indev_wait_release(lv_indev_get_act());
                return res;
            }

            angle = (p.x * 360) / w;
            if(angle < 0) angle = 0;
            if(angle >= 360) angle = 359;

        }
        else if(ext->type == LV_CPICKER_TYPE_DISC) {
            lv_style_int_t scale_w = lv_obj_get_style_scale_width(cpicker, LV_CPICKER_PART_MAIN);

            lv_coord_t r_in = w / 2;
            p.x -= r_in;
            p.y -= r_in;
            bool on_ring = true;
            r_in -= scale_w;
            if(r_in > LV_DPI / 2) {
                lv_style_int_t inner = lv_obj_get_style_pad_inner(cpicker, LV_CPICKER_PART_MAIN);
                r_in -= inner;

                if(r_in < LV_DPI / 2) r_in = LV_DPI / 2;
            }

            if(p.x * p.x + p.y * p.y < r_in * r_in) {
                on_ring = false;
            }

            /*If the inner area is being pressed, go to the next color mode on long press*/
            uint32_t diff = lv_tick_elaps(ext->last_change_time);
            if(!on_ring && diff > indev->driver.long_press_time && !ext->color_mode_fixed) {
                next_color_mode(cpicker);
                lv_indev_wait_release(lv_indev_get_act());
                return res;
            }

            /*Set the angle only if pressed on the ring*/
            if(!on_ring) return res;

            angle = _lv_atan2(p.x, p.y) % 360;
        }

        lv_color_hsv_t hsv_cur;
        hsv_cur = ext->hsv;

        switch(ext->color_mode) {
            case LV_CPICKER_COLOR_MODE_HUE:
                hsv_cur.h = angle;
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                hsv_cur.s = (angle * 100) / 360;
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                hsv_cur.v = (angle * 100) / 360;
                break;
        }

        if(lv_cpicker_set_hsv(cpicker, hsv_cur)) {
            res = lv_event_send(cpicker, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
    }
    else if(sign == LV_SIGNAL_HIT_TEST) {
        lv_hit_test_info_t * info = param;
        info->result = lv_cpicker_hit(cpicker, info->point);
    }

    return res;
}

/**
 * Get the style_list descriptor of a part of the object
 * @param cpicker pointer the object
 * @param part the part of the cpicker. (LV_PAGE_CPICKER_...)
 * @return pointer to the style_list descriptor of the specified part
 */
static lv_style_list_t * lv_cpicker_get_style(lv_obj_t * cpicker, uint8_t part)
{
    LV_ASSERT_OBJ(cpicker, LV_OBJX_NAME);

    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);
    lv_style_list_t * style_dsc_p;

    switch(part) {
        case LV_CPICKER_PART_MAIN :
            style_dsc_p = &cpicker->style_list;
            break;
        case LV_CPICKER_PART_KNOB:
            style_dsc_p = &ext->knob.style_list;
            break;
        default:
            style_dsc_p = NULL;
    }

    return style_dsc_p;
}

static bool lv_cpicker_hit(lv_obj_t * cpicker, const lv_point_t * p)
{
    bool is_point_on_coords = lv_obj_is_point_on_coords(cpicker, p);
    if(!is_point_on_coords)
        return false;

    lv_cpicker_ext_t * ext = (lv_cpicker_ext_t *)lv_obj_get_ext_attr(cpicker);
    if(ext->type == LV_CPICKER_TYPE_RECT)
        return true;

    /*Valid clicks can be only in the circle*/
    if(_lv_area_is_point_on(&cpicker->coords, p, LV_RADIUS_CIRCLE)) return true;
    else return false;
}

static void next_color_mode(lv_obj_t * cpicker)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);
    ext->color_mode = (ext->color_mode + 1) % 3;
    refr_knob_pos(cpicker);
    lv_obj_invalidate(cpicker);
}

static void refr_knob_pos(lv_obj_t * cpicker)
{
    invalidate_knob(cpicker);

    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);
    lv_coord_t w = lv_obj_get_width(cpicker);
    lv_coord_t h = lv_obj_get_height(cpicker);

    if(ext->type == LV_CPICKER_TYPE_RECT) {
        lv_coord_t ind_pos = 0;
        switch(ext->color_mode) {
            case LV_CPICKER_COLOR_MODE_HUE:
                ind_pos += (ext->hsv.h * w) / 360;
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                ind_pos += (ext->hsv.s * w) / 100;
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                ind_pos += (ext->hsv.v * w) / 100;
                break;
        }

        ext->knob.pos.x = ind_pos;
        ext->knob.pos.y = h / 2;
    }
    else if(ext->type == LV_CPICKER_TYPE_DISC) {
        lv_style_int_t scale_w = lv_obj_get_style_scale_width(cpicker, LV_CPICKER_PART_MAIN);
        lv_coord_t r = (w - scale_w) / 2;
        uint16_t angle = get_angle(cpicker);
        ext->knob.pos.x = (((int32_t)r * _lv_trigo_sin(angle)) >> LV_TRIGO_SHIFT);
        ext->knob.pos.y = (((int32_t)r * _lv_trigo_sin(angle + 90)) >> LV_TRIGO_SHIFT);
        ext->knob.pos.x = ext->knob.pos.x + w / 2;
        ext->knob.pos.y = ext->knob.pos.y + h / 2;
    }

    invalidate_knob(cpicker);
}

static lv_res_t double_click_reset(lv_obj_t * cpicker)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);
    lv_indev_t * indev = lv_indev_get_act();
    /*Double clicked? Use long press time as double click time out*/
    if(lv_tick_elaps(ext->last_click_time) < indev->driver.long_press_time) {
        lv_color_hsv_t hsv_cur;
        hsv_cur = ext->hsv;

        switch(ext->color_mode) {
            case LV_CPICKER_COLOR_MODE_HUE:
                hsv_cur.h = LV_CPICKER_DEF_HUE;
                break;
            case LV_CPICKER_COLOR_MODE_SATURATION:
                hsv_cur.s = LV_CPICKER_DEF_SATURATION;
                break;
            case LV_CPICKER_COLOR_MODE_VALUE:
                hsv_cur.v = LV_CPICKER_DEF_VALUE;
                break;
        }

        lv_indev_wait_release(indev);

        if(lv_cpicker_set_hsv(cpicker, hsv_cur)) {
            lv_res_t res = lv_event_send(cpicker, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
    }
    ext->last_click_time = lv_tick_get();

    return LV_RES_OK;
}

static lv_color_t angle_to_mode_color(lv_obj_t * cpicker, uint16_t angle)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);
    lv_color_t color;
    switch(ext->color_mode) {
        default:
        case LV_CPICKER_COLOR_MODE_HUE:
            color = lv_color_hsv_to_rgb(angle % 360, ext->hsv.s, ext->hsv.v);
            break;
        case LV_CPICKER_COLOR_MODE_SATURATION:
            color = lv_color_hsv_to_rgb(ext->hsv.h, ((angle % 360) * 100) / 360, ext->hsv.v);
            break;
        case LV_CPICKER_COLOR_MODE_VALUE:
            color = lv_color_hsv_to_rgb(ext->hsv.h, ext->hsv.s, ((angle % 360) * 100) / 360);
            break;
    }
    return color;
}

static uint16_t get_angle(lv_obj_t * cpicker)
{
    lv_cpicker_ext_t * ext = lv_obj_get_ext_attr(cpicker);
    uint16_t angle;
    switch(ext->color_mode) {
        default:
        case LV_CPICKER_COLOR_MODE_HUE:
            angle = ext->hsv.h;
            break;
        case LV_CPICKER_COLOR_MODE_SATURATION:
            angle = (ext->hsv.s * 360) / 100;
            break;
        case LV_CPICKER_COLOR_MODE_VALUE:
            angle = (ext->hsv.v * 360) / 100 ;
            break;
    }
    return angle;
}

#endif /* LV_USE_CPICKER != 0 */
