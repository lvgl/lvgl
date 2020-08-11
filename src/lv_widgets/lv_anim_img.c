/**
 * @file lv_anim_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_anim_img.h"
#if LV_USE_ANIM_IMG != 0
#if LV_USE_ANIMATION

/*Testing of dependencies*/
#if LV_USE_IMG == 0
    #error "lv_anim_img: lv_img is required. Enable it in lv_conf.h (LV_USE_IMG  1) "
#endif

#include "../lv_misc/lv_debug.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_draw/lv_img_decoder.h"
#include "../lv_misc/lv_fs.h"
#include "../lv_misc/lv_txt.h"
#include "../lv_misc/lv_math.h"
#include "../lv_misc/lv_log.h"
#include "../lv_misc/lv_anim.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_anim_img"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_design_res_t lv_anim_img_design(lv_obj_t * img, const lv_area_t * clip_area, lv_design_mode_t mode);
static lv_res_t lv_anim_img_signal(lv_obj_t * img, lv_signal_t sign, void * param);

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
 * Create an animation image objects
 * @param par pointer to an object, it will be the parent of the new animation image
 * @param copy pointer to a image object, if not NULL then the new object will be copied from it
 * @return pointer to the created animation image
 */
lv_obj_t * lv_animimg_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE(" animation image create started");

    /*Create a basic object inheritate from lv_img_t*/
    lv_obj_t * img = lv_img_create(par, copy);
    LV_ASSERT_MEM(img);
    if(img == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(img);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(img);

    /*Extend the basic object to image object*/
    lv_anim_img_ext_t * ext = lv_obj_allocate_ext_attr(img, sizeof(lv_anim_img_ext_t));

    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(img);
        return NULL;
    }

    ext->dsc       = NULL;
    ext->pic_count = -1;

    /*Init the new object*/
    lv_obj_set_signal_cb(img, lv_anim_img_signal);
    lv_obj_set_design_cb(img, lv_anim_img_design);

    if(copy == NULL) {
        //initial animation
        //      lv_obj_t * im
          lv_anim_init(&ext->anim);
          lv_anim_set_var(&ext->anim, img);
          lv_anim_set_time(&ext->anim, 30);
          lv_anim_set_exec_cb(&ext->anim, (lv_anim_exec_xcb_t)index_change);
          lv_anim_set_values(&ext->anim, 0 , 1);
          lv_anim_set_playback_time(&ext->anim, 100);
          lv_anim_set_repeat_count(&ext->anim, LV_ANIM_REPEAT_INFINITE);
    }
    else {
        lv_anim_img_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->dsc         = copy_ext->dsc;
        ext->pic_count   = copy_ext->pic_count;
        _lv_memcpy(&(ext->anim), &(copy_ext->anim), sizeof(lv_anim_t));
    }

    LV_LOG_INFO("animation image created");

    return img;
}

/**
 * Set the animation images source.
 * @param img pointer to an animation image object
 */
void lv_set_anim_img_sources(lv_obj_t * img,  lv_img_dsc_t ** dsc)
{
    lv_anim_img_ext_t* ext = lv_obj_get_ext_attr(img);
    ext->dsc = dsc;
}

/**
 * Startup the  image animation.
 * @param img pointer to an animation image object
 */
void lv_anim_img_startup(lv_obj_t * img)
{
    lv_anim_img_ext_t* ext = lv_obj_get_ext_attr(img);
    lv_anim_start(&ext->anim);
}



/*=====================
 * Setter functions
 *====================*/

/**
 * Set the number of image.
 * @param img pointer to an animation image object
 */
void lv_set_anim_img_numbers(lv_obj_t * img, uint8_t numbers)
{
    lv_anim_img_ext_t* ext = lv_obj_get_ext_attr(img);
    ext->pic_count = numbers;
    lv_anim_set_values(&ext->anim, 0 , numbers);
}
/**
 * Set the  image animation wait before repeat. unit:ms
 * @param img pointer to an animation image object
 */
void lv_set_anim_img_repeat_delay(lv_obj_t * img, uint32_t delay)
{
    lv_anim_img_ext_t* ext = lv_obj_get_ext_attr(img);
    ext->anim.repeat_delay = delay;
}

/**
 * Set the  image animation duration time. unit:ms
 * @param img pointer to an animation image object
 */
void lv_set_anim_img_duration(lv_obj_t * img, uint32_t duration)
{
    lv_anim_img_ext_t* ext = lv_obj_get_ext_attr(img);
    lv_anim_set_time(&ext->anim, duration);
}

/**
 * Set the  image animation time interval when playback. unit:ms
 * @param img pointer to an animation image object
 */
void lv_set_anim_img_playback_time(lv_obj_t * img, uint16_t interval)
{
    lv_anim_img_ext_t* ext = lv_obj_get_ext_attr(img);
    lv_anim_set_playback_time(&ext->anim, interval);
}

/**
 * Set the image animation reapeatly play times.
 * @param img pointer to an animation image object
 */
void lv_set_anim_img_startup_repeat_count(lv_obj_t * img, uint16_t count)
{
    lv_anim_img_ext_t* ext = lv_obj_get_ext_attr(img);
    lv_anim_set_repeat_count(&ext->anim, count);
}


/**
 * Set the pixel map to display by the image
 * @param img pointer to an animation image object
 * @param data the image data
 */
void lv_anim_img_set_src(lv_obj_t * img, const void * src_img)
{
    lv_img_set_src(img, src_img);
}

/**
 * Enable the auto size feature.
 * If enabled the object size will be same as the picture size.
 * @param img pointer to an image animation
 * @param en true: auto size enable, false: auto size disable
 */
void lv_anim_img_set_auto_size(lv_obj_t * img, bool en)
{
    lv_img_set_auto_size(img, en);
}

/**
 * Set an offset for the source of an image.
 * so the image will be displayed from the new origin.
 * @param img pointer to an image animation
 * @param x: the new offset along x axis.
 */
void lv_anim_img_set_offset_x(lv_obj_t * img, lv_coord_t x)
{
    lv_img_set_offset_x(img, x);
}

/**
 * Set an offset for the source of an image.
 * so the image will be displayed from the new origin.
 * @param img pointer to an image animation
 * @param y: the new offset along y axis.
 */
void lv_anim_img_set_offset_y(lv_obj_t * img, lv_coord_t y)
{
    lv_img_set_offset_y(img, y);
}

/**
 * Set the rotation center of the image.
 * The image will be rotated around this point
 * @param img pointer to an image animation object
 * @param pivot_x rotation center x of the image
 * @param pivot_y rotation center y of the image
 */
void lv_anim_img_set_pivot(lv_obj_t * img, lv_coord_t pivot_x, lv_coord_t pivot_y)
{
    lv_img_set_pivot(img, pivot_x, pivot_y);
}

/**
 * Set the rotation angle of the image.
 * The image will be rotated around the set pivot set by `lv_img_set_pivot()`
 * @param img pointer to an image animation object
 * @param angle rotation angle in degree with 0.1 degree resolution (0..3600: clock wise)
 */
void lv_anim_img_set_angle(lv_obj_t * img, int16_t angle)
{
    lv_img_set_angle(img, angle);
}

/**
 * Set the zoom factor of the image.
 * @param img pointer to an image animation object
 * @param zoom the zoom factor.
 * - 256 or LV_ZOOM_IMG_NONE for no zoom
 * - <256: scale down
 * - >256 scale up
 * - 128 half size
 * - 512 double size
 */
void lv_anim_img_set_zoom(lv_obj_t * img, uint16_t zoom)
{
   lv_img_set_zoom(img, zoom);
}

/**
 * Enable/disable anti-aliasing for the transformations (rotate, zoom) or not
 * @param img pointer to an image animation object
 * @param antialias true: anti-aliased; false: not anti-aliased
 */
void lv_anim_img_set_antialias(lv_obj_t * img, bool antialias)
{
    lv_img_set_antialias(img, antialias);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the source of the image
 * @param img pointer to an image animation object
 * @return the image source (symbol, file name or C array)
 */
const void * lv_anim_img_get_src(lv_obj_t * img)
{
    return lv_img_get_src(img);
}

/**
 * Get the name of the file set for an image
 * @param img pointer to an image animation
 * @return file name
 */
const char * lv_anim_img_get_file_name(const lv_obj_t * img)
{
    return lv_img_get_file_name(img);
}

/**
 * Get the auto size enable attribute
 * @param img pointer to an image animation
 * @return true: auto size is enabled, false: auto size is disabled
 */
bool lv_anim_img_get_auto_size(const lv_obj_t * img)
{
    return lv_img_get_auto_size(img);
}

/**
 * Get the offset.x attribute of the img object.
 * @param img pointer to an image animation
 * @return offset.x value.
 */
lv_coord_t lv_anim_img_get_offset_x(lv_obj_t * img)
{
    return lv_img_get_offset_x(img);
}

/**
 * Get the offset.y attribute of the img object.
 * @param img pointer to an image animation
 * @return offset.y value.
 */
lv_coord_t lv_anim_img_get_offset_y(lv_obj_t * img)
{
    return lv_img_get_offset_y(img);
}

/**
 * Get the rotation center of the image.
 * @param img pointer to an image animation object
 * @param center rotation center of the image
 */
void lv_anim_img_get_pivot(lv_obj_t * img, lv_point_t * pivot)
{
    lv_img_get_pivot(img, pivot);
}

/**
 * Get the rotation angle of the image.
 * @param img pointer to an image animation object
 * @return rotation angle in degree (0..359)
 */
uint16_t lv_anim_img_get_angle(lv_obj_t * img)
{
    return lv_img_get_angle(img);
}

/**
 * Get the zoom factor of the image.
 * @param img pointer to an image animation object
 * @return zoom factor (256: no zoom)
 */
uint16_t lv_anim_img_get_zoom(lv_obj_t * img)
{
    return lv_img_get_zoom(img);
}

/**
 * Get whether the transformations (rotate, zoom) are anti-aliased or not
 * @param img pointer to an image animation object
 * @return true: anti-aliased; false: not anti-aliased
 */
bool lv_anim_img_get_antialias(lv_obj_t * img)
{
    return lv_img_get_antialias(img);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the images
 * @param img pointer to an image animationobject
 * @param clip_area the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return an element of `lv_design_res_t`
 */
static lv_design_res_t lv_anim_img_design(lv_obj_t * img, const lv_area_t * clip_area, lv_design_mode_t mode)
{

    if(mode == LV_DESIGN_COVER_CHK) {

        ancestor_design(img, clip_area, mode);

        return LV_DESIGN_RES_COVER;
    }
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        ancestor_design(img, clip_area, mode);

    }
    else if(mode == LV_DESIGN_DRAW_POST) {
        ancestor_design(img, clip_area, mode);
    }

    return LV_DESIGN_RES_OK;
}

/**
 * Signal function of the image
 * @param img pointer to an image animation object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_anim_img_signal(lv_obj_t * img, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(img, sign, param);

    return res;
}

void index_change(lv_obj_t * obj, lv_anim_value_t index)
{
    static lv_coord_t last_index = -1;
    lv_anim_img_ext_t* ext_attr = lv_obj_get_ext_attr(obj);

    if(index == ext_attr->pic_count)
    {
        return;
    }

    if(last_index != index)
    {
        lv_anim_img_set_src(obj, ext_attr->dsc[index]);
        last_index = index;
    }

}

#endif
#endif
