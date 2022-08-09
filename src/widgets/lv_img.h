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
#include "../lv_conf_internal.h"

#if LV_USE_IMG != 0

#include "../core/lv_obj.h"
#include "../misc/lv_fs.h"
#include "../draw/lv_draw.h"
#include "../draw/lv_img_src.h"
#include "../misc/lv_timer.h"

/*********************
 *      DEFINES
 *********************/
/** Use this macro to declare an image in a C file*/
#define LV_IMG_DECLARE(var_name) extern const lv_img_dsc_t var_name;

/**********************
 *      TYPEDEFS
 **********************/

/** Some image are animated.
 *  In that case, you might want to control the animation with `lv_img_set_play_mode`
 *  These are flags you'll combine with | to express your need */
typedef enum {
    LV_IMG_CTRL_FORWARD  = 0,   /**< Let the animation play forward. */
    LV_IMG_CTRL_BACKWARD = 1,   /**< Let the animation play backward, if supported */
    LV_IMG_CTRL_PAUSE    = 2,   /**< Pause the animation at the next frame */
    LV_IMG_CTRL_PLAY     = 0,   /**< Play the animation */
    LV_IMG_CTRL_LOOP     = 8,   /**< When reached the destination frame, loop back */
    LV_IMG_CTRL_STOPAT   = 4,   /**< Stop at a given frame */


    LV_IMG_CTRL_MARKED = 0x80,  /**< Internal, don't use this */
} lv_img_ctrl_t;


/**
 * Descriptor of an image
 */
typedef struct {
    lv_obj_t            obj;
    lv_img_src_t    *   src;        /**< Image source*/
    lv_point_t          offset;
    lv_coord_t          w;          /**< Width of the image in px (Handled by the library)*/
    lv_coord_t          h;          /**< Height of the image in px (Handled by the library)*/
    lv_point_t          pivot;      /**< rotation center of the image, in px*/
    uint16_t            angle;      /**< rotation angle in 1/10th of degree of the image*/
    uint16_t            zoom;       /**< 256 means no zoom, 512 double size, 128 half size*/
    uint8_t             cf : 5;        /**< Color format from `lv_img_color_format_t`*/
    uint8_t             antialias : 1; /**< Apply anti-aliasing in transformations (rotate, zoom)*/
    uint8_t             obj_size_mode: 2; /**< Image size mode if image and object sizes differ */

    uint8_t             ctrl;       /**< The current control flags */
    lv_img_dec_ctx_t  * dec_ctx;    /**< Additional decoder context */
    uint8_t             caps;       /**< Decoder capabilities */
    lv_timer_t     *    anim_timer; /**< The timer task for animated images */
} lv_img_t;



extern const lv_obj_class_t lv_img_class;

/**
 * Image size mode, when image size and object size is different
 */
enum {
    /** Zoom doesn't affect the coordinates of the object,
     *  however if zoomed in the image is drawn out of the its coordinates.
     *  The layout's won't change on zoom */
    LV_IMG_SIZE_MODE_VIRTUAL = 0,

    /** If the object size is set to SIZE_CONTENT, then object size equals zoomed image size.
     *  It causes layout recalculation.
     *  If the object size is set explicitly, the image will be cropped when zoomed in.*/
    LV_IMG_SIZE_MODE_REAL,
};

typedef uint8_t lv_img_size_mode_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an image object
 * @param parent pointer to an object, it will be the parent of the new image
 * @return pointer to the created image
 */
lv_obj_t * lv_img_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

/**
 * Some image format support animation. In that case, you might want to control it.
 * @param obj       pointer to an image object
 * @param ctrl      A ORed mask of lv_img_ctrl_t flags you want
 */
void lv_img_set_play_mode(lv_obj_t * obj, const lv_img_ctrl_t ctrl);

/**
 * Set the current frame of the animation
 * @param obj       pointer to an image object
 * @param index     Index of the frame to reach
 * @warning This will only work once the source of the image is set and the image is animated
 */
lv_res_t lv_img_set_current_frame(lv_obj_t * obj, const lv_frame_index_t index);

/**
 * Set the stop frame for the animation
 * @param obj       pointer to an image object
 * @param index     Index of the frame to stop at. Will emit a EVENT_READY when reached.
 * @param forward   If 1, the animation plays forward, else plays backward
 * @warning This will only work once the source of the image is set and the image is animated
 */
lv_res_t lv_img_set_stop_at_frame(lv_obj_t * obj, const lv_frame_index_t index, const int forward);

/**
 * Set the image source to display on the the object
 *
 * @param obj       pointer to an image object
 * @param src       an image source instance that can be created from:
 *                  1. from a file: lv_img_src_from_file("S:/path/to/file")
 *                  2. from encoded image data: lv_img_src_from_data(&some_data, data_length)
 *                  3. from symbol: lv_img_src_from_symbol("Ok " LV_SYMBOL_OK)
 *                  4. from raw image converter: lv_img_src_from_raw(&img_bin)
 *                  5. from a void* as previous LVGL version code: lv_img_src_parse(void*)
 *
 * @return LV_RES_OK    If the image was correctly parsed
 */
lv_res_t lv_img_set_src(lv_obj_t * obj, lv_img_src_t * src);

/**
 * Set an offset for the source of an image so the image will be displayed from the new origin.
 * @param obj       pointer to an image
 * @param x         the new offset along x axis.
 */
void lv_img_set_offset_x(lv_obj_t * obj, lv_coord_t x);

/**
 * Set an offset for the source of an image.
 * so the image will be displayed from the new origin.
 * @param obj       pointer to an image
 * @param y         the new offset along y axis.
 */
void lv_img_set_offset_y(lv_obj_t * obj, lv_coord_t y);


/**
 * Set the rotation angle of the image.
 * The image will be rotated around the set pivot set by `lv_img_set_pivot()`
 * @param obj       pointer to an image object
 * @param angle     rotation angle in degree with 0.1 degree resolution (0..3600: clock wise)
 */
void lv_img_set_angle(lv_obj_t * obj, int16_t angle);

/**
 * Set the rotation center of the image.
 * The image will be rotated around this point
 * @param obj       pointer to an image object
 * @param x         rotation center x of the image
 * @param y         rotation center y of the image
 */
void lv_img_set_pivot(lv_obj_t * obj, lv_coord_t x, lv_coord_t y);


/**
 * Set the zoom factor of the image.
 * @param img       pointer to an image object
 * @param zoom      the zoom factor.
 * @example 256 or LV_ZOOM_IMG_NONE for no zoom
 * @example <256: scale down
 * @example >256 scale up
 * @example 128 half size
 * @example 512 double size
 */
void lv_img_set_zoom(lv_obj_t * obj, uint16_t zoom);

/**
 * Enable/disable anti-aliasing for the transformations (rotate, zoom) or not.
 * The quality is better with anti-aliasing looks better but slower.
 * @param obj       pointer to an image object
 * @param antialias true: anti-aliased; false: not anti-aliased
 */
void lv_img_set_antialias(lv_obj_t * obj, bool antialias);

/**
 * Set the image object size mode.
 *
 * @param obj       pointer to an image object
 * @param mode      the new size mode.
 */
void lv_img_set_size_mode(lv_obj_t * obj, lv_img_size_mode_t mode);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the source of the image
 * @param obj       pointer to an image object
 * @return          the image source data (symbol, file name or data for C arrays)
 */
lv_img_src_t * lv_img_get_src(lv_obj_t * obj);


/**
 * Get the current play control mode for this image.
 * @param obj       pointer to an image object
 * @return          the image play control mode if applicable
 */
lv_img_ctrl_t lv_img_get_play_mode(lv_obj_t * obj);

/**
 * Get the total number of frames in the animation
 * @param obj       pointer to an image object
 * @return          the total number of frames in the animation (or 1 if it's not animated)
 */
size_t lv_img_get_totalframes(lv_obj_t * obj);

/**
 * Get the current frame of the animation
 * @param obj       pointer to an image object
 * @return          the total number of frames in the animation (or 1 if it's not animated)
 */
size_t lv_img_get_current_frame(lv_obj_t * obj);

/**
 * Get the offset's x attribute of the image object.
 * @param img       pointer to an image
 * @return          offset X value.
 */
lv_coord_t lv_img_get_offset_x(lv_obj_t * obj);

/**
 * Get the offset's y attribute of the image object.
 * @param obj       pointer to an image
 * @return          offset Y value.
 */
lv_coord_t lv_img_get_offset_y(lv_obj_t * obj);

/**
 * Get the rotation angle of the image.
 * @param obj       pointer to an image object
 * @return      rotation angle in 0.1 degrees (0..3600)
 */
uint16_t lv_img_get_angle(lv_obj_t * obj);

/**
 * Get the pivot (rotation center) of the image.
 * @param img       pointer to an image object
 * @param pivot     store the rotation center here
 */
void lv_img_get_pivot(lv_obj_t * obj, lv_point_t * pivot);

/**
 * Get the zoom factor of the image.
 * @param obj       pointer to an image object
 * @return          zoom factor (256: no zoom)
 */
uint16_t lv_img_get_zoom(lv_obj_t * obj);

/**
 * Get whether the transformations (rotate, zoom) are anti-aliased or not
 * @param obj       pointer to an image object
 * @return          true: anti-aliased; false: not anti-aliased
 */
bool lv_img_get_antialias(lv_obj_t * obj);

/**
 * Get the size mode of the image
 * @param obj       pointer to an image object
 * @return          element of @ref lv_img_size_mode_t
 */
lv_img_size_mode_t lv_img_get_size_mode(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_IMG*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMG_H*/
