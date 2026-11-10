/**
* @file lv_test_alpha_scene.h
*
* The scene of `test_vg_lite_alpha`: an ordinary widget on a transparent
* screen, drawn over a background, and the means to read back what was
* rendered into the display buffer. One scene holds one widget over one
* background.
*/
#if LV_BUILD_TEST

#ifndef LV_TEST_ALPHA_SCENE_H
#define LV_TEST_ALPHA_SCENE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <lvgl/lvgl.h>
#include <lvgl_private/lvgl_private.h>

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_TEST_ALPHA_OBJ,      /**< A plain object with a background */
    LV_TEST_ALPHA_BUTTON,   /**< A themed button, without its shadow and rounded corners */
    LV_TEST_ALPHA_LABEL,    /**< A label with large text */
    LV_TEST_ALPHA_VECTOR,   /**< An object that fills a vector path in its draw event */
} lv_test_alpha_kind_t;

/** The widgets, all red, at the opacity in their name */
typedef enum {
    LV_TEST_ALPHA_WIDGET_OBJ_OPA_COVER,
    LV_TEST_ALPHA_WIDGET_OBJ_OPA_77,
    LV_TEST_ALPHA_WIDGET_BUTTON_OPA_COVER,
    LV_TEST_ALPHA_WIDGET_BUTTON_OPA_DD,
    LV_TEST_ALPHA_WIDGET_LABEL_OPA_COVER,
    LV_TEST_ALPHA_WIDGET_LABEL_OPA_77,
    LV_TEST_ALPHA_WIDGET_VECTOR_OPA_COVER,
    LV_TEST_ALPHA_WIDGET_VECTOR_OPA_77,
    LV_TEST_ALPHA_WIDGET_COUNT,
} lv_test_alpha_widget_id_t;

/** What the widget is drawn over */
typedef enum {
    LV_TEST_ALPHA_BG_NONE,          /**< Nothing: the transparent screen */
    LV_TEST_ALPHA_BG_OPAQUE,        /**< An opaque blue object */
    LV_TEST_ALPHA_BG_TRANSLUCENT,   /**< A semi-transparent blue object */
    LV_TEST_ALPHA_BG_COUNT,
} lv_test_alpha_bg_id_t;

typedef struct {
    const char * name;
    lv_test_alpha_kind_t kind;
    uint32_t color;         /**< 0xRRGGBB */
    lv_opa_t opa;
} lv_test_alpha_widget_t;

typedef struct {
    const char * name;
    uint32_t color;         /**< 0xRRGGBB */
    lv_opa_t opa;           /**< 0 for LV_TEST_ALPHA_BG_NONE */
} lv_test_alpha_bg_t;

typedef struct {
    uint8_t raw[4];         /**< The bytes in the display buffer, B G R A */
    lv_color32_t straight;  /**< The pixel decoded to straight alpha */
} lv_test_alpha_pixel_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

extern const lv_test_alpha_widget_t lv_test_alpha_widgets[LV_TEST_ALPHA_WIDGET_COUNT];
extern const lv_test_alpha_bg_t lv_test_alpha_bgs[LV_TEST_ALPHA_BG_COUNT];

/**
 * Make `screen` transparent and create the widget over the background on it.
 * The display should have an ARGB8888 color format, so that its bottom layer
 * is transparent too and the cleared buffer stays transparent where nothing is
 * drawn.
 */
void lv_test_alpha_scene_create(lv_obj_t * screen, lv_test_alpha_widget_id_t widget, lv_test_alpha_bg_id_t bg);

/**
 * Set the font of the labels of the scenes created afterwards (by default
 * the largest built-in font, Montserrat 48).
 */
void lv_test_alpha_scene_set_label_font(const lv_font_t * font);

/**
 * The point to sample the widget at: the center of the widget, or for a label
 * its most opaque pixel (inside a glyph). For a label this renders the label
 * over nothing on the active screen of the default display, and cleans the
 * screen afterwards.
 */
lv_point_t lv_test_alpha_scene_sample_point(lv_test_alpha_widget_id_t widget);

/**
 * The straight-alpha color the widget should produce over the background: the
 * widget's color and opacity composited over the background's with
 * source-over.
 */
lv_color32_t lv_test_alpha_expected(lv_test_alpha_widget_id_t widget, lv_test_alpha_bg_id_t bg);

/**
 * Read a pixel from a draw buffer holding the whole screen, interpreted by the
 * buffer's color format: premultiplied only if the format is
 * LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED.
 * @param buf           the display's draw buffer after a refresh
 * @param point         where, see lv_test_alpha_scene_sample_point()
 */
lv_test_alpha_pixel_t lv_test_alpha_scene_read(const lv_draw_buf_t * buf, lv_point_t point);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEST_ALPHA_SCENE_H*/

#endif /*LV_BUILD_TEST*/
