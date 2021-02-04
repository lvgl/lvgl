/**
 * @file lv_colorwheel.c
 *
 * Based on the work of   @AloyseTech and @paulpv.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_colorwheel.h"
#if LV_USE_COLORWHEEL

/*********************
 *      DEFINES
 *********************/

#define LV_CPICKER_DEF_QF 3

/* The OUTER_MASK_WIDTH define is required to assist with the placing of a mask over the outer ring of the widget as when the
 * multicoloured radial lines are calculated for the outer ring of the widget their lengths are jittering because of the
 * integer based arithmetic. From tests the maximum delta was found to be 2 so the current value is set to 3 to achieve
 * appropriate masking.
 */
#define OUTER_MASK_WIDTH 3

#define DRAG_LIMIT (LV_DPI / 10)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_draw_res_t lv_colorwheel_draw(lv_obj_t * colorwheel, const lv_area_t * clip_area, lv_draw_mode_t mode);
static lv_res_t lv_colorwheel_signal(lv_obj_t * colorwheel, lv_signal_t sign, void * param);
static lv_style_list_t * lv_colorwheel_get_style(lv_obj_t * colorwheel, uint8_t part);
static bool lv_colorwheel_hit(lv_obj_t * colorwheel, const lv_point_t * p);

static void draw_disc_grad(lv_obj_t * colorwheel, const lv_area_t * mask);
static void draw_knob(lv_obj_t * colorwheel, const lv_area_t * mask);
static void invalidate_knob(lv_obj_t * colorwheel);
static lv_area_t get_knob_area(lv_obj_t * colorwheel);

static void next_color_mode(lv_obj_t * colorwheel);
static lv_res_t double_click_reset(lv_obj_t * colorwheel);
static void refr_knob_pos(lv_obj_t * colorwheel);
static lv_color_t angle_to_mode_color(lv_obj_t * colorwheel, uint16_t angle);
static uint16_t get_angle(lv_obj_t * colorwheel);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;
static lv_draw_cb_t ancestor_draw;
static bool inited;

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
lv_obj_t * lv_colorwheel_create(lv_obj_t * parent, bool knob_recolor)
{
    static lv_style_t style_knob;
    if(!inited) {
        lv_style_init(&style_knob);
        lv_style_set_bg_color(&style_knob, LV_STATE_DEFAULT, LV_COLOR_GRAY);
        lv_style_set_border_color(&style_knob, LV_STATE_DEFAULT, LV_COLOR_WHITE);
        lv_style_set_border_width(&style_knob, LV_STATE_DEFAULT, LV_DPX(1));
        lv_style_set_bg_opa(&style_knob, LV_STATE_DEFAULT, LV_OPA_COVER);
        lv_style_set_radius(&style_knob, LV_STATE_DEFAULT, LV_RADIUS_CIRCLE);
        lv_style_set_pad_all(&style_knob, LV_STATE_DEFAULT, LV_DPX(5));
        inited = true;
    }
    lv_obj_t * colorwheel = lv_obj_create(parent, NULL);
    LV_ASSERT_MEM(colorwheel);
    if(colorwheel == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(colorwheel);
    if(ancestor_draw == NULL) ancestor_draw = lv_obj_get_draw_cb(colorwheel);

    /*Allocate the extended data*/
    lv_colorwheel_ext_t * ext = lv_obj_allocate_ext_attr(colorwheel, sizeof(lv_colorwheel_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(colorwheel);
        return NULL;
    }

    /*Initialize the allocated 'ext' */
    ext->hsv.h = 0;
    ext->hsv.s = 100;
    ext->hsv.v = 100;
    ext->mode = LV_COLORWHEEL_MODE_HUE;
    ext->mode_fixed = 0;
    ext->last_click_time = 0;
    ext->last_change_time = 0;
    ext->knob.recolor = knob_recolor;

    lv_style_list_init(&ext->knob.style_list);

    /*The signal and draw functions are not copied so set them here*/
    lv_obj_set_signal_cb(colorwheel, lv_colorwheel_signal);
    lv_obj_set_draw_cb(colorwheel, lv_colorwheel_draw);

    lv_obj_set_size(colorwheel, LV_DPI * 2, LV_DPI * 2);
    lv_obj_add_flag(colorwheel, LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_add_style(colorwheel, LV_COLORWHEEL_PART_KNOB, &style_knob);
    refr_knob_pos(colorwheel);

    return colorwheel;
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
bool lv_colorwheel_set_hsv(lv_obj_t * colorwheel, lv_color_hsv_t hsv)
{
    if(hsv.h > 360) hsv.h %= 360;
    if(hsv.s > 100) hsv.s = 100;
    if(hsv.v > 100) hsv.v = 100;

    lv_colorwheel_ext_t * ext = lv_obj_get_ext_attr(colorwheel);

    if(ext->hsv.h == hsv.h && ext->hsv.s == hsv.s && ext->hsv.v == hsv.v) return false;

    ext->hsv = hsv;

    refr_knob_pos(colorwheel);

    lv_obj_invalidate(colorwheel);

    return true;
}

/**
 * Set the current color of a color wheel.
 * @param colorwheel pointer to color wheel object
 * @param color current selected color
 * @return true if changed, otherwise false
 */
bool lv_colorwheel_set_rgb(lv_obj_t * colorwheel, lv_color_t color)
{
    lv_color32_t c32;
    c32.full = lv_color_to32(color);

    return lv_colorwheel_set_hsv(colorwheel,
                              lv_color_rgb_to_hsv(c32.ch.red, c32.ch.green, c32.ch.blue));
}

/**
 * Set the current color mode.
 * @param colorwheel pointer to color wheel object
 * @param mode color mode (hue/sat/val)
 */
void lv_colorwheel_set_mode(lv_obj_t * colorwheel, lv_colorwheel_mode_t mode)
{
    lv_colorwheel_ext_t * ext = lv_obj_get_ext_attr(colorwheel);

    ext->mode = mode;
    refr_knob_pos(colorwheel);
    lv_obj_invalidate(colorwheel);
}

/**
 * Set if the color mode is changed on long press on center
 * @param colorwheel pointer to color wheel object
 * @param fixed color mode cannot be changed on long press
 */
void lv_colorwheel_set_mode_fixed(lv_obj_t * colorwheel, bool fixed)
{
    lv_colorwheel_ext_t * ext = lv_obj_get_ext_attr(colorwheel);

    ext->mode_fixed = fixed;
}

/*=====================
 * Getter functions
 *====================*/


/**
 * Get the current selected hsv of a color wheel.
 * @param colorwheel pointer to color wheel object
 * @return current selected hsv
 */
lv_color_hsv_t lv_colorwheel_get_hsv(lv_obj_t * colorwheel)
{
    lv_colorwheel_ext_t * ext = lv_obj_get_ext_attr(colorwheel);

    return ext->hsv;
}

/**
 * Get the current selected color of a color wheel.
 * @param colorwheel pointer to color wheel object
 * @return color current selected color
 */
lv_color_t lv_colorwheel_get_rgb(lv_obj_t * colorwheel)
{
    lv_colorwheel_ext_t * ext = lv_obj_get_ext_attr(colorwheel);

    return lv_color_hsv_to_rgb(ext->hsv.h, ext->hsv.s, ext->hsv.v);
}

/**
 * Get the current color mode.
 * @param colorwheel pointer to color wheel object
 * @return color mode (hue/sat/val)
 */
lv_colorwheel_mode_t lv_colorwheel_get_color_mode(lv_obj_t * colorwheel)
{
    lv_colorwheel_ext_t * ext = lv_obj_get_ext_attr(colorwheel);

    return ext->mode;
}

/**
 * Get if the color mode is changed on long press on center
 * @param colorwheel pointer to color wheel object
 * @return mode cannot be changed on long press
 */
bool lv_colorwheel_get_color_mode_fixed(lv_obj_t * colorwheel)
{
    lv_colorwheel_ext_t * ext = lv_obj_get_ext_attr(colorwheel);

    return ext->mode_fixed;
}

/*=====================
 * Other functions
 *====================*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the color_picker
 * @param colorwheel pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DRAW_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DRAW_DRAW: draw the object (always return 'true')
 *             LV_DRAW_DRAW_POST: drawing after every children are drawn
 * @return return an element of `lv_draw_res_t`
 */
static lv_draw_res_t lv_colorwheel_draw(lv_obj_t * colorwheel, const lv_area_t * clip_area, lv_draw_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DRAW_COVER_CHK)  {
        return LV_DRAW_RES_NOT_COVER;
    }
    /*Draw the object*/
    else if(mode == LV_DRAW_DRAW_MAIN) {
        draw_disc_grad(colorwheel, clip_area);
        draw_knob(colorwheel, clip_area);
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DRAW_DRAW_POST) {
    }

    return LV_DRAW_RES_OK;
}

static void draw_disc_grad(lv_obj_t * colorwheel, const lv_area_t * mask)
{
    lv_coord_t w = lv_obj_get_width(colorwheel);
    lv_coord_t h = lv_obj_get_height(colorwheel);
    lv_coord_t cx = colorwheel->coords.x1 + w / 2;
    lv_coord_t cy = colorwheel->coords.y1 + h / 2;
    lv_coord_t r = w / 2;

    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    lv_obj_init_draw_line_dsc(colorwheel, LV_COLORWHEEL_PART_MAIN, &line_dsc);

    line_dsc.width = (r * 628 / (360 / LV_CPICKER_DEF_QF)) / 100;
    line_dsc.width += 2;
    uint16_t i;
    lv_coord_t cir_w = lv_obj_get_style_scale_width(colorwheel, LV_COLORWHEEL_PART_MAIN);

    /* Mask outer ring of widget to tidy up ragged edges of lines while drawing outer ring */
    lv_area_t mask_area_out;
    lv_area_copy(&mask_area_out, &colorwheel->coords);
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
        line_dsc.color = angle_to_mode_color(colorwheel, i);

        lv_point_t p[2];
        p[0].x = cx + (r * lv_trigo_sin(i) >> LV_TRIGO_SHIFT);
        p[0].y = cy + (r * lv_trigo_sin(i + 90) >> LV_TRIGO_SHIFT);
        p[1].x = cx + ((r - cir_w_extra) * lv_trigo_sin(i) >> LV_TRIGO_SHIFT);
        p[1].y = cy + ((r - cir_w_extra) * lv_trigo_sin(i + 90) >> LV_TRIGO_SHIFT);

        lv_draw_line(&p[0], &p[1], mask, &line_dsc);
    }
    /* Now remove mask to continue with inner part */
    lv_draw_mask_remove_id(mask_out_id);

    /*Mask out the inner area*/
    lv_draw_rect_dsc_t bg_dsc;
    lv_draw_rect_dsc_init(&bg_dsc);
    lv_obj_init_draw_rect_dsc(colorwheel, LV_COLORWHEEL_PART_MAIN, &bg_dsc);
    bg_dsc.radius = LV_RADIUS_CIRCLE;

    lv_area_t area_mid;
    lv_area_copy(&area_mid, &colorwheel->coords);
    area_mid.x1 += cir_w;
    area_mid.y1 += cir_w;
    area_mid.x2 -= cir_w;
    area_mid.y2 -= cir_w;

    lv_draw_rect(&area_mid, mask, &bg_dsc);

    lv_coord_t inner = cir_w / 2;
    lv_color_t color = lv_colorwheel_get_rgb(colorwheel);
    bg_dsc.bg_color = color;
    area_mid.x1 += inner;
    area_mid.y1 += inner;
    area_mid.x2 -= inner;
    area_mid.y2 -= inner;

    lv_draw_rect(&area_mid, mask, &bg_dsc);
}

static void draw_knob(lv_obj_t * colorwheel, const lv_area_t * mask)
{
    lv_colorwheel_ext_t * ext = lv_obj_get_ext_attr(colorwheel);

    lv_draw_rect_dsc_t cir_dsc;
    lv_draw_rect_dsc_init(&cir_dsc);
    lv_obj_init_draw_rect_dsc(colorwheel, LV_COLORWHEEL_PART_KNOB, &cir_dsc);

    cir_dsc.radius = LV_RADIUS_CIRCLE;

    if(ext->knob.recolor) {
        cir_dsc.bg_color = lv_colorwheel_get_rgb(colorwheel);
    }

    lv_area_t knob_area = get_knob_area(colorwheel);

    lv_draw_rect(&knob_area, mask, &cir_dsc);
}

static void invalidate_knob(lv_obj_t * colorwheel)
{
    lv_area_t knob_area = get_knob_area(colorwheel);

    lv_obj_invalidate_area(colorwheel, &knob_area);
}

static lv_area_t get_knob_area(lv_obj_t * colorwheel)
{
    lv_colorwheel_ext_t * ext = lv_obj_get_ext_attr(colorwheel);

    /*Get knob's radius*/
    uint16_t r = 0;
    r = lv_obj_get_style_scale_width(colorwheel, LV_COLORWHEEL_PART_MAIN) / 2;

    lv_coord_t left = lv_obj_get_style_pad_left(colorwheel, LV_COLORWHEEL_PART_KNOB);
    lv_coord_t right = lv_obj_get_style_pad_right(colorwheel, LV_COLORWHEEL_PART_KNOB);
    lv_coord_t top = lv_obj_get_style_pad_top(colorwheel, LV_COLORWHEEL_PART_KNOB);
    lv_coord_t bottom = lv_obj_get_style_pad_bottom(colorwheel, LV_COLORWHEEL_PART_KNOB);

    lv_area_t knob_area;
    knob_area.x1 = colorwheel->coords.x1 + ext->knob.pos.x - r - left;
    knob_area.y1 = colorwheel->coords.y1 + ext->knob.pos.y - r - right;
    knob_area.x2 = colorwheel->coords.x1 + ext->knob.pos.x + r + top;
    knob_area.y2 = colorwheel->coords.y1 + ext->knob.pos.y + r + bottom;

    return knob_area;
}

/**
 * Signal function of the color_picker
 * @param colorwheel pointer to a color_picker object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_colorwheel_signal(lv_obj_t * colorwheel, lv_signal_t sign, void * param)
{
    /* Include the ancient signal function */
    lv_res_t res;

    if(sign == LV_SIGNAL_GET_STYLE) {
        lv_get_style_info_t * info = param;
        info->result = lv_colorwheel_get_style(colorwheel, info->part);
        if(info->result != NULL) return LV_RES_OK;
        else return ancestor_signal(colorwheel, sign, param);
    }

    res = ancestor_signal(colorwheel, sign, param);
    if(res != LV_RES_OK) return res;

    lv_colorwheel_ext_t * ext = lv_obj_get_ext_attr(colorwheel);

    if(sign == LV_SIGNAL_CLEANUP) {
        _lv_obj_reset_style_list_no_refr(colorwheel, LV_COLORWHEEL_PART_KNOB);
    }
    else if(sign == LV_SIGNAL_REFR_EXT_DRAW_PAD) {
        lv_coord_t left = lv_obj_get_style_pad_left(colorwheel, LV_COLORWHEEL_PART_KNOB);
        lv_coord_t right = lv_obj_get_style_pad_right(colorwheel, LV_COLORWHEEL_PART_KNOB);
        lv_coord_t top = lv_obj_get_style_pad_top(colorwheel, LV_COLORWHEEL_PART_KNOB);
        lv_coord_t bottom = lv_obj_get_style_pad_bottom(colorwheel, LV_COLORWHEEL_PART_KNOB);

        lv_coord_t knob_pad = LV_MAX4(left, right, top, bottom) + 2;
        lv_coord_t * s = param;
        *s = LV_MAX(*s, knob_pad);
    }
    else if(sign == LV_SIGNAL_COORD_CHG) {
        /*Refresh extended draw area to make knob visible*/
        if(lv_obj_get_width(colorwheel) != lv_area_get_width(param) ||
           lv_obj_get_height(colorwheel) != lv_area_get_height(param)) {
            refr_knob_pos(colorwheel);
        }
    }
    else if(sign == LV_SIGNAL_STYLE_CHG) {
        /*Refresh extended draw area to make knob visible*/
        refr_knob_pos(colorwheel);
    }
    else if(sign == LV_SIGNAL_CONTROL) {
#if LV_USE_GROUP
        uint32_t c = *((uint32_t *)param); /*uint32_t because can be UTF-8*/

        if(c == LV_KEY_RIGHT || c == LV_KEY_UP) {
            lv_color_hsv_t hsv_cur;
            hsv_cur = ext->hsv;

            switch(ext->mode) {
                case LV_COLORWHEEL_MODE_HUE:
                    hsv_cur.h = (ext->hsv.h + 1) % 360;
                    break;
                case LV_COLORWHEEL_MODE_SATURATION:
                    hsv_cur.s = (ext->hsv.s + 1) % 100;
                    break;
                case LV_COLORWHEEL_MODE_VALUE:
                    hsv_cur.v = (ext->hsv.v + 1) % 100;
                    break;
            }

            if(lv_colorwheel_set_hsv(colorwheel, hsv_cur)) {
                res = lv_event_send(colorwheel, LV_EVENT_VALUE_CHANGED, NULL);
                if(res != LV_RES_OK) return res;
            }
        }
        else if(c == LV_KEY_LEFT || c == LV_KEY_DOWN) {
            lv_color_hsv_t hsv_cur;
            hsv_cur = ext->hsv;

            switch(ext->mode) {
                case LV_COLORWHEEL_MODE_HUE:
                    hsv_cur.h = ext->hsv.h > 0 ? (ext->hsv.h - 1) : 360;
                    break;
                case LV_COLORWHEEL_MODE_SATURATION:
                    hsv_cur.s = ext->hsv.s > 0 ? (ext->hsv.s - 1) : 100;
                    break;
                case LV_COLORWHEEL_MODE_VALUE:
                    hsv_cur.v = ext->hsv.v > 0 ? (ext->hsv.v - 1) : 100;
                    break;
            }

            if(lv_colorwheel_set_hsv(colorwheel, hsv_cur)) {
                res = lv_event_send(colorwheel, LV_EVENT_VALUE_CHANGED, NULL);
                if(res != LV_RES_OK) return res;
            }
        }
#endif
    }
    else if(sign == LV_SIGNAL_PRESSED) {
        ext->last_change_time = lv_tick_get();
        lv_indev_get_point(lv_indev_get_act(), &ext->last_press_point);
        res = double_click_reset(colorwheel);
        if(res != LV_RES_OK) return res;
    }
    else if(sign == LV_SIGNAL_PRESSING) {
        lv_indev_t * indev = lv_indev_get_act();
        if(indev == NULL) return res;

        lv_indev_type_t indev_type = lv_indev_get_type(indev);
        lv_point_t p;
        if(indev_type == LV_INDEV_TYPE_ENCODER || indev_type == LV_INDEV_TYPE_KEYPAD) {
            p.x = colorwheel->coords.x1 + lv_obj_get_width(colorwheel) / 2;
            p.y = colorwheel->coords.y1 + lv_obj_get_height(colorwheel) / 2;
        }
        else {
            lv_indev_get_point(indev, &p);
        }

        if((LV_ABS(p.x - ext->last_press_point.x) > DRAG_LIMIT) ||
           (LV_ABS(p.y - ext->last_press_point.y) > DRAG_LIMIT)) {
            ext->last_change_time = lv_tick_get();
            ext->last_press_point.x = p.x;
            ext->last_press_point.y = p.y;
        }

        p.x -= colorwheel->coords.x1;
        p.y -= colorwheel->coords.y1;

        /*Ignore pressing in the inner area*/
        uint16_t w = lv_obj_get_width(colorwheel);

        int16_t angle = 0;
        lv_coord_t cir_w = lv_obj_get_style_scale_width(colorwheel, LV_COLORWHEEL_PART_MAIN);

        lv_coord_t r_in = w / 2;
        p.x -= r_in;
        p.y -= r_in;
        bool on_ring = true;
        r_in -= cir_w;
        if(r_in > LV_DPI / 2) {
            lv_coord_t inner = cir_w / 2;
            r_in -= inner;

            if(r_in < LV_DPI / 2) r_in = LV_DPI / 2;
        }

        if(p.x * p.x + p.y * p.y < r_in * r_in) {
            on_ring = false;
        }

        /*If the inner area is being pressed, go to the next color mode on long press*/
        uint32_t diff = lv_tick_elaps(ext->last_change_time);
        if(!on_ring && diff > indev->driver.long_press_time && !ext->mode_fixed) {
            next_color_mode(colorwheel);
            lv_indev_wait_release(lv_indev_get_act());
            return res;
        }

        /*Set the angle only if pressed on the ring*/
        if(!on_ring) return res;

        angle = lv_atan2(p.x, p.y) % 360;

        lv_color_hsv_t hsv_cur;
        hsv_cur = ext->hsv;

        switch(ext->mode) {
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

        if(lv_colorwheel_set_hsv(colorwheel, hsv_cur)) {
            res = lv_event_send(colorwheel, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
    }
    else if(sign == LV_SIGNAL_HIT_TEST) {
        lv_hit_test_info_t * info = param;
        info->result = lv_colorwheel_hit(colorwheel, info->point);
    }

    return res;
}


/**
 * Get the style_list descriptor of a part of the object
 * @param colorwheel pointer the object
 * @param part the part of the colorwheel. (LV_PAGE_CPICKER_...)
 * @return pointer to the style_list descriptor of the specified part
 */
static lv_style_list_t * lv_colorwheel_get_style(lv_obj_t * colorwheel, uint8_t part)
{
    lv_colorwheel_ext_t * ext = lv_obj_get_ext_attr(colorwheel);
    lv_style_list_t * style_dsc_p;

    switch(part) {
        case LV_COLORWHEEL_PART_MAIN :
            style_dsc_p = &colorwheel->style_list;
            break;
        case LV_COLORWHEEL_PART_KNOB:
            style_dsc_p = &ext->knob.style_list;
            break;
        default:
            style_dsc_p = NULL;
    }

    return style_dsc_p;
}

static bool lv_colorwheel_hit(lv_obj_t * colorwheel, const lv_point_t * p)
{
    bool is_point_on_coords = _lv_obj_is_click_point_on(colorwheel, p);
    if(!is_point_on_coords) return false;

    /*Valid clicks can be only in the circle*/
    if(_lv_area_is_point_on(&colorwheel->coords, p, LV_RADIUS_CIRCLE)) return true;
    else return false;
}

static void next_color_mode(lv_obj_t * colorwheel)
{
    lv_colorwheel_ext_t * ext = lv_obj_get_ext_attr(colorwheel);
    ext->mode = (ext->mode + 1) % 3;
    refr_knob_pos(colorwheel);
    lv_obj_invalidate(colorwheel);
}

static void refr_knob_pos(lv_obj_t * colorwheel)
{
    invalidate_knob(colorwheel);

    lv_colorwheel_ext_t * ext = lv_obj_get_ext_attr(colorwheel);
    lv_coord_t w = lv_obj_get_width(colorwheel);
    lv_coord_t h = lv_obj_get_height(colorwheel);

    lv_coord_t scale_w = lv_obj_get_style_scale_width(colorwheel, LV_COLORWHEEL_PART_MAIN);
    lv_coord_t r = (w - scale_w) / 2;
    uint16_t angle = get_angle(colorwheel);
    ext->knob.pos.x = (((int32_t)r * lv_trigo_sin(angle)) >> LV_TRIGO_SHIFT);
    ext->knob.pos.y = (((int32_t)r * lv_trigo_sin(angle + 90)) >> LV_TRIGO_SHIFT);
    ext->knob.pos.x = ext->knob.pos.x + w / 2;
    ext->knob.pos.y = ext->knob.pos.y + h / 2;

    invalidate_knob(colorwheel);
}

static lv_res_t double_click_reset(lv_obj_t * colorwheel)
{
    lv_colorwheel_ext_t * ext = lv_obj_get_ext_attr(colorwheel);
    lv_indev_t * indev = lv_indev_get_act();
    /*Double clicked? Use long press time as double click time out*/
    if(lv_tick_elaps(ext->last_click_time) < indev->driver.long_press_time) {
        lv_color_hsv_t hsv_cur;
        hsv_cur = ext->hsv;

        switch(ext->mode) {
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

        if(lv_colorwheel_set_hsv(colorwheel, hsv_cur)) {
            lv_res_t res = lv_event_send(colorwheel, LV_EVENT_VALUE_CHANGED, NULL);
            if(res != LV_RES_OK) return res;
        }
    }
    ext->last_click_time = lv_tick_get();

    return LV_RES_OK;
}

static lv_color_t angle_to_mode_color(lv_obj_t * colorwheel, uint16_t angle)
{
    lv_colorwheel_ext_t * ext = lv_obj_get_ext_attr(colorwheel);
    lv_color_t color;
    angle = angle % 360;

    switch(ext->mode) {
        default:
        case LV_COLORWHEEL_MODE_HUE:
            color = lv_color_hsv_to_rgb(angle, ext->hsv.s, ext->hsv.v);
            break;
        case LV_COLORWHEEL_MODE_SATURATION:
            color = lv_color_hsv_to_rgb(ext->hsv.h, (angle * 100) / 360, ext->hsv.v);
            break;
        case LV_COLORWHEEL_MODE_VALUE:
            color = lv_color_hsv_to_rgb(ext->hsv.h, ext->hsv.s, (angle * 100) / 360);
            break;
    }
    return color;
}

static uint16_t get_angle(lv_obj_t * colorwheel)
{
    lv_colorwheel_ext_t * ext = lv_obj_get_ext_attr(colorwheel);
    uint16_t angle;
    switch(ext->mode) {
        default:
        case LV_COLORWHEEL_MODE_HUE:
            angle = ext->hsv.h;
            break;
        case LV_COLORWHEEL_MODE_SATURATION:
            angle = (ext->hsv.s * 360) / 100;
            break;
        case LV_COLORWHEEL_MODE_VALUE:
            angle = (ext->hsv.v * 360) / 100 ;
            break;
    }
    return angle;
}

#endif /*LV_USE_COLORWHEEL*/
