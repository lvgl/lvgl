/**
 * @file lv_matrix.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_matrix.h"

#if LV_USE_MATRIX

#include "../stdlib/lv_string.h"
#include <math.h>
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

static bool _is_identity_or_translation(const lv_matrix_t * matrix);
static void _multiply_matrix(lv_matrix_t * matrix, const lv_matrix_t * mul);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/* matrix functions */
void lv_matrix_identity(lv_matrix_t * matrix)
{
    matrix->m[0][0] = 1.0f;
    matrix->m[0][1] = 0.0f;
    matrix->m[0][2] = 0.0f;
    matrix->m[1][0] = 0.0f;
    matrix->m[1][1] = 1.0f;
    matrix->m[1][2] = 0.0f;
    matrix->m[2][0] = 0.0f;
    matrix->m[2][1] = 0.0f;
    matrix->m[2][2] = 1.0f;
}

void lv_matrix_translate(lv_matrix_t * matrix, float dx, float dy)
{
    if(_is_identity_or_translation(matrix)) {
        /*optimization for matrix translation.*/
        matrix->m[0][2] += dx;
        matrix->m[1][2] += dy;
        return;
    }

    lv_matrix_t tlm = {{
            {1.0f, 0.0f, dx},
            {0.0f, 1.0f, dy},
            {0.0f, 0.0f, 1.0f},
        }
    };

    _multiply_matrix(matrix, &tlm);
}

void lv_matrix_scale(lv_matrix_t * matrix, float scale_x, float scale_y)
{
    lv_matrix_t scm = {{
            {scale_x, 0.0f, 0.0f},
            {0.0f, scale_y, 0.0f},
            {0.0f, 0.0f, 1.0f},
        }
    };

    _multiply_matrix(matrix, &scm);
}

void lv_matrix_rotate(lv_matrix_t * matrix, float degree)
{
    float radian = degree / 180.0f * (float)M_PI;
    float cos_r = cosf(radian);
    float sin_r = sinf(radian);

    lv_matrix_t rtm = {{
            {cos_r, -sin_r, 0.0f},
            {sin_r, cos_r, 0.0f},
            {0.0f, 0.0f, 1.0f},
        }
    };

    _multiply_matrix(matrix, &rtm);
}

void lv_matrix_skew(lv_matrix_t * matrix, float skew_x, float skew_y)
{
    float rskew_x = skew_x / 180.0f * (float)M_PI;
    float rskew_y = skew_y / 180.0f * (float)M_PI;
    float tan_x = tanf(rskew_x);
    float tan_y = tanf(rskew_y);

    lv_matrix_t skm = {{
            {1.0f, tan_x, 0.0f},
            {tan_y, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f},
        }
    };

    _multiply_matrix(matrix, &skm);
}

void lv_matrix_multiply(lv_matrix_t * matrix, const lv_matrix_t * m)
{
    _multiply_matrix(matrix, m);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool _is_identity_or_translation(const lv_matrix_t * matrix)
{
    return (matrix->m[0][0] == 1.0f &&
            matrix->m[0][1] == 0.0f &&
            matrix->m[1][0] == 0.0f &&
            matrix->m[1][1] == 1.0f &&
            matrix->m[2][0] == 0.0f &&
            matrix->m[2][1] == 0.0f &&
            matrix->m[2][2] == 1.0f);
}

static void _multiply_matrix(lv_matrix_t * matrix, const lv_matrix_t * mul)
{
    /*TODO: use NEON to optimize this function on ARM architecture.*/
    lv_matrix_t tmp;

    for(int y = 0; y < 3; y++) {
        for(int x = 0; x < 3; x++) {
            tmp.m[y][x] = (matrix->m[y][0] * mul->m[0][x])
                          + (matrix->m[y][1] * mul->m[1][x])
                          + (matrix->m[y][2] * mul->m[2][x]);
        }
    }

    lv_memcpy(matrix, &tmp, sizeof(lv_matrix_t));
}

#endif /*LV_USE_MATRIX*/
