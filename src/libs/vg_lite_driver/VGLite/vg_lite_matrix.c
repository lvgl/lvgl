/****************************************************************************
*
*    Copyright 2012 - 2023 Vivante Corporation, Santa Clara, California.
*    All Rights Reserved.
*
*    Permission is hereby granted, free of charge, to any person obtaining
*    a copy of this software and associated documentation files (the
*    'Software'), to deal in the Software without restriction, including
*    without limitation the rights to use, copy, modify, merge, publish,
*    distribute, sub license, and/or sell copies of the Software, and to
*    permit persons to whom the Software is furnished to do so, subject
*    to the following conditions:
*
*    The above copyright notice and this permission notice (including the
*    next paragraph) shall be included in all copies or substantial
*    portions of the Software.
*
*    THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
*    IN NO EVENT SHALL VIVANTE AND/OR ITS SUPPLIERS BE LIABLE FOR ANY
*    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
*    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
*    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*****************************************************************************/

#include "../../../lv_conf_internal.h"
#if LV_USE_VG_LITE_DRIVER

#include <math.h>
#include <string.h>
#include "vg_lite_context.h"


vg_lite_error_t vg_lite_identity(vg_lite_matrix_t * matrix)
{
#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_identity %p\n", matrix);
#endif

    /* Set identify matrix. */
    matrix->m[0][0] = 1.0f;
    matrix->m[0][1] = 0.0f;
    matrix->m[0][2] = 0.0f;
    matrix->m[1][0] = 0.0f;
    matrix->m[1][1] = 1.0f;
    matrix->m[1][2] = 0.0f;
    matrix->m[2][0] = 0.0f;
    matrix->m[2][1] = 0.0f;
    matrix->m[2][2] = 1.0f;

    matrix->scaleX  = 1.0f;
    matrix->scaleY  = 1.0f;
    matrix->angle   = 0.0f;

    return VG_LITE_SUCCESS;
}

static void multiply(vg_lite_matrix_t * matrix, vg_lite_matrix_t * mult)
{
    vg_lite_matrix_t temp;
    int row, column;

    /* Process all rows. */
    for(row = 0; row < 3; row++) {
        /* Process all columns. */
        for(column = 0; column < 3; column++) {
            /* Compute matrix entry. */
            temp.m[row][column] = (matrix->m[row][0] * mult->m[0][column])
                                  + (matrix->m[row][1] * mult->m[1][column])
                                  + (matrix->m[row][2] * mult->m[2][column]);
        }
    }

    /* Copy temporary matrix into result. */
    memcpy(matrix, &temp, sizeof(vg_lite_float_t) * 9);
}

vg_lite_error_t vg_lite_translate(vg_lite_float_t x, vg_lite_float_t y, vg_lite_matrix_t * matrix)
{
#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_translate %f %f %p\n", x, y, matrix);
#endif

    /* Set translation matrix. */
    vg_lite_matrix_t t = {
        {
            { 1.0f, 0.0f, x },
            { 0.0f, 1.0f, y },
            { 0.0f, 0.0f, 1.0f }
        },
        1.0f, 1.0f, 0.0f
    };

    /* Multiply with current matrix. */
    multiply(matrix, &t);

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_scale(vg_lite_float_t scale_x, vg_lite_float_t scale_y, vg_lite_matrix_t * matrix)
{
#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_scale %f %f %p\n", scale_x, scale_y, matrix);
#endif

    /* Set scale matrix. */
    vg_lite_matrix_t s = {
        {
            { scale_x, 0.0f, 0.0f },
            { 0.0f, scale_y, 0.0f },
            { 0.0f, 0.0f, 1.0f }
        },
        1.0f, 1.0f, 0.0f
    };

    /* Multiply with current matrix. */
    multiply(matrix, &s);

#if VG_SW_BLIT_PRECISION_OPT
    matrix->scaleX = matrix->scaleX * scale_x;
    matrix->scaleY = matrix->scaleY * scale_y;
#endif /* VG_SW_BLIT_PRECISION_OPT */

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_rotate(vg_lite_float_t degrees, vg_lite_matrix_t * matrix)
{
#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_rotate %f %p\n", degrees, matrix);
#endif

    /* Convert degrees into radians. */
    vg_lite_float_t angle = (degrees / 180.0f) * 3.141592654f;

    /* Compuet cosine and sine values. */
    vg_lite_float_t cos_angle = cosf(angle);
    vg_lite_float_t sin_angle = sinf(angle);

    /* Set rotation matrix. */
    vg_lite_matrix_t r = {
        {
            { cos_angle, -sin_angle, 0.0f },
            { sin_angle, cos_angle, 0.0f },
            { 0.0f, 0.0f, 1.0f }
        },
        1.0f, 1.0f, 0.0f
    };

    /* Multiply with current matrix. */
    multiply(matrix, &r);

#if VG_SW_BLIT_PRECISION_OPT
    matrix->angle = matrix->angle + degrees;
    if(matrix->angle >= 360) {
        vg_lite_uint32_t count = (vg_lite_uint32_t)matrix->angle / 360;
        matrix->angle = matrix->angle - count * 360;
    }
#endif /* VG_SW_BLIT_PRECISION_OPT */

    return VG_LITE_SUCCESS;
}

#endif /* LV_USE_VG_LITE_DRIVER */

