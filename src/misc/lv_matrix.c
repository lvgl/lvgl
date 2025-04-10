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
#include "lv_math.h"
#include <math.h>
#include <float.h>
#include "../misc/lv_log.h"
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
    if(lv_matrix_is_identity_or_translation(matrix)) {
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

    lv_matrix_multiply(matrix, &tlm);
}

void lv_matrix_scale(lv_matrix_t * matrix, float scale_x, float scale_y)
{
    lv_matrix_t scm = {{
            {scale_x, 0.0f, 0.0f},
            {0.0f, scale_y, 0.0f},
            {0.0f, 0.0f, 1.0f},
        }
    };

    lv_matrix_multiply(matrix, &scm);
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

    lv_matrix_multiply(matrix, &rtm);
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

    lv_matrix_multiply(matrix, &skm);
}

void lv_matrix_multiply(lv_matrix_t * matrix, const lv_matrix_t * mul)
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

bool lv_matrix_inverse(lv_matrix_t * matrix, const lv_matrix_t * m)
{
    float det00, det01, det02;
    float d;
    bool is_affine;

    /* Test for identity matrix. */
    if(m == NULL) {
        lv_matrix_identity(matrix);
        return true;
    }

    det00 = (m->m[1][1] * m->m[2][2]) - (m->m[2][1] * m->m[1][2]);
    det01 = (m->m[2][0] * m->m[1][2]) - (m->m[1][0] * m->m[2][2]);
    det02 = (m->m[1][0] * m->m[2][1]) - (m->m[2][0] * m->m[1][1]);

    /* Compute determinant. */
    d = (m->m[0][0] * det00) + (m->m[0][1] * det01) + (m->m[0][2] * det02);

    /* Return 0 if there is no inverse matrix. */
    if(d == 0.0f)
        return false;

    /* Compute reciprocal. */
    d = 1.0f / d;

    /* Determine if the matrix is affine. */
    is_affine = (m->m[2][0] == 0.0f) && (m->m[2][1] == 0.0f) && (m->m[2][2] == 1.0f);

    matrix->m[0][0] = d * det00;
    matrix->m[0][1] = d * ((m->m[2][1] * m->m[0][2]) - (m->m[0][1] * m->m[2][2]));
    matrix->m[0][2] = d * ((m->m[0][1] * m->m[1][2]) - (m->m[1][1] * m->m[0][2]));
    matrix->m[1][0] = d * det01;
    matrix->m[1][1] = d * ((m->m[0][0] * m->m[2][2]) - (m->m[2][0] * m->m[0][2]));
    matrix->m[1][2] = d * ((m->m[1][0] * m->m[0][2]) - (m->m[0][0] * m->m[1][2]));
    matrix->m[2][0] = is_affine ? 0.0f : d * det02;
    matrix->m[2][1] = is_affine ? 0.0f : d * ((m->m[2][0] * m->m[0][1]) - (m->m[0][0] * m->m[2][1]));
    matrix->m[2][2] = is_affine ? 1.0f : d * ((m->m[0][0] * m->m[1][1]) - (m->m[1][0] * m->m[0][1]));

    /* Success. */
    return true;
}

lv_point_precise_t lv_matrix_transform_precise_point(const lv_matrix_t * matrix, const lv_point_precise_t * point)
{
    lv_point_precise_t p;
    lv_value_precise_t w = point->x * matrix->m[2][0] + point->y * matrix->m[2][1] + matrix->m[2][2];
    if(LV_ABS(w) < FLT_EPSILON) {
        LV_LOG_ERROR("matrix is invalid");
        p.x = 0;
        p.y = 0;
    }
    else {
        lv_value_precise_t inv_w = 1.0f / w;
        p.x = (lv_value_precise_t)roundf((point->x * matrix->m[0][0] + point->y * matrix->m[0][1] + matrix->m[0][2]) * inv_w);
        p.y = (lv_value_precise_t)roundf((point->x * matrix->m[1][0] + point->y * matrix->m[1][1] + matrix->m[1][2]) * inv_w);
    }
    return p;
}

lv_area_t lv_matrix_transform_area(const lv_matrix_t * matrix, const lv_area_t * area)
{
    lv_area_t res;
    lv_point_precise_t p[4] = {
        {area->x1, area->y1},
        {area->x1, area->y2},
        {area->x2, area->y1},
        {area->x2, area->y2},
    };
    p[0] = lv_matrix_transform_precise_point(matrix, &p[0]);
    p[1] = lv_matrix_transform_precise_point(matrix, &p[1]);
    p[2] = lv_matrix_transform_precise_point(matrix, &p[2]);
    p[3] = lv_matrix_transform_precise_point(matrix, &p[3]);

    res.x1 = (int32_t)(LV_MIN4(p[0].x, p[1].x, p[2].x, p[3].x));
    res.x2 = (int32_t)(LV_MAX4(p[0].x, p[1].x, p[2].x, p[3].x));
    res.y1 = (int32_t)(LV_MIN4(p[0].y, p[1].y, p[2].y, p[3].y));
    res.y2 = (int32_t)(LV_MAX4(p[0].y, p[1].y, p[2].y, p[3].y));

    return res;
}

bool lv_matrix_is_identity_or_translation(const lv_matrix_t * matrix)
{
    return (matrix->m[0][0] == 1.0f &&
            matrix->m[0][1] == 0.0f &&
            matrix->m[1][0] == 0.0f &&
            matrix->m[1][1] == 1.0f &&
            matrix->m[2][0] == 0.0f &&
            matrix->m[2][1] == 0.0f &&
            matrix->m[2][2] == 1.0f);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_MATRIX*/
