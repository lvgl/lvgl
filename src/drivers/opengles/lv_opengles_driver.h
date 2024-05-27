/**
 * @file lv_opengles_driver.h
 *
 */

#ifndef LV_OPENGLES_DRIVER_H
#define LV_OPENGLES_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#if LV_USE_OPENGLES

void lv_opengles_init(uint8_t * frame_buffer, int32_t hor, int32_t ver);

void lv_opengles_update(uint8_t * frame_buffer, int32_t hor, int32_t ver);

void lv_opengles_deinit(void);

#endif /* LV_USE_OPENGLES */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_OPENGLES_DRIVER_H */
