/**
 * @file lv_draw_nanovg_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nanovg_utils.h"

#if LV_USE_DRAW_NANOVG

#include "lv_draw_nanovg_private.h"

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

void lv_nanovg_transform(NVGcontext * ctx, const lv_matrix_t * matrix)
{
    LV_ASSERT_NULL(ctx);
    LV_ASSERT_NULL(matrix);

    nvgResetTransform(ctx);
    nvgTransform(ctx,
                 matrix->m[0][0],
                 matrix->m[1][0],
                 matrix->m[0][1],
                 matrix->m[1][1],
                 matrix->m[0][2],
                 matrix->m[1][2]);
}

void lv_nanovg_path_append_rect(NVGcontext * ctx, float x, float y, float w, float h, float r)
{
    LV_ASSERT_NULL(ctx);

    if(r > 0) {
        const float half_w = w / 2.0f;
        const float half_h = h / 2.0f;

        /*clamping cornerRadius by minimum size*/
        const float r_max = LV_MIN(half_w, half_h);

        nvgRoundedRect(ctx, x, y, w, h, r > r_max ? r_max : r);
        return;
    }

    nvgRect(ctx, x, y, w, h);
}

/**********************
*   STATIC FUNCTIONS
**********************/

#endif /* LV_USE_DRAW_NANOVG */
