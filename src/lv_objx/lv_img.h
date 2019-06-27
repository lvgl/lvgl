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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#if LV_USE_IMG != 0

#include "../lv_core/lv_obj.h"
#include "../lv_misc/lv_fs.h"
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
    const void * src; /*Image source: Pointer to an array or a file or a symbol*/
    lv_point_t offset;
    lv_coord_t w;          /*Width of the image (Handled by the library)*/
    lv_coord_t h;          /*Height of the image (Handled by the library)*/
    uint8_t src_type : 2;  /*See: lv_img_src_t*/
    uint8_t auto_size : 1; /*1: automatically set the object size to the image size*/
    uint8_t cf : 5;        /*Color format from `lv_img_color_format_t`*/
} lv_img_ext_t;

/*Styles*/
enum {
    LV_IMG_STYLE_MAIN,
};
typedef uint8_t lv_img_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an image objects
 * @param par pointer to an object, it will be the parent of the new button
 * @param copy pointer to a image object, if not NULL then the new object will be copied from it
 * @return pointer to the created image
 */
lv_obj_t * lv_img_create(lv_obj_t * par, const lv_obj_t * copy);

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
 * Enable the auto size feature.
 * If enabled the object size will be same as the picture size.
 * @param img pointer to an image
 * @param en true: auto size enable, false: auto size disable
 */
void lv_img_set_auto_size(lv_obj_t * img, bool autosize_en);

/**
 * Set an offset for the source of an image.
 * so the image will be displayed from the new origin.
 * @param img pointer to an image
 * @param x: the new offset along x axis.
 */
void lv_img_set_offset_x(lv_obj_t * img, lv_coord_t x);

/**
 * Set an offset for the source of an image.
 * so the image will be displayed from the new origin.
 * @param img pointer to an image
 * @param y: the new offset along y axis.
 */
void lv_img_set_offset_y(lv_obj_t * img, lv_coord_t y);

/**
 * Set the style of an image
 * @param img pointer to an image object
 * @param type which style should be set (can be only `LV_IMG_STYLE_MAIN`)
 * @param style pointer to a style
 */
static inline void lv_img_set_style(lv_obj_t * img, lv_img_style_t type, const lv_style_t * style)
{
    (void)type; /*Unused*/
    lv_obj_set_style(img, style);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the source of the image
 * @param img pointer to an image object
 * @return the image source (symbol, file name or C array)
 */
const void * lv_img_get_src(lv_obj_t * img);

/**
 * Get the name of the file set for an image
 * @param img pointer to an image
 * @return file name
 */
const char * lv_img_get_file_name(const lv_obj_t * img);

/**
 * Get the auto size enable attribute
 * @param img pointer to an image
 * @return true: auto size is enabled, false: auto size is disabled
 */
bool lv_img_get_auto_size(const lv_obj_t * img);

/**
 * Get the offset.x attribute of the img object.
 * @param img pointer to an image
 * @return offset.x value.
 */
lv_coord_t lv_img_get_offset_x(lv_obj_t * img);

/**
 * Get the offset.y attribute of the img object.
 * @param img pointer to an image
 * @return offset.y value.
 */
lv_coord_t lv_img_get_offset_y(lv_obj_t * img);

/**
 * Get the style of an image object
 * @param img pointer to an image object
 * @param type which style should be get (can be only `LV_IMG_STYLE_MAIN`)
 * @return pointer to the image's style
 */
static inline const lv_style_t * lv_img_get_style(const lv_obj_t * img, lv_img_style_t type)
{
    (void)type; /*Unused*/
    return lv_obj_get_style(img);
}

/**********************
 *      MACROS
 **********************/

/*Use this macro to declare an image in a c file*/
#define LV_IMG_DECLARE(var_name) extern const lv_img_dsc_t var_name;

#endif /*LV_USE_IMG*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_IMG_H*/
