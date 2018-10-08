/**
 * @file lv_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_img.h"
#if USE_LV_IMG != 0

/*Testing of dependencies*/
#if USE_LV_LABEL == 0
#error "lv_img: lv_label is required. Enable it in lv_conf.h (USE_LV_LABEL  1) "
#endif

#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_fs.h"
#include "../lv_misc/lv_ufs.h"
#include "../lv_misc/lv_txt.h"
#include "../lv_misc/lv_log.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_img_design(lv_obj_t * img, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_img_signal(lv_obj_t * img, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create an image objects
 * @param par pointer to an object, it will be the parent of the new button
 * @param copy pointer to a image object, if not NULL then the new object will be copied from it
 * @return pointer to the created image
 */
lv_obj_t * lv_img_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("image create started");

    lv_obj_t * new_img = NULL;

    /*Create a basic object*/
    new_img = lv_obj_create(par, copy);
    lv_mem_assert(new_img);
    if(new_img == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_img);

    /*Extend the basic object to image object*/
    lv_img_ext_t * ext = lv_obj_allocate_ext_attr(new_img, sizeof(lv_img_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;

    ext->src = NULL;
    ext->src_type = LV_IMG_SRC_UNKNOWN;
    ext->cf = LV_IMG_CF_UNKOWN;
    ext->w = lv_obj_get_width(new_img);
    ext->h = lv_obj_get_height(new_img);
    ext->auto_size = 1;

    /*Init the new object*/
    lv_obj_set_signal_func(new_img, lv_img_signal);
    lv_obj_set_design_func(new_img, lv_img_design);

    if(copy == NULL) {
        lv_obj_set_click(new_img, false);
        /* Enable auto size for non screens
         * because image screens are wallpapers
         * and must be screen sized*/
        if(par != NULL) {
            ext->auto_size = 1;
            lv_obj_set_style(new_img, NULL);                        /*Inherit the style  by default*/
        } else {
            ext->auto_size = 0;
            lv_obj_set_style(new_img, &lv_style_plain);            /*Set a style for screens*/
        }
    } else {
        lv_img_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->auto_size = copy_ext->auto_size;
        lv_img_set_src(new_img, copy_ext->src);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_img);
    }


    LV_LOG_INFO("image created");

    return new_img;
}


/*=====================
 * Setter functions
 *====================*/


/**
 * Set the pixel map to display by the image
 * @param img pointer to an image object
 * @param data the image data
 */
void lv_img_set_src(lv_obj_t * img, const void * src_img)
{
    lv_img_src_t src_type = lv_img_src_get_type(src_img);
    lv_img_ext_t * ext = lv_obj_get_ext_attr(img);

#if LV_LOG_LEVEL >= LV_LOG_LEVEL_INFO
    switch(src_type) {
        case LV_IMG_SRC_FILE:
            LV_LOG_TRACE("lv_img_set_src: `LV_IMG_SRC_FILE` type found");
            break;
        case LV_IMG_SRC_VARIABLE:
            LV_LOG_TRACE("lv_img_set_src: `LV_IMG_SRC_VARIABLE` type found");
            break;
        case LV_IMG_SRC_SYMBOL:
            LV_LOG_TRACE("lv_img_set_src: `LV_IMG_SRC_SYMBOL` type found");
            break;
        default:
            LV_LOG_WARN("lv_img_set_src: unknown type");
    }
#endif

    /*If the new source type is unknown free the memories of the old source*/
    if(src_type == LV_IMG_SRC_UNKNOWN) {
        LV_LOG_WARN("lv_img_set_src: unknown image type");
        if(ext->src_type == LV_IMG_SRC_SYMBOL || ext->src_type == LV_IMG_SRC_FILE) {
            lv_mem_free(ext->src);
        }
        ext->src = NULL;
        ext->src_type = LV_IMG_SRC_UNKNOWN;
        return;
    }

    lv_img_header_t header;
    lv_img_dsc_get_info(src_img, &header);

    /*Save the source*/
    if(src_type == LV_IMG_SRC_VARIABLE) {
        LV_LOG_INFO("lv_img_set_src:  `LV_IMG_SRC_VARIABLE` type found");
        ext->src = src_img;
    } else if(src_type == LV_IMG_SRC_FILE || src_type == LV_IMG_SRC_SYMBOL) {


        /* If the new and the old src are the same then it was only a refresh.*/
        if(ext->src != src_img) {
            lv_mem_free(ext->src);
            char * new_str = lv_mem_alloc(strlen(src_img) + 1);
            lv_mem_assert(new_str);
            if(new_str == NULL) return;
            strcpy(new_str, src_img);
            ext->src = new_str;
        }
    }

    if(src_type == LV_IMG_SRC_SYMBOL) {
        /*`lv_img_dsc_get_info` couldn't set the with and height of a font so set it here*/
        lv_style_t * style = lv_img_get_style(img);
        lv_point_t size;
        lv_txt_get_size(&size, src_img, style->text.font, style->text.letter_space, style->text.line_space, LV_COORD_MAX, LV_TXT_FLAG_NONE);
        header.w = size.x;
        header.h = size.y;
    }

    ext->src_type = src_type;
    ext->w = header.w;
    ext->h = header.h;
    ext->cf = header.cf;

    if(lv_img_get_auto_size(img) != false) {
        lv_obj_set_size(img, ext->w, ext->h);
    }

    lv_obj_invalidate(img);
}

/**
 * Enable the auto size feature.
 * If enabled the object size will be same as the picture size.
 * @param img pointer to an image
 * @param autosize_en true: auto size enable, false: auto size disable
 */
void lv_img_set_auto_size(lv_obj_t * img, bool autosize_en)
{
    lv_img_ext_t * ext = lv_obj_get_ext_attr(img);

    ext->auto_size = (autosize_en == false ? 0 : 1);
}


/*=====================
 * Getter functions
 *====================*/


/**
 * Get the source of the image
 * @param img pointer to an image object
 * @return the image source (symbol, file name or C array)
 */
const void * lv_img_get_src(lv_obj_t * img)
{
    lv_img_ext_t * ext = lv_obj_get_ext_attr(img);

    return ext->src;
}

/**
 * Get the name of the file set for an image
 * @param img pointer to an image
 * @return file name
 */
const char * lv_img_get_file_name(const lv_obj_t * img)
{
    lv_img_ext_t * ext = lv_obj_get_ext_attr(img);

    if(ext->src_type == LV_IMG_SRC_FILE) return ext->src;
    else return "";
}


/**
 * Get the auto size enable attribute
 * @param img pointer to an image
 * @return true: auto size is enabled, false: auto size is disabled
 */
bool lv_img_get_auto_size(const lv_obj_t * img)
{
    lv_img_ext_t * ext = lv_obj_get_ext_attr(img);

    return ext->auto_size == 0 ? false : true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the images
 * @param img pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_img_design(lv_obj_t * img, const lv_area_t * mask, lv_design_mode_t mode)
{
    lv_style_t * style = lv_obj_get_style(img);
    lv_img_ext_t * ext = lv_obj_get_ext_attr(img);

    if(mode == LV_DESIGN_COVER_CHK) {
        bool cover = false;
        if(ext->src_type == LV_IMG_SRC_UNKNOWN || ext->src_type == LV_IMG_SRC_SYMBOL) return false;

        if(ext->cf == LV_IMG_CF_TRUE_COLOR || ext->cf == LV_IMG_CF_RAW) cover = lv_area_is_in(mask, &img->coords);

        return cover;
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
        if(ext->h == 0 || ext->w == 0) return true;
        lv_area_t coords;
        lv_opa_t opa_scale = lv_obj_get_opa_scale(img);

        lv_obj_get_coords(img, &coords);

        if(ext->src_type == LV_IMG_SRC_FILE || ext->src_type == LV_IMG_SRC_VARIABLE) {
            LV_LOG_TRACE("lv_img_design: start to draw image");
            lv_area_t cords_tmp;
            cords_tmp.y1 = coords.y1;
            cords_tmp.y2 = coords.y1 + ext->h - 1;

            for(; cords_tmp.y1 < coords.y2; cords_tmp.y1 += ext->h, cords_tmp.y2 += ext->h) {
                cords_tmp.x1 = coords.x1;
                cords_tmp.x2 = coords.x1 + ext->w - 1;
                for(; cords_tmp.x1 < coords.x2; cords_tmp.x1 += ext->w, cords_tmp.x2 += ext->w) {
                    lv_draw_img(&cords_tmp, mask, ext->src, style, opa_scale);
                }
            }
        } else if(ext->src_type == LV_IMG_SRC_SYMBOL) {
            LV_LOG_TRACE("lv_img_design: start to draw symbol");
            lv_draw_label(&coords, mask, style, opa_scale, ext->src, LV_TXT_FLAG_NONE, NULL);
        } else {
            /*Trigger the error handler of image drawer*/
            LV_LOG_WARN("lv_img_design: image source type is unknown");
            lv_draw_img(&img->coords, mask, NULL, style, opa_scale);
        }
    }

    return true;
}


/**
 * Signal function of the image
 * @param img pointer to an image object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_img_signal(lv_obj_t * img, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(img, sign, param);
    if(res != LV_RES_OK) return res;

    lv_img_ext_t * ext = lv_obj_get_ext_attr(img);
    if(sign == LV_SIGNAL_CLEANUP) {
        if(ext->src_type == LV_IMG_SRC_FILE || ext->src_type == LV_IMG_SRC_SYMBOL) {
            lv_mem_free(ext->src);
            ext->src = NULL;
            ext->src_type = LV_IMG_SRC_UNKNOWN;
        }
    } else if(sign == LV_SIGNAL_STYLE_CHG) {
        /*Refresh the file name to refresh the symbol text size*/
        if(ext->src_type == LV_IMG_SRC_SYMBOL) {
            lv_img_set_src(img, ext->src);

        }
    } else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_img";
    }

    return res;
}

#endif
