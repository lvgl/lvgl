/**
 * @file lv_draw_gles_math.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_GLES

#include "lv_draw_gles_math.h"
#include "../../misc/lv_mem.h"

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
void lv_draw_gles_math_mat4_zero(mat4 m)
{
    lv_memset_00(m, sizeof(mat4));
}
void lv_draw_gles_math_mat4_identity(mat4 m)
{
    lv_memset_00(m, sizeof(mat4));
    m[0][0] = 1.0f;
    m[1][1] = 1.0f;
    m[2][2] = 1.0f;
    m[3][3] = 1.0f;
}

void lv_draw_gles_math_mat4_copy(mat4 m, mat4 res)
{
    lv_memcpy(res, m, sizeof(mat4));
}

void lv_draw_gles_math_mat4_mul(mat4 m1, mat4 m2, mat4 res)
{
    mat4 tmp;

    tmp[0][0] = m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3];
    tmp[0][1] = m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3];
    tmp[0][2] = m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2] + m1[3][2] * m2[0][3];
    tmp[0][3] = m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1] + m1[2][3] * m2[0][2] + m1[3][3] * m2[0][3];
    tmp[1][0] = m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3];
    tmp[1][1] = m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3];
    tmp[1][2] = m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2] + m1[3][2] * m2[1][3];
    tmp[1][3] = m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1] + m1[2][3] * m2[1][2] + m1[3][3] * m2[1][3];
    tmp[2][0] = m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2] + m1[3][0] * m2[2][3];
    tmp[2][1] = m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2] + m1[3][1] * m2[2][3];
    tmp[2][2] = m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2] + m1[3][2] * m2[2][3];
    tmp[2][3] = m1[0][3] * m2[2][0] + m1[1][3] * m2[2][1] + m1[2][3] * m2[2][2] + m1[3][3] * m2[2][3];
    tmp[3][0] = m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1] + m1[2][0] * m2[3][2] + m1[3][0] * m2[3][3];
    tmp[3][1] = m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1] + m1[2][1] * m2[3][2] + m1[3][1] * m2[3][3];
    tmp[3][2] = m1[0][2] * m2[3][0] + m1[1][2] * m2[3][1] + m1[2][2] * m2[3][2] + m1[3][2] * m2[3][3];
    tmp[3][3] = m1[0][3] * m2[3][0] + m1[1][3] * m2[3][1] + m1[2][3] * m2[3][2] + m1[3][3] * m2[3][3];

    lv_memcpy(res, tmp, sizeof(mat4));
}

void lv_draw_gles_math_translate(mat4 m, vec3 v)
{
    mat4 tmp;
    lv_draw_gles_math_mat4_identity(tmp);
    tmp[3][0] = v[0];
    tmp[3][1] = v[1];
    tmp[3][2] = v[2];
    lv_draw_gles_math_mat4_mul(m, tmp, m);
}

void lv_draw_gles_math_scale(mat4 m, vec3 v)
{
    mat4 tmp;
    lv_draw_gles_math_mat4_zero(tmp);
    tmp[0][0] = v[0];
    tmp[1][1] = v[1];
    tmp[2][2] = v[2];
    tmp[3][3] = 1.0f;
    lv_draw_gles_math_mat4_mul(m, tmp, m);
}

void lv_draw_gles_math_ortho(float left, float right, float bottom, float top, float near, float far, mat4 res)
{
    lv_draw_gles_math_mat4_zero(res);
    res[0][0] = 2.0f/(right - left);
    res[1][1] = 2.0f/(top - bottom);
    res[2][2] = 2.0f/(near - far);
    res[3][3] = 1.0f;

    res[3][0] = (right + left) / (left - right);
    res[3][1] = (top + bottom) / (bottom - top);
    res[3][2] = (far + near) / (near - far);

}
/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GPU_GLES*/
