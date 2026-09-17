/**
 * @file lv_api_map_v9_6.h
 *
 */

#ifndef LV_API_MAP_V9_6_H
#define LV_API_MAP_V9_6_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../core/lv_area.h"

/* `display` is evaluated more than once, don't pass an expression with side effects*/
#define lv_display_rotate_point_ccw(display, point)                            \
    lv_point_rotate(point, lv_display_get_rotation(display),                   \
                    lv_display_get_original_horizontal_resolution(display),    \
                    lv_display_get_original_vertical_resolution(display))

#define LV_DISPLAY_ROTATION_0   LV_ROTATION_0
#define LV_DISPLAY_ROTATION_90  LV_ROTATION_90
#define LV_DISPLAY_ROTATION_180 LV_ROTATION_180
#define LV_DISPLAY_ROTATION_270 LV_ROTATION_270
typedef lv_rotation_t lv_display_rotation_t;

#define lv_indev_set_ccw(indev) lv_indev_set_rotation_dir(indev, LV_ROTATION_DIR_CCW)
#define lv_indev_clear_ccw(indev) lv_indev_set_rotation_dir(indev, LV_ROTATION_DIR_CW)
#define lv_indev_get_ccw(indev) (lv_indev_get_rotation_dir(indev) == LV_ROTATION_DIR_CCW)

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_API_MAP_V9_6_H */
