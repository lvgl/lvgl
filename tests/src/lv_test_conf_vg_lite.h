#ifndef LV_TEST_CONF_VG_LITE_H
#define LV_TEST_CONF_VG_LITE_H

/* Use VG-Lite GPU. */
#define LV_USE_DRAW_VG_LITE 1

/* Enable VG-Lite custom external 'gpu_init()' function */
#define LV_VG_LITE_USE_GPU_INIT 1

/* Enable VG-Lite assert. */
#define LV_VG_LITE_USE_ASSERT 1

/* VG-Lite flush commit trigger threshold. GPU will try to batch these many draw tasks. */
#define LV_VG_LITE_FLUSH_MAX_COUNT 0

/* Enable border to simulate shadow
 * NOTE: which usually improves performance,
 * but does not guarantee the same rendering quality as the software. */
#define LV_VG_LITE_USE_BOX_SHADOW 1

/* VG-Lite gradient maximum cache number.
 * NOTE: The memory usage of a single gradient image is 4K bytes.
 */
#define LV_VG_LITE_GRAD_CACHE_CNT 32

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
