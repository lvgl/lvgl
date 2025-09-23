/**
 * @file lv_opengles_texture_private.h
 *
 */

#ifndef LV_OPENGLES_TEXTURE_PRIVATE_H
#define LV_OPENGLES_TEXTURE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    unsigned int texture_id;
    uint8_t * fb1;
} lv_opengles_texture_t;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_OPENGLES_TEXTURE_PRIVATE_H */
