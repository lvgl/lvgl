/**
 * @file lv_imgbtn.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_imgbtn.h"

#if LV_USE_IMGBTN != 0

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_imgbtn_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_imgbtn_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);
static lv_draw_res_t lv_imgbtn_draw(lv_obj_t * imgbtn, const lv_area_t * clip_area, lv_draw_mode_t mode);
static lv_res_t lv_imgbtn_signal(lv_obj_t * imgbtn, lv_signal_t sign, void * param);
static void refr_img(lv_obj_t * imgbtn);
static lv_imgbtn_state_t suggest_state(lv_obj_t * imgbtn, lv_imgbtn_state_t state);
lv_imgbtn_state_t get_state(const lv_obj_t * imgbtn);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_imgbtn_class = {
        .base_class = &lv_obj_class,
        .instance_size = sizeof(lv_imgbtn_t),
        .constructor_cb = lv_imgbtn_constructor,
        .signal_cb = lv_imgbtn_signal,
        .draw_cb = lv_imgbtn_draw,
};

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
lv_obj_t * lv_imgbtn_create(lv_obj_t * parent)
{
   return lv_obj_create_from_class(&lv_imgbtn_class, parent, NULL);
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set images for a state of the image button
 * @param imgbtn pointer to an image button object
 * @param state for which state set the new image
 * @param src_left pointer to an image source for the left side of the button (a C array or path to
 * a file)
 * @param src_mid pointer to an image source for the middle of the button (ideally 1px wide) (a C
 * array or path to a file)
 * @param src_right pointer to an image source for the right side of the button (a C array or path
 * to a file)
 */
void lv_imgbtn_set_src(lv_obj_t * obj, lv_imgbtn_state_t state, const void * src_left, const void * src_mid,
                             const void * src_right)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    if(lv_img_src_get_type(src_left) == LV_IMG_SRC_SYMBOL ||
       lv_img_src_get_type(src_mid) == LV_IMG_SRC_SYMBOL ||
       lv_img_src_get_type(src_right) == LV_IMG_SRC_SYMBOL) {
        LV_LOG_WARN("lv_imgbtn_set_src: symbols are not supported in tiled mode");
        return;
    }

    lv_imgbtn_t * imgbtn = (lv_imgbtn_t *) obj;

    imgbtn->img_src_left[state] = src_left;
    imgbtn->img_src_mid[state] = src_mid;
    imgbtn->img_src_right[state] = src_right;

    refr_img(obj);
}

/*=====================
 * Getter functions
 *====================*/


/**
 * Get the left image in a given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `lv_btn_state_t`) `
 * @return pointer to the left image source (a C array or path to a file)
 */
const void * lv_imgbtn_get_src_left(lv_obj_t * obj, lv_imgbtn_state_t state)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_imgbtn_t * imgbtn = (lv_imgbtn_t *) obj;

    return imgbtn->img_src_left[state];
}

/**
 * Get the middle image in a given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `lv_btn_state_t`) `
 * @return pointer to the middle image source (a C array or path to a file)
 */
const void * lv_imgbtn_get_src_middle(lv_obj_t * obj, lv_imgbtn_state_t state)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_imgbtn_t * imgbtn = (lv_imgbtn_t *) obj;

    return imgbtn->img_src_mid[state];
}

/**
 * Get the right image in a given state
 * @param imgbtn pointer to an image button object
 * @param state the state where to get the image (from `lv_btn_state_t`) `
 * @return pointer to the left image source (a C array or path to a file)
 */
const void * lv_imgbtn_get_src_right(lv_obj_t * obj, lv_imgbtn_state_t state)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_imgbtn_t * imgbtn = (lv_imgbtn_t *) obj;

    return imgbtn->img_src_right[state];
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_imgbtn_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy)
{
   lv_imgbtn_t * imgbtn = (lv_imgbtn_t *) obj;
   /*Initialize the allocated 'ext' */
   lv_memset_00((void *)imgbtn->img_src_mid, sizeof(imgbtn->img_src_mid));
   lv_memset_00(imgbtn->img_src_left, sizeof(imgbtn->img_src_left));
   lv_memset_00(imgbtn->img_src_right, sizeof(imgbtn->img_src_right));

   imgbtn->act_cf = LV_IMG_CF_UNKNOWN;
}


static lv_draw_res_t lv_imgbtn_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode)
{
    lv_imgbtn_t * imgbtn = (lv_imgbtn_t *) obj;
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DRAW_MODE_COVER_CHECK) {
        return LV_DRAW_RES_NOT_COVER;

        lv_draw_res_t cover = lv_obj_draw_base(&lv_imgbtn_class, obj, clip_area, mode);
        if(cover != LV_DRAW_RES_COVER) return cover;

        if(imgbtn->act_cf == LV_IMG_CF_TRUE_COLOR || imgbtn->act_cf == LV_IMG_CF_RAW) {
            cover = _lv_area_is_in(clip_area, &obj->coords, 0) ? LV_DRAW_RES_COVER : LV_DRAW_RES_NOT_COVER;
        } else {
            cover = LV_DRAW_RES_NOT_COVER;
        }

        return cover;
    }
    /*Draw the object*/
    else if(mode == LV_DRAW_MODE_MAIN_DRAW) {

        lv_obj_draw_base(&lv_imgbtn_class, obj, clip_area, mode);

        /*Just draw an image*/
        lv_imgbtn_t * imgbtn = (lv_imgbtn_t *) obj;
        lv_imgbtn_state_t state  = suggest_state(obj, get_state(obj));

        /*Simply draw the middle src if no tiled*/
        const void * src = imgbtn->img_src_left[state];
        if(lv_img_src_get_type(src) == LV_IMG_SRC_SYMBOL) {
            LV_LOG_WARN("lv_imgbtn_draw: SYMBOLS are not supported in tiled mode")
                        return LV_DRAW_RES_OK;
        }

        lv_coord_t tw = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
        lv_coord_t th = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
        lv_area_t coords;
        lv_area_copy(&coords, &obj->coords);
        coords.x1 -= tw;
        coords.x2 += tw;
        coords.y1 -= th;
        coords.y2 += th;

        lv_draw_img_dsc_t img_dsc;
        lv_draw_img_dsc_init(&img_dsc);
        lv_obj_init_draw_img_dsc(obj, LV_PART_MAIN, &img_dsc);

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

        src = imgbtn->img_src_right[state];
        if(src) {
            lv_img_decoder_get_info(src, &header);
            right_w = header.w;
            coords_part.x1 = coords.x2 - header.w + 1;
            coords_part.y1 = coords.y1;
            coords_part.x2 = coords.x2;
            coords_part.y2 = coords.y1 + header.h - 1;
            lv_draw_img(&coords_part, clip_area, src, &img_dsc);
        }

        src = imgbtn->img_src_mid[state];
        if(src) {
            lv_area_t clip_center_area;
            clip_center_area.x1 = coords.x1 + left_w;
            clip_center_area.x2 = coords.x2 - right_w;
            clip_center_area.y1 = coords.y1;
            clip_center_area.y2 = coords.y2;

            bool comm_res;
            comm_res = _lv_area_intersect(&clip_center_area, &clip_center_area, clip_area);
            if(comm_res) {
                lv_coord_t i;
                lv_img_decoder_get_info(src, &header);

                coords_part.x1 = coords.x1 + left_w;
                coords_part.y1 = coords.y1;
                coords_part.x2 = coords_part.x1 + header.w - 1;
                coords_part.y2 = coords_part.y1 + header.h - 1;

                for(i = 0; i < clip_center_area.x2 + header.w - 1; i += header.w) {

                    lv_draw_img(&coords_part, &clip_center_area, src, &img_dsc);
                    coords_part.x1 = coords_part.x2 + 1;
                    coords_part.x2 += header.w;
                }
            }
        }
    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DRAW_MODE_POST_DRAW) {
        lv_obj_draw_base(&lv_imgbtn_class, obj, clip_area, mode);
    }

    return LV_DRAW_RES_OK;
}

/**
 * Signal function of the image button
 * @param imgbtn pointer to a image button object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_imgbtn_signal(lv_obj_t * obj, lv_signal_t sign, void * param)
{
    lv_res_t res = lv_obj_signal_base(&lv_imgbtn_class, obj, sign, param);
    if(res != LV_RES_OK) return res;

    if(sign == LV_SIGNAL_PRESSED || sign == LV_SIGNAL_RELEASED || sign == LV_SIGNAL_PRESS_LOST) {
        refr_img(obj);
    }

    return res;
}

static void refr_img(lv_obj_t * obj)
{
    lv_imgbtn_t * imgbtn = (lv_imgbtn_t *) obj;
    lv_imgbtn_state_t state  = suggest_state(obj, get_state(obj));
    lv_img_header_t header;

    const void * src = imgbtn->img_src_mid[state];
    if(src == NULL) return;

    lv_res_t info_res = LV_RES_OK;
    if(lv_img_src_get_type(src) == LV_IMG_SRC_SYMBOL) {
        const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
        header.h = lv_font_get_line_height(font);
        header.w = _lv_txt_get_width(src, (uint16_t)strlen(src), font, 0, LV_TEXT_FLAG_NONE);
        header.always_zero = 0;
        header.cf = LV_IMG_CF_ALPHA_1BIT;
    }
    else {
        info_res = lv_img_decoder_get_info(src, &header);
    }

    if(info_res == LV_RES_OK) {
        imgbtn->act_cf = header.cf;
        lv_obj_set_height(obj, header.h); /*Keep the user defined width*/
    }
    else {
        imgbtn->act_cf = LV_IMG_CF_UNKNOWN;
    }

    lv_obj_invalidate(obj);
}

/**
 * If `src` is not defined for the current state try to get a state which is related to the current but has `src`.
 * E.g. if the PRESSED src is not set but the RELEASED does, use the RELEASED.
 * @param imgbtn pointer to an image button
 * @param state the state to convert
 * @return the suggested state
 */
static lv_imgbtn_state_t suggest_state(lv_obj_t * obj, lv_imgbtn_state_t state)
{
    lv_imgbtn_t * imgbtn = (lv_imgbtn_t *) obj;
    if(imgbtn->img_src_mid[state] == NULL) {
        switch(state) {
            case LV_IMGBTN_STATE_PRESSED:
                if(imgbtn->img_src_mid[LV_IMGBTN_STATE_RELEASED]) return LV_IMGBTN_STATE_RELEASED;
                break;
            case LV_IMGBTN_STATE_CHECKED_RELEASED:
                if(imgbtn->img_src_mid[LV_IMGBTN_STATE_RELEASED]) return LV_IMGBTN_STATE_RELEASED;
                break;
            case LV_IMGBTN_STATE_CHECKED_PRESSED:
                if(imgbtn->img_src_mid[LV_IMGBTN_STATE_CHECKED_RELEASED]) return LV_IMGBTN_STATE_CHECKED_RELEASED;
                if(imgbtn->img_src_mid[LV_IMGBTN_STATE_PRESSED]) return LV_IMGBTN_STATE_PRESSED;
                if(imgbtn->img_src_mid[LV_IMGBTN_STATE_RELEASED]) return LV_IMGBTN_STATE_RELEASED;
                break;
            case LV_IMGBTN_STATE_DISABLED:
                if(imgbtn->img_src_mid[LV_IMGBTN_STATE_RELEASED]) return LV_IMGBTN_STATE_RELEASED;
                break;
            case LV_IMGBTN_STATE_CHECKED_DISABLED:
                if(imgbtn->img_src_mid[LV_IMGBTN_STATE_CHECKED_RELEASED]) return LV_IMGBTN_STATE_CHECKED_RELEASED;
                if(imgbtn->img_src_mid[LV_IMGBTN_STATE_RELEASED]) return LV_IMGBTN_STATE_RELEASED;
                break;
            default:
                break;
        }
    }

    return state;
}

lv_imgbtn_state_t get_state(const lv_obj_t * imgbtn)
{
    LV_ASSERT_OBJ(imgbtn, MY_CLASS);

    lv_state_t obj_state = lv_obj_get_state(imgbtn);

    if(obj_state & LV_STATE_DISABLED) {
        if(obj_state & LV_STATE_CHECKED) return LV_IMGBTN_STATE_CHECKED_DISABLED;
        else return LV_IMGBTN_STATE_DISABLED;
    }

    if(obj_state & LV_STATE_CHECKED) {
        if(obj_state & LV_STATE_PRESSED) return LV_IMGBTN_STATE_CHECKED_PRESSED;
        else return LV_IMGBTN_STATE_CHECKED_RELEASED;
    }
    else {
        if(obj_state & LV_STATE_PRESSED) return LV_IMGBTN_STATE_PRESSED;
        else return LV_IMGBTN_STATE_RELEASED;
    }
}

#endif
