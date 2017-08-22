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
#include "lv_conf.h"
#include "misc_conf.h"
#if USE_LV_IMG != 0 && USE_FSINT != 0

#include "../lv_obj/lv_obj.h"
#include "misc/fs/fsint.h"

#ifndef LV_IMG_ENABLE_SYMBOLS
#define LV_IMG_ENABLE_SYMBOLS 0
#endif

#if LV_IMG_ENABLE_SYMBOLS != 0
#include "lv_label.h"
#include "misc/gfx/fonts/symbol_def.h"
#endif

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
    cord_t w;                   /*Width of the image (doubled when upscaled) (Handled by the library)*/
    cord_t h;                   /*Height of the image (doubled when upscaled) (Handled by the library)*/
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
 * Signal function of the image
 * @param img pointer to animage object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_img_signal(lv_obj_t * img, lv_signal_t sign, void * param);

/**
 * Create a file to the RAMFS from a picture data
 * @param fn file name of the new file (e.g. "pic1", will be available at "U:/pic1")
 * @param data pointer to a color map with lv_img_raw_header_t header
 * @return result of the file operation. FS_RES_OK or any error from fs_res_t
 */
fs_res_t lv_img_create_file(const char * fn, const color_int_t * data);

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
 * @param en true: auto size enable, false: auto size disable
 */
void lv_img_set_auto_size(lv_obj_t * img, bool en);

/**
 * Enable the upscaling with LV_DOWNSCALE.
 * If enabled the object size will be same as the picture size.
 * @param img pointer to an image
 * @param en true: upscale enable, false: upscale disable
 */
void lv_img_set_upscale(lv_obj_t * img, bool en);

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

/**********************
 *      MACROS
 **********************/

/*Use this macro to declare an image in a c file*/
#define LV_IMG_DECLARE(var_name) extern const color_int_t var_name[];

#endif  /*USE_LV_IMG*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif  /*LV_IMG_H*/
