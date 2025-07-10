#ifndef LV_TEST_CONF_VG_LITE_H
#define LV_TEST_CONF_VG_LITE_H

#define LV_USE_DRAW_VGLITE 1

#define LV_VGLITE_USE_GPU_INIT 1

#define LV_VGLITE_VECTOR 0

/* Make the VGLite pipeline run to completion */
#define LV_USE_VGLITE_DRAW_THREAD 0

#define LV_USE_VGLITE_DRAW_ASYNC 0

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
#define LV_DRAW_TRANSFORM_USE_MATRIX            0

#define LV_USE_LOG 1

#define LV_LOG_USE_FILE_LINE 1

#define LV_DRAW_SW_DRAW_UNIT_CNT 1

#define LV_DRAW_BUF_STRIDE_ALIGN                64

#endif /* LV_TEST_CONF_VG_LITE_H */
