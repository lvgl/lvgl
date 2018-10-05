/**
 * @file lv_imgbtn.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_imgbtn.h"
#if USE_LV_IMGBTN != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_imgbtn_design(lv_obj_t * imgbtn, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_imgbtn_signal(lv_obj_t * imgbtn, lv_signal_t sign, void * param);
static void refr_img(lv_obj_t * imgbtn);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;
static lv_design_func_t ancestor_design;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a image button object
 * @param par pointer to an object, it will be the parent of the new image button
 * @param copy pointer to a image button object, if not NULL then the new object will be copied from it
 * @return pointer to the created image button
 */
lv_obj_t * lv_imgbtn_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("image button create started");

    /*Create the ancestor of image button*/
    lv_obj_t * new_imgbtn = lv_btn_create(par, copy);
    lv_mem_assert(new_imgbtn);
    if(new_imgbtn == NULL) return NULL;

    /*Allocate the image button type specific extended data*/
    lv_imgbtn_ext_t * ext = lv_obj_allocate_ext_attr(new_imgbtn, sizeof(lv_imgbtn_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_imgbtn);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_func(new_imgbtn);

    /*Initialize the allocated 'ext' */
    ext->img_src[LV_BTN_STATE_REL] = NULL;
    ext->img_src[LV_BTN_STATE_PR] = NULL;
    ext->img_src[LV_BTN_STATE_TGL_REL] = NULL;
    ext->img_src[LV_BTN_STATE_TGL_PR] = NULL;;
    ext->img_src[LV_BTN_STATE_INA] = NULL;
    ext->act_cf = LV_IMG_CF_UNKOWN;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_imgbtn, lv_imgbtn_signal);
    lv_obj_set_design_func(new_imgbtn, lv_imgbtn_design);

    /*Init the new image button image button*/
    if(copy == NULL) {

    }
    /*Copy an existing image button*/
    else {
        lv_imgbtn_ext_t * copy_ext = lv_obj_get_ext_attr(copy);

        lv_imgbtn_set_src(new_imgbtn, LV_BTN_STATE_REL, copy_ext->img_src[LV_BTN_STATE_REL]);
        lv_imgbtn_set_src(new_imgbtn, LV_BTN_STATE_PR, copy_ext->img_src[LV_BTN_STATE_PR]);
        lv_imgbtn_set_src(new_imgbtn, LV_BTN_STATE_TGL_REL, copy_ext->img_src[LV_BTN_STATE_TGL_REL]);
        lv_imgbtn_set_src(new_imgbtn, LV_BTN_STATE_TGL_PR, copy_ext->img_src[LV_BTN_STATE_TGL_PR]);
        lv_imgbtn_set_src(new_imgbtn, LV_BTN_STATE_INA, copy_ext->img_src[LV_BTN_STATE_INA]);
        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_imgbtn);
    }

    LV_LOG_INFO("image button created");

    return new_imgbtn;
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
    lv_imgbtn_ext_t * ext = lv_obj_get_ext_attr(imgbtn);

    ext->img_src[state] = src;

    refr_img(imgbtn);
}

/**
 * Set a style of a image button.
 * @param imgbtn pointer to image button object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_imgbtn_set_style(lv_obj_t * imgbtn, lv_imgbtn_style_t type, lv_style_t * style)
{
    lv_btn_set_style(imgbtn, type, style);
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
    lv_imgbtn_ext_t * ext = lv_obj_get_ext_attr(imgbtn);

    return ext->img_src[state];
}

/**
 * Get style of a image button.
 * @param imgbtn pointer to image button object
 * @param type which style should be get
 * @return style pointer to the style
 */
lv_style_t * lv_imgbtn_get_style(const lv_obj_t * imgbtn, lv_imgbtn_style_t type)
{
    return lv_btn_get_style(imgbtn, type);
}

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
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_imgbtn_design(lv_obj_t * imgbtn, const lv_area_t * mask, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
        lv_imgbtn_ext_t * ext = lv_obj_get_ext_attr(imgbtn);
        bool cover = false;
        if(ext->act_cf == LV_IMG_CF_TRUE_COLOR || ext->act_cf == LV_IMG_CF_RAW) {
            cover = lv_area_is_in(mask, &imgbtn->coords);
        }

        return cover;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        /*Just draw an image*/
        lv_imgbtn_ext_t * ext = lv_obj_get_ext_attr(imgbtn);
        lv_btn_state_t state = lv_imgbtn_get_state(imgbtn);
        const void * src = ext->img_src[state];
        lv_style_t * style = lv_imgbtn_get_style(imgbtn, state);
        lv_opa_t opa_scale = lv_obj_get_opa_scale(imgbtn);
        lv_draw_img(&imgbtn->coords, mask, src, style, opa_scale);
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {

    }

    return true;
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

    if(sign == LV_SIGNAL_STYLE_CHG) {
        /* If the style changed then the button was clicked, released etc. so probably the state was changed as well
         * Set the new image for the new state.*/
        refr_img(imgbtn);
    } else if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_imgbtn";
    }

    return res;
}


static void refr_img(lv_obj_t * imgbtn)
{
    lv_imgbtn_ext_t * ext = lv_obj_get_ext_attr(imgbtn);
    lv_btn_state_t state = lv_imgbtn_get_state(imgbtn);
    lv_img_header_t header;
    const void * src = ext->img_src[state];

    lv_res_t info_res;
    info_res = lv_img_dsc_get_info(src, &header);
    if(info_res == LV_RES_OK) {
        ext->act_cf = header.cf;
        lv_obj_set_size(imgbtn, header.w, header.h);
    } else {
        ext->act_cf = LV_IMG_CF_UNKOWN;
    }

}

#endif
