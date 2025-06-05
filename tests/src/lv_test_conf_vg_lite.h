#ifndef LV_TEST_CONF_VG_LITE_H
#define LV_TEST_CONF_VG_LITE_H

/* Use VG-Lite GPU. */
#define LV_USE_DRAW_VGLITE 1

/* Disable 16 pixels alignment */
#define LV_VG_LITE_THORVG_16PIXELS_ALIGN 0

/* Enable ThorVG internal rendering engine */
#define LV_USE_THORVG_INTERNAL      1

/* Simulate VG-Lite hardware using ThorVG */
#define LV_USE_VG_LITE_THORVG       1

/* Enable performance monitor log mode for build test */
#define LV_USE_PERF_MONITOR_LOG_MODE 1

/*Using matrix for transformations.
 *Requirements:
    `LV_USE_MATRIX = 1`.
    The rendering engine needs to support 3x3 matrix transformations.*/
#define LV_DRAW_TRANSFORM_USE_MATRIX            1

#endif /* LV_TEST_CONF_VG_LITE_H */
