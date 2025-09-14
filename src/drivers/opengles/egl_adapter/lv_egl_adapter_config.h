/**
 * @file lv_egl_adapter_config.h
 *
 */

#ifndef GLMARK2_GL_VISUAL_CONFIG_H_
#define GLMARK2_GL_VISUAL_CONFIG_H_

/*********************
 *      DEFINES
 *********************/

/*********************
 *      INCLUDES
 *********************/

#include <stdint.h>
#include <stdbool.h>
//#include "private/__lv_egl_adapter.h"

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Configuration parameters for a GL visual
 */
struct lv_egl_adapter_config {
    /**
     * How well a lv_egl_adapter_config matches another target config.
     *
     * The returned score has no meaning on its own. Its only purpose is
     * to allow comparison of how well different configs match a target
     * config, with a higher scores denoting a better match.
     *
     * Also note that this operation is not commutative:
     * a.match_score(b) != b.match_score(a)
     *
     * @return the match score
     */

    int id;
    int red;
    int green;
    int blue;
    int alpha;
    int depth;
    int stencil;
    int buffer;
    int samples;
    bool vsync;

};

#ifndef __LV_EGL_ADAPTER_CONFIG_DEFINED
    #define __LV_EGL_ADAPTER_CONFIG_DEFINED
    typedef struct lv_egl_adapter_config * lv_egl_adapter_config_t;
#endif /* __LV_EGL_ADAPTER_CONFIG_DEFINED */

#ifdef __cplusplus
extern "C" {
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_egl_adapter_config_make_default(void *);
//int lv_egl_adapter_config_match_score(lv_egl_adapter_config_t actual, lv_egl_adapter_config_t target);
int lv_egl_adapter_config_match_score(void * viscon_ptr, lv_egl_adapter_config_t target);

//int judge_viability(int component, int target, int scale);
lv_egl_adapter_config_t lv_egl_adapter_config_create();
lv_egl_adapter_config_t lv_egl_adapter_config_by_id(uint32_t requested_mode_config_id);
void lv_egl_adapter_config_cleanup(void ** viscon_ptr);

void lv_egl_adapter_config_set_config_id(void * viscon_ptr, uint32_t requested_mode_config_id);
void lv_egl_adapter_config_set_alpha_bit_count(void * viscon_ptr, uint32_t alpha_bits);
void lv_egl_adapter_config_set_depth_bit_count(void * viscon_ptr, uint32_t depth_bits);
void lv_egl_adapter_config_set_sample_count(void * viscon_ptr, uint32_t num_samples);
void lv_egl_adapter_config_set_rgb_bit_count(void * viscon_ptr, uint32_t red_bits, uint32_t green_bits,
                                             uint32_t blue_bits);
void lv_egl_adapter_config_set_rgba_bit_count(void * viscon_ptr, uint32_t red_bits, uint32_t green_bits,
                                              uint32_t blue_bits, uint32_t alpha_bits);
void lv_egl_adapter_config_set_vsync(void * viscon_ptr, bool enabled);
bool lv_egl_adapter_config_get_vsync(void * viscon_ptr);

#ifdef __cplusplus
}
#endif

/**********************
 *      MACROS
 **********************/

#endif
