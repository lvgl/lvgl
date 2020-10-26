/**
 * @file lv_imgbtn.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../lv_misc/lv_debug.h"
#include "../lv_themes/lv_theme.h"
#include "lv_imgbtn.h"
#include "lv_label.h"


#if LV_USE_IMGBTN != 0

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_imgbtn"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_design_res_t lv_imgbtn_design(lv_obj_t * imgbtn, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_imgbtn_signal(lv_obj_t * imgbtn, lv_signal_t sign, void * param);
static void refr_img(lv_obj_t * imgbtn);

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
 * Create a image button object
 * @param par pointer to an object, it will be the parent of the new image button
 * @param copy pointer to a image button object, if not NULL then the new object will be copied from
 * it
 * @return pointer to the created image button
 */
lv_obj_t * lv_imgbtn_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("image button create started");

    /*Create the ancestor of image button*/
    lv_obj_t * imgbtn = lv_btn_create(par, copy);
    LV_ASSERT_MEM(imgbtn);
    if(imgbtn == NULL) return NULL;

    /*Allocate the image button type specific extended data*/
    lv_imgbtn_ext_t * ext = lv_obj_allocate_ext_attr(imgbtn, sizeof(lv_imgbtn_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(imgbtn);
        return NULL;
    }

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(imgbtn);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(imgbtn);

    /*Initialize the allocated 'ext' */
    _lv_memset_00((void *)ext->img_src_mid, sizeof(ext->img_src_mid));
#if LV_IMGBTN_TILED
    _lv_memset_00(ext->img_src_left, sizeof(ext->img_src_left));
    _lv_memset_00(ext->img_src_right, sizeof(ext->img_src_right));
#endif
    ext->tiled = 0;

    ext->act_cf = LV_IMG_CF_UNKNOWN;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(imgbtn, lv_imgbtn_signal);
    lv_obj_set_design_cb(imgbtn, lv_imgbtn_design);

    /*Init the new image button image button*/
    if(copy == NULL) {
        lv_theme_apply(imgbtn, LV_THEME_IMGBTN);
    }
    /*Copy an existing image button*/
    else {
        lv_imgbtn_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        _lv_memcpy((void *)ext->img_src_mid, copy_ext->img_src_mid, sizeof(ext->img_src_mid));
#if LV_IMGBTN_TILED
        _lv_memcpy((void *)ext->img_src_left, copy_ext->img_src_left, sizeof(ext->img_src_left));
        _lv_memcpy((void *)ext->img_src_right, copy_ext->img_src_right, sizeof(ext->img_src_right));
#endif
        ext->tiled = copy_ext->tiled;
        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(imgbtn, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
    }

    LV_LOG_INFO("image button created");

    return imgbtn;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set images for a state of the image button
 * @param imgbtn pointer to an image button object
 * @param state for which state set the new image (from `lv_btn_state_t`) `
 * @param src pointer to an image source (a C array or path to a file)
 */
void lv_imgbtn_set_src(lv_obj_t * imgbtn, lv_btn_state_t state, const void * src)
{
    LV_ASSERT_OBJ(imgbtn, LV_OBJX_NAME);

    lv_imgbtn_ext_t * ext = lv_obj_get_ext_attr(imgbtn);

    ext->img_src_mid[state] = src;
#if LV_IMGBTN_TILED
    ext->img_src_left[state] = NULL;
    ext->img_src_right[state] = NULL;
#endif
    ext->tiled = 0;
    refr_img(imgbtn);
}

#if LV_IMGBTN_TILED
/**
 * Set images for a state of the image button
 * @param imgbtn pointer to an image button object
 * @param state for which state set the new image (from `lv_btn_state_t`) `
 * @param src_left pointer to an image source for the left side of the button (a C array or path to
 * a file)
 * @param src_mid pointer to an image source for the middle of the button (ideally 1px wide) (a C
 * array or path to a file)
 * @param src_right pointer to an image source for the right side of the button (a C array or path
 * to a file)
 */
void lv_imgbtn_set_src_tiled(lv_obj_t * imgbtn, lv_btn_state_t state, const void * src_left, const void * src_mid,
                             const void * src_right)
{
    LV_ASSERT_OBJ(imgbtn, LV_OBJX_NAME);

    if(lv_img_src_get_type(src_left) == LV_IMG_SRC_SYMBOL ||
       lv_img_src_get_type(src_mid) == LV_IMG_SRC_SYMBOL ||
       lv_img_src_get_type(src_right) == LV_IMG_SRC_SYMBOL) {
        LV_LOG_WARN("lv_imgbtn_set_src: symbols are not supported in tiled mode");
        return;
    }

    lv_imgbtn_ext_t * ext = lv_obj_get_ext_attr(imgbtn);

    ext->img_src_left[state] = src_left;
    ext->img_src_mid[state] = src_mid;
    ext->img_src_right[state] = src_right;

    ext->tiled = 1;

    refr_img(imgbtn);
}

#endif

/**
 * Set the state of the image button
 * @param imgbtn pointer to an image button object
 * @param state the new state of the button (from lv_btn_state_t enum)
 */
void lv_imgbtn_set_state(lv_obj_t * imgbtn, lv_btn_state_t state)
{
    lv_btn_set_state(imgbtn, state);
    refr_img(imgbtn);
}

/**
 * Toggle the state of the image button (ON->OFF, OFF->ON)
 * @param imgbtn pointer to a image button object
 */
void lv_imgbtn_toggle(lv_obj_t * imgbtn)
{
    lv_btn_toggle(imgbtn);
    refr_img(imgbtn);
}
/*=====================
 * Getter functions
 *====================*/

/**
 * Get the images in a  given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `lv_btn_state_t`) `
 * @return pointer to an image source (a C array or path to a file)
 */
const void * lv_imgbtn_get_src(lv_obj_t * imgbtn, lv_btn_state_t state)
{
    LV_ASSERT_OBJ(imgbtn, LV_OBJX_NAME);

    lv_imgbtn_ext_t * ext = lv_obj_get_ext_attr(imgbtn);

    return ext->img_src_mid[state];
}
#if LV_IMGBTN_TILED

/**
 * Get the left image in a given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `lv_btn_state_t`) `
 * @return pointer to the left image source (a C array or path to a file)
 */
const void * lv_imgbtn_get_src_left(lv_obj_t * imgbtn, lv_btn_state_t state)
{
    LV_ASSERT_OBJ(imgbtn, LV_OBJX_NAME);

    lv_imgbtn_ext_t * ext = lv_obj_get_ext_attr(imgbtn);

    return ext->img_src_left[state];
}

/**
 * Get the middle image in a given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `lv_btn_state_t`) `
 * @return pointer to the middle image source (a C array or path to a file)
 */
const void * lv_imgbtn_get_src_middle(lv_obj_t * imgbtn, lv_btn_state_t state)
{
    LV_ASSERT_OBJ(imgbtn, LV_OBJX_NAME);

    lv_imgbtn_ext_t * ext = lv_obj_get_ext_attr(imgbtn);

    return ext->img_src_mid[state];
}

/**
 * Get the right image in a given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `lv_btn_state_t`) `
 * @return pointer to the left image source (a C array or path to a file)
 */
const void * lv_imgbtn_get_src_right(lv_obj_t * imgbtn, lv_btn_state_t state)
{
    LV_ASSERT_OBJ(imgbtn, LV_OBJX_NAME);

    lv_imgbtn_ext_t * ext = lv_obj_get_ext_attr(imgbtn);

    return ext->img_src_right[state];
}

#endif

/*=====================
 * Other functions
 *====================*/

/*
 * New object specific "other" functions come here
 */

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the image buttons
 * @param imgbtn pointer to an object
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_imgbtn_design(lv_obj_t * imgbtn, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        lv_imgbtn_ext_t * ext = lv_obj_get_ext_attr(imgbtn);
        lv_design_res_t cover = LV_DESIGN_RES_NOT_COVER;
        if(ext->act_cf == LV_IMG_CF_TRUE_COLOR || ext->act_cf == LV_IMG_CF_RAW) {
            cover = _lv_area_is_in(clip_area, &imgbtn->coords, 0) ? LV_DESIGN_RES_COVER : LV_DESIGN_RES_NOT_COVER;
        }

        return cover;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_area_t img_coords;

        lv_obj_get_coords(imgbtn, &img_coords);

        lv_draw_rect_dsc_t bg_dsc;
        lv_draw_rect_dsc_init(&bg_dsc);
        lv_obj_init_draw_rect_dsc(imgbtn, LV_IMGBTN_PART_MAIN, &bg_dsc);

        /*If the border is drawn later disable loading its properties*/
        if(lv_obj_get_style_border_post(imgbtn, LV_OBJ_PART_MAIN)) {
            bg_dsc.border_opa = LV_OPA_TRANSP;
        }

        lv_area_t bg_coords;
        lv_area_copy(&bg_coords, &img_coords);
        bg_coords.x1 -= lv_obj_get_style_pad_left(imgbtn, LV_IMGBTN_PART_MAIN);
        bg_coords.x2 += lv_obj_get_style_pad_right(imgbtn, LV_IMGBTN_PART_MAIN);
        bg_coords.y1 -= lv_obj_get_style_pad_top(imgbtn, LV_IMGBTN_PART_MAIN);
        bg_coords.y2 += lv_obj_get_style_pad_bottom(imgbtn, LV_IMGBTN_PART_MAIN);

        lv_draw_rect(&bg_coords, clip_area, &bg_dsc);

        if(lv_obj_get_style_clip_corner(imgbtn, LV_OBJ_PART_MAIN)) {
            lv_draw_mask_radius_param_t * mp = _lv_mem_buf_get(sizeof(lv_draw_mask_radius_param_t));

            lv_coord_t r = lv_obj_get_style_radius(imgbtn, LV_OBJ_PART_MAIN);

            lv_draw_mask_radius_init(mp, &bg_coords, r, false);
            /*Add the mask and use `img+8` as custom id. Don't use `obj` directly because it might be used by the user*/
            lv_draw_mask_add(mp, imgbtn + 8);
        }

        /*Just draw an image*/
        lv_imgbtn_ext_t * ext    = lv_obj_get_ext_attr(imgbtn);
        lv_btn_state_t state     = lv_imgbtn_get_state(imgbtn);

        /*Simply draw the middle src if no tiled*/
        if(!ext->tiled) {
            const void * src = ext->img_src_mid[state];
            if(lv_img_src_get_type(src) == LV_IMG_SRC_SYMBOL) {
                lv_draw_label_dsc_t label_dsc;
                lv_draw_label_dsc_init(&label_dsc);
                lv_obj_init_draw_label_dsc(imgbtn, LV_IMGBTN_PART_MAIN, &label_dsc);
                lv_draw_label(&imgbtn->coords, clip_area, &label_dsc, src, NULL);
            }
            else {
                lv_draw_img_dsc_t img_dsc;
                lv_draw_img_dsc_init(&img_dsc);
                lv_obj_init_draw_img_dsc(imgbtn, LV_IMGBTN_PART_MAIN, &img_dsc);
                lv_draw_img(&imgbtn->coords, clip_area, src, &img_dsc);
            }
        }
        else {
#if LV_IMGBTN_TILED
            const void * src = ext->img_src_left[state];
            if(lv_img_src_get_type(src) == LV_IMG_SRC_SYMBOL) {
                LV_LOG_WARN("lv_imgbtn_design: SYMBOLS are not supported in tiled mode")
                return LV_DESIGN_RES_OK;
            }

            lv_coord_t w = lv_obj_get_style_transform_width(imgbtn, LV_OBJ_PART_MAIN);
            lv_coord_t h = lv_obj_get_style_transform_height(imgbtn, LV_OBJ_PART_MAIN);
            lv_area_t coords;
            lv_area_copy(&coords, &imgbtn->coords);
            coords.x1 -= w;
            coords.x2 += w;
            coords.y1 -= h;
            coords.y2 += h;

            lv_draw_img_dsc_t img_dsc;
            lv_draw_img_dsc_init(&img_dsc);
            lv_obj_init_draw_img_dsc(imgbtn, LV_IMGBTN_PART_MAIN, &img_dsc);

            lv_img_header_t header;
            lv_area_t coords_part;
            lv_coord_t left_w = 0;
            lv_coord_t right_w = 0;

            if(src) {
                lv_img_decoder_get_info(src, &header);
                left_w = header.w;
                coords_part.x1 = coords.x1;
                coords_part.y1 = coords.y1;
                coords_part.x2 = coords.x1 + header.w - 1;
                coords_part.y2 = coords.y1 + header.h - 1;
                lv_draw_img(&coords_part, clip_area, src, &img_dsc);
            }

            src = ext->img_src_right[state];
            if(src) {
                lv_img_decoder_get_info(src, &header);
                right_w = header.w;
                coords_part.x1 = coords.x2 - header.w + 1;
                coords_part.y1 = coords.y1;
                coords_part.x2 = coords.x2;
                coords_part.y2 = coords.y1 + header.h - 1;
                lv_draw_img(&coords_part, clip_area, src, &img_dsc);
            }

            src = ext->img_src_mid[state];
            if(src) {
                lv_area_t clip_center_area;
                clip_center_area.x1 = coords.x1 + left_w;
                clip_center_area.x2 = coords.x2 - right_w;
                clip_center_area.y1 = coords.y1;
                clip_center_area.y2 = coords.y2;

                bool comm_res;
                comm_res = _lv_area_intersect(&clip_center_area, &clip_center_area, clip_area);
                if(comm_res) {
                    lv_coord_t obj_w = lv_obj_get_width(imgbtn);
                    lv_coord_t i;
                    lv_img_decoder_get_info(src, &header);

                    coords_part.x1 = coords.x1 + left_w;
                    coords_part.y1 = coords.y1;
                    coords_part.x2 = coords_part.x1 + header.w - 1;
                    coords_part.y2 = coords_part.y1 + header.h - 1;

                    for(i = 0; i < obj_w - right_w - left_w; i += header.w) {

                        lv_draw_img(&coords_part, &clip_center_area, src, &img_dsc);
                        coords_part.x1 = coords_part.x2 + 1;
                        coords_part.x2 += header.w;
                    }
                }
            }
#endif
        }
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {
        if(lv_obj_get_style_clip_corner(imgbtn, LV_OBJ_PART_MAIN)) {
            lv_draw_mask_radius_param_t * param = lv_draw_mask_remove_custom(imgbtn + 8);
            _lv_mem_buf_release(param);
        }

        lv_draw_rect_dsc_t draw_dsc;
        lv_draw_rect_dsc_init(&draw_dsc);

        /*If the border is drawn later disable loading other properties*/
        if(lv_obj_get_style_border_post(imgbtn, LV_OBJ_PART_MAIN)) {
            draw_dsc.bg_opa = LV_OPA_TRANSP;
            draw_dsc.pattern_opa = LV_OPA_TRANSP;
            draw_dsc.shadow_opa = LV_OPA_TRANSP;
            lv_obj_init_draw_rect_dsc(imgbtn, LV_OBJ_PART_MAIN, &draw_dsc);


            lv_area_t bg_coords;
            lv_area_copy(&bg_coords, &imgbtn->coords);
            bg_coords.x1 -= lv_obj_get_style_pad_left(imgbtn, LV_IMGBTN_PART_MAIN);
            bg_coords.x2 += lv_obj_get_style_pad_right(imgbtn, LV_IMGBTN_PART_MAIN);
            bg_coords.y1 -= lv_obj_get_style_pad_top(imgbtn, LV_IMGBTN_PART_MAIN);
            bg_coords.y2 += lv_obj_get_style_pad_bottom(imgbtn, LV_IMGBTN_PART_MAIN);

            lv_draw_rect(&bg_coords, clip_area, &draw_dsc);
        }
    }

    return LV_DESIGN_RES_OK;
}

/**
 * Signal function of the image button
 * @param imgbtn pointer to a image button object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_imgbtn_signal(lv_obj_t * imgbtn, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(imgbtn, sign, param);
    if(res != LV_RES_OK) return res;
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);

    if(sign == LV_SIGNAL_STYLE_CHG) {
        /* If the style changed then the button was clicked, released etc. so probably the state was
         * changed as well Set the new image for the new state.*/
        refr_img(imgbtn);
    }
    else if(sign == LV_SIGNAL_REFR_EXT_DRAW_PAD) {
        /*Handle the padding of the background*/
        lv_style_int_t left = lv_obj_get_style_pad_left(imgbtn, LV_IMGBTN_PART_MAIN);
        lv_style_int_t right = lv_obj_get_style_pad_right(imgbtn, LV_IMGBTN_PART_MAIN);
        lv_style_int_t top = lv_obj_get_style_pad_top(imgbtn, LV_IMGBTN_PART_MAIN);
        lv_style_int_t bottom = lv_obj_get_style_pad_bottom(imgbtn, LV_IMGBTN_PART_MAIN);

        imgbtn->ext_draw_pad = LV_MATH_MAX(imgbtn->ext_draw_pad, left);
        imgbtn->ext_draw_pad = LV_MATH_MAX(imgbtn->ext_draw_pad, right);
        imgbtn->ext_draw_pad = LV_MATH_MAX(imgbtn->ext_draw_pad, top);
        imgbtn->ext_draw_pad = LV_MATH_MAX(imgbtn->ext_draw_pad, bottom);
    }
    else if(sign == LV_SIGNAL_PRESSED || sign == LV_SIGNAL_RELEASED || sign == LV_SIGNAL_PRESS_LOST) {
        refr_img(imgbtn);
    }
    else if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    }

    return res;
}

static void refr_img(lv_obj_t * imgbtn)
{
    lv_imgbtn_ext_t * ext = lv_obj_get_ext_attr(imgbtn);
    lv_btn_state_t state  = lv_imgbtn_get_state(imgbtn);
    lv_img_header_t header;

    const void * src = ext->img_src_mid[state];
    if(src == NULL) return;

    lv_res_t info_res = LV_RES_OK;
    if(lv_img_src_get_type(src) == LV_IMG_SRC_SYMBOL) {
        const lv_font_t * font = lv_obj_get_style_text_font(imgbtn, LV_IMGBTN_PART_MAIN);
        header.h = lv_font_get_line_height(font);
        header.w = _lv_txt_get_width(src, (uint16_t)strlen(src), font, 0, LV_TXT_FLAG_NONE);
        header.always_zero = 0;
        header.cf = LV_IMG_CF_ALPHA_1BIT;
    }
    else {
        info_res = lv_img_decoder_get_info(src, &header);
    }

    if(info_res == LV_RES_OK) {
        ext->act_cf = header.cf;
        if(ext->tiled) lv_obj_set_height(imgbtn, header.h); /*Keep the sure defined width*/
        else  lv_obj_set_size(imgbtn, header.w, header.h);
    }
    else {
        ext->act_cf = LV_IMG_CF_UNKNOWN;
    }

    lv_obj_invalidate(imgbtn);
}

#endif
