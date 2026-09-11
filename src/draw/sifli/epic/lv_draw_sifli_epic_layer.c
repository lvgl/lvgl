/**
 * @file lv_draw_sifli_epic_layer.c
 *
 */

/**
 * Copyright 2024 SiFli Technologies
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_sifli_epic.h"

#if LV_USE_SIFLI_EPIC
#include "lv_sifli_epic_utils.h"
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sifli_epic_layer(lv_draw_task_t * task)
{
    const lv_draw_image_dsc_t * dsc = (const lv_draw_image_dsc_t *)task->draw_dsc;
    lv_layer_t * layer_to_draw = (lv_layer_t *)dsc->src;

    if(dsc->opa <= LV_OPA_MIN) {
        return;
    }

    if(layer_to_draw == NULL) {
        EPIC_ASSERT_MSG(false, "EPIC: Layer source is null");
        return;
    }

    if(layer_to_draw->draw_buf == NULL) {
        EPIC_ASSERT_MSG(false, "EPIC: Layer source draw buffer is null");
        return;
    }

    /* Keep layer composition on the same image path as normal draw_buf sources.
     * This matches the mature downstream route and avoids duplicating source
     * layout/format handling for temporary layers. */
    lv_draw_buf_flush_cache(layer_to_draw->draw_buf, NULL);

    lv_draw_image_dsc_t new_draw_dsc = *dsc;
    new_draw_dsc.src = layer_to_draw->draw_buf;

    lv_draw_task_t img_task = *task;
    img_task.draw_dsc = &new_draw_dsc;

    lv_draw_sifli_epic_img(&img_task);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_SIFLI_EPIC*/
