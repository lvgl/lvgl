/**
 * @file lv_img.h
 * 
 */

#ifndef LV_IMG_H
#define LV_IMG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#if USE_LV_IMG != 0

#include "../lv_core/lv_obj.h"
#include "../lv_misc/lv_fs.h"
#include "../lv_misc/lv_fonts/lv_symbol_def.h"
#include "lv_label.h"
#include "../lv_draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of image*/
typedef struct
{
    /*No inherited ext. because inherited from the base object*/ /*Ext. of ancestor*/
    /*New data for this type */
    const void * src;             /*Image source: Pointer to an array or a file or a symbol*/

    lv_coord_t w;               /*Width of the image (doubled when upscaled) (Handled by the library)*/
    lv_coord_t h;               /*Height of the image (doubled when upscaled) (Handled by the library)*/
    uint8_t src_type  :2;       /*See: lv_img_src_t*/
    uint8_t auto_size :1;       /*1: automatically set the object size to the image size*/
    uint8_t chroma_keyed :1;    /*1: Chroma keyed image, LV_COLOR_TRANSP (lv_conf.h) pixels will be transparent (Handled by the library)*/
    uint8_t alpha_byte   :1;    /*1: Extra byte for every pixel to define opacity*/
}lv_img_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an image objects
 * @param par pointer to an object, it will be the parent of the new button
 * @param copy pointer to a image object, if not NULL then the new object will be copied from it
 * @return pointer to the created image
 */
lv_obj_t * lv_img_create(lv_obj_t * par, lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the pixel map to display by the image
 * @param img pointer to an image object
 * @param data the image data
 */
void lv_img_set_src(lv_obj_t * img, const void * src_img);

/**
 * Obsolete since v5.1. Just for compatibility with v5.0. Will be removed in v6.0.
 * Use 'lv_img_set_src()' instead.
 * @param img
 * @param fn
 */
static inline void lv_img_set_file(lv_obj_t * img, const char * fn)
{

}

/**
 * Enable the auto size feature.
 * If enabled the object size will be same as the picture size.
 * @param img pointer to an image
 * @param autosize_en true: auto size enable, false: auto size disable
 */
void lv_img_set_auto_size(lv_obj_t * img, bool autosize_en);

/**
 * Set the style of an image
 * @param img pointer to an image object
 * @param style pointer to a style
 */
static inline void lv_img_set_style(lv_obj_t *img, lv_style_t *style)
{
    lv_obj_set_style(img, style);
}

/**
 * Obsolete since v5.1. Just for compatibility with v5.0. Will be removed in v6.0
 * @param img
 * @param upscale
 */
static inline void lv_img_set_upscale(lv_obj_t * img, bool upcale)
{

}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the type of an image source
 * @param src pointer to an image source:
 *  - pointer to an 'lv_img_t' variable (image stored internally and compiled into the code)
 *  - a path to an file (e.g. "S:/folder/image.bin")
 *  - or a symbol (e.g. SYMBOL_CLOSE)
 * @return type of the image source LV_IMG_SRC_VARIABLE/FILE/SYMBOL/UNKOWN
 */
lv_img_src_t lv_img_get_src_type(const void * src);


/**
 * Get the name of the file set for an image
 * @param img pointer to an image
 * @return file name
 */
const char * lv_img_get_file_name(lv_obj_t * img);

/**
 * Get the auto size enable attribute
 * @param img pointer to an image
 * @return true: auto size is enabled, false: auto size is disabled
 */
bool lv_img_get_auto_size(lv_obj_t * img);

/**
 * Get the style of an image object
 * @param img pointer to an image object
 * @return pointer to the image's style
 */
static inline lv_style_t* lv_img_get_style(lv_obj_t *img)
{
    return lv_obj_get_style(img);
}

/**
 * Obsolete since v5.1. Just for compatibility with v5.0. Will be removed in v6.0
 * @param img
 * @return false
 */
static inline bool lv_img_get_upscale(lv_obj_t * img)
{
    return false;
}

/**********************
 *      MACROS
 **********************/

/*Use this macro to declare an image in a c file*/
#define LV_IMG_DECLARE(var_name) extern const lv_img_t var_name;

#endif  /*USE_LV_IMG*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_IMG_H*/
