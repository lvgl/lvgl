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
void lv_opengles_render_texture(unsigned int texture, const lv_area_t * texture_area, lv_opa_t opa, int32_t disp_w,
                                int32_t disp_h);

#endif /* LV_USE_OPENGLES */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_OPENGLES_DRIVER_H */
