/**
 * @file lv_colorwheel.c
 *
 * Based on the work of @AloyseTech and @paulpv.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_colorwheel.h"
#if LV_USE_COLORWHEEL

#include "../../../misc/lv_assert.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_colorwheel_class

#define LV_CPICKER_DEF_QF 3

/**
 * The OUTER_MASK_WIDTH define is required to assist with the placing of a mask over the outer ring of the widget as when the
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
static void lv_colorwheel_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_colorwheel_event(const lv_obj_class_t * class_p, lv_event_t * e);

static void draw_disc_grad(lv_event_t * e);
static void draw_knob(lv_event_t * e);
static void invalidate_knob(lv_obj_t * obj);
static lv_area_t get_knob_area(lv_obj_t * obj);

static void next_color_mode(lv_obj_t * obj);
static lv_res_t double_click_reset(lv_obj_t * obj);
static void refr_knob_pos(lv_obj_t * obj);
static lv_color_t angle_to_mode_color_fast(lv_obj_t * obj, uint16_t angle);
static uint16_t get_angle(lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_colorwheel_class = {.instance_size = sizeof(lv_colorwheel_t), .base_class = &lv_obj_class,
                                            .constructor_cb = lv_colorwheel_constructor,
                                            .event_cb = lv_colorwheel_event,
                                            .width_def = LV_DPI_DEF * 2,
                                            .height_def = LV_DPI_DEF * 2,
                                            .editable = LV_OBJ_CLASS_EDITABLE_TRUE,
                                           };

static bool create_knob_recolor;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a color_picker object
 * @param par pointer to an object, it will be the parent of the new color_picker
 * @return pointer to the created color_picker
 */
lv_obj_t * lv_colorwheel_create(lv_obj_t * parent, bool knob_recolor)
{
    LV_LOG_INFO("begin");
    create_knob_recolor = knob_recolor;

    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the current hsv of a color wheel.
 * @param colorwheel pointer to color wheel object
 * @param color current selected hsv
 * @return true if changed, otherwise false
 */
bool lv_colorwheel_set_hsv(lv_obj_t * obj, lv_color_hsv_t hsv)
{
    if(hsv.h > 360) hsv.h %= 360;
    if(hsv.s > 100) hsv.s = 100;
    if(hsv.v > 100) hsv.v = 100;

    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_colorwheel_t * colorwheel = (lv_colorwheel_t *)obj;

    if(colorwheel->hsv.h == hsv.h && colorwheel->hsv.s == hsv.s && colorwheel->hsv.v == hsv.v) return false;

    colorwheel->hsv = hsv;

    refr_knob_pos(obj);

    lv_obj_invalidate(obj);

    return true;
}

/**
 * Set the current color of a color wheel.
 * @param colorwheel pointer to color wheel object
 * @param color current selected color
 * @return true if changed, otherwise false
 */
bool lv_colorwheel_set_rgb(lv_obj_t * obj, lv_color_t color)
{
    lv_color32_t c32;
    c32.full = lv_color_to32(color);

    return lv_colorwheel_set_hsv(obj,  lv_color_rgb_to_hsv(c32.ch.red, c32.ch.green, c32.ch.blue));
}

/**
 * Set the current color mode.
 * @param colorwheel pointer to color wheel object
 * @param mode color mode (hue/sat/val)
 */
void lv_colorwheel_set_mode(lv_obj_t * obj, lv_colorwheel_mode_t mode)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_colorwheel_t * colorwheel = (lv_colorwheel_t *)obj;

    colorwheel->mode = mode;
    refr_knob_pos(obj);
    lv_obj_invalidate(obj);
}

/**
 * Set if the color mode is changed on long press on center
 * @param colorwheel pointer to color wheel object
 * @param fixed color mode cannot be changed on long press
 */
void lv_colorwheel_set_mode_fixed(lv_obj_t * obj, bool fixed)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_colorwheel_t * colorwheel = (lv_colorwheel_t *)obj;

    colorwheel->mode_fixed = fixed;
}

/*=====================
 * Getter functions
 *====================*/


/**
 * Get the current selected hsv of a color wheel.
 * @param colorwheel pointer to color wheel object
 * @return current selected hsv
 */
lv_color_hsv_t lv_colorwheel_get_hsv(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_colorwheel_t * colorwheel = (lv_colorwheel_t *)obj;

    return colorwheel->hsv;
}

/**
 * Get the current selected color of a color wheel.
 * @param colorwheel pointer to color wheel object
 * @return color current selected color
 */
lv_color_t lv_colorwheel_get_rgb(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_colorwheel_t * colorwheel = (lv_colorwheel_t *)obj;

    return lv_color_hsv_to_rgb(colorwheel->hsv.h, colorwheel->hsv.s, colorwheel->hsv.v);
}

/**
 * Get the current color mode.
 * @param colorwheel pointer to color wheel object
 * @return color mode (hue/sat/val)
 */
lv_colorwheel_mode_t lv_colorwheel_get_color_mode(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_colorwheel_t * colorwheel = (lv_colorwheel_t *)obj;

    return colorwheel->mode;
}

/**
 * Get if the color mode is changed on long press on center
 * @param colorwheel pointer to color wheel object
 * @return mode cannot be changed on long press
 */
bool lv_colorwheel_get_color_mode_fixed(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_colorwheel_t * colorwheel = (lv_colorwheel_t *)obj;

    return colorwheel->mode_fixed;
}

/*=====================
 * Other functions
 *====================*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_colorwheel_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_colorwheel_t * colorwheel = (lv_colorwheel_t *)obj;
    colorwheel->hsv.h = 0;
    colorwheel->hsv.s = 100;
    colorwheel->hsv.v = 100;
    colorwheel->mode = LV_COLORWHEEL_MODE_HUE;
    colorwheel->mode_fixed = 0;
    colorwheel->last_click_time = 0;
    colorwheel->last_change_time = 0;
    colorwheel->knob.recolor = create_knob_recolor;

    lv_obj_add_flag(obj, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_CHAIN);
    refr_knob_pos(obj);
}

static void draw_disc_grad(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    const lv_area_t * clip_area = lv_event_get_param(e);
    lv_coord_t w = lv_obj_get_width(obj);
    lv_coord_t h = lv_obj_get_height(obj);
    lv_coord_t cx = obj->coords.x1 + w / 2;
    lv_coord_t cy = obj->coords.y1 + h / 2;
    lv_coord_t r = w / 2;

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &line_dsc);

    line_dsc.width = (r * 628 / (256 / LV_CPICKER_DEF_QF)) / 100;
    line_dsc.width += 2;
    uint16_t i;
    uint32_t a = 0;
    lv_coord_t cir_w = lv_obj_get_style_arc_width(obj, LV_PART_MAIN);

#if LV_DRAW_COMPLEX
    /*Mask outer and inner ring of widget to tidy up ragged edges of lines while drawing outer ring*/
    lv_draw_mask_radius_param_t mask_out_param;
    lv_draw_mask_radius_init(&mask_out_param, &obj->coords, LV_RADIUS_CIRCLE, false);
    int16_t mask_out_id = lv_draw_mask_add(&mask_out_param, 0);

    lv_area_t mask_area;
    lv_area_copy(&mask_area, &obj->coords);
    mask_area.x1 += cir_w;
    mask_area.x2 -= cir_w;
    mask_area.y1 += cir_w;
    mask_area.y2 -= cir_w;
    lv_draw_mask_radius_param_t mask_in_param;
    lv_draw_mask_radius_init(&mask_in_param, &mask_area, LV_RADIUS_CIRCLE, true);
    int16_t mask_in_id = lv_draw_mask_add(&mask_in_param, 0);

    /*The inner and outer line ends will be masked out.
     *So make lines a little bit longer because the masking makes a more even result*/
    lv_coord_t cir_w_extra = line_dsc.width;
#else
    lv_coord_t cir_w_extra = 0;
#endif

    for(i = 0; i <= 256; i += LV_CPICKER_DEF_QF, a += 360 * LV_CPICKER_DEF_QF) {
        line_dsc.color = angle_to_mode_color_fast(obj, i);
        uint16_t angle_trigo = (uint16_t)(a >> 8); /*i * 360 / 256 is the scale to apply, but we can skip multiplication here*/

        lv_point_t p[2];
        p[0].x = cx + ((r + cir_w_extra) * lv_trigo_sin(angle_trigo) >> LV_TRIGO_SHIFT);
        p[0].y = cy + ((r + cir_w_extra) * lv_trigo_cos(angle_trigo) >> LV_TRIGO_SHIFT);
        p[1].x = cx + ((r - cir_w - cir_w_extra) * lv_trigo_sin(angle_trigo) >> LV_TRIGO_SHIFT);
        p[1].y = cy + ((r - cir_w - cir_w_extra) * lv_trigo_cos(angle_trigo) >> LV_TRIGO_SHIFT);

        lv_draw_line(&p[0], &p[1], clip_area, &line_dsc);
    }

#if LV_DRAW_COMPLEX
    lv_draw_mask_free_param(&mask_out_param);
    lv_draw_mask_free_param(&mask_in_param);
    lv_draw_mask_remove_id(mask_out_id);
    lv_draw_mask_remove_id(mask_in_id);
#endif
}

static void draw_knob(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    const lv_area_t * clip_area = lv_event_get_param(e);
    lv_colorwheel_t * colorwheel = (lv_colorwheel_t *)obj;

    lv_draw_rect_dsc_t cir_dsc;
    lv_draw_rect_dsc_init(&cir_dsc);
    lv_obj_init_draw_rect_dsc(obj, LV_PART_KNOB, &cir_dsc);

    cir_dsc.radius = LV_RADIUS_CIRCLE;

    if(colorwheel->knob.recolor) {
        cir_dsc.bg_color = lv_colorwheel_get_rgb(obj);
    }

    lv_area_t knob_area = get_knob_area(obj);

    lv_draw_rect(&knob_area, clip_area, &cir_dsc);
}

static void invalidate_knob(lv_obj_t * obj)
{
    lv_area_t knob_area = get_knob_area(obj);

    lv_obj_invalidate_area(obj, &knob_area);
}

static lv_area_t get_knob_area(lv_obj_t * obj)
{
    lv_colorwheel_t * colorwheel = (lv_colorwheel_t *)obj;

    /*Get knob's radius*/
    uint16_t r = 0;
    r = lv_obj_get_style_arc_width(obj, LV_PART_MAIN) / 2;

    lv_coord_t left = lv_obj_get_style_pad_left(obj, LV_PART_KNOB);
    lv_coord_t right = lv_obj_get_style_pad_right(obj, LV_PART_KNOB);
    lv_coord_t top = lv_obj_get_style_pad_top(obj, LV_PART_KNOB);
    lv_coord_t bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_KNOB);

    lv_area_t knob_area;
    knob_area.x1 = obj->coords.x1 + colorwheel->knob.pos.x - r - left;
    knob_area.y1 = obj->coords.y1 + colorwheel->knob.pos.y - r - right;
    knob_area.x2 = obj->coords.x1 + colorwheel->knob.pos.x + r + top;
    knob_area.y2 = obj->coords.y1 + colorwheel->knob.pos.y + r + bottom;

    return knob_area;
}

static void lv_colorwheel_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    /*Call the ancestor's event handler*/
    lv_res_t res = lv_obj_event_base(MY_CLASS, e);

    if(res != LV_RES_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    lv_colorwheel_t * colorwheel = (lv_colorwheel_t *)obj;

    if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        lv_coord_t left = lv_obj_get_style_pad_left(obj, LV_PART_KNOB);
        lv_coord_t right = lv_obj_get_style_pad_right(obj, LV_PART_KNOB);
        lv_coord_t top = lv_obj_get_style_pad_top(obj, LV_PART_KNOB);
        lv_coord_t bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_KNOB);

        lv_coord_t knob_pad = LV_MAX4(left, right, top, bottom) + 2;
        lv_coord_t * s = lv_event_get_param(e);
        *s = LV_MAX(*s, knob_pad);
    }
    else if(code == LV_EVENT_SIZE_CHANGED) {
        void * param = lv_event_get_param(e);
        /*Refresh extended draw area to make knob visible*/
        if(lv_obj_get_width(obj) != lv_area_get_width(param) ||
           lv_obj_get_height(obj) != lv_area_get_height(param)) {
            refr_knob_pos(obj);
        }
    }
    else if(code == LV_EVENT_STYLE_CHANGED) {
        /*Refresh extended draw area to make knob visible*/
        refr_knob_pos(obj);
    }
    else if(code == LV_EVENT_KEY) {
        uint32_t c = *((uint32_t *)lv_event_get_param(e)); /*uint32_t because can be UTF-8*/

        if(c == LV_KEY_RIGHT || c == LV_KEY_UP) {
            lv_color_hsv_t hsv_cur;
            hsv_cur = colorwheel->hsv;

            switch(colorwheel->mode) {
                case LV_COLORWHEEL_MODE_HUE:
                    hsv_cur.h = (colorwheel->hsv.h + 1) % 360;
                    break;
                case LV_COLORWHEEL_MODE_SATURATION:
                    hsv_cur.s = (colorwheel->hsv.s + 1) % 100;
                    break;
                case LV_COLORWHEEL_MODE_VALUE:
                    hsv_cur.v = (colorwheel->hsv.v + 1) % 100;
                    break;
            }

            if(lv_colorwheel_set_hsv(obj, hsv_cur)) {
                res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
                if(res != LV_RES_OK) return;
            }
        }
        else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) {
            lv_color_hsv_t hsv_cur;
            hsv_cur = colorwheel->hsv;

            switch(colorwheel->mode) {
                case LV_COLORWHEEL_MODE_HUE:
                    hsv_cur.h = colorwheel->hsv.h > 0 ? (colorwheel->hsv.h - 1) : 360;
                    break;
                case LV_COLORWHEEL_MODE_SATURATION:
                    hsv_cur.s = colorwheel->hsv.s > 0 ? (colorwheel->hsv.s - 1) : 100;
                    break;
                case LV_COLORWHEEL_MODE_VALUE:
                    hsv_cur.v = colorwheel->hsv.v > 0 ? (colorwheel->hsv.v - 1) : 100;
                    break;
            }

            if(lv_colorwheel_set_hsv(obj, hsv_cur)) {
                res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
                if(res != LV_RES_OK) return;
            }
        }
    }
    else if(code == LV_EVENT_PRESSED) {
        colorwheel->last_change_time = lv_tick_get();
        lv_indev_get_point(lv_indev_get_act(), &colorwheel->last_press_point);
        res = double_click_reset(obj);
        if(res != LV_RES_OK) return;
    }
    else if(code == LV_EVENT_PRESSING) {
        lv_indev_t * indev = lv_indev_get_act();
        if(indev == NULL) return;

        lv_indev_type_t indev_type = lv_indev_get_type(indev);
        lv_point_t p;
        if(indev_type == LV_INDEV_TYPE_ENCODER || indev_type == LV_INDEV_TYPE_KEYPAD) {
            p.x = obj->coords.x1 + lv_obj_get_width(obj) / 2;
            p.y = obj->coords.y1 + lv_obj_get_height(obj) / 2;
        }
        else {
            lv_indev_get_point(indev, &p);
        }

        lv_coord_t drag_limit = indev->driver->scroll_limit;
        if((LV_ABS(p.x - colorwheel->last_press_point.x) > drag_limit) ||
           (LV_ABS(p.y - colorwheel->last_press_point.y) > drag_limit)) {
            colorwheel->last_change_time = lv_tick_get();
            colorwheel->last_press_point.x = p.x;
            colorwheel->last_press_point.y = p.y;
        }

        p.x -= obj->coords.x1;
        p.y -= obj->coords.y1;

        /*Ignore pressing in the inner area*/
        uint16_t w = lv_obj_get_width(obj);

        int16_t angle = 0;
        lv_coord_t cir_w = lv_obj_get_style_arc_width(obj, LV_PART_MAIN);

        lv_coord_t r_in = w / 2;
        p.x -= r_in;
        p.y -= r_in;
        bool on_ring = true;
        r_in -= cir_w;
        if(r_in > LV_DPI_DEF / 2) {
            lv_coord_t inner = cir_w / 2;
            r_in -= inner;

            if(r_in < LV_DPI_DEF / 2) r_in = LV_DPI_DEF / 2;
        }

        if(p.x * p.x + p.y * p.y < r_in * r_in) {
            on_ring = false;
        }

        /*If the inner area is being pressed, go to the next color mode on long press*/
        uint32_t diff = lv_tick_elaps(colorwheel->last_change_time);
        if(!on_ring && diff > indev->driver->long_press_time && !colorwheel->mode_fixed) {
            next_color_mode(obj);
            lv_indev_wait_release(lv_indev_get_act());
            return;
        }

        /*Set the angle only if pressed on the ring*/
        if(!on_ring) return;

        angle = lv_atan2(p.x, p.y) % 360;

        lv_color_hsv_t hsv_cur;
        hsv_cur = colorwheel->hsv;

        switch(colorwheel->mode) {
            case LV_COLORWHEEL_MODE_HUE:
                hsv_cur.h = angle;
                break;
            case LV_COLORWHEEL_MODE_SATURATION:
                hsv_cur.s = (angle * 100) / 360;
                break;
            case LV_COLORWHEEL_MODE_VALUE:
                hsv_cur.v = (angle * 100) / 360;
                break;
        }

        if(lv_colorwheel_set_hsv(obj, hsv_cur)) {
            res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return;
        }
    }
    else if(code == LV_EVENT_HIT_TEST) {
        lv_hit_test_info_t * info = lv_event_get_param(e);;

        /*Valid clicks can be only in the circle*/
        info->res = _lv_area_is_point_on(&obj->coords, info->point, LV_RADIUS_CIRCLE);
    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        draw_disc_grad(e);
        draw_knob(e);
    }
    else if(code == LV_EVENT_COVER_CHECK) {
        lv_cover_check_info_t * info = lv_event_get_param(e);
        if(info->res != LV_COVER_RES_MASKED) info->res = LV_COVER_RES_NOT_COVER;
    }
}



static void next_color_mode(lv_obj_t * obj)
{
    lv_colorwheel_t * colorwheel = (lv_colorwheel_t *)obj;
    colorwheel->mode = (colorwheel->mode + 1) % 3;
    refr_knob_pos(obj);
    lv_obj_invalidate(obj);
}

static void refr_knob_pos(lv_obj_t * obj)
{
    invalidate_knob(obj);

    lv_colorwheel_t * colorwheel = (lv_colorwheel_t *)obj;
    lv_coord_t w = lv_obj_get_width(obj);

    lv_coord_t scale_w = lv_obj_get_style_arc_width(obj, LV_PART_MAIN);
    lv_coord_t r = (w - scale_w) / 2;
    uint16_t angle = get_angle(obj);
    colorwheel->knob.pos.x = (((int32_t)r * lv_trigo_sin(angle)) >> LV_TRIGO_SHIFT);
    colorwheel->knob.pos.y = (((int32_t)r * lv_trigo_cos(angle)) >> LV_TRIGO_SHIFT);
    colorwheel->knob.pos.x = colorwheel->knob.pos.x + w / 2;
    colorwheel->knob.pos.y = colorwheel->knob.pos.y + w / 2;

    invalidate_knob(obj);
}

static lv_res_t double_click_reset(lv_obj_t * obj)
{
    lv_colorwheel_t * colorwheel = (lv_colorwheel_t *)obj;
    lv_indev_t * indev = lv_indev_get_act();
    /*Double clicked? Use long press time as double click time out*/
    if(lv_tick_elaps(colorwheel->last_click_time) < indev->driver->long_press_time) {
        lv_color_hsv_t hsv_cur;
        hsv_cur = colorwheel->hsv;

        switch(colorwheel->mode) {
            case LV_COLORWHEEL_MODE_HUE:
                hsv_cur.h = 0;
                break;
            case LV_COLORWHEEL_MODE_SATURATION:
                hsv_cur.s = 100;
                break;
            case LV_COLORWHEEL_MODE_VALUE:
                hsv_cur.v = 100;
                break;
        }

        lv_indev_wait_release(indev);

        if(lv_colorwheel_set_hsv(obj, hsv_cur)) {
            lv_res_t res = lv_event_send(obj, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
    }
    colorwheel->last_click_time = lv_tick_get();

    return LV_RES_OK;
}

#define SWAPPTR(A, B) do { uint8_t * t = A; A = B; B = t; } while(0)
#define HSV_PTR_SWAP(sextant,r,g,b)     if((sextant) & 2) { SWAPPTR((r), (b)); } if((sextant) & 4) { SWAPPTR((g), (b)); } if(!((sextant) & 6)) { \
        if(!((sextant) & 1)) { SWAPPTR((r), (g)); } } else { if((sextant) & 1) { SWAPPTR((r), (g)); } }

/**
 * Based on the idea from https://www.vagrearg.org/content/hsvrgb
 * Here we want to compute an approximate RGB value from a HSV input color space. We don't want to be accurate
 * (for that, there's lv_color_hsv_to_rgb), but we want to be fast.
 *
 * Few tricks are used here: Hue is in range [0; 6 * 256] (so that the sextant is in the high byte and the fractional part is in the low byte)
 * both s and v are in [0; 255] range (very convenient to avoid divisions).
 *
 * We fold all symmetry by swapping the R, G, B pointers so that the code is the same for all sextants.
 * We replace division by 255 by a division by 256, a.k.a a shift right by 8 bits.
 * This is wrong, but since this is only used to compute the pixels on the screen and not the final color, it's ok.
 */
static void fast_hsv2rgb(uint16_t h, uint8_t s, uint8_t v, uint8_t * r, uint8_t * g, uint8_t * b);
static void fast_hsv2rgb(uint16_t h, uint8_t s, uint8_t v, uint8_t * r, uint8_t * g, uint8_t * b)
{
    if(!s) {
        *r = *g = *b = v;
        return;
    }

    uint8_t sextant = h >> 8;
    HSV_PTR_SWAP(sextant, r, g, b); /*Swap pointers so the conversion code is the same*/

    *g = v;

    uint8_t bb = ~s;
    uint16_t ww = v * bb; /*Don't try to be precise, but instead, be fast*/
    *b = ww >> 8;

    uint8_t h_frac = h & 0xff;

    if(!(sextant & 1)) {
        /*Up slope*/
        ww = !h_frac ? ((uint16_t)s << 8) : (s * (uint8_t)(-h_frac)); /*Skip multiply if not required*/
    }
    else {
        /*Down slope*/
        ww = s * h_frac;
    }
    bb = ww >> 8;
    bb = ~bb;
    ww = v * bb;
    *r = ww >> 8;
}

static lv_color_t angle_to_mode_color_fast(lv_obj_t * obj, uint16_t angle)
{
    lv_colorwheel_t * ext = (lv_colorwheel_t *)obj;
    uint8_t r = 0, g = 0, b = 0;
    static uint16_t h = 0;
    static uint8_t s = 0, v = 0, m = 255;

    switch(ext->mode) {
        default:
        case LV_COLORWHEEL_MODE_HUE:
            /*Don't recompute costly scaling if it does not change*/
            if(m != ext->mode) {
                s = (uint8_t)(((uint16_t)ext->hsv.s * 51) / 20);
                v = (uint8_t)(((uint16_t)ext->hsv.v * 51) / 20);
                m = ext->mode;
            }
            fast_hsv2rgb(angle * 6, s, v, &r, &g,
                         &b); /*A smart compiler will replace x * 6 by (x << 2) + (x << 1) if it's more efficient*/
            break;
        case LV_COLORWHEEL_MODE_SATURATION:
            /*Don't recompute costly scaling if it does not change*/
            if(m != ext->mode) {
                h = (uint16_t)(((uint32_t)ext->hsv.h * 6 * 256) / 360);
                v = (uint8_t)(((uint16_t)ext->hsv.v * 51) / 20);
                m = ext->mode;
            }
            fast_hsv2rgb(h, angle, v, &r, &g, &b);
            break;
        case LV_COLORWHEEL_MODE_VALUE:
            /*Don't recompute costly scaling if it does not change*/
            if(m != ext->mode) {
                h = (uint16_t)(((uint32_t)ext->hsv.h * 6 * 256) / 360);
                s = (uint8_t)(((uint16_t)ext->hsv.s * 51) / 20);
                m = ext->mode;
            }
            fast_hsv2rgb(h, s, angle, &r, &g, &b);
            break;
    }
    return lv_color_make(r, g, b);
}

static uint16_t get_angle(lv_obj_t * obj)
{
    lv_colorwheel_t * colorwheel = (lv_colorwheel_t *)obj;
    uint16_t angle;
    switch(colorwheel->mode) {
        default:
        case LV_COLORWHEEL_MODE_HUE:
            angle = colorwheel->hsv.h;
            break;
        case LV_COLORWHEEL_MODE_SATURATION:
            angle = (colorwheel->hsv.s * 360) / 100;
            break;
        case LV_COLORWHEEL_MODE_VALUE:
            angle = (colorwheel->hsv.v * 360) / 100 ;
            break;
    }
    return angle;
}

#endif /*LV_USE_COLORWHEEL*/
