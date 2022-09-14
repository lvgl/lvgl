// honey the codewitch (github codewitch-honey-crisis)
#ifndef LV_TINY_TTF_H
#define LV_TINY_TTF_H
#include "../../../lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "../../../lv_conf_internal.h"
// create a font from the specified file or path with the specified line height.
lv_font_t * lv_tiny_ttf_create_file(const char * path, lv_coord_t line_height);
// create a font from the specified data pointer with the specified line height.
lv_font_t * lv_tiny_ttf_create_data(const void * data, size_t data_size, lv_coord_t line_height);
// destroy a font previously created with lv_tiny_ttf_create_xxxx()
void lv_tiny_ttf_destroy(lv_font_t * font);
#ifdef __cplusplus
}
#endif
#endif // LV_TINY_TTF_H