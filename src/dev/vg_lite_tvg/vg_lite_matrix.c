/**
 * @file vg_lite_matrix.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_VG_LITE && LV_USE_VG_LITE_THORVG

#include <math.h>
#include <string.h>
#include "vg_lite.h"

/*********************
 *      DEFINES
 *********************/

#ifndef M_PI
    #define M_PI 3.1415926f
#endif

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

vg_lite_error_t vg_lite_identity(vg_lite_matrix_t * matrix)
{
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
    memcpy(matrix, &temp, sizeof(temp));
}

vg_lite_error_t vg_lite_translate(vg_lite_float_t x, vg_lite_float_t y, vg_lite_matrix_t * matrix)
{
    /* Set translation matrix. */
    vg_lite_matrix_t t = { { {1.0f, 0.0f, x},
            {0.0f, 1.0f, y},
            {0.0f, 0.0f, 1.0f}
        }
    };

    /* Multiply with current matrix. */
    multiply(matrix, &t);
    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_scale(vg_lite_float_t scale_x, vg_lite_float_t scale_y, vg_lite_matrix_t * matrix)
{
    /* Set scale matrix. */
    vg_lite_matrix_t s = { { {scale_x, 0.0f, 0.0f},
            {0.0f, scale_y, 0.0f},
            {0.0f, 0.0f, 1.0f}
        }
    };

    /* Multiply with current matrix. */
    multiply(matrix, &s);
    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_rotate(vg_lite_float_t degrees, vg_lite_matrix_t * matrix)
{
    /* Convert degrees into radians. */
    vg_lite_float_t angle = (vg_lite_float_t)(degrees / 180.0f * M_PI);

    /* Compuet cosine and sine values. */
    vg_lite_float_t cos_angle = cosf(angle);
    vg_lite_float_t sin_angle = sinf(angle);

    /* Set rotation matrix. */
    vg_lite_matrix_t r = { { {cos_angle, -sin_angle, 0.0f},
            {sin_angle, cos_angle, 0.0f},
            {0.0f, 0.0f, 1.0f}
        }
    };

    /* Multiply with current matrix. */
    multiply(matrix, &r);
    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_perspective(vg_lite_float_t px, vg_lite_float_t py, vg_lite_matrix_t * matrix)
{
    /* set prespective matrix */
    vg_lite_matrix_t p = { { {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {px, py, 1.0f}
        }
    };
    /* Multiply with current matrix. */
    multiply(matrix, &p);
    return VG_LITE_SUCCESS;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_VG_LITE_THORVG*/