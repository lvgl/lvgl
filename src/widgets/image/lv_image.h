/**
 * @file lv_img.h
 *
 */

#ifndef LV_IMAGE_H
#define LV_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_IMG != 0

/*Testing of dependencies*/
#if LV_USE_LABEL == 0
#error "lv_img: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL 1)"
#endif

#include "../../core/lv_obj.h"
#include "../../misc/lv_fs.h"
#include "../../draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Data of image
 */
typedef struct {
    lv_obj_t obj;
    const void * src;   /*Image source: Pointer to an array or a file or a symbol*/
    lv_point_t offset;
    int32_t w;          /*Width of the image (Handled by the library)*/
    int32_t h;          /*Height of the image (Handled by the library)*/
    uint32_t rotation;    /*rotation angle of the image*/
    uint32_t scale_x;      /*256 means no zoom, 512 double size, 128 half size*/
    uint32_t scale_y;      /*256 means no zoom, 512 double size, 128 half size*/
    lv_point_t pivot;     /*rotation center of the image*/
    uint8_t src_type : 2;  /*See: lv_image_src_t*/
    uint8_t cf : 5;        /*Color format from `lv_color_format_t`*/
    uint8_t antialias : 1; /*Apply anti-aliasing in transformations (rotate, zoom)*/
    uint8_t obj_size_mode: 2; /*Image size mode when image size and object size is different.*/
} lv_image_t;

extern const lv_obj_class_t lv_image_class;

/**
 * Image size mode, when image size and object size is different
 */
enum _lv_image_size_mode_t {
    /** Zoom doesn't affect the coordinates of the object,
     *  however if zoomed in the image is drawn out of the its coordinates.
     *  The layout's won't change on zoom */
    LV_IMAGE_SIZE_MODE_VIRTUAL = 0,

    /** If the object size is set to SIZE_CONTENT, then object size equals zoomed image size.
     *  It causes layout recalculation.
     *  If the object size is set explicitly, the image will be cropped when zoomed in.*/
    LV_IMAGE_SIZE_MODE_REAL,
};

#ifdef DOXYGEN
typedef _lv_image_size_mode_t lv_image_size_mode_t;
#else
typedef uint8_t lv_image_size_mode_t;
#endif /*DOXYGEN*/

#if LV_USE_OBJ_PROPERTY
enum {
    LV_PROPERTY_ID(IMAGE, SRC,        LV_PROPERTY_TYPE_POINTER,   0),
    LV_PROPERTY_ID(IMAGE, OFFSET_X,   LV_PROPERTY_TYPE_INT,       1),
    LV_PROPERTY_ID(IMAGE, OFFSET_Y,   LV_PROPERTY_TYPE_INT,       2),
    LV_PROPERTY_ID(IMAGE, ROTATION,   LV_PROPERTY_TYPE_INT,       3),
    LV_PROPERTY_ID(IMAGE, PIVOT,      LV_PROPERTY_TYPE_POINTER,   4),
    LV_PROPERTY_ID(IMAGE, SCALE,      LV_PROPERTY_TYPE_INT,       5),
    LV_PROPERTY_ID(IMAGE, ANTIALIAS,  LV_PROPERTY_TYPE_INT,       6),
    LV_PROPERTY_ID(IMAGE, SIZE_MODE,  LV_PROPERTY_TYPE_INT,       7),
    LV_PROPERTY_IMAGE_END,
};
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create an image object
 * @param parent pointer to an object, it will be the parent of the new image
 * @return pointer to the created image
 */
lv_obj_t * lv_image_create(lv_obj_t * parent);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the image data to display on the object
 * @param obj       pointer to an image object
 * @param src       1) pointer to an ::lv_image_dsc_t descriptor (converted by LVGL's image converter) (e.g. &my_img) or
 *                  2) path to an image file (e.g. "S:/dir/img.bin")or
 *                  3) a SYMBOL (e.g. LV_SYMBOL_OK)
 */
void lv_image_set_src(lv_obj_t * obj, const void * src);

/**
 * Set an offset for the source of an image so the image will be displayed from the new origin.
 * @param obj       pointer to an image
 * @param x         the new offset along x axis.
 */
void lv_image_set_offset_x(lv_obj_t * obj, int32_t x);

/**
 * Set an offset for the source of an image.
 * so the image will be displayed from the new origin.
 * @param obj       pointer to an image
 * @param y         the new offset along y axis.
 */
void lv_image_set_offset_y(lv_obj_t * obj, int32_t y);


/**
 * Set the rotation angle of the image.
 * The image will be rotated around the set pivot set by `lv_image_set_pivot()`
 * Note that indexed and alpha only images can't be transformed.
 * @param obj       pointer to an image object
 * @param angle  rotation in degree with 0.1 degree resolution (0..3600: clock wise)
 */
void lv_image_set_rotation(lv_obj_t * obj, int32_t angle);

/**
 * Set the rotation center of the image.
 * The image will be rotated around this point.
 * x, y can be set with value of LV_PCT, lv_image_get_pivot will return the true pixel coordinate of pivot in this case.
 * @param obj       pointer to an image object
 * @param x         rotation center x of the image
 * @param y         rotation center y of the image
 */
void lv_image_set_pivot(lv_obj_t * obj, int32_t x, int32_t y);

/**
 * Set pivot similar to get_pivot
 */
static inline void _lv_image_set_pivot(lv_obj_t * obj, lv_point_t * pivot)
{
    lv_image_set_pivot(obj, pivot->x, pivot->y);
}


/**
 * Set the zoom factor of the image.
 * Note that indexed and alpha only images can't be transformed.
 * @param img       pointer to an image object
 * @param zoom      the zoom factor.
 * @example 256 or LV_ZOOM_IMAGE_NONE for no zoom
 * @example <256: scale down
 * @example >256 scale up
 * @example 128 half size
 * @example 512 double size
 */
void lv_image_set_scale(lv_obj_t * obj, uint32_t zoom);

/**
 * Set the horizontal zoom factor of the image.
 * Note that indexed and alpha only images can't be transformed.
 * @param img       pointer to an image object
 * @param zoom      the zoom factor.
 * @example 256 or LV_ZOOM_IMAGE_NONE for no zoom
 * @example <256: scale down
 * @example >256 scale up
 * @example 128 half size
 * @example 512 double size
 */
void lv_image_set_scale_x(lv_obj_t * obj, uint32_t zoom);

/**
 * Set the vertical zoom factor of the image.
 * Note that indexed and alpha only images can't be transformed.
 * @param img       pointer to an image object
 * @param zoom      the zoom factor.
 * @example 256 or LV_ZOOM_IMAGE_NONE for no zoom
 * @example <256: scale down
 * @example >256 scale up
 * @example 128 half size
 * @example 512 double size
 */
void lv_image_set_scale_y(lv_obj_t * obj, uint32_t zoom);


/**
 * Enable/disable anti-aliasing for the transformations (rotate, zoom) or not.
 * The quality is better with anti-aliasing looks better but slower.
 * @param obj       pointer to an image object
 * @param antialias true: anti-aliased; false: not anti-aliased
 */
void lv_image_set_antialias(lv_obj_t * obj, bool antialias);

/**
 * Set the image object size mode.
 *
 * @param obj       pointer to an image object
 * @param mode      the new size mode.
 */
void lv_image_set_size_mode(lv_obj_t * obj, lv_image_size_mode_t mode);
/*=====================
 * Getter functions
 *====================*/

/**
 * Get the source of the image
 * @param obj       pointer to an image object
 * @return          the image source (symbol, file name or ::lv-img_dsc_t for C arrays)
 */
const void * lv_image_get_src(lv_obj_t * obj);

/**
 * Get the offset's x attribute of the image object.
 * @param obj       pointer to an image
 * @return          offset X value.
 */
int32_t lv_image_get_offset_x(lv_obj_t * obj);

/**
 * Get the offset's y attribute of the image object.
 * @param obj       pointer to an image
 * @return          offset Y value.
 */
int32_t lv_image_get_offset_y(lv_obj_t * obj);

/**
 * Get the rotation of the image.
 * @param obj       pointer to an image object
 * @return      rotation in 0.1 degrees (0..3600)
 */
int32_t lv_image_get_rotation(lv_obj_t * obj);

/**
 * Get the pivot (rotation center) of the image.
 * If pivot is set with LV_PCT, convert it to px before return.
 * @param obj       pointer to an image object
 * @param pivot     store the rotation center here
 */
void lv_image_get_pivot(lv_obj_t * obj, lv_point_t * pivot);

/**
 * Get the zoom factor of the image.
 * @param obj       pointer to an image object
 * @return          zoom factor (256: no zoom)
 */
int32_t lv_image_get_scale(lv_obj_t * obj);

/**
 * Get the horizontal zoom factor of the image.
 * @param obj       pointer to an image object
 * @return          zoom factor (256: no zoom)
 */
int32_t lv_image_get_scale_x(lv_obj_t * obj);

/**
 * Get the vertical zoom factor of the image.
 * @param obj       pointer to an image object
 * @return          zoom factor (256: no zoom)
 */
int32_t lv_image_get_scale_y(lv_obj_t * obj);

/**
 * Get whether the transformations (rotate, zoom) are anti-aliased or not
 * @param obj       pointer to an image object
 * @return          true: anti-aliased; false: not anti-aliased
 */
bool lv_image_get_antialias(lv_obj_t * obj);

/**
 * Get the size mode of the image
 * @param obj       pointer to an image object
 * @return          element of @ref lv_image_size_mode_t
 */
lv_image_size_mode_t lv_image_get_size_mode(lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

/** Use this macro to declare an image in a C file*/
#define LV_IMAGE_DECLARE(var_name) extern const lv_image_dsc_t var_name;

#endif /*LV_USE_IMG*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IMAGE_H*/
