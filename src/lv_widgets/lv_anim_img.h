/**
 * @file lv_anim_img.h
 *
 */

#ifndef LV_ANIM_IMG_H
#define LV_ANIM_IMG_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#if LV_USE_ANIM_IMG != 0
#if LV_USE_ANIMATION

#include "lv_img.h"
#include "../lv_core/lv_obj.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_misc/lv_anim.h"
#include "../lv_misc/lv_fs.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of image*/
typedef struct {
    lv_img_ext_t img;
    lv_anim_t anim;
    /*picture sequence */
    lv_img_dsc_t **dsc;
    int8_t      pic_count;

} lv_anim_img_ext_t;


/*Image parts*/
enum {
    LV_ANIM_IMG_PART_MAIN,
};
typedef uint8_t lv_anim_img_part_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an animation image objects
 * @param par pointer to an object, it will be the parent of the new button
 * @param copy pointer to a image object, if not NULL then the new object will be copied from it
 * @return pointer to the created animation image object
 */
lv_obj_t * lv_animimg_create(lv_obj_t * par, const lv_obj_t * copy);


/**
 * Startup the  image animation.
 * @param img pointer to an animation image object
 */
void lv_anim_img_startup(lv_obj_t * img);




/*=====================
 * Setter functions
 *====================*/

/**
 * Set the  image animation duration time. unit:ms
 * @param img pointer to an animation image object
 */
void lv_set_anim_img_duration(lv_obj_t * img, uint32_t duration);

/**
 * Set the  image animation wait before repeat. unit:ms
 * @param img pointer to an animation image object
 */
void lv_set_anim_img_repeat_delay(lv_obj_t * img, uint32_t delay);

/**
 * Set the  image animation images source.
 * @param img pointer to an animation image object
 */
void lv_set_anim_img_sources(lv_obj_t * img,  lv_img_dsc_t ** dsc);

/**
 * Set the number of image.
 * @param img pointer to an animation image object
 */
void lv_set_anim_img_numbers(lv_obj_t * img, uint8_t numbers);

/**
 * Set the  image animation time interval when playback. unit:ms
 * @param img pointer to an animation image object
 */
void lv_set_anim_img_playback_time(lv_obj_t * img, uint16_t interval);

/**
 * Set the image animation reapeatly play times.
 * @param img pointer to an animation image object
 */
void lv_set_anim_img_startup_repeat_count(lv_obj_t * img, uint16_t count);

/**
 * Set the specify pixel map to display by the image
 * @param img pointer to an image object
 * @param data the image data
 */
void lv_anim_img_set_src(lv_obj_t * img, const void * src_img);

/**
 * Enable the auto size feature.
 * If enabled the object size will be same as the picture size.
 * @param img pointer to an image
 * @param en true: auto size enable, false: auto size disable
 */
void lv_anim_img_set_auto_size(lv_obj_t * img, bool autosize_en);

/**
 * Set an offset for the source of an image.
 * so the image will be displayed from the new origin.
 * @param img pointer to an image
 * @param x: the new offset along x axis.
 */
void lv_anim_img_set_offset_x(lv_obj_t * img, lv_coord_t x);

/**
 * Set an offset for the source of an image.
 * so the image will be displayed from the new origin.
 * @param img pointer to an image
 * @param y: the new offset along y axis.
 */
void lv_anim_img_set_offset_y(lv_obj_t * img, lv_coord_t y);

/**
 * Set the rotation center of the image.
 * The image will be rotated around this point
 * @param img pointer to an image object
 * @param pivot_x rotation center x of the image
 * @param pivot_y rotation center y of the image
 */
void lv_anim_img_set_pivot(lv_obj_t * img, lv_coord_t pivot_x, lv_coord_t pivot_y);

/**
 * Set the rotation angle of the image.
 * The image will be rotated around the set pivot set by `lv_anim_img_set_pivot()`
 * @param img pointer to an image object
 * @param angle rotation angle in degree with 0.1 degree resolution (0..3600: clock wise)
 */
void lv_anim_img_set_angle(lv_obj_t * img, int16_t angle);

/**
 * Set the zoom factor of the image.
 * @param img pointer to an image object
 * @param zoom the zoom factor. base on 256 times
 * - 256 or LV_ZOOM_IMG_NONE for no zoom
 * - <256: scale down
 * - >256 scale up
 * - 128 half size
 * - 512 double size
 */
void lv_anim_img_set_zoom(lv_obj_t * img, uint16_t zoom);

/**
 * Enable/disable anti-aliasing for the transformations (rotate, zoom) or not
 * @param img pointer to an image object
 * @param antialias true: anti-aliased; false: not anti-aliased
 */
void lv_anim_img_set_antialias(lv_obj_t * img, bool antialias);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the source of the image
 * @param img pointer to an image object
 * @return the image source (symbol, file name or C array)
 */
const void * lv_anim_img_get_src(lv_obj_t * img);

/**
 * Get the name of the file set for an image
 * @param img pointer to an image
 * @return file name
 */
const char * lv_anim_img_get_file_name(const lv_obj_t * img);

/**
 * Get the auto size enable attribute
 * @param img pointer to an image
 * @return true: auto size is enabled, false: auto size is disabled
 */
bool lv_anim_img_get_auto_size(const lv_obj_t * img);

/**
 * Get the offset.x attribute of the img object.
 * @param img pointer to an image
 * @return offset.x value.
 */
lv_coord_t lv_anim_img_get_offset_x(lv_obj_t * img);

/**
 * Get the offset.y attribute of the img object.
 * @param img pointer to an image
 * @return offset.y value.
 */
lv_coord_t lv_anim_img_get_offset_y(lv_obj_t * img);

/**
 * Get the rotation angle of the image.
 * @param img pointer to an image object
 * @return rotation angle in degree (0..359)
 */
uint16_t lv_anim_img_get_angle(lv_obj_t * img);

/**
 * Get the rotation center of the image.
 * @param img pointer to an image object
 * @param center rotation center of the image
 */
void lv_anim_img_get_pivot(lv_obj_t * img, lv_point_t * center);

/**
 * Get the zoom factor of the image.
 * @param img pointer to an image object
 * @return zoom factor (256: no zoom)
 */
uint16_t lv_anim_img_get_zoom(lv_obj_t * img);

/**
 * Get whether the transformations (rotate, zoom) are anti-aliased or not
 * @param img pointer to an image object
 * @return true: anti-aliased; false: not anti-aliased
 */
bool lv_anim_img_get_antialias(lv_obj_t * img);

void index_change(lv_obj_t * obj, lv_anim_value_t index);

#endif /*LV_USE_ANIM_IMG*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
#endif /*LV_ANIM_IMG_H*/
