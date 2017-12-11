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
    char* fn;                   /*Image file name. E.g. "U:/my_image"*/
    lv_coord_t w;                   /*Width of the image (doubled when upscaled) (Handled by the library)*/
    lv_coord_t h;                   /*Height of the image (doubled when upscaled) (Handled by the library)*/
    uint8_t auto_size :1;       /*1: automatically set the object size to the image size*/
    uint8_t upscale   :1;       /*1: upscale to double size with antialaissing*/
    uint8_t transp    :1;       /*Transp. bit in the image header (Handled by the library)*/
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

/**
 * Create a file to the RAMFS from a picture data
 * @param fn file name of the new file (e.g. "pic1", will be available at "U:/pic1")
 * @param data pointer to a color map with lv_img_raw_header_t header
 * @return result of the file operation. LV_FS_RES_OK or any error from lv_fs_res_t
 */
lv_fs_res_t lv_img_create_file(const char * fn, const lv_color_int_t * data);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set a file to the image
 * @param img pointer to an image object
 * @param fn file name in the RAMFS to set as picture (e.g. "U:/pic1").
 */
void lv_img_set_file(lv_obj_t * img, const char * fn);

/**
 * Enable the auto size feature.
 * If enabled the object size will be same as the picture size.
 * @param img pointer to an image
 * @param autosize_en true: auto size enable, false: auto size disable
 */
void lv_img_set_auto_size(lv_obj_t * img, bool autosize_en);

/**
 * Enable the upscaling if LV_ANTIALIAS is enabled.
 * If enabled the object size will be same as the picture size.
 * @param img pointer to an image
 * @param en true: upscale enable, false: upscale disable
 */
void lv_img_set_upscale(lv_obj_t * img, bool en);

/**
 * Set the style of an image
 * @param img pointer to an image object
 * @param style pointer to a style
 */
static inline void lv_img_set_style(lv_obj_t *img, lv_style_t *style)
{
    lv_obj_set_style(img, style);
}

/*=====================
 * Getter functions
 *====================*/

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
 * Get the upscale enable attribute
 * @param img pointer to an image
 * @return true: upscale is enabled, false: upscale is disabled
 */
bool lv_img_get_upscale(lv_obj_t * img);

/**
 * Get the style of an image object
 * @param img pointer to an image object
 * @return pointer to the image's style
 */
static inline lv_style_t* lv_img_get_style(lv_obj_t *img)
{
    return lv_obj_get_style(img);
}

/**********************
 *      MACROS
 **********************/

/*Use this macro to declare an image in a c file*/
#define LV_IMG_DECLARE(var_name) extern const lv_color_int_t var_name[];

#endif  /*USE_LV_IMG*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_IMG_H*/
