/**
 * @file lv_egl_adapter_config.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_egl_adapter_config.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/*********************
*      DEFINES
*********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
int judge_viability(int actual, int goal, bool least_over);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_egl_adapter_config_make_default(void * viscon_ptr)
{
    lv_egl_adapter_config_t adapter_config = (lv_egl_adapter_config_t)viscon_ptr;
    adapter_config->id = 0;
    adapter_config->red = 1;
    adapter_config->green = 1;
    adapter_config->blue = 1;
    adapter_config->alpha = 1;
    adapter_config->depth = 1;
    adapter_config->stencil = -1;
    adapter_config->buffer = 1;
    adapter_config->samples = 0;
    adapter_config->vsync = true;

}

void lv_egl_adapter_config_set_config_id(void * viscon_ptr, uint32_t requested_mode_config_id)
{
    ((lv_egl_adapter_config_t)viscon_ptr)->id = requested_mode_config_id;
}
void lv_egl_adapter_config_set_vsync(void * viscon_ptr, bool enabled)
{
    ((lv_egl_adapter_config_t)viscon_ptr)->vsync = enabled;
}
bool lv_egl_adapter_config_get_vsync(void * viscon_ptr)
{
    return ((lv_egl_adapter_config_t)viscon_ptr)->vsync;
}
void lv_egl_adapter_config_set_alpha_bit_count(void * viscon_ptr, uint32_t alpha_bits)
{
    ((lv_egl_adapter_config_t)viscon_ptr)->alpha = alpha_bits;
}
void lv_egl_adapter_config_set_depth_bit_count(void * viscon_ptr, uint32_t depth_bits)
{
    ((lv_egl_adapter_config_t)viscon_ptr)->depth = depth_bits;
}
void lv_egl_adapter_config_set_sample_count(void * viscon_ptr, uint32_t num_samples)
{
    ((lv_egl_adapter_config_t)viscon_ptr)->samples = num_samples;
}
void lv_egl_adapter_config_set_rgb_bit_count(void * viscon_ptr, uint32_t red_bits, uint32_t green_bits,
                                             uint32_t blue_bits)
{
    lv_egl_adapter_config_t adapter_config = (lv_egl_adapter_config_t)viscon_ptr;
    adapter_config->red = red_bits;
    adapter_config->green = green_bits;
    adapter_config->blue = blue_bits;
}
void lv_egl_adapter_config_set_rgba_bit_count(void * viscon_ptr, uint32_t red_bits, uint32_t green_bits,
                                              uint32_t blue_bits, uint32_t alpha_bits)
{
    lv_egl_adapter_config_set_rgb_bit_count(viscon_ptr, red_bits, green_bits, blue_bits);
    lv_egl_adapter_config_set_alpha_bit_count(viscon_ptr, alpha_bits);
}

int bugged_lv_egl_adapter_config_match_score(lv_egl_adapter_config_t actual, lv_egl_adapter_config_t target)
{
    int score = 0;

    /* A target config id trumps all other considerations and must match
     * exactly. */
    if(target->id)
        return target->id == actual->id ? 1000 : -1000;

    /*
     * R,G,B,A integer values are at most 8 bits wide (for current widespread
     * hardware), so we need to scale them by 4 to get them in the [0,32] range.
     */
    const bool MOST_OVER = false;
    const bool LEAST_OVER = true;

    score += judge_viability(actual->red,      target->red,        MOST_OVER);
    score += judge_viability(actual->green,    target->green,      MOST_OVER);
    score += judge_viability(actual->blue,     target->blue,       MOST_OVER);
    score += judge_viability(actual->alpha,    target->alpha,      MOST_OVER);
    score *= 4;

    score += judge_viability(actual->depth,    target->depth,      MOST_OVER);
    score += judge_viability(actual->stencil,  target->stencil,    MOST_OVER);
    score += judge_viability(actual->buffer,   target->buffer,     MOST_OVER);

    score -= judge_viability(actual->samples,  target->samples,    LEAST_OVER);

    return score;
}


int lv_egl_adapter_config_score_component(int component, int target, int scale)  //const
{
    /*
     * The maximum (positive) score that can be returned is based
     * on the maximum bit width of the components. We assume this to
     * be 32 bits, which is a reasonable assumption for current platforms.
     */
    const int MAXIMUM_COMPONENT_SCORE = 32;
    const int UNACCEPTABLE_COMPONENT_PENALTY = -1000;
    int score = 0;

    if(((component > 0) && (target == 0)) ||
       ((component == 0) && (target > 0))) {
        /*
         * Penalize components that are not present but have been requested,
         * and components that have been excluded but are present.
         */
        score = UNACCEPTABLE_COMPONENT_PENALTY;
    }
    else if(component == target) {
        /* Reward exact matches with the maximum per component score */
        score = MAXIMUM_COMPONENT_SCORE;
    }
    else if((component > 8) && (target <= 8) && (scale > 1)) {
        /* Penalize RGBA component widths larger than 8, since they are
         * unlikely to be what the users want or properly supported for
         * display. Such widths can still be used, but only if explicitly
         * requested. */
        score = UNACCEPTABLE_COMPONENT_PENALTY;
    }
    else {
        /*
         * Reward deeper than requested component values, penalize shallower
         * than requested component values. Because the ranges of component
         * values vary we use a scaling factor to even them out, so that the
         * score for all components ranges from [0,MAXIMUM_COMPONENT_SCORE).
         * If scale > 0, we reward the largest positive difference from target,
         * otherwise the smallest positive difference from target.
         * We also reward the smallest positive difference from the target,
         * if the target < 0, i.e., we don't care about this value.
         */
        int diff = ((scale < 0) ? -scale : scale) * (component - target);
        if(diff > 0) {
            score = (scale < 0 || target < 0) ?
                    MAXIMUM_COMPONENT_SCORE - diff : diff;
            score = (MAXIMUM_COMPONENT_SCORE < score) ? MAXIMUM_COMPONENT_SCORE : score;
            score = (score > 0) ? score : 0;
        }
        else {
            score = 0;
        }
    }

    return score;
}

int lv_egl_adapter_config_match_score(void * viscon_ptr, lv_egl_adapter_config_t target)
{
    lv_egl_adapter_config_t adapter_config = (lv_egl_adapter_config_t)viscon_ptr;
    int score = 0;

    /* A target config id trumps all other considerations and must match
     * exactly. */
    if(target->id)
        return target->id == adapter_config->id ? 1000 : -1000;

    /*
     * R,G,B,A integer values are at most 8 bits wide (for current widespread
     * hardware), so we need to scale them by 4 to get them in the [0,32] range.
     */
    score += lv_egl_adapter_config_score_component(adapter_config->red,      target->red,     4);
    score += lv_egl_adapter_config_score_component(adapter_config->green,    target->green,   4);
    score += lv_egl_adapter_config_score_component(adapter_config->blue,     target->blue,    4);
    score += lv_egl_adapter_config_score_component(adapter_config->alpha,    target->alpha,   4);
    score += lv_egl_adapter_config_score_component(adapter_config->depth,    target->depth,   1);
    score += lv_egl_adapter_config_score_component(adapter_config->stencil,  target->stencil, 1);
    score += lv_egl_adapter_config_score_component(adapter_config->buffer,   target->buffer,  1);
    score += lv_egl_adapter_config_score_component(adapter_config->samples,  target->samples, -1);

    return score;
}


lv_egl_adapter_config_t lv_egl_adapter_config_create()
{
    lv_egl_adapter_config_t local_visual_config = (lv_egl_adapter_config_t)malloc(sizeof(struct lv_egl_adapter_config));
    if(local_visual_config) {
        lv_egl_adapter_config_make_default(local_visual_config);
    }
    return local_visual_config;
}

lv_egl_adapter_config_t lv_egl_adapter_config_by_id(uint32_t requested_mode_config_id)
{
    lv_egl_adapter_config_t local_visual_config = (lv_egl_adapter_config_t)malloc(sizeof(struct lv_egl_adapter_config));
    if(local_visual_config) {
        lv_egl_adapter_config_make_default(local_visual_config);
        lv_egl_adapter_config_set_config_id(local_visual_config, requested_mode_config_id);
    }
    return local_visual_config;
}

void lv_egl_adapter_config_cleanup(void ** viscon_ptr)
{
    lv_egl_adapter_config_t adapter_config = * (lv_egl_adapter_config_t *)viscon_ptr;
    if(adapter_config) free(adapter_config);
    *viscon_ptr = NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

int judge_viability(int actual, int goal, bool least_over)
{
    const int WIN = 32;
    const int LOSS = -1000;
    const int DRAW = 0;

    if(actual == goal) {
        return WIN;
    }
    else if(((actual > 0) && (goal == 0)) ||
            ((actual == 0) && (goal > 0)) ||
            ((actual > 8) && (goal <= 8))) {
        return LOSS;
    }
    else {
        int diff = actual - goal;
        if(diff > 0) {
            int rank = (least_over || (goal < 0)) ?
                       WIN - diff : diff;
            return (rank > WIN) ? WIN : (rank > 0) ? rank : 0;
        }
        return DRAW;
    }
}