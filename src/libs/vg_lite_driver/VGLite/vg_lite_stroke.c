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

#include "vg_lite_context.h"


#define PI                          3.141592653589793238462643383279502f
#define SINF(x)                     ((vg_lite_float_t) sin(x))
#define COSF(x)                     ((vg_lite_float_t) cos(x))
#define FABSF(x)                    ((vg_lite_float_t) fabs(x))
#define SQRTF(x)                    ((vg_lite_float_t) sqrt(x))
#define CLAMP(x, min, max)          (((x) < (min)) ? (min) : \
                                     ((x) > (max)) ? (max) : (x))
#define ACOSF(x)                    ((vg_lite_float_t) acos(x))
#define FMODF(x, y)                 ((vg_lite_float_t) fmod((x), (y)))
#define CEILF(x)                    ((vg_lite_float_t) ceil(x))

#define SIZEOF(a)                   ((size_t)(sizeof(a)))
#define PTR2SIZE(p)                 ((uintptr_t)(p))

#define FLOAT_EPSILON               0.001f

#define SWING_NO                    0
#define SWING_OUT                   1
#define SWING_IN                    2

/* Point curve type for generated stroke path. */
#define CURVE_LINE                  0
#define CURVE_QUAD_CONTROL          1
#define CURVE_QUAD_ANCHOR           2
#define CURVE_ARC_SCCW              3
#define CURVE_ARC_SCCW_HALF         4

#define FLOAT_PI                    3.141592654f
#define FLOAT_PI_TWO                6.283185307f
#define FLOAT_PI_THREE_QUARTER      2.356194490f
#define FLOAT_PI_HALF               1.570796327f
#define FLOAT_PI_QUARTER            0.7853981634f
#define FLOAT_PI_EIGHTH             0.3926990817f
/* cos(PI/8) */
#define FLOAT_COS_PI_EIGHTH         0.9238795325f

#define FLOAT_DIFF_EPSILON          0.125f
#define FLOAT_SWING_CENTER_RANGE    0.125f
#define FLOAT_ANGLE_EPSILON         0.0045f
#define FLOAT_ANGLE_EPSILON_COS     0.99999f
#define FLOAT_MIN_ARC_ANGLE         0.044f
#define FLOAT_MIN_ARC_ANGLE_COS     0.999f

/* Float constants. */
#define gcvMAX_POS_FLOAT            ((vg_lite_float_t)  3.4028235e+038)
#define gcvMAX_NEG_FLOAT            ((vg_lite_float_t) -3.4028235e+038)

#define FLOAT_MIN                   gcvMAX_NEG_FLOAT
#define FLOAT_MAX                   gcvMAX_POS_FLOAT

#define FLOAT_FAT_LINE_WIDTH        2.5f

/* Point flatten type for flattened line segments. */
#define vgcFLATTEN_NO               0
#define vgcFLATTEN_START            1
#define vgcFLATTEN_MIDDLE           2
#define vgcFLATTEN_END              3

typedef struct vg_lite_control_coord {
    vg_lite_float_t                     startX;
    vg_lite_float_t                     startY;
    vg_lite_float_t                     lastX;
    vg_lite_float_t                     lastY;
    vg_lite_float_t                     controlX;
    vg_lite_float_t                     controlY;
} vg_lite_control_coord_t;

/* Command size calculation shortcuts. */
#define COMMANDSIZE(CoordinateCount, CoordinateType) \
    ((1+CoordinateCount) * SIZEOF(CoordinateType))

extern int32_t get_data_size(vg_lite_format_t format);

static uint32_t _commandSize_float[] = {
    COMMANDSIZE(0, vg_lite_float_t),              /*   0: END             */
    COMMANDSIZE(0, vg_lite_float_t),              /*   1: CLOSE           */
    COMMANDSIZE(2, vg_lite_float_t),              /*   2: MOVE            */
    COMMANDSIZE(2, vg_lite_float_t),              /*   3: MOVE_REL        */
    COMMANDSIZE(2, vg_lite_float_t),              /*   4: LINE            */
    COMMANDSIZE(2, vg_lite_float_t),              /*   5: LINE_REL        */
    COMMANDSIZE(4, vg_lite_float_t),              /*   6: QUAD            */
    COMMANDSIZE(4, vg_lite_float_t),              /*   7: QUAD_REL        */
    COMMANDSIZE(6, vg_lite_float_t),              /*   8: CUBIC           */
    COMMANDSIZE(6, vg_lite_float_t),              /*   9: CUBIC_REL       */
    COMMANDSIZE(0, vg_lite_float_t),              /*   10: BREAK          */
    COMMANDSIZE(1, vg_lite_float_t),              /*   11: HLINE          */
    COMMANDSIZE(1, vg_lite_float_t),              /*   12: HLINE_REL      */
    COMMANDSIZE(1, vg_lite_float_t),              /*   13: VLINE          */
    COMMANDSIZE(1, vg_lite_float_t),              /*   14: VLINE_REL      */
    COMMANDSIZE(2, vg_lite_float_t),              /*   15: SQUAD          */
    COMMANDSIZE(2, vg_lite_float_t),              /*   16: SQUAD_REL      */
    COMMANDSIZE(4, vg_lite_float_t),              /*   17: SCUBIC         */
    COMMANDSIZE(4, vg_lite_float_t),              /*   18: SCUBIC_REL     */
    COMMANDSIZE(5, vg_lite_float_t),              /*   19: SCCWARC        */
    COMMANDSIZE(5, vg_lite_float_t),              /*   20: SCCWARC_REL    */
    COMMANDSIZE(5, vg_lite_float_t),              /*   21: SCWARC         */
    COMMANDSIZE(5, vg_lite_float_t),              /*   22: SCWARC_REL     */
    COMMANDSIZE(5, vg_lite_float_t),              /*   23: LCCWARC        */
    COMMANDSIZE(5, vg_lite_float_t),              /*   24: LCCWARC_REL    */
    COMMANDSIZE(5, vg_lite_float_t),              /*   25: LCWARC         */
    COMMANDSIZE(5, vg_lite_float_t),              /*   26: LCWARC_REL     */
};

#if gcFEATURE_VG_STROKE_PATH

static vg_lite_float_t _GetS8_NS_NB(int8_t * Data)
{
    int8_t x0 = *((int8_t *) Data);
    vg_lite_float_t x = (vg_lite_float_t) x0;

    return x;
}

static vg_lite_float_t _GetS16_NS_NB(int8_t * Data)
{
    int16_t x0 = *((int16_t *) Data);
    vg_lite_float_t x = (vg_lite_float_t) x0;

    return x;
}

static vg_lite_float_t _GetS32_NS_NB(int8_t * Data)
{
    int32_t x0 = *((int32_t *) Data);
    vg_lite_float_t x = (vg_lite_float_t) x0;

    return x;
}

static vg_lite_float_t _GetF_NS_NB(int8_t * Data)
{
    vg_lite_float_t x = *((vg_lite_float_t *) Data);

    return x;
}

/* Special sqrt(1.0f + x) for quick calculation when 0 <= x <= 1. */
static vg_lite_float_t _Sqrt(
    vg_lite_float_t X
)
{
    vg_lite_float_t x = X;
    vg_lite_float_t s = 1.0f;

    s += x * 0.5f;
    x *= X;
    s -= x * 0.12445995211601257f;
    x *= X;
    s += x * 0.058032196015119553f;
    x *= X;
    s -= x * 0.025314478203654289f;
    x *= X;
    s += x * 0.0059584137052297592f;

    return s;
}

static vg_lite_error_t _set_point_tangent(
    vg_lite_path_point_ptr Point,
    vg_lite_float_t Dx,
    vg_lite_float_t Dy
)
{
    if(!Point)
        return VG_LITE_INVALID_ARGUMENT;

    if(Dx == 0.0f) {
        if(Dy == 0.0f) {
            if(Point->prev) {
                Point->length = 0.0f;
                Point->tangentX = Point->prev->tangentX;
                Point->tangentY = Point->prev->tangentY;
            }
            else {
                Point->length = 0.0f;
                Point->tangentX = 0.0f;
                Point->tangentY = 0.0f;
            }
        }
        else {
            Point->tangentX = 0.0f;
            if(Dy > 0.0f) {
                Point->length = Dy;
                Point->tangentY = 1.0f;
            }
            else {
                Point->length = -Dy;
                Point->tangentY = -1.0f;
            }
        }
    }
    else if(Dy == 0.0f) {
        Point->tangentY = 0.0f;
        if(Dx > 0.0f) {
            Point->length = Dx;
            Point->tangentX = 1.0f;
        }
        else {
            Point->length = -Dx;
            Point->tangentX = -1.0f;
        }
    }
    else {
        vg_lite_float_t l, tx, ty;

        vg_lite_float_t dx, dy;
        vg_lite_float_t t, t2;

        dx = (Dx >= 0.0f ? Dx : -Dx);
        dy = (Dy >= 0.0f ? Dy : -Dy);
        if(dx >= dy) {
            t = dy / dx;
            t2 = t * t;
            l = _Sqrt(t2);
            Point->length = l * dx;

            tx = 1.0f / l;
            ty = tx * t;
        }
        else {
            t = dx / dy;
            t2 = t * t;
            l = _Sqrt(t2);
            Point->length = l * dy;

            ty = 1.0f / l;
            tx = ty * t;
        }
        if(Dx < 0.0f) tx = -tx;
        if(Dy < 0.0f) ty = -ty;

        tx = CLAMP(tx, -1.0f, 1.0f);
        ty = CLAMP(ty, -1.0f, 1.0f);
        Point->tangentX = tx;
        Point->tangentY = ty;
    }

    return VG_LITE_SUCCESS;
}

static vg_lite_error_t _add_point_to_point_list_wdelta(
    vg_lite_stroke_t * stroke_conversion,
    vg_lite_float_t X,
    vg_lite_float_t Y,
    vg_lite_float_t DX,
    vg_lite_float_t DY,
    uint8_t flatten_flag
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_path_point_ptr last_point;
    vg_lite_path_point_ptr point;

    if(!stroke_conversion)
        return VG_LITE_INVALID_ARGUMENT;

    last_point = stroke_conversion->path_end;
    point = (vg_lite_path_point_ptr)vg_lite_os_malloc(sizeof(*point));
    if(!point)
        return VG_LITE_OUT_OF_RESOURCES;

    memset(point, 0, sizeof(*point));

    point->x = X;
    point->y = Y;
    point->flatten_flag = flatten_flag;

    /* Calculate tangent for last_point. */
    VG_LITE_ERROR_HANDLER(_set_point_tangent(last_point, DX, DY));

    last_point->next = point;
    stroke_conversion->path_end = point;
    point->prev = last_point;
    stroke_conversion->point_count++;

    stroke_conversion->cur_list->path_end = point;
    stroke_conversion->cur_list->point_count++;

    return error;

ErrorHandler:
    vg_lite_os_free(point);
    point = NULL;
    return error;
}

static vg_lite_error_t _create_new_point_list(
    vg_lite_stroke_t * stroke_conversion,
    vg_lite_float_t X,
    vg_lite_float_t Y,
    uint8_t flatten_flag
)
{
    vg_lite_error_t status = VG_LITE_SUCCESS;
    vg_lite_path_point_ptr point;
    vg_lite_path_list_ptr path_list_divide;

    if(!stroke_conversion)
        return VG_LITE_INVALID_ARGUMENT;

    point = (vg_lite_path_point_ptr)vg_lite_os_malloc(sizeof(*point));
    if(!point)
        return VG_LITE_OUT_OF_RESOURCES;
    memset(point, 0, sizeof(*point));

    point->x = X;
    point->y = Y;
    point->flatten_flag = flatten_flag;
    point->prev = NULL;
    stroke_conversion->point_count = 0;

    stroke_conversion->path_end = stroke_conversion->path_points = point;
    stroke_conversion->point_count++;

    path_list_divide = (vg_lite_path_list_ptr)vg_lite_os_malloc(sizeof(*path_list_divide));
    if(!path_list_divide)
        return VG_LITE_OUT_OF_RESOURCES;
    memset(path_list_divide, 0, sizeof(*path_list_divide));

    path_list_divide->path_end = path_list_divide->path_points = point;
    path_list_divide->point_count++;

    if(stroke_conversion->cur_list == NULL) {
        stroke_conversion->cur_list = path_list_divide;
        stroke_conversion->path_list_divide = path_list_divide;
    }
    else {
        stroke_conversion->cur_list->next = path_list_divide;
        stroke_conversion->cur_list = stroke_conversion->cur_list->next;
    }

    return status;
}

static vg_lite_error_t _add_point_to_point_list(
    vg_lite_stroke_t * stroke_conversion,
    vg_lite_float_t X,
    vg_lite_float_t Y,
    uint8_t flatten_flag
)
{
    vg_lite_error_t status = VG_LITE_SUCCESS;
    vg_lite_path_point_ptr last_point;
    vg_lite_path_point_ptr point;

    if(!stroke_conversion)
        return VG_LITE_INVALID_ARGUMENT;

    last_point = stroke_conversion->path_end;
    if(last_point == NULL) {
        point = (vg_lite_path_point_ptr)vg_lite_os_malloc(sizeof(*point));
        if(!point)
            return VG_LITE_OUT_OF_RESOURCES;
        memset(point, 0, sizeof(*point));

        point->x = X;
        point->y = Y;
        point->flatten_flag = flatten_flag;
        point->prev = NULL;
        stroke_conversion->path_end = stroke_conversion->path_points = point;
        stroke_conversion->point_count++;
        status = VG_LITE_SUCCESS;
    }
    else {
        vg_lite_float_t dX = X - last_point->x;
        vg_lite_float_t dY = Y - last_point->y;
        vg_lite_float_t deltaX = (dX >= 0.0f ? dX : -dX);
        vg_lite_float_t deltaY = (dY >= 0.0f ? dY : -dY);

        /* Check for degenerated line. */
        if(deltaX == 0.0f && deltaY == 0.0f) {
            /* Skip degenerated line. */
            status = VG_LITE_SUCCESS;
            goto ErrorHandler;
        }
        if(deltaX < FLOAT_EPSILON && deltaY < FLOAT_EPSILON) {
            vg_lite_float_t ratioX, ratioY;

            if(deltaX == 0.0f) {
                ratioX = 0.0f;
            }
            else if(X == 0.0f) {
                ratioX = deltaX;
            }
            else {
                ratioX = deltaX / X;
                if(ratioX < 0.0f) ratioX = -ratioX;
            }
            if(deltaY == 0.0f) {
                ratioY = 0.0f;
            }
            else if(Y == 0.0f) {
                ratioY = deltaY;
            }
            else {
                ratioY = deltaY / Y;
                if(ratioY < 0.0f) ratioY = -ratioY;
            }
            if(ratioX < 1.0e-6f && ratioY < 1.0e-6f) {
                /* Skip degenerated line. */
                status = VG_LITE_SUCCESS;
                goto ErrorHandler;
            }
        }

        status = _add_point_to_point_list_wdelta(stroke_conversion, X, Y, dX, dY, flatten_flag);
    }

ErrorHandler:
    return status;
}

#define gcFEATURE_VG_SIMPLYFIED_BEZIER  1

#if gcFEATURE_VG_SIMPLYFIED_BEZIER
void quad_bezier(float * x, float * y, const float curve[6], float t)
{
    const float * v0, * v1, * v2;
    float mt, t2, mt2, res[2];

    v0 = &curve[0];
    v1 = &curve[2];
    v2 = &curve[4];

    mt = 1 - t;
    t2 = t * t;
    mt2 = mt * mt;

    for(uint8_t i = 0; i < 2; ++i) {
        res[i] = v0[i] * mt2 + 2 * v1[i] * mt * t + v2[i] * t2;
    }

    *x = res[0];
    *y = res[1];
}

void cubic_bezier(float * x, float * y, const float curve[8], float t)
{
    const float * v0, * v1, * v2, * v3;
    float mt, t2, mt2, t3, mt3, res[2];

    v0 = &curve[0];
    v1 = &curve[2];
    v2 = &curve[4];
    v3 = &curve[6];

    mt = 1 - t;
    t2 = t * t;
    t3 = t2 * t;
    mt2 = mt * mt;
    mt3 = mt2 * mt;

    for(uint8_t i = 0; i < 2; ++i) {
        res[i] = v0[i] * mt3 + 3 * v1[i] * mt2 * t + 3 * v2[i] * mt * t2 + v3[i] * t3;
    }

    *x = res[0];
    *y = res[1];
}

void pointer_warp_affine(float out[2], float pt[2], vg_lite_matrix_t * matrix)
{
    float x, y;

    x = pt[0];
    y = pt[1];

    out[0] = matrix->m[0][0] * x + matrix->m[1][0] * y + matrix->m[2][0];
    out[1] = matrix->m[0][1] * x + matrix->m[1][1] * y + matrix->m[2][1];
}

void get_aligned_quad(float out[6], float curve[6])
{
    float * v0, * v1, * v2;
    float angle, dx, dy;
    vg_lite_matrix_t matrix;

    v0 = &curve[0];
    v1 = &curve[2];
    v2 = &curve[4];

    dx = v2[0] - v0[0];
    dy = v2[1] - v0[1];
    angle = (dy >= 0) ? acosf(dx / sqrtf(dx * dx + dy * dy)) : (2 * 3.1415926535f - acosf(dx / sqrtf(dx * dx + dy * dy)));

    vg_lite_identity(&matrix);
    vg_lite_translate(-v0[0], -v0[1], &matrix);
    vg_lite_rotate(-angle, &matrix);

    pointer_warp_affine(&out[0], v0, &matrix);
    pointer_warp_affine(&out[2], v1, &matrix);
    pointer_warp_affine(&out[4], v2, &matrix);
}

void get_aligned_cubic(float out[8], float curve[8])
{
    float * v0, * v1, * v2, * v3;
    float angle, dx, dy;
    vg_lite_matrix_t matrix;

    v0 = &curve[0];
    v1 = &curve[2];
    v2 = &curve[4];
    v3 = &curve[6];

    dx = v3[0] - v0[0];
    dy = v3[1] - v0[1];
    angle = (dy >= 0) ? acosf(dx / sqrtf(dx * dx + dy * dy)) : (2 * 3.1415926535f - acosf(dx / sqrtf(dx * dx + dy * dy)));

    vg_lite_identity(&matrix);
    vg_lite_translate(-v0[0], -v0[1], &matrix);
    vg_lite_rotate(-angle, &matrix);

    pointer_warp_affine(&out[0], v0, &matrix);
    pointer_warp_affine(&out[2], v1, &matrix);
    pointer_warp_affine(&out[4], v2, &matrix);
    pointer_warp_affine(&out[6], v3, &matrix);
}

void split_quad(float out1[6], float out2[6], float curve[6], float split)
{
    float * v0, * v1, * v2;
    float s, s2, ms, ms2;

    v0 = &curve[0];
    v1 = &curve[2];
    v2 = &curve[4];

    s = split;
    ms = 1 - split;
    s2 = s * s;
    ms2 = ms * ms;

    float B[2][3] = {
        {v0[0], v1[0], v2[0]},
        {v0[1], v1[1], v2[1]}
    };

    /* First curve */
    {
        float C[2][3] = { {0} };
        float A[9] = {
            1, 0, 0,
            ms, s, 0,
            ms2, 2 * ms * s, s2
        };
        /* C = A �� B */
        for(uint8_t i = 0; i < 2; ++i) {
            for(size_t y = 0; y < 3; ++y)
                for(size_t x = 0; x < 1; ++x)
                    for(size_t z = 0; z < 3; ++z) {
                        C[i][x + y * 1] += A[z + y * 3] * B[i][x + z * 1];
                    }
        }

        out1[0] = C[0][0];
        out1[1] = C[1][0];
        out1[2] = C[0][1];
        out1[3] = C[1][1];
        out1[4] = C[0][2];
        out1[5] = C[1][2];
    }

    /* Second curve */
    {
        float C[2][3] = { {0} };
        float A[9] = {
            ms2, 2 * s * ms, s2,
            0, ms, s,
            0, 0, 1
        };
        /* C = A �� B */
        for(uint8_t i = 0; i < 2; ++i) {
            for(size_t y = 0; y < 3; ++y)
                for(size_t x = 0; x < 1; ++x)
                    for(size_t z = 0; z < 3; ++z) {
                        C[i][x + y * 1] += A[z + y * 3] * B[i][x + z * 1];
                    }
        }

        out2[0] = C[0][0];
        out2[1] = C[1][0];
        out2[2] = C[0][1];
        out2[3] = C[1][1];
        out2[4] = C[0][2];
        out2[5] = C[1][2];
    }
}

void split_cubic(float out1[8], float out2[8], float curve[8], float split)
{
    float * v0, * v1, * v2, * v3;
    float s, s2, s3, ms, ms2, ms3;

    v0 = &curve[0];
    v1 = &curve[2];
    v2 = &curve[4];
    v3 = &curve[6];

    s = split;
    ms = 1 - split;
    s2 = s * s;
    ms2 = ms * ms;
    s3 = s2 * s;
    ms3 = ms2 * ms;

    float B[2][4] = {
        {v0[0], v1[0], v2[0], v3[0]},
        {v0[1], v1[1], v2[1], v3[1]}
    };

    /* First curve */
    {
        float C[2][4] = { {0} };
        float A[16] = {
            1, 0, 0, 0,
            ms, s, 0, 0,
            ms2, 2 * ms * s, s2, 0,
            ms3, 3 * s * ms2, 3 * s2 * ms, s3
        };
        /* C = A �� B */
        for(uint8_t i = 0; i < 2; ++i) {
            for(size_t y = 0; y < 4; ++y)
                for(size_t x = 0; x < 1; ++x)
                    for(size_t z = 0; z < 4; ++z) {
                        C[i][x + y * 1] += A[z + y * 4] * B[i][x + z * 1];
                    }
        }

        out1[0] = C[0][0];
        out1[1] = C[1][0];
        out1[2] = C[0][1];
        out1[3] = C[1][1];
        out1[4] = C[0][2];
        out1[5] = C[1][2];
        out1[6] = C[0][3];
        out1[7] = C[1][3];
    }

    /* Second curve */
    {
        float C[2][4] = { {0} };
        float A[16] = {
            ms3, 3 * s * ms2, 3 * s2 * ms, s3,
            0, ms2, 2 * ms * s, s2,
            0, 0, ms, s,
            0, 0, 0, 1
        };
        /* C = A �� B */
        for(uint8_t i = 0; i < 2; ++i) {
            for(size_t y = 0; y < 4; ++y)
                for(size_t x = 0; x < 1; ++x)
                    for(size_t z = 0; z < 4; ++z) {
                        C[i][x + y * 1] += A[z + y * 4] * B[i][x + z * 1];
                    }
        }

        out2[0] = C[0][0];
        out2[1] = C[1][0];
        out2[2] = C[0][1];
        out2[3] = C[1][1];
        out2[4] = C[0][2];
        out2[5] = C[1][2];
        out2[6] = C[0][3];
        out2[7] = C[1][3];
    }
}
#if (CHIPID != 0x265)
static vg_lite_error_t _flatten_quad_bezier(
    vg_lite_stroke_t * stroke_conversion,
    vg_lite_float_t rootCurve[6],
    vg_lite_float_t subCurve[6],
    vg_lite_uint8_t level)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    float * v0, * v1, * v2;
    float dx2, dy2, d1;
    float subCurve1[6], subCurve2[6];
    vg_lite_path_point_ptr point0, point1;
    vg_lite_float_t * curve;
    curve = (level == 0) ? rootCurve : subCurve;

    if(!stroke_conversion)
        return VG_LITE_INVALID_ARGUMENT;
    if(level > 10) return error;

    v0 = &curve[0];
    v1 = &curve[2];
    v2 = &curve[4];

    if(level == 0) {
        /* Add extra P0 for incoming tangent. */
        point0 = stroke_conversion->path_end;
        /* First add P1 to calculate incoming tangent, which is saved in P0. */
        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v1[0], v1[1], vgcFLATTEN_START));

        point1 = stroke_conversion->path_end;
        /* Change the point1's coordinates back to P0. */
        point1->x = v0[0];
        point1->y = v0[1];
        point0->length = 0.0f;
    }

    dx2 = v2[0] - v0[0];
    dy2 = v2[1] - v0[1];
    d1 = fabsf((v1[0] - v2[0]) * dy2 - (v1[1] - v2[1]) * dx2);

    if(d1 * d1 < 0.25 * (dx2 * dx2 + dy2 * dy2)) {
        float bound[4];

        bound[0] = MIN(v0[0], v2[0]);
        bound[1] = MIN(v0[1], v2[1]);
        bound[2] = MAX(v0[0], v2[0]);
        bound[3] = MAX(v0[1], v2[1]);

        if(!(v1[0] >= bound[0] && v1[0] <= bound[2] && v1[1] >= bound[1] && v1[1] <= bound[3])) {
            /* Compute root. */
            float alignedCurve[6];
            float d, n, t, pt[2];

            get_aligned_quad(alignedCurve, curve);

            n = alignedCurve[0] - alignedCurve[2];
            d = alignedCurve[0] - 2.f * alignedCurve[2] + alignedCurve[4];
            if(fabsf(d) > 1e-12f) {
                t = n / d;
                if(t > 1e-12f && t < 1.f - 1e-12f) {
                    quad_bezier(&pt[0], &pt[1], curve, t);
                    VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, pt[0], pt[1], vgcFLATTEN_MIDDLE));
                }
            }
        }
        else if(level == 0) {
            float pt[2];
            uint8_t n = 16;
            for(uint8_t i = 1; i < n; i++) {
                vg_lite_float_t t = (vg_lite_float_t)i / (vg_lite_float_t)n;
                quad_bezier(&pt[0], &pt[1], curve, t);
                VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, pt[0], pt[1], vgcFLATTEN_MIDDLE));
            }
        }
        if(level == 0) {
            VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v2[0], v2[1], vgcFLATTEN_END));
        }
        else if((v2[0] != rootCurve[4]) || (v2[1] != rootCurve[5])) {
            VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v2[0], v2[1], vgcFLATTEN_MIDDLE));
        }
        if(level == 0) {
            /* Add extra P2 for outgoing tangent. */
            /* First change P2(point0)'s coordinates to P1. */
            point0 = stroke_conversion->path_end;
            point0->x = v1[0];
            point0->y = v1[1];

            /* Add P2 to calculate outgoing tangent. */
            VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v2[0], v2[1], vgcFLATTEN_NO));

            point1 = stroke_conversion->path_end;

            /* Change point0's coordinates back to P2. */
            point0->x = v2[0];
            point0->y = v2[1];
            point0->length = 0.0f;
        }
        return error;
    }

    split_quad(subCurve1, subCurve2, curve, 0.5);
    VG_LITE_ERROR_HANDLER(_flatten_quad_bezier(stroke_conversion, rootCurve, subCurve1, level + 1));
    VG_LITE_ERROR_HANDLER(_flatten_quad_bezier(stroke_conversion, rootCurve, subCurve2, level + 1));
    if(level == 0) {
        /* Add point 2 separately to avoid cumulative errors. */
        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v2[0], v2[1], vgcFLATTEN_END));

        /* Add extra P2 for outgoing tangent. */
        /* First change P2(point0)'s coordinates to P1. */
        point0 = stroke_conversion->path_end;
        point0->x = v1[0];
        point0->y = v1[1];

        /* Add P2 to calculate outgoing tangent. */
        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v2[0], v2[1], vgcFLATTEN_NO));

        point1 = stroke_conversion->path_end;

        /* Change point0's coordinates back to P2. */
        point0->x = v2[0];
        point0->y = v2[1];
        point0->length = 0.0f;
    }
ErrorHandler:
    return error;
}
#else
static vg_lite_error_t _flatten_quad_bezier_original(
    vg_lite_stroke_t * stroke_conversion,
    vg_lite_float_t curve[6],
    vg_lite_uint8_t level)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    float * v0, * v1, * v2;
    float dx2, dy2, d1;
    float subCurve1[6], subCurve2[6];
    vg_lite_path_point_ptr point0, point1;

    if(!stroke_conversion)
        return VG_LITE_INVALID_ARGUMENT;
    if(level > 10) return error;

    v0 = &curve[0];
    v1 = &curve[2];
    v2 = &curve[4];

    if(level == 0) {
        /* Add extra P0 for incoming tangent. */
        point0 = stroke_conversion->path_end;
        /* First add P1 to calculate incoming tangent, which is saved in P0. */
        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v1[0], v1[1], vgcFLATTEN_NO));

        point1 = stroke_conversion->path_end;
        /* Change the point1's coordinates back to P0. */
        point1->x = v0[0];
        point1->y = v0[1];
        point0->length = 0.0f;
    }

    dx2 = v2[0] - v0[0];
    dy2 = v2[1] - v0[1];
    d1 = fabsf((v1[0] - v2[0]) * dy2 - (v1[1] - v2[1]) * dx2);

    if(d1 * d1 < 0.25 * (dx2 * dx2 + dy2 * dy2)) {
        float bound[4];

        bound[0] = MIN(v0[0], v2[0]);
        bound[1] = MIN(v0[1], v2[1]);
        bound[2] = MAX(v0[0], v2[0]);
        bound[3] = MAX(v0[1], v2[1]);

        if(!(v1[0] >= bound[0] && v1[0] <= bound[2] && v1[1] >= bound[1] && v1[1] <= bound[3])) {
            /* Compute root. */
            float alignedCurve[6];
            float d, n, t, pt[2];

            get_aligned_quad(alignedCurve, curve);

            n = alignedCurve[0] - alignedCurve[2];
            d = alignedCurve[0] - 2.f * alignedCurve[2] + alignedCurve[4];
            if(fabsf(d) > 1e-12f) {
                t = n / d;
                if(t > 1e-12f && t < 1.f - 1e-12f) {
                    quad_bezier(&pt[0], &pt[1], curve, t);
                    VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, pt[0], pt[1], vgcFLATTEN_NO));
                }
            }
        }
        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v2[0], v2[1], vgcFLATTEN_NO));
        return error;
    }

    split_quad(subCurve1, subCurve2, curve, 0.5);
    VG_LITE_ERROR_HANDLER(_flatten_quad_bezier_original(stroke_conversion, subCurve1, level + 1));
    VG_LITE_ERROR_HANDLER(_flatten_quad_bezier_original(stroke_conversion, subCurve2, level + 1));
    if(level == 0) {
        /* Add point 2 separately to avoid cumulative errors. */
        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v2[0], v2[1], vgcFLATTEN_NO));

        /* Add extra P2 for outgoing tangent. */
        /* First change P2(point0)'s coordinates to P1. */
        point0 = stroke_conversion->path_end;
        point0->x = v1[0];
        point0->y = v1[1];

        /* Add P2 to calculate outgoing tangent. */
        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v2[0], v2[1], vgcFLATTEN_NO));

        point1 = stroke_conversion->path_end;

        /* Change point0's coordinates back to P2. */
        point0->x = v2[0];
        point0->y = v2[1];
        point0->length = 0.0f;
    }
ErrorHandler:
    return error;
}
#endif

#if (CHIPID != 0x265)
static vg_lite_error_t _flatten_cubic_bezier(
    vg_lite_stroke_t * stroke_conversion,
    vg_lite_float_t rootCurve[8],
    vg_lite_float_t subCurve[8],
    vg_lite_uint8_t level)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    float * v0, * v1, * v2, * v3;
    float dx3, dy3, d1, d2;
    float subCurve1[8], subCurve2[8];
    vg_lite_path_point_ptr point0, point1;
    vg_lite_float_t * curve;

    if(!stroke_conversion)
        return VG_LITE_INVALID_ARGUMENT;
    if(level > 10) return error;

    curve = (level == 0) ? rootCurve : subCurve;
    v0 = &curve[0];
    v1 = &curve[2];
    v2 = &curve[4];
    v3 = &curve[6];

    if(level == 0) {
        /* Add extra P0 for incoming tangent. */
        point0 = stroke_conversion->path_end;
        /* First add P1/P2/P3 to calculate incoming tangent, which is saved in P0. */
        if(v0[0] != v1[0] || v0[1] != v1[1]) {
            VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v1[0], v1[1], vgcFLATTEN_START));
        }
        else if(v0[0] != v2[0] || v0[1] != v2[1]) {
            VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v2[0], v2[1], vgcFLATTEN_START));
        }
        else {
            VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v3[0], v3[1], vgcFLATTEN_START));
        }
        point1 = stroke_conversion->path_end;
        /* Change the point1's coordinates back to P0. */
        point1->x = v0[0];
        point1->y = v0[1];
        point0->length = 0.0f;
    }

    dx3 = v3[0] - v0[0];
    dy3 = v3[1] - v0[1];
    d1 = fabsf((v1[0] - v3[0]) * dy3 - (v1[1] - v3[1]) * dx3);
    d2 = fabsf((v2[0] - v3[0]) * dy3 - (v2[1] - v3[1]) * dx3);

    if((d1 + d2) * (d1 + d2) < 0.25f * (dx3 * dx3 + dy3 * dy3)) {
        float bound[4];

        bound[0] = MIN(v0[0], v3[0]);
        bound[1] = MIN(v0[1], v3[1]);
        bound[2] = MAX(v0[0], v3[0]);
        bound[3] = MAX(v0[1], v3[1]);
        if(!(v1[0] >= bound[0] && v1[0] <= bound[2] && v1[1] >= bound[1] && v1[1] <= bound[3]) ||
           !(v2[0] >= bound[0] && v2[0] <= bound[2] && v2[1] >= bound[1] && v2[1] <= bound[3])) {
            /* Compute root. */
            float alignedCurve[8];
            float a, b, c, b2ac, root[2], t, pt[2];
            uint8_t rootNum;

            get_aligned_cubic(alignedCurve, curve);

            a = -3.f * alignedCurve[0] + 9.f * alignedCurve[2] - 9.f * alignedCurve[4] + 3.f * alignedCurve[6];
            b = 6.f * alignedCurve[0] - 12.f * alignedCurve[2] + 6.f * alignedCurve[4];
            c = -3.f * alignedCurve[0] + 3.f * alignedCurve[2];
            rootNum = 0;
            if(fabs(a) < 1e-12f) {   // linear solution
                t = -c / b;
                if(t > 1e-12f && t < 1.f - 1e-12f)
                    root[rootNum++] = t;
            }
            else {   // quadtratic solution
                b2ac = b * b - 4.f * a * c;
                if(b2ac > 1e-12f) {
                    t = (-b + (float)sqrt(b2ac)) / (2.f * a);
                    if(t > 1e-12f && t < 1.f - 1e-12f)
                        root[rootNum++] = t;
                    t = (-b - (float)sqrt(b2ac)) / (2.f * a);
                    if(t > 1e-12f && t < 1.f - 1e-12f)
                        root[rootNum++] = t;
                }
            }
            if(rootNum == 2 && root[0] > root[1]) {
                /* Exchange root. */
                float tmp;
                tmp = root[0];
                root[0] = root[1];
                root[1] = tmp;
            }
            if((rootNum <= 2) && (level == 0)) {
                float  pt[2];
                uint8_t n = 8;
                if(rootNum == 2) {
                    float step = (root[1] - root[0]) / 8;
                    float t = root[0];
                    while(t < root[1]) {
                        cubic_bezier(&pt[0], &pt[1], curve, t);
                        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, pt[0], pt[1], vgcFLATTEN_MIDDLE));
                        t += step;
                    }
                }
                else {
                    for(uint8_t i = 1; i < n; i++) {
                        vg_lite_float_t t = (vg_lite_float_t)i / (vg_lite_float_t)n;
                        cubic_bezier(&pt[0], &pt[1], curve, t);
                        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, pt[0], pt[1], vgcFLATTEN_MIDDLE));
                    }
                }
            }
            else {
                for(uint8_t i = 0; i < rootNum; i++) {
                    cubic_bezier(&pt[0], &pt[1], curve, root[i]);
                    VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, pt[0], pt[1], vgcFLATTEN_MIDDLE));
                }
            }
        }
        else if(level == 0) {
            vg_lite_float_t a1x, a1y, a2x, a2y, a3x, a3y;
            vg_lite_float_t ddf0, ddf1, t1, t2, upper_bound;
            vg_lite_uint32_t n;
            vg_lite_float_t pt[2];
            a1x = 3 * (v1[0] - v0[0]);
            a1y = 3 * (v1[1] - v0[1]);
            a2x = 3 * (v0[0] - v1[0] - v1[0] + v2[0]);
            a2y = 3 * (v0[1] - v1[1] - v1[1] + v2[1]);
            a3x = 3 * (v1[0] - v2[0]) + v3[0] - v0[0];
            a3y = 3 * (v1[1] - v2[1]) + v3[1] - v0[1];

            ddf0 = a2x * a2x + a2y * a2y;
            t1 = a2x + a3x + a3x + a3x;
            t2 = a2y + a3y + a3y + a3y;
            ddf1 = t1 * t1 + t2 * t2;
            upper_bound = ddf0 > ddf1 ? ddf0 : ddf1;
            upper_bound = SQRTF(upper_bound);
            upper_bound += upper_bound;
            upper_bound = SQRTF(upper_bound);
            if(stroke_conversion->fattened) {
                upper_bound *= stroke_conversion->line_width;
            }
            n = (vg_lite_uint32_t)ceil(upper_bound);

            if(n == 0 || n > 64) {
                n = (vg_lite_uint8_t)(64 / (level + 1));
            }
            for(vg_lite_uint32_t i = 1; i < n; i++) {
                vg_lite_float_t t = (vg_lite_float_t)i / (vg_lite_float_t)n;
                cubic_bezier(&pt[0], &pt[1], curve, t);
                VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, pt[0], pt[1], vgcFLATTEN_MIDDLE));
            }
        }

        if(level == 0) {
            VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v3[0], v3[1], vgcFLATTEN_END));
        }
        else {
            vg_lite_float_t pt[2], t;
            for(int i = 1; i < 4; i++) {
                t = (vg_lite_float_t)i / 4;
                cubic_bezier(&pt[0], &pt[1], curve, t);
                VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, pt[0], pt[1], vgcFLATTEN_MIDDLE));
            }
            if((v3[0] != rootCurve[6]) || (v3[1] != rootCurve[7])) {
                VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v3[0], v3[1], vgcFLATTEN_MIDDLE));
            }
        }

        /* Add extra P3 for outgoing tangent. */
        /* First change P3(point0)'s coordinates to P0/P1/P2. */
        if(level == 0) {
            point0 = stroke_conversion->path_end;
            if(v3[0] != v2[0] || v3[1] != v2[1]) {
                point0->x = v2[0];
                point0->y = v2[1];
            }
            else if(v3[0] != v1[0] || v3[1] != v1[1]) {
                point0->x = v1[0];
                point0->y = v1[1];
            }
            else {
                point0->x = v0[0];
                point0->y = v0[1];
            }

            /* Add P3 to calculate outgoing tangent. */
            VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v3[0], v3[1], vgcFLATTEN_NO));

            point1 = stroke_conversion->path_end;

            /* Change point0's coordinates back to P3. */
            point0->x = v3[0];
            point0->y = v3[1];
            point0->length = 0.0f;
        }
        return error;
    }

    split_cubic(subCurve1, subCurve2, curve, 0.5);
    VG_LITE_ERROR_HANDLER(_flatten_cubic_bezier(stroke_conversion, rootCurve, subCurve1, level + 1));
    VG_LITE_ERROR_HANDLER(_flatten_cubic_bezier(stroke_conversion, rootCurve, subCurve2, level + 1));
    if(level == 0) {
        /* Add point 3 separately to avoid cumulative errors. */
        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v3[0], v3[1], vgcFLATTEN_END));

        /* Add extra P3 for outgoing tangent. */
        /* First change P3(point0)'s coordinates to P0/P1/P2. */
        point0 = stroke_conversion->path_end;
        if(v3[0] != v2[0] || v3[1] != v2[1]) {
            point0->x = v2[0];
            point0->y = v2[1];
        }
        else if(v3[0] != v1[0] || v3[1] != v1[1]) {
            point0->x = v1[0];
            point0->y = v1[1];
        }
        else {
            point0->x = v0[0];
            point0->y = v0[1];
        }

        /* Add P3 to calculate outgoing tangent. */
        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v3[0], v3[1], vgcFLATTEN_NO));

        point1 = stroke_conversion->path_end;

        /* Change point0's coordinates back to P3. */
        point0->x = v3[0];
        point0->y = v3[1];
        point0->length = 0.0f;
    }
ErrorHandler:
    return error;
}
#else
static vg_lite_error_t _flatten_cubic_bezier_original(
    vg_lite_stroke_t * stroke_conversion,
    vg_lite_float_t curve[8],
    vg_lite_uint8_t level)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    float * v0, * v1, * v2, * v3;
    float dx3, dy3, d1, d2;
    float subCurve1[8], subCurve2[8];
    vg_lite_path_point_ptr point0, point1;

    if(!stroke_conversion)
        return VG_LITE_INVALID_ARGUMENT;
    if(level > 10) return error;

    v0 = &curve[0];
    v1 = &curve[2];
    v2 = &curve[4];
    v3 = &curve[6];

    if(level == 0) {
        /* Add extra P0 for incoming tangent. */
        point0 = stroke_conversion->path_end;
        /* First add P1/P2/P3 to calculate incoming tangent, which is saved in P0. */
        if(v0[0] != v1[0] || v0[1] != v1[1]) {
            VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v1[0], v1[1], vgcFLATTEN_NO));
        }
        else if(v0[0] != v2[0] || v0[1] != v2[1]) {
            VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v2[0], v2[1], vgcFLATTEN_NO));
        }
        else {
            VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v3[0], v3[1], vgcFLATTEN_NO));
        }
        point1 = stroke_conversion->path_end;
        /* Change the point1's coordinates back to P0. */
        point1->x = v0[0];
        point1->y = v0[1];
        point0->length = 0.0f;
    }

    dx3 = v3[0] - v0[0];
    dy3 = v3[1] - v0[1];
    d1 = fabsf((v1[0] - v3[0]) * dy3 - (v1[1] - v3[1]) * dx3);
    d2 = fabsf((v2[0] - v3[0]) * dy3 - (v2[1] - v3[1]) * dx3);

    if((d1 + d2) * (d1 + d2) < 0.25f * (dx3 * dx3 + dy3 * dy3)) {
        float bound[4];

        bound[0] = MIN(v0[0], v3[0]);
        bound[1] = MIN(v0[1], v3[1]);
        bound[2] = MAX(v0[0], v3[0]);
        bound[3] = MAX(v0[1], v3[1]);
        if(!(v1[0] >= bound[0] && v1[0] <= bound[2] && v1[1] >= bound[1] && v1[1] <= bound[3]) ||
           !(v2[0] >= bound[0] && v2[0] <= bound[2] && v2[1] >= bound[1] && v2[1] <= bound[3])) {
            /* Compute root. */
            float alignedCurve[8];
            float a, b, c, b2ac, root[2], t, pt[2];
            uint8_t rootNum;

            get_aligned_cubic(alignedCurve, curve);

            a = -3.f * alignedCurve[0] + 9.f * alignedCurve[2] - 9.f * alignedCurve[4] + 3.f * alignedCurve[6];
            b = 6.f * alignedCurve[0] - 12.f * alignedCurve[2] + 6.f * alignedCurve[4];
            c = -3.f * alignedCurve[0] + 3.f * alignedCurve[2];
            rootNum = 0;
            if(fabs(a) < 1e-12f) {   // linear solution
                t = -c / b;
                if(t > 1e-12f && t < 1.f - 1e-12f)
                    root[rootNum++] = t;
            }
            else {   // quadtratic solution
                b2ac = b * b - 4.f * a * c;
                if(b2ac > 1e-12f) {
                    t = (-b + (float)sqrt(b2ac)) / (2.f * a);
                    if(t > 1e-12f && t < 1.f - 1e-12f)
                        root[rootNum++] = t;
                    t = (-b - (float)sqrt(b2ac)) / (2.f * a);
                    if(t > 1e-12f && t < 1.f - 1e-12f)
                        root[rootNum++] = t;
                }
            }

            if(rootNum == 2 && root[0] > root[1]) {
                /* Exchange root. */
                float tmp;
                tmp = root[0];
                root[0] = root[1];
                root[1] = tmp;
            }

            for(uint8_t i = 0; i < rootNum; ++i) {
                cubic_bezier(&pt[0], &pt[1], curve, root[i]);
                VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, pt[0], pt[1], vgcFLATTEN_NO));
            }
        }
        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v3[0], v3[1], vgcFLATTEN_NO));
        return error;
    }

    split_cubic(subCurve1, subCurve2, curve, 0.5);
    VG_LITE_ERROR_HANDLER(_flatten_cubic_bezier_original(stroke_conversion, subCurve1, level + 1));
    VG_LITE_ERROR_HANDLER(_flatten_cubic_bezier_original(stroke_conversion, subCurve2, level + 1));
    if(level == 0) {
        /* Add point 3 separately to avoid cumulative errors. */
        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v3[0], v3[1], vgcFLATTEN_NO));

        /* Add extra P3 for outgoing tangent. */
        /* First change P3(point0)'s coordinates to P0/P1/P2. */
        point0 = stroke_conversion->path_end;
        if(v3[0] != v2[0] || v3[1] != v2[1]) {
            point0->x = v2[0];
            point0->y = v2[1];
        }
        else if(v3[0] != v1[0] || v3[1] != v1[1]) {
            point0->x = v1[0];
            point0->y = v1[1];
        }
        else {
            point0->x = v0[0];
            point0->y = v0[1];
        }

        /* Add P3 to calculate outgoing tangent. */
        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, v3[0], v3[1], vgcFLATTEN_NO));

        point1 = stroke_conversion->path_end;

        /* Change point0's coordinates back to P3. */
        point0->x = v3[0];
        point0->y = v3[1];
        point0->length = 0.0f;
    }
ErrorHandler:
    return error;
}
#endif
#else
static vg_lite_error_t
_flatten_quad_bezier(
    vg_lite_stroke_t * stroke_conversion,
    vg_lite_float_t X0,
    vg_lite_float_t Y0,
    vg_lite_float_t X1,
    vg_lite_float_t Y1,
    vg_lite_float_t X2,
    vg_lite_float_t Y2
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t n;
    vg_lite_path_point_ptr point0, point1;
    vg_lite_float_t x, y;
    vg_lite_float_t a1x, a1y, a2x, a2y;
    vg_lite_float_t f1, f2, t1, t2, upper_bound;

    if(!stroke_conversion)
        return VG_LITE_INVALID_ARGUMENT;

    /* Formula.
    * f(t) = (1 - t)^2 * p0 + 2 * t * (1 - t) * p1 + t^2 * p2
    *      = a0 + a1 * t + a2 * t^2
    *   a0 = p0
    *   a1 = 2 * (-p0 + p1)
    *   a2 = p0 - 2 * p1 + p2
    */
    x = X1 - X0;
    a1x = x + x;
    y = Y1 - Y0;
    a1y = y + y;
    a2x = X0 - X1 - X1 + X2;
    a2y = Y0 - Y1 - Y1 + Y2;

    /* Step 1: Calculate N. */
    /* Lefan's method. */
    /* dist(t) = ...
    * t2 = ...
    * if 0 <= t2 <= 1
    *    upper_bound = dist(t2)
    * else
    *    upper_bound = max(dist(0), dist(1))
    * N = ceil(sqrt(upper_bound / epsilon / 8))
    */
    /* Prepare dist(t). */
    f1 = a1x * a2y - a2x * a1y;
    if(f1 != 0.0f) {
        if(f1 < 0.0f) f1 = -f1;

        /* Calculate t2. */
        t1 = a2x * a2x + a2y * a2y;
        t2 = -(x * a2x + y * a2y) / t1;
        /* Calculate upper_bound. */
        if(t2 >= 0.0f && t2 <= 1.0f) {
            f2 = x + a2x * t2;
            f2 *= f2;
            t1 = y + a2y * t2;
            t1 *= t1;
            upper_bound = t1 + f2;
        }
        else {
            f2 = x + a2x;
            f2 *= f2;
            t1 = y + a2y;
            t1 *= t1;
            t2 = t1 + f2;
            t1 = x * x + y * y;
            upper_bound = t1 < t2 ? t1 : t2;
        }
        /* Calculate n. */
        upper_bound = f1 / SQRTF(upper_bound);
        upper_bound = SQRTF(upper_bound);
        if(stroke_conversion->fattened) {
            upper_bound *= stroke_conversion->line_width;
        }
        n = (uint32_t) ceil(upper_bound);
    }
    else {
        /* n = 0 => n = 256. */
        n = 256;
    }

    if(n == 0 || n > 256) {
        n = 256;
    }

    /* Add extra P0 for incoming tangent. */
    point0 = stroke_conversion->path_end;
    /* First add P1 to calculate incoming tangent, which is saved in P0. */
    VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, X1, Y1, vgcFLATTEN_START));

    point1 = stroke_conversion->path_end;
    /* Change the point1's coordinates back to P0. */
    point1->x = X0;
    point1->y = Y0;
    point0->length = 0.0f;

    if(n > 1) {
        vg_lite_float_t d, dsquare, dx, dy, ddx, ddy;
        vg_lite_float_t ratioX, ratioY;
        uint32_t i;

        /* Step 2: Calculate deltas. */
        /*   Df(t) = f(t + d) - f(t)
        *         = a1 * d + a2 * d^2 + 2 * a2 * d * t
        *  DDf(t) = Df(t + d) - Df(t)
        *         = 2 * a2 * d^2
        *    f(0) = a0
        *   Df(0) = a1 * d + a2 * d^2
        *  DDf(0) = 2 * a2 * d^2
        */
        d = 1.0f / (vg_lite_float_t) n;
        dsquare = d * d;
        ddx = a2x * dsquare;
        ddy = a2y * dsquare;
        dx  = a1x * d + ddx;
        dy  = a1y * d + ddy;
        ddx += ddx;
        ddy += ddy;

        /* Step 3: Add points. */
        ratioX = dx / X0;
        if(ratioX < 0.0f) ratioX = -ratioX;
        ratioY = dy / Y0;
        if(ratioY < 0.0f) ratioY = -ratioY;
        if(ratioX > 1.0e-6f && ratioY > 1.0e-6f) {
            x = X0;
            y = Y0;
            for(i = 1; i < n; i++) {
                x += dx;
                y += dy;

                /* Add a point to subpath. */
                VG_LITE_ERROR_HANDLER(_add_point_to_point_list_wdelta(stroke_conversion, x, y, dx, dy, vgcFLATTEN_MIDDLE));

                dx += ddx;
                dy += ddy;
            }

        }
        else {
            for(i = 1; i < n; i++) {
                vg_lite_float_t t = (vg_lite_float_t) i / (vg_lite_float_t) n;
                vg_lite_float_t u = 1.0f - t;
                vg_lite_float_t a0 = u * u;
                vg_lite_float_t a1 = 2.0f * t * u;
                vg_lite_float_t a2 = t * t;

                x  = a0 * X0 + a1 * X1 + a2 * X2;
                y  = a0 * Y0 + a1 * Y1 + a2 * Y2;

                /* Add a point to subpath. */
                VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, x, y, vgcFLATTEN_MIDDLE));
            }
        }
    }

    /* Add point 2 separately to avoid cumulative errors. */
    VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, X2, Y2, vgcFLATTEN_END));

    /* Add extra P2 for outgoing tangent. */
    /* First change P2(point0)'s coordinates to P1. */
    point0 = stroke_conversion->path_end;
    point0->x = X1;
    point0->y = Y1;

    /* Add P2 to calculate outgoing tangent. */
    VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, X2, Y2, vgcFLATTEN_NO));

    point1 = stroke_conversion->path_end;

    /* Change point0's coordinates back to P2. */
    point0->x = X2;
    point0->y = Y2;
    point0->length = 0.0f;

ErrorHandler:
    return error;
}

static vg_lite_error_t
_flatten_cubic_bezier(
    vg_lite_stroke_t  * stroke_conversion,
    vg_lite_float_t X0,
    vg_lite_float_t Y0,
    vg_lite_float_t X1,
    vg_lite_float_t Y1,
    vg_lite_float_t X2,
    vg_lite_float_t Y2,
    vg_lite_float_t X3,
    vg_lite_float_t Y3
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t n;
    vg_lite_path_point_ptr point0, point1;
    vg_lite_float_t x, y;
    vg_lite_float_t a1x, a1y, a2x, a2y, a3x, a3y;
    vg_lite_float_t ddf0, ddf1, t1, t2, upper_bound;

    if(!stroke_conversion)
        return VG_LITE_INVALID_ARGUMENT;

    /* Formula.
    * f(t) = (1 - t)^3 * p0 + 3 * t * (1 - t)^2 * p1 + 3 * t^2 * (1 - t) * p2 + t^3 * p3
    *      = a0 + a1 * t + a2 * t^2 + a3 * t^3
    */
    x = X1 - X0;
    a1x = x + x + x;
    y = Y1 - Y0;
    a1y = y + y + y;
    x = X0 - X1 - X1 + X2;
    a2x = x + x + x;
    y = Y0 - Y1 - Y1 + Y2;
    a2y = y + y + y;
    x = X1 - X2;
    a3x = x + x + x + X3 - X0;
    y = Y1 - Y2;
    a3y = y + y + y + Y3 - Y0;

    /* Step 1: Calculate N. */
    /* Lefan's method. */
    /*  df(t)/dt  = a1 + 2 * a2 * t + 3 * a3 * t^2
    * d2f(t)/dt2 = 2 * a2 + 6 * a3 * t
    * N = ceil(sqrt(max(ddfx(0)^2 + ddfy(0)^2, ddfx(1)^2 + ddyf(1)^2) / epsilon / 8))
    */

    ddf0 = a2x * a2x + a2y * a2y;
    t1 = a2x + a3x + a3x + a3x;
    t2 = a2y + a3y + a3y + a3y;
    ddf1 = t1 * t1 + t2 * t2;
    upper_bound = ddf0 > ddf1 ? ddf0 : ddf1;
    upper_bound = SQRTF(upper_bound);
    upper_bound += upper_bound;
    upper_bound = SQRTF(upper_bound);
    if(stroke_conversion->fattened) {
        upper_bound *= stroke_conversion->line_width;
    }
    n = (uint32_t) ceil(upper_bound);

    if(n == 0 || n > 256) {
        n = 256;
    }

    /* Add extra P0 for incoming tangent. */
    point0 = stroke_conversion->path_end;
    /* First add P1/P2/P3 to calculate incoming tangent, which is saved in P0. */
    if(X0 != X1 || Y0 != Y1) {
        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, X1, Y1, vgcFLATTEN_START));
    }
    else if(X0 != X2 || Y0 != Y2) {
        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, X2, Y2, vgcFLATTEN_START));
    }
    else {
        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, X3, Y3, vgcFLATTEN_START));
    }
    point1 = stroke_conversion->path_end;
    /* Change the point1's coordinates back to P0. */
    point1->x = X0;
    point1->y = Y0;
    point0->length = 0.0f;

    if(n > 1) {
        vg_lite_float_t d, dsquare, dcube, dx, dy, ddx, ddy, dddx, dddy;
        vg_lite_float_t ratioX, ratioY;
        uint32_t i;

        /* Step 2: Calculate deltas */
        /*   Df(t) = f(t + d) - f(t)
        *  DDf(t) = Df(t + d) - Df(t)
        * DDDf(t) = DDf(t + d) - DDf(t)
        *    f(0) = a0
        *   Df(0) = a1 * d + a2 * d^2 + a3 * d^3
        *  DDf(0) = 2 * a2 * d^2 + 6 * a3 * d^3
        * DDDf(0) = 6 * a3 * d^3
        */
        d = 1.0f / (vg_lite_float_t) n;
        dsquare   = d * d;
        dcube     = dsquare * d;
        ddx  = a2x * dsquare;
        ddy  = a2y * dsquare;
        dddx = a3x * dcube;
        dddy = a3y * dcube;
        dx   = a1x * d + ddx + dddx;
        dy   = a1y * d + ddy + dddy;
        ddx  += ddx;
        ddy  += ddy;
        dddx *= 6.0f;
        dddy *= 6.0f;
        ddx  += dddx;
        ddy  += dddy;

        /* Step 3: Add points. */
        ratioX = dx / X0;
        if(ratioX < 0.0f) ratioX = -ratioX;
        ratioY = dy / Y0;
        if(ratioY < 0.0f) ratioY = -ratioY;
        if(ratioX > 1.0e-6f && ratioY > 1.0e-6f) {
            x = X0;
            y = Y0;
            for(i = 1; i < n; i++) {
                x += dx;
                y += dy;

                /* Add a point to subpath. */
                VG_LITE_ERROR_HANDLER(_add_point_to_point_list_wdelta(stroke_conversion, x, y, dx, dy, vgcFLATTEN_MIDDLE));
                dx += ddx;
                ddx += dddx;
                dy += ddy;
                ddy += dddy;
            }
        }
        else {
            for(i = 1; i < n; i++) {
                vg_lite_float_t t = (vg_lite_float_t) i / (vg_lite_float_t) n;
                vg_lite_float_t tSquare = t * t;
                vg_lite_float_t tCube = tSquare * t;
                vg_lite_float_t a0 =  1.0f -  3.0f * t + 3.0f * tSquare -        tCube;
                vg_lite_float_t a1 =          3.0f * t - 6.0f * tSquare + 3.0f * tCube;
                vg_lite_float_t a2 =                     3.0f * tSquare - 3.0f * tCube;
                vg_lite_float_t a3 =                                             tCube;

                x  = a0 * X0 + a1 * X1 + a2 * X2 + a3 * X3;
                y  = a0 * Y0 + a1 * Y1 + a2 * Y2 + a3 * Y3;

                /* Add a point to subpath. */
                VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, x, y, vgcFLATTEN_MIDDLE));
            }
        }
    }

    /* Add point 3 separately to avoid cumulative errors. */
    VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, X3, Y3, vgcFLATTEN_END));

    /* Add extra P3 for outgoing tangent. */
    /* First change P3(point0)'s coordinates to P0/P1/P2. */
    point0 = stroke_conversion->path_end;
    if(X3 != X2 || Y3 != Y2) {
        point0->x = X2;
        point0->y = Y2;
    }
    else if(X3 != X1 || Y3 != Y1) {
        point0->x = X1;
        point0->y = Y1;
    }
    else {
        point0->x = X0;
        point0->y = Y0;
    }

    /* Add P3 to calculate outgoing tangent. */
    VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, X3, Y3, vgcFLATTEN_NO));

    point1 = stroke_conversion->path_end;

    /* Change point0's coordinates back to P3. */
    point0->x = X3;
    point0->y = Y3;
    point0->length = 0.0f;

ErrorHandler:
    return error;
}
#endif /* gcFEATURE_VG_SIMPLYFIED_BEZIER */

#define GETINCREMENT(pointer, datatype_size) \
    (datatype_size - (PTR2SIZE(pointer) & (datatype_size - 1)))

#define SKIPTODATA(pointer, datatype_size, SIZE) \
    /* Determine the increment value. */ \
    increment = GETINCREMENT(pointer, datatype_size); \
    /* Skip to the data. */ \
    pointer += increment; \
    SIZE -= increment

#define VGSL_GETVALUE(X) \
    X = get_value(data_pointer); \
    data_pointer += data_type_size; \
    size -= data_type_size

#define VGSL_GETCOORDXY(X, Y) \
    VGSL_GETVALUE(X); \
    VGSL_GETVALUE(Y); \
    if (is_relative) { X += ox; Y += oy; }

typedef vg_lite_float_t(*vg_value_getter)(int8_t * Data);

static vg_lite_error_t _flatten_path(
    vg_lite_stroke_t * stroke_conversion,
    vg_lite_path_t * path
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t increment;
    uint8_t is_relative;
    uint32_t size;
    uint32_t path_command;
    uint32_t prev_command;
    uint8_t data_type_size;
    int8_t * data_pointer = NULL;
    int8_t * data_pointer_use = NULL;
    vg_lite_float_t sx, sy;
    vg_lite_float_t ox, oy;
    vg_lite_float_t px, py;
    vg_lite_float_t x0, y0, x1, y1, x2, y2;
    vg_value_getter get_value = NULL;

    if(!stroke_conversion || !path)
        return VG_LITE_INVALID_ARGUMENT;

    sx = sy = ox = oy = px = py = 0.0f;

    prev_command = 0xFF;

    /* Select the data picker. */
    switch(path->format) {
        case VG_LITE_S8:
            data_type_size = 1;
            get_value = _GetS8_NS_NB;
            break;

        case VG_LITE_S16:
            data_type_size = 2;
            get_value = _GetS16_NS_NB;
            break;

        case VG_LITE_S32:
            data_type_size = 4;
            get_value = _GetS32_NS_NB;
            break;

        case VG_LITE_FP32:
            data_type_size = 4;
            get_value = _GetF_NS_NB;
            break;

        default:
            error = VG_LITE_INVALID_ARGUMENT;
            goto ErrorHandler;
    }

    if((path->path_type == VG_LITE_DRAW_FILL_PATH) || (path->path_type == VG_LITE_DRAW_FILL_STROKE_PATH)) {
        if(path->path_length % (3 * data_type_size) == 0) {
            /* add END_PATH if path_data have no END_PATH */
            stroke_conversion->add_end = 1;
            path->path_length = path->path_length + data_type_size;
            data_pointer_use = (int8_t *)vg_lite_os_malloc(path->path_length);
            if(!data_pointer_use)
                return VG_LITE_OUT_OF_RESOURCES;
            memset(data_pointer_use, 0, path->path_length);
            memcpy((int8_t *)data_pointer_use, (int8_t *)path->path, path->path_length - data_type_size);
            vg_lite_os_free(path->path);
            path->path = data_pointer_use;
            path->pdata_internal = 1;
        }
        if(path->add_end == 1) {
            stroke_conversion->add_end = 1;
        }
        else {
            stroke_conversion->add_end = 0;
        }
    }

    /* Determine the data size. */
    size = path->path_length;

    /* Determine the beginning of the path data. */
    data_pointer = (int8_t *)path->path;

    /* Add an extra gcvVGCMD_MOVE 0.0 0.0 to handle the case the first command is not gcvVGCMD_MOVE. */
    if((*data_pointer & 0xfe) != VLC_OP_MOVE) {
        /* Add first point to subpath. */
        VG_LITE_ERROR_HANDLER(_create_new_point_list(stroke_conversion, 0.f, 0.f, vgcFLATTEN_NO));
    }

    while(size > 0) {
        /* Get the command. */
        path_command = *data_pointer & 0x1F;

        /* Assume absolute. */
        is_relative = VGL_FALSE;

        switch(path_command) {
            case VLC_OP_END:
                /* Skip the command. */
                size -= 1;

                if(prev_command == VLC_OP_END) {
                    /* Continuous gcvVGCMD_CLOSE - do nothing. */
                    break;
                }

                if((prev_command & 0xfe) == VLC_OP_MOVE) {
                    /* Delete the invalid path list. */
                    vg_lite_path_list_ptr path_list_divide = stroke_conversion->cur_list;
                    vg_lite_os_free(path_list_divide->path_points);
                    vg_lite_os_free(path_list_divide);
                    if(stroke_conversion->cur_list == stroke_conversion->path_list_divide) {
                        stroke_conversion->cur_list = stroke_conversion->path_list_divide = NULL;
                        stroke_conversion->path_end = NULL;
                        stroke_conversion->path_points = NULL;
                        stroke_conversion->point_count = 0;
                    }
                    else {
                        stroke_conversion->cur_list = stroke_conversion->path_list_divide;
                        while(stroke_conversion->cur_list->next != path_list_divide)
                            stroke_conversion->cur_list = stroke_conversion->cur_list->next;
                        stroke_conversion->path_end = stroke_conversion->cur_list->path_end;
                        stroke_conversion->point_count = stroke_conversion->cur_list->point_count;
                        stroke_conversion->cur_list->next = NULL;
                    }
                    break;
                }

                if(!stroke_conversion->add_end) {
                    /* Check if subPath is already closed. */
                    if(ox != sx || oy != sy) {
                        /* Add a line from current point to the first point of current subpath. */
                        VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, sx, sy, vgcFLATTEN_NO));
                    }
                    if(stroke_conversion->path_points != stroke_conversion->path_end) {
                        /* Copy tangent data from first point to last_point. */
                        vg_lite_path_point_ptr first_point = stroke_conversion->path_points;
                        vg_lite_path_point_ptr last_point = stroke_conversion->path_end;
                        last_point->length = first_point->length;
                        last_point->tangentX = first_point->tangentX;
                        last_point->tangentY = first_point->tangentY;
                    }
                    else {
                        /* Single point path. */
                        vg_lite_path_point_ptr point = stroke_conversion->path_points;
                        point->tangentX = 0.0f;
                        point->tangentY = 0.0f;
                        point->length = 0.0f;
                    }
                    stroke_conversion->cur_list->closed = 1;
                    stroke_conversion->closed = 1;
                    stroke_conversion->path_end->next = NULL;
                }
                break;

            case VLC_OP_CLOSE:
                /* Skip the command. */
                SKIPTODATA(data_pointer, data_type_size, size);

                if(prev_command == VLC_OP_CLOSE) {
                    /* Continuous gcvVGCMD_CLOSE - do nothing. */
                    break;
                }

                /* Check if subPath is already closed. */
                if(ox != sx || oy != sy) {
                    /* Add a line from current point to the first point of current subpath. */
                    VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, sx, sy, vgcFLATTEN_NO));
                }

                if(stroke_conversion->path_points != stroke_conversion->path_end) {
                    /* Copy tangent data from first point to last_point. */
                    vg_lite_path_point_ptr first_point = stroke_conversion->path_points;
                    vg_lite_path_point_ptr last_point = stroke_conversion->path_end;
                    last_point->length = first_point->length;
                    last_point->tangentX = first_point->tangentX;
                    last_point->tangentY = first_point->tangentY;
                }
                else {
                    /* Single point path. */
                    vg_lite_path_point_ptr point = stroke_conversion->path_points;
                    point->tangentX = 0.0f;
                    point->tangentY = 0.0f;
                    point->length = 0.0f;
                }

                px = ox = sx;
                py = oy = sy;

                stroke_conversion->cur_list->closed = 1;
                stroke_conversion->closed = 1;
                break;

            case VLC_OP_MOVE_REL:
                is_relative = 1;

            case VLC_OP_MOVE:        /* Indicate the beginning of a new sub-path. */
                /* Skip to the data. */
                SKIPTODATA(data_pointer, data_type_size, size);
                VGSL_GETCOORDXY(x0, y0);

                if((prev_command & 0xfe) == VLC_OP_MOVE) {
                    /* Continuous gcvVGCMD_MOVE draw nothing */
                    stroke_conversion->path_points->x = x0;
                    stroke_conversion->path_points->y = y0;
                }
                else {
                    /* First command is gcvVGCMD_MOVE. */
                    /* Add first point to subpath. */
                    VG_LITE_ERROR_HANDLER(_create_new_point_list(stroke_conversion, x0, y0, vgcFLATTEN_NO));
                }

                sx = px = ox = x0;
                sy = py = oy = y0;
                break;

            case VLC_OP_LINE_REL:
                is_relative = 1;

            case VLC_OP_LINE:
                /* Skip to the data. */
                SKIPTODATA(data_pointer, data_type_size, size);
                VGSL_GETCOORDXY(x0, y0);

                /* Add a point to subpath. */
                VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, x0, y0, vgcFLATTEN_NO));

                px = ox = x0;
                py = oy = y0;
                break;

            case VLC_OP_QUAD_REL:
                is_relative = 1;

            case VLC_OP_QUAD:
                /* Skip to the data. */
                SKIPTODATA(data_pointer, data_type_size, size);
                VGSL_GETCOORDXY(x0, y0);
                VGSL_GETCOORDXY(x1, y1);

                if((ox == x0 && oy == y0) && (ox == x1 && oy == y1)) {
                    /* Degenerated Bezier curve.  Becomes a point. */
                    /* Discard zero-length segments. */
                }
                else if((ox == x0 && oy == y0) || (x0 == x1 && y0 == y1)) {
                    /* Degenerated Bezier curve.  Becomes a line. */
                    /* Add a point to subpath. */
                    VG_LITE_ERROR_HANDLER(_add_point_to_point_list(stroke_conversion, x1, y1, vgcFLATTEN_NO));
                }
                else {
#if gcFEATURE_VG_SIMPLYFIED_BEZIER
                    vg_lite_float_t curve[6] = { ox, oy, x0, y0, x1, y1 };
#if (CHIPID != 0x265)
                    vg_lite_float_t subCurve[6] = { 0, 0, 0, 0, 0, 0};
                    VG_LITE_ERROR_HANDLER(_flatten_quad_bezier(stroke_conversion, curve, subCurve, 0));
#else
                    VG_LITE_ERROR_HANDLER(_flatten_quad_bezier_original(stroke_conversion, curve, 0));
#endif
#else
                    VG_LITE_ERROR_HANDLER(_flatten_quad_bezier(stroke_conversion, ox, oy, x0, y0, x1, y1));
#endif
                }

                px = x0;
                py = y0;
                ox = x1;
                oy = y1;
                break;

            case VLC_OP_CUBIC_REL:
                is_relative = 1;

            case VLC_OP_CUBIC:
                /* Skip to the data. */
                SKIPTODATA(data_pointer, data_type_size, size);
                VGSL_GETCOORDXY(x0, y0);
                VGSL_GETCOORDXY(x1, y1);
                VGSL_GETCOORDXY(x2, y2);

                if((ox == x0 && oy == y0) && (ox == x1 && oy == y1) && (ox == x2 && oy == y2)) {
                    /* Degenerated Bezier curve.  Becomes a point. */
                    /* Discard zero-length segments. */
                }
                else {
#if gcFEATURE_VG_SIMPLYFIED_BEZIER
                    vg_lite_float_t curve[8] = { ox, oy, x0, y0, x1, y1, x2, y2 };
#if (CHIPID != 0x265)
                    vg_lite_float_t subCurve[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
                    VG_LITE_ERROR_HANDLER(_flatten_cubic_bezier(stroke_conversion, curve, subCurve, 0));
#else
                    VG_LITE_ERROR_HANDLER(_flatten_cubic_bezier_original(stroke_conversion, curve, 0));
#endif
#else
                    VG_LITE_ERROR_HANDLER(_flatten_cubic_bezier(stroke_conversion, ox, oy, x0, y0, x1, y1, x2, y2));
#endif
                }

                px = x1;
                py = y1;
                ox = x2;
                oy = y2;
                break;
            default:
                error = VG_LITE_INVALID_ARGUMENT;
                goto ErrorHandler;
        }
        prev_command = path_command;
    }

    if((prev_command != VLC_OP_END)) {
        stroke_conversion->cur_list->path_end->next = NULL;
        stroke_conversion->path_end->next = NULL;
        if(stroke_conversion->point_count == 1) {
            /* Single point path. */
            vg_lite_path_point_ptr point = stroke_conversion->path_points;
            point->tangentX = 0.0f;
            point->tangentY = 0.0f;
            point->length = 0.0f;
        }
    }

ErrorHandler:
    return error;
}

static vg_lite_error_t
_add_point_to_right_stroke_point_list_tail(
    vg_lite_stroke_t * stroke_conversion,
    vg_lite_float_t X,
    vg_lite_float_t Y
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_path_point_ptr point;

    if(!stroke_conversion)
        return VG_LITE_INVALID_ARGUMENT;

    point = (vg_lite_path_point_ptr)vg_lite_os_malloc(sizeof(*point));
    if(!point)
        return VG_LITE_OUT_OF_RESOURCES;

    memset(point, 0, sizeof(*point));

    point->x = X;
    point->y = Y;
    point->curve_type = CURVE_LINE;
    point->prev = stroke_conversion->right_point;
    point->next = NULL;
    stroke_conversion->right_point->next = point;
    stroke_conversion->right_point = point;
    stroke_conversion->stroke_count++;

    stroke_conversion->last_stroke->point_count++;

    return error;
}

static vg_lite_error_t
_add_point_to_left_point_list_head(
    vg_lite_stroke_t  * stroke_conversion,
    vg_lite_float_t X,
    vg_lite_float_t Y
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_path_point_ptr point;

    if(!stroke_conversion)
        return VG_LITE_INVALID_ARGUMENT;

    point = (vg_lite_path_point_ptr)vg_lite_os_malloc(sizeof(*point));
    if(!point)
        return VG_LITE_OUT_OF_RESOURCES;

    memset(point, 0, sizeof(*point));

    point->x = X;
    point->y = Y;
    point->curve_type = CURVE_LINE;
    point->next = stroke_conversion->left_point;
    point->prev = NULL;
    stroke_conversion->left_point->prev = point;
    stroke_conversion->left_point = point;
    stroke_conversion->stroke_count++;

    stroke_conversion->last_stroke->point_count++;

    return error;
}

static vg_lite_error_t _add_stroke_sub_path(
    vg_lite_stroke_t * stroke_conversion,
    vg_lite_sub_path_ptr * sub_path
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    if(!stroke_conversion || !sub_path)
        return VG_LITE_INVALID_ARGUMENT;

    *sub_path = (vg_lite_sub_path_ptr)vg_lite_os_malloc(sizeof(**sub_path));
    if(!*sub_path)
        return VG_LITE_OUT_OF_RESOURCES;

    memset(*sub_path, 0, sizeof(**sub_path));

    if(stroke_conversion->last_stroke != NULL) {
        stroke_conversion->last_stroke->next = *sub_path;
        stroke_conversion->last_stroke = *sub_path;
    }
    else {
        stroke_conversion->last_stroke = stroke_conversion->stroke_paths = *sub_path;
    }

    return error;
}

static vg_lite_error_t
_add_zero_length_stroke_sub_path(
    vg_lite_stroke_t  * stroke_conversion,
    vg_lite_sub_path_ptr * stroke_subpath
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_path_point_ptr new_point, Point;
    vg_lite_sub_path_ptr stroke_sub_path;
    vg_lite_float_t half_width;

    if(!stroke_conversion)
        return VG_LITE_INVALID_ARGUMENT;

    half_width = stroke_conversion->half_width;
    Point = stroke_conversion->path_points;
    if(stroke_conversion->cap_style == VG_LITE_CAP_BUTT) {
        /* No need to draw zero-length subPath for gcvCAP_BUTT. */
        error = VG_LITE_SUCCESS;
        goto ErrorHandler;
    }

    VG_LITE_ERROR_HANDLER(_add_stroke_sub_path(stroke_conversion, &stroke_sub_path));

    if(stroke_conversion->cap_style == VG_LITE_CAP_SQUARE) {
        /* Draw a square along the point's direction. */
        vg_lite_float_t dx, dy;

        if(Point->tangentX == 0.0f || Point->tangentY == 0.0f) {
            dx = half_width;
            dy = 0.0f;
        }
        else {
            dx =  Point->tangentY * half_width;
            dy = -Point->tangentX * half_width;
        }

        new_point = (vg_lite_path_point_ptr)vg_lite_os_malloc(sizeof(*new_point));
        if(!new_point)
            return VG_LITE_OUT_OF_RESOURCES;
        memset(new_point, 0, sizeof(*new_point));

        new_point->x = Point->x + dx + dy;
        new_point->y = Point->y - dx + dy;
        new_point->curve_type = CURVE_LINE;
        stroke_sub_path->point_list = stroke_conversion->right_point = new_point;
        stroke_sub_path->point_count = 1;

        VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                         Point->x + dx - dy, Point->y + dx + dy));

        VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                         Point->x - dx - dy, Point->y + dx - dy));

        VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                         Point->x - dx + dy, Point->y - dx - dy));
    }
    else {
        /* Draw a circle. */
        new_point = (vg_lite_path_point_ptr)vg_lite_os_malloc(sizeof(*new_point));
        if(!new_point)
            return VG_LITE_OUT_OF_RESOURCES;
        memset(new_point, 0, sizeof(*new_point));

        new_point->x = Point->x + half_width;
        new_point->y = Point->y;
        new_point->curve_type = CURVE_LINE;
        stroke_sub_path->point_list = stroke_conversion->right_point = new_point;
        stroke_sub_path->point_count = 1;

        /* Add upper half circle. */
        VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                         Point->x - half_width, Point->y));

        stroke_conversion->right_point->curve_type = CURVE_ARC_SCCW_HALF;
        stroke_conversion->right_point->tangentX = Point->x;
        stroke_conversion->right_point->tangentY = Point->y;

        /* Add lower half circle. */
        VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                         Point->x + half_width, Point->y));

        stroke_conversion->right_point->curve_type = CURVE_ARC_SCCW_HALF;
        stroke_conversion->right_point->tangentX = Point->x;
        stroke_conversion->right_point->tangentY = Point->y;
    }

    stroke_sub_path->end_point = stroke_conversion->right_point;
    stroke_sub_path->end_point->next = NULL;

ErrorHandler:
    return error;
}

/* Special asin(x) for quick calculation when -sqrt(0.5) <= x <= sqrt(0.5). */
static vg_lite_float_t _Asin(
    vg_lite_float_t X
)
{
    vg_lite_float_t x = X;
    vg_lite_float_t x2 = X * X;
    vg_lite_float_t s = X;

    x *= x2;
    s += x * 0.1660510562575219f;
    x *= x2;
    s += x * 0.084044676143618186f;
    x *= x2;
    s += x * 0.0023776176698039313f;
    x *= x2;
    s += x * 0.10211922020091345f;

    return s;
}

/* Special cos(x) for quick calculation when -PI <= x <= PI. */
static vg_lite_float_t _Cos(
    vg_lite_float_t X
)
{
    vg_lite_float_t x2 = X * X;
    vg_lite_float_t x = x2;
    vg_lite_float_t s = 1.0f;

    s -= x * 0.49985163079668843f;
    x *= x2;
    s += x * 0.041518066216932693f;
    x *= x2;
    s -= x * 0.0013422997970712939f;
    x *= x2;
    s += x * 0.000018930111278021357f;

    return s;
}

/* Special sin(x) for quick calculation when -PI <= x <= PI. */
static vg_lite_float_t _Sine(
    vg_lite_float_t X
)
{
    vg_lite_float_t x = X;
    vg_lite_float_t x2 = X * X;
    vg_lite_float_t s = X;

    x *= x2;
    s -= x * 0.16664527099620879f;
    x *= x2;
    s += x * 0.0083154803736487041f;
    x *= x2;
    s -= x * 0.00019344151251408578f;
    x *= x2;
    s += x * 0.0000021810214160988925f;

    return s;
}

static vg_lite_float_t
_Angle(
    vg_lite_float_t X,
    vg_lite_float_t Y,
    vg_lite_float_t Length
)
{
    vg_lite_float_t angle;
    vg_lite_float_t ux = (X >= 0.0f ? X : -X);
    vg_lite_float_t uy = (Y >= 0.0f ? Y : -Y);

    if(ux > uy) {
        angle = ((uy > 0.0f && ux < Length) ? _Asin(uy / Length) : 0.0f);
    }
    else {
        angle = ((ux > 0.0f && uy < Length) ? (FLOAT_PI_HALF - _Asin(ux / Length)) : FLOAT_PI_HALF);
    }

    if(X < 0.0f) angle = FLOAT_PI - angle;
    if(Y < 0.0f) angle = -angle;

    return angle;
}

/* The arc is always counter clockwise and less than half circle (small). */
static vg_lite_error_t
_convert_circle_arc(
    vg_lite_stroke_t * stroke_conversion,
    vg_lite_float_t Radius,
    vg_lite_float_t CenterX,
    vg_lite_float_t CenterY,
    vg_lite_float_t StartX,
    vg_lite_float_t StartY,
    vg_lite_float_t EndX,
    vg_lite_float_t EndY,
    uint8_t Half_circle,
    vg_lite_path_point_ptr * point_list
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    /*gceVGCMD segmentCommand;*/
    vg_lite_float_t theta1, theta_span;
    uint32_t segs;
    vg_lite_float_t theta, theta_half, theta2;
    vg_lite_float_t cos_theta_half;
    vg_lite_float_t control_ratio;
    vg_lite_float_t controlX, controlY, anchorX, anchorY;
    /*gctFLOAT lastX, lastY;*/
    vg_lite_path_point_ptr point, start_point, last_point;

    if(!stroke_conversion || !point_list)
        return VG_LITE_INVALID_ARGUMENT;

    /* Converting. */
    theta1 = _Angle(StartX - CenterX, StartY - CenterY, Radius);
    if(Half_circle) {
        theta_span = FLOAT_PI;
        segs = 4;
        theta = FLOAT_PI_QUARTER;
        theta_half = FLOAT_PI_EIGHTH;
        cos_theta_half = FLOAT_COS_PI_EIGHTH;
    }
    else {
        theta_span = _Angle(EndX - CenterX, EndY - CenterY, Radius) - theta1;
        if(theta_span == 0.0f) {
            /* Handle specail case for huge scaling. */
            *point_list = NULL;
            error = VG_LITE_SUCCESS;
            return error;
        }

        if((theta_span < 0)) {
            theta_span += FLOAT_PI_TWO;
        }

        /* Calculate the number of quadratic Bezier curves. */
        /* Assumption: most of angles are small angles. */
        if(theta_span <= FLOAT_PI_QUARTER)         segs = 1;
        else if(theta_span <= FLOAT_PI_HALF)            segs = 2;
        else if(theta_span <= FLOAT_PI_THREE_QUARTER)   segs = 3;
        else                                            segs = 4;

        theta = theta_span / segs;
        theta_half = theta / 2.0f;
        cos_theta_half = _Cos(theta_half);
    }

    /* Determine the segment command. */
    /*egmentCommand = gcvVGCMD_ARC_QUAD;*/

    /* Generate quadratic Bezier curves. */
    start_point = last_point = NULL;
    control_ratio = Radius / cos_theta_half;
    while(segs-- > 0) {
        theta1 += theta;

        theta2 = theta1 - theta_half;
        if(theta2 > FLOAT_PI) theta2 -= FLOAT_PI_TWO;
        controlX = CenterX + _Cos(theta2)  * control_ratio;
        controlY = CenterY + _Sine(theta2) * control_ratio;

        theta2 = theta1;
        if(theta2 > FLOAT_PI) theta2 -= FLOAT_PI_TWO;
        anchorX = CenterX + _Cos(theta2)  * Radius;
        anchorY = CenterY + _Sine(theta2) * Radius;

        if(segs == 0) {
            /* Use end point directly to avoid accumulated errors. */
            anchorX = EndX;
            anchorY = EndY;
        }

        /* Add control point. */
        point = (vg_lite_path_point_ptr)vg_lite_os_malloc(sizeof(*point));
        if(!point)
            return VG_LITE_OUT_OF_RESOURCES;

        memset(point, 0, sizeof(*point));

        point->x = controlX;
        point->y = controlY;
        point->curve_type = CURVE_QUAD_CONTROL;
        if(last_point) {
            last_point->next = point;
            last_point = point;
        }
        else {
            start_point = last_point = point;
        }

        /* Add anchor point. */
        point = (vg_lite_path_point_ptr)vg_lite_os_malloc(sizeof(*point));
        if(!point) {
            error = VG_LITE_OUT_OF_RESOURCES;
            goto ErrorHandler;
        }

        memset(point, 0, sizeof(*point));

        point->x = anchorX;
        point->y = anchorY;
        point->curve_type = CURVE_QUAD_ANCHOR;
        last_point->next = point;
        last_point = point;
    }

    if(last_point) {
        last_point->next = NULL;
    }
    *point_list = start_point;

    return error;

ErrorHandler:
    /* Return status. */
    if(start_point) {
        vg_lite_os_free(start_point);
        start_point = last_point = NULL;
    }
    return error;
}

static vg_lite_error_t
_start_new_stroke_sub_path(
    vg_lite_stroke_t * stroke_conversion,
    vg_lite_float_t X,
    vg_lite_float_t Y,
    vg_lite_float_t Dx,
    vg_lite_float_t Dy,
    uint8_t add_end_cap,
    vg_lite_sub_path_ptr * stroke_subpath
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    vg_lite_sub_path_ptr stroke_sub_path;
    vg_lite_path_point_ptr new_point;

    if(!stroke_conversion || !stroke_subpath)
        return VG_LITE_INVALID_ARGUMENT;

    VG_LITE_ERROR_HANDLER(_add_stroke_sub_path(stroke_conversion, &stroke_sub_path));

    new_point = (vg_lite_path_point_ptr)vg_lite_os_malloc(sizeof(*new_point));
    if(!new_point)
        return VG_LITE_OUT_OF_RESOURCES;

    memset(new_point, 0, sizeof(*new_point));
    new_point->x = X + Dx;
    new_point->y = Y + Dy;
    new_point->prev = NULL;
    new_point->curve_type = CURVE_LINE;
    stroke_conversion->stroke_points = stroke_conversion->right_point = new_point;

    stroke_sub_path->point_list = stroke_conversion->right_point = new_point;

    new_point = (vg_lite_path_point_ptr)vg_lite_os_malloc(sizeof(*new_point));
    if(!new_point)
        return VG_LITE_OUT_OF_RESOURCES;

    memset(new_point, 0, sizeof(*new_point));
    new_point->x = X - Dx;
    new_point->y = Y - Dy;
    new_point->curve_type = CURVE_LINE;
    new_point->next = NULL;
    stroke_conversion->stroke_end = stroke_conversion->left_point = new_point;

    stroke_conversion->stroke_count = 2;

    stroke_sub_path->end_point = stroke_conversion->left_point = new_point;
    stroke_sub_path->point_count = 2;

    if(add_end_cap) {
        /* Add end cap if the subPath is not closed. */
        switch(stroke_conversion->cap_style) {
            case VG_LITE_CAP_BUTT:
                /* No adjustment needed. */
                break;
            case VG_LITE_CAP_ROUND:
                /* Add curve. */
                /* Add the starting point again as arc. */
                VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                                 stroke_sub_path->point_list->x, stroke_sub_path->point_list->y));
                stroke_conversion->right_point->curve_type = CURVE_ARC_SCCW_HALF;
                stroke_conversion->right_point->tangentX = X;
                stroke_conversion->right_point->tangentY = Y;
                /* Change the starting point to end point. */
                stroke_sub_path->point_list->x = stroke_sub_path->end_point->x;
                stroke_sub_path->point_list->y = stroke_sub_path->end_point->y;
                break;
            case VG_LITE_CAP_SQUARE:
                stroke_conversion->right_point->x += Dy;
                stroke_conversion->right_point->y -= Dx;
                stroke_conversion->left_point->x += Dy;
                stroke_conversion->left_point->y -= Dx;
                break;
        }
    }

    *stroke_subpath = stroke_sub_path;

ErrorHandler:
    return error;
}

static void
_adjust_joint_point(
    vg_lite_path_point_ptr Point,
    vg_lite_path_point_ptr join_point,
    vg_lite_float_t X,
    vg_lite_float_t Y,
    vg_lite_float_t Ratio
)
{
    vg_lite_float_t mx = (join_point->x + X) / 2.0f;
    vg_lite_float_t my = (join_point->y + Y) / 2.0f;
    vg_lite_float_t dx = mx - Point->x;
    vg_lite_float_t dy = my - Point->y;

    dx = dx * Ratio;
    dy = dy * Ratio;
    join_point->x = Point->x + dx;
    join_point->y = Point->y + dy;
}

static uint8_t
_is_angle_span_acute(
    vg_lite_float_t Ux,
    vg_lite_float_t Uy,
    vg_lite_float_t Vx,
    vg_lite_float_t Vy
)
{
    return ((Ux * Vx + Uy * Vy) > 0.0f ? 1 : 0);
}

static vg_lite_error_t
_draw_swing_pie_area(
    vg_lite_stroke_t * stroke_conversion,
    vg_lite_path_point_ptr center_point,
    uint8_t end_at_prev_point
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    if(!stroke_conversion)
        return VG_LITE_INVALID_ARGUMENT;

    if(stroke_conversion->swing_ccw) {
        vg_lite_path_point_ptr start_point = stroke_conversion->swing_stroke;
        vg_lite_path_point_ptr end_point = NULL, real_end_point = NULL;
        vg_lite_path_point_ptr point, prev_point;
        uint32_t count = 0;

        {
            if(end_at_prev_point) {
                /* Detach the end point from leftStrokePoint. */
                /* The end point will be added back later. */
                real_end_point = stroke_conversion->left_point;
                stroke_conversion->left_point = real_end_point->next;
                stroke_conversion->left_point->prev = NULL;
            }

            VG_LITE_ERROR_HANDLER(_add_point_to_left_point_list_head(stroke_conversion,
                                                                     center_point->x, center_point->y));
            end_point = stroke_conversion->left_point;

            /* Reverse the point list from startPoint to endPoint. */
            for(point = start_point; point; point = prev_point) {
                prev_point = point->prev;
                point->prev = point->next;
                point->next = prev_point;
                count++;
            }

            if(end_point) {
                end_point->next = start_point->prev;
            }
            start_point->prev->prev = end_point;
            start_point->prev = NULL;
            stroke_conversion->left_point = start_point;

            VG_LITE_ERROR_HANDLER(_add_point_to_left_point_list_head(stroke_conversion,
                                                                     center_point->x, center_point->y));
            VG_LITE_ERROR_HANDLER(_add_point_to_left_point_list_head(stroke_conversion,
                                                                     stroke_conversion->swing_start->x,
                                                                     stroke_conversion->swing_start->y));
            VG_LITE_ERROR_HANDLER(_add_point_to_left_point_list_head(stroke_conversion,
                                                                     end_point->prev->x, end_point->prev->y));

            if(end_at_prev_point) {
                if(real_end_point) {
                    real_end_point->next = stroke_conversion->left_point;
                }
                stroke_conversion->left_point->prev = real_end_point;
                stroke_conversion->left_point = real_end_point;
            }
        }
    }
    else {
        vg_lite_path_point_ptr start_point = stroke_conversion->swing_stroke;
        vg_lite_path_point_ptr end_point = NULL, real_end_point = NULL;
        vg_lite_path_point_ptr point, next_point;
        uint32_t count = 0;

        {
            if(end_at_prev_point) {
                /* Detach the end point from leftStrokePoint. */
                /* The end point will be added back later. */
                real_end_point = stroke_conversion->right_point;
                stroke_conversion->right_point = real_end_point->prev;
                stroke_conversion->right_point->next = NULL;
            }

            VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                             center_point->x, center_point->y));
            end_point = stroke_conversion->right_point;

            /* Reverse the point list from startPoint to endPoint. */
            for(point = start_point; point; point = next_point) {
                next_point = point->next;
                point->next = point->prev;
                point->prev = next_point;
                count++;
            }
            end_point->prev = start_point->next;
            start_point->next->next = end_point;
            start_point->next = NULL;
            stroke_conversion->right_point = start_point;

            VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                             center_point->x, center_point->y));
            VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                             stroke_conversion->swing_start->x,
                                                                             stroke_conversion->swing_start->y));
            VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                             end_point->next->x, end_point->next->y));

            if(end_at_prev_point) {
                real_end_point->prev = stroke_conversion->right_point;
                stroke_conversion->right_point->next = real_end_point;
                stroke_conversion->right_point = real_end_point;
            }
        }
    }

    stroke_conversion->swing_handling = SWING_NO;

ErrorHandler:
    return error;
}

static vg_lite_error_t
_process_line_joint(
    vg_lite_stroke_t * stroke_conversion,
    vg_lite_path_point_ptr Point,
    vg_lite_float_t Length,
    vg_lite_float_t prev_length,
    uint32_t Swing_handling,
    vg_lite_float_t X1,
    vg_lite_float_t Y1,
    vg_lite_float_t X2,
    vg_lite_float_t Y2
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_join_style_t join_style;
    vg_lite_float_t half_width;
    vg_lite_float_t ratio;
    vg_lite_float_t min_length_square;
    vg_lite_float_t cos_theta;
    uint8_t counter_clockwise;
    uint8_t fat_line;
    uint32_t swing_handling = SWING_NO;
    uint8_t handle_short_line = 0;

    if(!stroke_conversion)
        return VG_LITE_INVALID_ARGUMENT;

    join_style = stroke_conversion->join_style;
    half_width = stroke_conversion->half_width;
    fat_line = stroke_conversion->fattened;
    if(stroke_conversion->swing_length < half_width) {
        if(stroke_conversion->need_swing) {
            swing_handling = SWING_OUT;
        }
        else {
            handle_short_line = 1;
        }
    }
    else if(stroke_conversion->stroke_length - stroke_conversion->swing_length < half_width) {
        if(stroke_conversion->need_swing) {
            swing_handling = SWING_IN;
        }
        else {
            handle_short_line = 1;
        }
    }

    if(swing_handling != Swing_handling) {
        error = VG_LITE_INVALID_ARGUMENT;
        goto ErrorHandler;
    }

    /* For flattened curves/arcs, the join style is always round. */
    if((Point->flatten_flag != vgcFLATTEN_NO) && fat_line) {
        join_style = VG_LITE_JOIN_ROUND;
    }

    /* First, determine the turn is clockwise or counter-clockwise. */
    cos_theta = Point->prev->tangentX * Point->tangentX + Point->prev->tangentY * Point->tangentY;

    if(cos_theta > FLOAT_ANGLE_EPSILON_COS) {
        /* Straight line or semi-straight line--no need to handle join. */
        if(stroke_conversion->swing_handling != SWING_NO) {
            /* Begin to swing to the opposite direction. */
            /* Draw the swing area (pie area). */
            VG_LITE_ERROR_HANDLER(_draw_swing_pie_area(stroke_conversion, Point->prev, 1));
        }

        /* Add the new stroke points. */
        VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, X1, Y1));
        VG_LITE_ERROR_HANDLER(_add_point_to_left_point_list_head(stroke_conversion, X2, Y2));
        if(stroke_conversion->swing_handling != SWING_NO) {
            stroke_conversion->swing_count++;
        }

        goto endCheck;
    }
    else if(cos_theta < -FLOAT_ANGLE_EPSILON_COS) {
        /* Almost 180 degree turn. */
        counter_clockwise = 1;
        ratio = FLOAT_MAX;
        min_length_square = FLOAT_MAX;
    }
    else {
        vg_lite_float_t angleSign = Point->prev->tangentX * Point->tangentY - Point->prev->tangentY * Point->tangentX;
        counter_clockwise = (angleSign >= 0.0f ? 1 : 0);
        ratio = 2.0f / (1.0f + cos_theta);
        min_length_square = half_width * half_width * (1.0f - cos_theta) / (1.0f + cos_theta) + 0.02f;
    }

    if(stroke_conversion->swing_handling != SWING_NO) {
        if(counter_clockwise != stroke_conversion->swing_ccw) {
            /* Swing to the opposite direction. */
            /* Draw the swing area (pie area). */
            VG_LITE_ERROR_HANDLER(_draw_swing_pie_area(stroke_conversion, Point->prev, 1));
        }
    }

    if(counter_clockwise) {
        if(stroke_conversion->swing_handling != SWING_NO) {
            vg_lite_path_point_ptr prev_point = stroke_conversion->left_point->next;   /* Skip the line segment movement. */
            vg_lite_float_t deltaX = X2 - prev_point->x;
            vg_lite_float_t deltaY = Y2 - prev_point->y;
            if(_is_angle_span_acute(stroke_conversion->swing_deltax,
                                    stroke_conversion->swing_deltay,
                                    deltaX, deltaY)) {
                /* Continue swinging. */
                stroke_conversion->swing_deltax = deltaX;
                stroke_conversion->swing_deltay = deltaY;
            }
            else {
                /* Swing to the max. */
                /* Draw the swing area (pie area). */
                VG_LITE_ERROR_HANDLER(_draw_swing_pie_area(stroke_conversion, Point->prev, 1));
            }
        }

        /* Check if the miter length is too long for inner intersection. */
        if(stroke_conversion->swing_handling == SWING_NO
           && ! handle_short_line
           && min_length_square <= Length * Length
           && min_length_square <= prev_length * prev_length) {
            /* Adjust leftStrokePoint to the intersection point. */
            _adjust_joint_point(Point, stroke_conversion->left_point, X2, Y2, ratio);
        }
        else if(stroke_conversion->swing_handling == SWING_NO && Point->flatten_flag == vgcFLATTEN_NO) {
            /* Add the point to avoid incorrect sharp angle. */
            VG_LITE_ERROR_HANDLER(_add_point_to_left_point_list_head(stroke_conversion, Point->x, Point->y));
            /* Add the point to form a loop to avoid out-of-bound problem. */
            VG_LITE_ERROR_HANDLER(_add_point_to_left_point_list_head(stroke_conversion, X2, Y2));
        }
        else if(stroke_conversion->swing_handling == SWING_NO && (! fat_line || Swing_handling == SWING_NO)) {
            /* Flattened line segments should not have sharp angle. */
            /* Add the point to form a loop to avoid out-of-bound problem. */
            VG_LITE_ERROR_HANDLER(_add_point_to_left_point_list_head(stroke_conversion, X2, Y2));
        }
        else {
            if(stroke_conversion->swing_handling == SWING_NO) {
                vg_lite_path_point_ptr prev_point = stroke_conversion->left_point;

                /* Start swing handling. */
                stroke_conversion->swing_handling = Swing_handling;
                stroke_conversion->swing_ccw = 1;
                stroke_conversion->swing_start = Point;
                stroke_conversion->swing_centlen = 0.0f;
                stroke_conversion->swing_count = 0;

                /* Save stroking path delta. */
                stroke_conversion->swing_deltax = X2 - prev_point->x;
                stroke_conversion->swing_deltay = Y2 - prev_point->y;

                /* Add extra center point for swing out pie area. */
                /* VIV: [todo] Should adjust prev_point, instead of adding new point? */
                VG_LITE_ERROR_HANDLER(_add_point_to_left_point_list_head(stroke_conversion, Point->x, Point->y));

                /* Add extra start stroke point for swing out pie area. */
                VG_LITE_ERROR_HANDLER(_add_point_to_left_point_list_head(stroke_conversion, prev_point->x, prev_point->y));

                stroke_conversion->swing_stroke = stroke_conversion->left_point;
            }

            /* Add curve. */
            /* Note that the curve will be reversed, so the direction is CW. */
            /* Then, left side is in reversed order, so the direction is CCW. */
            VG_LITE_ERROR_HANDLER(_add_point_to_left_point_list_head(stroke_conversion, X2, Y2));
            stroke_conversion->left_point->curve_type = CURVE_ARC_SCCW;
            stroke_conversion->left_point->tangentX = Point->x;
            stroke_conversion->left_point->tangentY = Point->y;

            stroke_conversion->swing_count++;
        }

        switch(join_style) {
            case VG_LITE_JOIN_ROUND:
                if(cos_theta > FLOAT_MIN_ARC_ANGLE_COS) {
                    /* Add a point. */
                    VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, X1, Y1));
                }
                else {
                    /* Add curve. */
                    VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, X1, Y1));
                    stroke_conversion->right_point->curve_type = CURVE_ARC_SCCW;
                    stroke_conversion->right_point->tangentX = Point->x;
                    stroke_conversion->right_point->tangentY = Point->y;
                }
                break;
            case VG_LITE_JOIN_MITER:
                if(ratio <= stroke_conversion->miter_square) {
                    /* Adjust lastRightStrokePoint to the outer intersection point. */
                    _adjust_joint_point(Point, stroke_conversion->right_point, X1, Y1, ratio);
                    break;
                }
            /* Else use Bevel join style. */
            case VG_LITE_JOIN_BEVEL:
                VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, X1, Y1));
                break;
        }
    }
    else {
        if(stroke_conversion->swing_handling != SWING_NO) {
            vg_lite_path_point_ptr prev_point = stroke_conversion->right_point->prev;  /* Skip the line segment movement. */
            vg_lite_float_t deltaX = X1 - prev_point->x;
            vg_lite_float_t deltaY = Y1 - prev_point->y;
            if(_is_angle_span_acute(stroke_conversion->swing_deltax,
                                    stroke_conversion->swing_deltay,
                                    deltaX, deltaY)) {
                /* Continue swinging. */
                stroke_conversion->swing_deltax = deltaX;
                stroke_conversion->swing_deltay = deltaY;
            }
            else {
                /* Swing to the max. */
                /* Draw the swing area (pie area). */
                VG_LITE_ERROR_HANDLER(_draw_swing_pie_area(stroke_conversion, Point->prev, 1));
            }
        }

        /* Check if the miter length is too long for inner intersection. */
        if(stroke_conversion->swing_handling == SWING_NO
           && ! handle_short_line
           && min_length_square <= Length * Length
           && min_length_square <= prev_length * prev_length) {
            /* Adjust lastRightStrokePoint to the intersection point. */
            _adjust_joint_point(Point, stroke_conversion->right_point, X1, Y1, ratio);
        }
        else if(stroke_conversion->swing_handling == SWING_NO && Point->flatten_flag == vgcFLATTEN_NO) {
            /* Add the point to avoid incorrect sharp angle. */
            VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, Point->x, Point->y));
            /* Add the point to form a loop to avoid out-of-bound problem. */
            VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, X1, Y1));
        }
        else if(stroke_conversion->swing_handling == SWING_NO && (! fat_line || Swing_handling == SWING_NO)) {
            /* Flattened line segments should not have sharp angle. */
            /* Add the point to form a loop to avoid out-of-bound problem. */
            VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, X1, Y1));
        }
        else {
            if(stroke_conversion->swing_handling == SWING_NO) {
                vg_lite_path_point_ptr prev_point = stroke_conversion->right_point;

                /* Start swing handling. */
                stroke_conversion->swing_handling = Swing_handling;
                stroke_conversion->swing_ccw = 0;
                stroke_conversion->swing_start = Point;
                stroke_conversion->swing_centlen = 0.0f;
                stroke_conversion->swing_count = 0;

                /* Save stroking path delta. */
                stroke_conversion->swing_deltax = X1 - prev_point->x;
                stroke_conversion->swing_deltay = Y1 - prev_point->y;

                /* Add extra center point for swing out pie area. */
                /* VIV: [todo] Should adjust prev_point, instead of adding new point? */
                VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, Point->x, Point->y));

                /* Add extra start stroke point for swing out pie area. */
                VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, prev_point->x, prev_point->y));

                stroke_conversion->swing_stroke = stroke_conversion->right_point;
            }

            if(cos_theta > FLOAT_MIN_ARC_ANGLE_COS) {
                /* Add a point. */
                VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, X1, Y1));
            }
            else {
                /* Add curve. */
                /* Note that the curve will be reversed, so the direction is CCW. */
                stroke_conversion->right_point->curve_type = CURVE_ARC_SCCW;
                stroke_conversion->right_point->tangentX = Point->x;
                stroke_conversion->right_point->tangentY = Point->y;
                VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion, X1, Y1));
            }
            stroke_conversion->swing_count++;
        }

        switch(join_style) {
            case VG_LITE_JOIN_ROUND:
                if(cos_theta > FLOAT_MIN_ARC_ANGLE_COS) {
                    /* Add a point. */
                    VG_LITE_ERROR_HANDLER(_add_point_to_left_point_list_head(stroke_conversion, X2, Y2));
                }
                else {
                    /* Add curve. */
                    stroke_conversion->left_point->curve_type = CURVE_ARC_SCCW;
                    stroke_conversion->left_point->tangentX = Point->x;
                    stroke_conversion->left_point->tangentY = Point->y;
                    VG_LITE_ERROR_HANDLER(_add_point_to_left_point_list_head(stroke_conversion, X2, Y2));
                }
                break;
            case VG_LITE_JOIN_MITER:
                if(ratio <= stroke_conversion->miter_square) {
                    /* Adjust leftStrokePoint to the outer intersection point. */
                    _adjust_joint_point(Point, stroke_conversion->left_point, X2, Y2, ratio);
                    break;
                }
            /* Else use Bevel join style. */
            case VG_LITE_JOIN_BEVEL:
                VG_LITE_ERROR_HANDLER(_add_point_to_left_point_list_head(stroke_conversion, X2, Y2));
                break;
        }
    }

endCheck:
    if(stroke_conversion->need_swing) {
        stroke_conversion->swing_length += Point->length;
    }
    if(stroke_conversion->swing_handling != SWING_NO) {
        if(Point->flatten_flag == vgcFLATTEN_END ||
           (stroke_conversion->swing_handling == SWING_OUT &&
            stroke_conversion->swing_length > half_width)) {
            /* Draw the swing area (pie area). */
            VG_LITE_ERROR_HANDLER(_draw_swing_pie_area(stroke_conversion, Point, 0));
        }
        else {
            /* Check if center line will move too far. */
            stroke_conversion->swing_centlen += Point->length;
            if(stroke_conversion->swing_centlen > FLOAT_SWING_CENTER_RANGE) {
                /* Draw the swing area (pie area). */
                VG_LITE_ERROR_HANDLER(_draw_swing_pie_area(stroke_conversion, Point, 0));
            }
        }
    }

ErrorHandler:
    return error;
}

static vg_lite_error_t
_close_stroke_sub_path(
    vg_lite_stroke_t * stroke_conversion,
    vg_lite_path_point_ptr Point,
    vg_lite_float_t Length,
    vg_lite_float_t prev_length,
    uint8_t Swing_handling,
    vg_lite_path_point_ptr first_stroke_point,
    vg_lite_path_point_ptr last_stroke_point
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    if(!stroke_conversion)
        return VG_LITE_INVALID_ARGUMENT;

    /* Handle line joint style for the first/last point in closed path. */
    VG_LITE_ERROR_HANDLER(_process_line_joint(
                              stroke_conversion, Point,
                              Length, prev_length, Swing_handling,
                              first_stroke_point->x, first_stroke_point->y,
                              last_stroke_point->x, last_stroke_point->y
                          ));

    if(stroke_conversion->cap_style == VG_LITE_CAP_SQUARE
       && stroke_conversion->join_style == VG_LITE_JOIN_MITER
       && stroke_conversion->pattern_count > 0) {
        stroke_conversion->left_point->x = last_stroke_point->x;
        stroke_conversion->left_point->y = last_stroke_point->y;
    }

    /* Adjust the two end ponts of the first point. */
    first_stroke_point->x = stroke_conversion->right_point->x;
    first_stroke_point->y = stroke_conversion->right_point->y;
    last_stroke_point->x = stroke_conversion->left_point->x;
    last_stroke_point->y = stroke_conversion->left_point->y;

    /* Concatnate right and left point lists. */
    stroke_conversion->right_point->next = stroke_conversion->left_point;
    stroke_conversion->left_point->prev = stroke_conversion->right_point;

    /*gcmERROR_RETURN(_CheckStrokeSubPath(stroke_conversion->lastStrokeSubPath));*/

ErrorHandler:
    return error;
}

static vg_lite_error_t _end_stroke_sub_path(
    vg_lite_stroke_t * stroke_conversion,
    vg_lite_float_t X,
    vg_lite_float_t Y,
    vg_lite_float_t Dx,
    vg_lite_float_t Dy
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;

    if(!stroke_conversion)
        return VG_LITE_INVALID_ARGUMENT;

    /* Add points for end of line. */
    VG_LITE_RETURN_ERROR(_add_point_to_right_stroke_point_list_tail(stroke_conversion, X + Dx, Y + Dy));
    VG_LITE_RETURN_ERROR(_add_point_to_left_point_list_head(stroke_conversion, X - Dx, Y - Dy));

    /* Add end cap if the subPath is not closed. */
    switch(stroke_conversion->cap_style) {
        case VG_LITE_CAP_BUTT:
            /* No adjustment needed. */
            break;
        case VG_LITE_CAP_ROUND:
            /* Add curve. */
            stroke_conversion->left_point->curve_type = CURVE_ARC_SCCW_HALF;
            stroke_conversion->left_point->tangentX = X;
            stroke_conversion->left_point->tangentY = Y;
            break;
        case VG_LITE_CAP_SQUARE:
            stroke_conversion->right_point->x -= Dy;
            stroke_conversion->right_point->y += Dx;
            stroke_conversion->left_point->x -= Dy;
            stroke_conversion->left_point->y += Dx;
            break;
    }

    /* Concatnate right and left point lists. */
    stroke_conversion->right_point->next = stroke_conversion->left_point;
    stroke_conversion->left_point->prev = stroke_conversion->right_point;

    /*gcmERROR_RETURN(_CheckStrokeSubPath(stroke_conversion->lastStrokeSubPath));*/
    return error;
}

static vg_lite_error_t _get_next_dash_length(
    vg_lite_stroke_t * stroke_conversion,
    uint32_t * dash_index,
    vg_lite_float_t * dash_length
)
{
    if(!stroke_conversion || !dash_index || !dash_length)
        return VG_LITE_INVALID_ARGUMENT;

    (*dash_index)++;
    if(*dash_index == stroke_conversion->pattern_count) {
        *dash_index = 0;
    }
    *dash_length = stroke_conversion->dash_pattern[*dash_index];

    return VG_LITE_SUCCESS;
}

static vg_lite_error_t
_create_stroke_path(
    vg_lite_stroke_t * stroke_conversion
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_sub_path_ptr stroke_sub_path = NULL, first_stroke_sub_path = NULL;
    vg_lite_path_point_ptr point, next_point;
    vg_lite_float_t half_width;
    vg_lite_float_t x, y;
    vg_lite_float_t dx, dy, ux, uy;
    vg_lite_float_t length, prev_length, first_length;
    vg_lite_float_t dash_length;
    uint32_t dash_index;
    uint8_t dashing;
    uint8_t add_end_cap;
    uint8_t need_to_handle_swing = 1 /* (stroke_conversion->strokeCapStyle == gcvCAP_BUTT) */;
    vg_lite_uint8_t dash_phase_reset;

    vg_lite_path_point_ptr first_right_point = NULL;
    vg_lite_path_point_ptr last_left_point = NULL;
    vg_lite_float_t first_dx = 0.0f, first_dy = 0.0f;
    uint8_t drawing = 0;
    vg_lite_float_t total_length = 0.0f;
    vg_lite_float_t accu_length = 0.0f;
    uint32_t swing_handling = SWING_NO;

    if(!stroke_conversion)
        return VG_LITE_INVALID_ARGUMENT;

    half_width = stroke_conversion->half_width;
    dashing = stroke_conversion->pattern_count > 0 ? 1 : 0;
    dash_index = stroke_conversion->dash_index;
    dash_length = stroke_conversion->dash_length;
    dash_phase_reset = stroke_conversion->dash_reset;

    /* VIV: [todo] Need to check/debug closed stroke path. */
    need_to_handle_swing = (stroke_conversion->cap_style == VG_LITE_CAP_BUTT || stroke_conversion->closed);
    if(need_to_handle_swing) {
        uint8_t reallyneed_to_handle_swing = 0;

        /* Calculate the total length. */
        for(point = stroke_conversion->path_points; point; point = point->next) {
            total_length += point->length;

            if(point->flatten_flag != vgcFLATTEN_NO) {
                reallyneed_to_handle_swing = 1;
            }
        }
        stroke_conversion->stroke_length = total_length;
        if(reallyneed_to_handle_swing) {
            swing_handling = SWING_OUT;
        }
        else {
            need_to_handle_swing = 0;
            swing_handling = SWING_NO;
        }
    }
    stroke_conversion->need_swing = need_to_handle_swing;

    point = stroke_conversion->path_points;
    next_point = point->next;
    if(next_point == NULL) {
        if(!dashing || ((dash_index & 0x1) == 0)) {
            /* Single point (zero-length) subpath. */
            /* Note that one-MOVE_TO subpaths are removed during parsing. */
            VG_LITE_ERROR_HANDLER(_add_zero_length_stroke_sub_path(stroke_conversion, &stroke_sub_path));
        }
        goto ErrorHandler;
    }

    /* Adjust closed status for dashing. */
    if(dashing && stroke_conversion->closed && ((dash_index & 0x1) == 1)) {
        stroke_conversion->closed = VGL_FALSE;
    }

    /* Set add_end_cap. */
    add_end_cap = dashing ? 1 : (stroke_conversion->closed ? 0 : 1);

    /* Process first line. */
    first_length = point->length;
    ux = point->tangentX;
    uy = point->tangentY;
    dx =  uy * half_width;
    dy = -ux * half_width;
    if(need_to_handle_swing) {
        stroke_conversion->swing_length = first_length;
    }

    if(dashing) {
        vg_lite_float_t delta_length;

        /* Draw dashes. */
        x = point->x;
        y = point->y;
        do {
            if((dash_index & 0x1) == 0) {
                VG_LITE_ERROR_HANDLER(_start_new_stroke_sub_path(
                                          stroke_conversion,
                                          x, y,
                                          dx, dy, add_end_cap,
                                          &stroke_sub_path
                                      ));

                drawing = 1;
                add_end_cap = 1;
                if(stroke_conversion->closed && (first_stroke_sub_path == NULL)) {
                    first_stroke_sub_path = stroke_conversion->last_stroke;
                    first_right_point = stroke_conversion->right_point;
                    last_left_point = stroke_conversion->left_point;
                    first_dx = dx;
                    first_dy = dy;
                }
            }

            delta_length = first_length - dash_length;
            if(delta_length >= FLOAT_EPSILON) {
                /* Move (x, y) forward along the line by dash_length. */
                x += ux * dash_length;
                y += uy * dash_length;

                if((dash_index & 0x1) == 0) {
                    VG_LITE_ERROR_HANDLER(_end_stroke_sub_path(
                                              stroke_conversion,
                                              x, y,
                                              dx, dy
                                          ));

                    drawing = 0;
                }

                VG_LITE_ERROR_HANDLER(_get_next_dash_length(stroke_conversion, &dash_index, &dash_length));
                first_length = delta_length;
            }
            else if(delta_length <= -FLOAT_EPSILON) {
                dash_length = -delta_length;
                break;
            }
            else {
                if((dash_index & 0x1) == 0) {
                    VG_LITE_ERROR_HANDLER(_end_stroke_sub_path(
                                              stroke_conversion,
                                              next_point->x, next_point->y,
                                              dx, dy
                                          ));

                    drawing = 0;
                }

                VG_LITE_ERROR_HANDLER(_get_next_dash_length(stroke_conversion, &dash_index, &dash_length));
                first_length = 0;
                break;
            }
        } while(1);
    }
    else {
        VG_LITE_ERROR_HANDLER(_start_new_stroke_sub_path(
                                  stroke_conversion,
                                  point->x, point->y,
                                  dx, dy, add_end_cap,
                                  &stroke_sub_path
                              ));

        drawing = 1;
        add_end_cap = 1;
    }

    /* Process the rest of lines. */
    prev_length = first_length;
    for(point = next_point, next_point = point->next; next_point;
        point = next_point, next_point = point->next) {
        if(!dashing || ((dash_index & 0x1) == 0 && drawing)) {
            /* Add points for end of line for line join process with next line. */
            VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                             point->x + dx, point->y + dy));
            VG_LITE_ERROR_HANDLER(_add_point_to_left_point_list_head(stroke_conversion,
                                                                     point->x - dx, point->y - dy));
        }

        length = point->length;
        ux = point->tangentX;
        uy = point->tangentY;
        dx =  uy * half_width;
        dy = -ux * half_width;
        if(need_to_handle_swing) {
            accu_length += point->prev->length;
            stroke_conversion->swing_length = accu_length;
            if(accu_length < half_width) {
                swing_handling = SWING_OUT;
            }
            else if(total_length - accu_length < half_width) {
                swing_handling = SWING_IN;
            }
            else {
                swing_handling = SWING_NO;
            }
        }

        if(!dashing) {
            /* Handle line joint style. */
            VG_LITE_ERROR_HANDLER(_process_line_joint(
                                      stroke_conversion, point,
                                      length, prev_length, swing_handling,
                                      point->x + dx, point->y + dy,
                                      point->x - dx, point->y - dy
                                  ));
        }
        else {
            vg_lite_float_t delta_length;

            /* Draw dashes. */
            x = point->x;
            y = point->y;
            if((dash_index & 0x1) == 0) {
                if(drawing) {
                    /* Handle line joint style. */
                    VG_LITE_ERROR_HANDLER(_process_line_joint(
                                              stroke_conversion, point,
                                              dash_length, prev_length, swing_handling,
                                              x + dx, y + dy,
                                              x - dx, y - dy
                                          ));
                }
                else {
                    /* Start a new sub path. */
                    VG_LITE_ERROR_HANDLER(_start_new_stroke_sub_path(
                                              stroke_conversion,
                                              x, y,
                                              dx, dy, add_end_cap,
                                              &stroke_sub_path
                                          ));

                    drawing = 1;
                    add_end_cap = 1;
                }
            }
            do {
                delta_length = length - dash_length;
                if(delta_length >= FLOAT_EPSILON) {
                    /* Move (x, y) forward along the line by dash_length. */
                    x += ux * dash_length;
                    y += uy * dash_length;

                    if((dash_index & 0x1) == 0) {
                        VG_LITE_ERROR_HANDLER(_end_stroke_sub_path(
                                                  stroke_conversion,
                                                  x, y, dx, dy
                                              ));

                        drawing = 0;
                    }

                    VG_LITE_ERROR_HANDLER(_get_next_dash_length(stroke_conversion, &dash_index, &dash_length));
                    length = delta_length;
                }
                else if(delta_length <= -FLOAT_EPSILON) {
                    dash_length = -delta_length;
                    break;
                }
                else {
                    if((dash_index & 0x1) == 0) {
                        VG_LITE_ERROR_HANDLER(_end_stroke_sub_path(
                                                  stroke_conversion,
                                                  next_point->x, next_point->y,
                                                  dx, dy
                                              ));

                        drawing = 0;
                    }

                    VG_LITE_ERROR_HANDLER(_get_next_dash_length(stroke_conversion, &dash_index, &dash_length));
                    length = 0;
                    break;
                }

                if((dash_index & 0x1) == 0) {
                    VG_LITE_ERROR_HANDLER(_start_new_stroke_sub_path(
                                              stroke_conversion,
                                              x, y,
                                              dx, dy, add_end_cap,
                                              &stroke_sub_path
                                          ));

                    drawing = 1;
                    add_end_cap = 1;
                }
            } while(1);
        }

        prev_length = length;
    }

    if(need_to_handle_swing) {
        accu_length += point->prev->length;
        stroke_conversion->swing_length = accu_length;
        if(accu_length < half_width) {
            swing_handling = SWING_OUT;
        }
        else if(total_length - accu_length < half_width) {
            swing_handling = SWING_IN;
        }
        else {
            swing_handling = SWING_NO;
        }
    }

    if(stroke_conversion->swing_handling != SWING_NO) {
        /* Draw the swing area (pie area). */
        VG_LITE_ERROR_HANDLER(_draw_swing_pie_area(stroke_conversion, stroke_conversion->path_end, VGL_FALSE));
    }

    if(stroke_conversion->closed) {
        if(! dashing || drawing) {
            /* Add points for end of line. */
            VG_LITE_ERROR_HANDLER(_add_point_to_right_stroke_point_list_tail(stroke_conversion,
                                                                             point->x + dx, point->y + dy));
            VG_LITE_ERROR_HANDLER(_add_point_to_left_point_list_head(stroke_conversion,
                                                                     point->x - dx, point->y - dy));

            if(! dashing) {
                if(stroke_sub_path) {
                    /* Handle line joint style for the first/last point in closed path. */
                    VG_LITE_ERROR_HANDLER(_close_stroke_sub_path(
                                              stroke_conversion, point,
                                              first_length, prev_length, swing_handling,
                                              stroke_sub_path->point_list, stroke_sub_path->end_point
                                          ));
                }
            }
            else {
                /* Handle line joint style for the first/last point in closed path. */
                if(first_right_point && last_left_point) {
                    VG_LITE_ERROR_HANDLER(_close_stroke_sub_path(
                                              stroke_conversion, point,
                                              first_length, prev_length, swing_handling,
                                              first_right_point, last_left_point
                                          ));
                }
                else {
                    error = VG_LITE_INVALID_ARGUMENT;
                    goto ErrorHandler;
                }
            }
        }
        else if(stroke_conversion->cap_style != VG_LITE_CAP_BUTT) {
            /* No closing join need.  Add end cap for the starting point. */

            if(stroke_conversion->cap_style == VG_LITE_CAP_SQUARE) {
                if(first_right_point && last_left_point) {
                    first_right_point->x += first_dy;
                    first_right_point->y -= first_dx;
                    last_left_point->x += first_dy;
                    last_left_point->y -= first_dx;
                }
                else {
                    error = VG_LITE_INVALID_ARGUMENT;
                    goto ErrorHandler;
                }
            }
            else {
                vg_lite_sub_path_ptr last_stroke = stroke_conversion->last_stroke;
                vg_lite_path_point_ptr start_point = last_stroke->point_list;
                vg_lite_path_point_ptr extra_point;

                /* Add curve. */
                /* Add extra point to the beginning with end point's coordinates. */
                extra_point = (vg_lite_path_point_ptr)vg_lite_os_malloc(sizeof(*extra_point));
                if(!extra_point)
                    return VG_LITE_OUT_OF_RESOURCES;
                memset(extra_point, 0, sizeof(*extra_point));

                extra_point->x = last_stroke->end_point->x;
                extra_point->y = last_stroke->end_point->y;
                extra_point->next = start_point;
                start_point->prev = extra_point;
                start_point->curve_type = CURVE_ARC_SCCW;
                start_point->tangentX = stroke_conversion->path_points->x;
                start_point->tangentY = stroke_conversion->path_points->y;
                last_stroke->point_list = extra_point;
            }
        }
    }
    else if(! dashing ||
            (((dash_index & 0x1) == 0) && (dash_length < stroke_conversion->dash_pattern[dash_index]))) {
        /* Add end cap if the subPath is not closed. */
        VG_LITE_ERROR_HANDLER(_end_stroke_sub_path(
                                  stroke_conversion,
                                  point->x, point->y,
                                  dx, dy
                              ));

        drawing = 0;
    }

    if(!dash_phase_reset) {
        /* Update dash index and length for next subpath. */
        if(dashing) {
            if(((dash_index & 0x1) == 1) &&
               (stroke_conversion->dash_pattern[dash_index] - dash_length < FLOAT_EPSILON)) {
                stroke_conversion->dash_index = dash_index - 1;
                stroke_conversion->dash_length = 0;
            }
            else {
                stroke_conversion->dash_index = dash_index;
                stroke_conversion->dash_length = dash_length;
            }
        }
    }

ErrorHandler:
    return error;
}

static vg_lite_error_t _copy_stroke_path(
    vg_lite_stroke_t * stroke_conversion,
    vg_lite_path_t * path
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_path_point_ptr point, prev_point, tmp_point;
    vg_lite_float_t totalsize = 0, real_size = 0;
    float * pfloat;
    char * cpath;
#if (CHIPID==0x355 || CHIPID==0x255)
    char last_opcode = 0;
#endif
    void * temp_stroke_data = NULL;
    uint32_t temp_stroke_size;
    vg_lite_sub_path_ptr sub_path;
    vg_lite_float_t half_width;

#if (CHIPID==0x355)
    vg_lite_buffer_t buffer = { 0 };
    uint32_t bytes;
#endif

    if(!stroke_conversion || !path)
        return VG_LITE_INVALID_ARGUMENT;

    half_width = stroke_conversion->half_width;
    sub_path = stroke_conversion->stroke_paths;

    if(!stroke_conversion || !path || !sub_path)
        return VG_LITE_SUCCESS;

    while(sub_path) {
        tmp_point = prev_point = point = sub_path->point_list;
        totalsize += _commandSize_float[VLC_OP_LINE] * sub_path->point_count + _commandSize_float[VLC_OP_CLOSE];
        for(; tmp_point; tmp_point = tmp_point->next) {
            if(tmp_point->curve_type == CURVE_ARC_SCCW || tmp_point->curve_type == CURVE_ARC_SCCW_HALF) {
                totalsize += 4 * _commandSize_float[VLC_OP_QUAD];
            }
        }

        temp_stroke_data = path->stroke_path;
        temp_stroke_size = path->stroke_size;

        path->stroke_size += (int32_t)totalsize;

#if (CHIPID==0x355)
        if(sub_path->next == NULL) {
            bytes = (8 + path->stroke_size + 7 + 8) & ~7;
            buffer.width = bytes;
            buffer.height = 1;
            buffer.stride = 0;
            buffer.format = VG_LITE_A8;
            VG_LITE_RETURN_ERROR(vg_lite_allocate(&buffer));

            memset(buffer.memory, 0, buffer.stride);
            ((uint32_t *)buffer.memory)[0] = VG_LITE_DATA((path->stroke_size + 7) / 8);
            ((uint32_t *)buffer.memory)[1] = 0;
            if(temp_stroke_data) {
                memcpy((char *)buffer.memory + 8, temp_stroke_data, temp_stroke_size);
                vg_lite_os_free(temp_stroke_data);
                temp_stroke_data = NULL;
            }

            path->stroke_path = 0;
            pfloat = (vg_lite_float_t *)((char *)buffer.memory + 8 + temp_stroke_size);
        }
        else
#endif
        {
            path->stroke_path = (void *)vg_lite_os_malloc(path->stroke_size);
            if(!path->stroke_path) {
                error = VG_LITE_OUT_OF_RESOURCES;
                goto ErrorHandler;
            }

            memset(path->stroke_path, 0, path->stroke_size);

            if(temp_stroke_data) {
                memcpy(path->stroke_path, temp_stroke_data, temp_stroke_size);
                vg_lite_os_free(temp_stroke_data);
                temp_stroke_data = NULL;
            }

            pfloat = (vg_lite_float_t *)((char *)path->stroke_path + temp_stroke_size);
        }
#if (CHIPID==0x355 || CHIPID==0x255)
        if(last_opcode == VLC_OP_CLOSE) {
            cpath = (char *)(pfloat - 1) + 1;
            *cpath++ = VLC_OP_MOVE;
            cpath = (char *)pfloat;
        }
        else
#endif
        {
            cpath = (char *)pfloat;
            *cpath = VLC_OP_MOVE;
            pfloat++;
        }

        *pfloat++ = point->x;
        *pfloat++ = point->y;
        real_size += _commandSize_float[VLC_OP_MOVE];
#if (CHIPID==0x355 || CHIPID==0x255)
        if(last_opcode == VLC_OP_CLOSE)
            real_size -= 4;
#endif

        for(point = point->next; point; prev_point = point, point = point->next) {
            if(point->curve_type == CURVE_LINE) {
                if(point->x == prev_point->x && point->y == prev_point->y) {
                    path->stroke_size -= _commandSize_float[VLC_OP_LINE];
                    /* Skip zero-length lines. */
                    continue;
                }

                /* Add new command. */
                cpath = (char *)pfloat;
                *cpath = VLC_OP_LINE;
                pfloat++;

                /* Set the coordinates. */
                *pfloat++ = point->x;
                *pfloat++ = point->y;
                real_size += _commandSize_float[VLC_OP_LINE];
            }
            else if(point->curve_type == CURVE_QUAD_CONTROL) {
                /* Add new command. */
                cpath = (char *)pfloat;
                *cpath = VLC_OP_QUAD;
                pfloat++;

                /* Set the coordinates. */
                prev_point = point, point = point->next;
                *pfloat++ = prev_point->x;
                *pfloat++ = prev_point->y;
                *pfloat++ = point->x;
                *pfloat++ = point->y;

                real_size += _commandSize_float[VLC_OP_QUAD];
            }
            else {
                vg_lite_path_point_ptr point_list, p, nextP;
                vg_lite_path_point_ptr p2;

                if(point->curve_type == CURVE_ARC_SCCW) {
                    /* Convert an arc to Bezier curves. */
                    VG_LITE_ERROR_HANDLER(_convert_circle_arc(stroke_conversion, half_width,
                                                              point->tangentX, point->tangentY,
                                                              prev_point->x, prev_point->y,
                                                              point->x, point->y,
                                                              0, &point_list));
                }
                else {
                    /* Convert a half circle to Bezier curves. */
                    VG_LITE_ERROR_HANDLER(_convert_circle_arc(stroke_conversion, half_width,
                                                              point->tangentX, point->tangentY,
                                                              prev_point->x, prev_point->y,
                                                              point->x, point->y,
                                                              1, &point_list));

                }

                if(point_list) {
                    for(p = point_list; p; p = nextP) {
                        /* Add new command. */
                        cpath = (char *)pfloat;
                        *cpath = VLC_OP_QUAD;
                        pfloat++;

                        /* Set the coordinates. */
                        p2 = p->next;
                        nextP = p2->next;

                        *pfloat++ = p->x;
                        *pfloat++ = p->y;
                        *pfloat++ = p2->x;
                        *pfloat++ = p2->y;
                        real_size += _commandSize_float[VLC_OP_QUAD];
                        vg_lite_os_free(p);
                        vg_lite_os_free(p2);
                    }
                }
                else {
                    /* Handle special case of huge scaling. */
                    /* Add new command. */
                    cpath = (char *)pfloat;
                    *cpath = VLC_OP_LINE;
                    pfloat++;

                    /* Set the coordinates. */
                    *pfloat++ = point->x;
                    *pfloat++ = point->y;
                    real_size += _commandSize_float[VLC_OP_LINE];
                }
            }
        }

        /* Create a CLOSE_PATH command at the end. */
        cpath = (char *)pfloat;
        if(sub_path->next)
            *cpath = VLC_OP_CLOSE;
        else
            *cpath = VLC_OP_END;
        real_size += _commandSize_float[VLC_OP_CLOSE];
        path->stroke_size = temp_stroke_size + (int32_t)real_size;
        totalsize = 0;
        real_size = 0;
        sub_path = sub_path->next;
#if (CHIPID==0x355 || CHIPID==0x255)
        last_opcode = *cpath;
#endif
    }

#if (CHIPID==0x355)
    /* Initialize command buffer postfix. */
    ((uint32_t *)buffer.memory)[(bytes >> 2) - 2] = VG_LITE_RETURN();
    ((uint32_t *)buffer.memory)[(bytes >> 2) - 1] = 0;

    /* Mark stroke as uploaded. */
    path->stroke->uploaded.handle = buffer.handle;
    path->stroke->uploaded.address = buffer.address;
    path->stroke->uploaded.memory = buffer.memory;
    path->stroke->uploaded.bytes = bytes;
    VLM_PATH_STROKE_ENABLE_UPLOAD(*path);
#endif

ErrorHandler:
    if(temp_stroke_data) {
        vg_lite_os_free(temp_stroke_data);
        temp_stroke_data = NULL;
    }
    return error;
}

static vg_lite_error_t _initialize_stroke_dash_parameters(
    vg_lite_stroke_t * stroke_conversion
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t count;
    uint32_t i;
    vg_lite_float_t * pattern_src;
    vg_lite_float_t * pattern, * temp_pattern;
    vg_lite_float_t length;

    if(!stroke_conversion)
        return VG_LITE_INVALID_ARGUMENT;

    count = stroke_conversion->pattern_count;
    if(count == 0 || !stroke_conversion->dash_pattern)
        return error;

    length = stroke_conversion->dash_phase;

    /* The last pattern is ignored if the number is odd. */
    if(count & 0x1) count--;

    pattern = (vg_lite_float_t *)vg_lite_os_malloc(count * sizeof(vg_lite_float_t));
    if(!pattern)
        return VG_LITE_OUT_OF_RESOURCES;

    temp_pattern = pattern;
    stroke_conversion->pattern_length = 0.0f;
    pattern_src = stroke_conversion->dash_pattern;

    for(i = 0; i < count; i++, pattern++, pattern_src++) {
        if(*pattern_src < 0.0f) {
            *pattern = 0.0f;
        }
        else {
            *pattern = *pattern_src;
        }
        stroke_conversion->pattern_length += *pattern;
    }

    if(stroke_conversion->pattern_length < FLOAT_EPSILON) {
        stroke_conversion->pattern_count = 0;
        vg_lite_os_free(temp_pattern);
        temp_pattern = NULL;
        return error;
    }

    while(length < 0.0f) {
        length += stroke_conversion->pattern_length;
    }

    while(length >= stroke_conversion->pattern_length) {
        length -= stroke_conversion->pattern_length;
    }

    pattern = stroke_conversion->dash_pattern;
    for(i = 0; i < stroke_conversion->pattern_count; i++, pattern++) {
        if(length <= *pattern) break;

        length -= *pattern;
    }

    stroke_conversion->dash_index = i;
    stroke_conversion->dash_length = *pattern - length;

    vg_lite_os_free(temp_pattern);
    temp_pattern = NULL;

    return error;
}

vg_lite_error_t vg_lite_update_stroke(
    vg_lite_path_t * path
)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    vg_lite_stroke_t * stroke_conversion;
    vg_lite_path_list_ptr cur_list;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_update_stroke %p\n", path);
#endif

    if(!path)
        return VG_LITE_INVALID_ARGUMENT;

    if(!path->path_length)
        return VG_LITE_SUCCESS;

    if(!path->path)
        return VG_LITE_INVALID_ARGUMENT;

    if(!path->stroke)
        return VG_LITE_INVALID_ARGUMENT;

    stroke_conversion = path->stroke;
    cur_list = stroke_conversion->cur_list;

#if (CHIPID==0x355)
    if(path->stroke && path->stroke->uploaded.handle != NULL) {
        vg_lite_kernel_free_t free_cmd;
        free_cmd.memory_handle = path->stroke->uploaded.handle;
        error = vg_lite_kernel(VG_LITE_FREE, &free_cmd);
        if(error != VG_LITE_SUCCESS)
            return error;

        path->stroke->uploaded.address = 0;
        path->stroke->uploaded.bytes = 0;
        path->stroke->uploaded.handle = NULL;
        path->stroke->uploaded.memory = NULL;
    }
#endif

    /* Free the existing stroke path. */
    if(path->stroke_path) {
        vg_lite_os_free(path->stroke_path);
        /* Reset the stroke. */
        path->stroke_path = NULL;
    }

    if(stroke_conversion->line_width >= FLOAT_FAT_LINE_WIDTH
       &&  stroke_conversion->line_width >= 1.0f) {
        stroke_conversion->fattened = 1;
    }

    stroke_conversion->add_end = path->add_end;

    VG_LITE_RETURN_ERROR(_initialize_stroke_dash_parameters(stroke_conversion));
    VG_LITE_RETURN_ERROR(_flatten_path(stroke_conversion, path));

    for(cur_list = stroke_conversion->path_list_divide; cur_list; cur_list = cur_list->next) {
        stroke_conversion->path_end = cur_list->path_end;
        stroke_conversion->path_points = cur_list->path_points;
        stroke_conversion->point_count = cur_list->point_count;
        stroke_conversion->closed = cur_list->closed;

        VG_LITE_RETURN_ERROR(_create_stroke_path(stroke_conversion));
    }

    if(stroke_conversion->path_list_divide) {
        stroke_conversion->path_end = stroke_conversion->path_list_divide->path_end;
        stroke_conversion->path_points = stroke_conversion->path_list_divide->path_points;
        stroke_conversion->point_count = stroke_conversion->path_list_divide->point_count;
    }

    VG_LITE_RETURN_ERROR(_copy_stroke_path(stroke_conversion, path));

    /* add VLC_OP_END if stroke_path is empty. */
    if(path->stroke_size == 0) {
        path->stroke_path = vg_lite_os_malloc(_commandSize_float[VLC_OP_END]);
        if(!path->stroke_path)
            return VG_LITE_OUT_OF_RESOURCES;
        *(uint8_t *)path->stroke_path = VLC_OP_END;
        path->stroke_size = _commandSize_float[VLC_OP_END];
    }

    return error;
}

vg_lite_error_t vg_lite_set_stroke(
    vg_lite_path_t * path,
    vg_lite_cap_style_t cap_style,
    vg_lite_join_style_t join_style,
    vg_lite_float_t line_width,
    vg_lite_float_t miter_limit,
    vg_lite_float_t * dash_pattern,
    vg_lite_uint32_t pattern_count,
    vg_lite_float_t dash_phase,
    vg_lite_color_t stroke_color
)
{
#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_set_stroke %p %d %d %f %f %p %d %f 0x%08X\n", path, cap_style, join_style, line_width, miter_limit,
               dash_pattern, pattern_count, dash_phase, stroke_color);
#endif

    if(!path || line_width <= 0)
        return VG_LITE_INVALID_ARGUMENT;

    if(miter_limit < 1.0f)
        miter_limit = 1.0f;

    if(!path->stroke) {
        path->stroke = (vg_lite_stroke_t *)vg_lite_os_malloc(sizeof(vg_lite_stroke_t));
        if(!path->stroke)
            return VG_LITE_OUT_OF_RESOURCES;
        memset(path->stroke, 0, sizeof(vg_lite_stroke_t));
    }
    else {
        if(path->stroke) {
            if(path->stroke->path_list_divide) {
                vg_lite_path_list_ptr cur_list;
                while(path->stroke->path_list_divide) {
                    cur_list = path->stroke->path_list_divide->next;
                    if(path->stroke->path_list_divide->path_points) {
                        vg_lite_path_point_ptr temp_point;
                        while(path->stroke->path_list_divide->path_points) {
                            temp_point = path->stroke->path_list_divide->path_points->next;
                            vg_lite_os_free(path->stroke->path_list_divide->path_points);
                            path->stroke->path_list_divide->path_points = temp_point;
                        }
                        temp_point = NULL;
                    }
                    vg_lite_os_free(path->stroke->path_list_divide);
                    path->stroke->path_list_divide = cur_list;
                }
                cur_list = NULL;
            }

            if(path->stroke->stroke_paths) {
                vg_lite_sub_path_ptr temp_sub_path;
                while(path->stroke->stroke_paths) {
                    temp_sub_path = path->stroke->stroke_paths->next;
                    if(path->stroke->stroke_paths->point_list) {
                        vg_lite_path_point_ptr temp_point;
                        while(path->stroke->stroke_paths->point_list) {
                            temp_point = path->stroke->stroke_paths->point_list->next;
                            vg_lite_os_free(path->stroke->stroke_paths->point_list);
                            path->stroke->stroke_paths->point_list = temp_point;
                        }
                        temp_point = NULL;
                    }
                    vg_lite_os_free(path->stroke->stroke_paths);
                    path->stroke->stroke_paths = temp_sub_path;
                }
                temp_sub_path = NULL;
            }

            if(path->stroke->dash_pattern)
                vg_lite_os_free(path->stroke->dash_pattern);

            path->stroke_valid = 0;
        }

        memset(path->stroke, 0, sizeof(vg_lite_stroke_t));
        path->stroke_size = 0;
    }

    /* Clamp dash pattern and phase. */
    pattern_count &= 0xFFFFFFFE;
    float * dash_pattern_copy = NULL;
    if(pattern_count > 0) {
        dash_pattern_copy = vg_lite_os_malloc(pattern_count * sizeof(float));
        if(!dash_pattern_copy)
            return VG_LITE_OUT_OF_RESOURCES;
    }
    for(uint32_t i = 0; i < pattern_count; ++i)
        dash_pattern_copy[i] = (dash_pattern[i] > 0.f) ? dash_pattern[i] : 0.f;
    if(dash_phase < 0.f) {
        float dash_total_length = 0.f;
        for(uint32_t i = 0; i < pattern_count; ++i)
            dash_total_length += dash_pattern_copy[i];
        if(dash_total_length > 0.f)
            dash_phase += (int)(-dash_phase / dash_total_length + 1) * dash_total_length;
        else
            dash_phase = 0.f;
    }

    path->stroke->cap_style = cap_style;
    path->stroke->join_style = join_style;
    path->stroke->line_width = line_width;
    path->stroke->miter_limit = miter_limit;
    path->stroke->half_width = line_width / 2.0f;
    path->stroke->miter_square = path->stroke->miter_limit * path->stroke->miter_limit;
    path->stroke->dash_pattern = dash_pattern_copy;
    path->stroke->pattern_count = pattern_count;
    path->stroke->dash_phase = dash_phase;
    path->stroke_color = stroke_color;

    return VG_LITE_SUCCESS;
}

#else  /* gcFEATURE_VG_STROKE_PATH */

vg_lite_error_t vg_lite_update_stroke(
    vg_lite_path_t * path
)
{
    return VG_LITE_NOT_SUPPORT;
}

vg_lite_error_t vg_lite_set_stroke(
    vg_lite_path_t * path,
    vg_lite_cap_style_t cap_style,
    vg_lite_join_style_t join_style,
    vg_lite_float_t line_width,
    vg_lite_float_t miter_limit,
    vg_lite_float_t * dash_pattern,
    vg_lite_uint32_t pattern_count,
    vg_lite_float_t dash_phase,
    vg_lite_color_t stroke_color
)
{
    return VG_LITE_NOT_SUPPORT;
}

#endif /* gcFEATURE_VG_STROKE_PATH */

#if gcFEATURE_VG_ARC_PATH

static vg_lite_float_t _angle(
    vg_lite_float_t Ux,
    vg_lite_float_t Uy,
    vg_lite_float_t Vx,
    vg_lite_float_t Vy
)
{

    vg_lite_float_t dot, length, angle, cosVal;
    int32_t sign;

    dot    = Ux * Vx + Uy * Vy;
    length = SQRTF(Ux * Ux + Uy * Uy) * SQRTF(Vx * Vx + Vy * Vy);
    sign   = (Ux * Vy - Uy * Vx < 0) ? -1 : 1;
    cosVal = dot / length;
    cosVal = CLAMP(cosVal, -1.0f, 1.0f);
    angle  = sign * ACOSF(cosVal);
    return angle;
}

void compute_quadpathbounds(vg_lite_path_t * path, float currentX, float currentY, float controlX, float controlY,
                            float lastX, float lastY)
{
    int j = 0;
    float newPointX = 0;
    float newPointY = 0;

    for(j = 1; j <= 256; j++) {
        float t = (float)j / 256;
        float u = 1.0f - t;
        float term1 = u * u;
        float term2 = t * u * 2.0f;
        float term3 = t * t;
        newPointX = currentX * term1 + controlX * term2 + lastX * term3;
        newPointY = currentY * term1 + controlY * term2 + lastY * term3;

        path->bounding_box[0] = MIN(path->bounding_box[0], newPointX);
        path->bounding_box[1] = MIN(path->bounding_box[1], newPointY);
        path->bounding_box[2] = MAX(path->bounding_box[2], newPointX);
        path->bounding_box[3] = MAX(path->bounding_box[3], newPointY);
    }
}

vg_lite_error_t _convert_hline(
    vg_lite_float_t EndX,
    vg_lite_float_t EndY,
    uint8_t Relative,
    vg_lite_control_coord_t * coords,
    void ** path_data,
    uint32_t * offset,
    uint32_t last_size
)
{
    vg_lite_float_t endX, endY;
    uint8_t segmentCommand;
    int32_t segs;
    uint32_t bufferSize;
    char * pchar, * linePath;
    vg_lite_float_t * pfloat;

    /*******************************************************************
    ** Converting.
    */
    if(path_data == NULL || *path_data == NULL || offset == NULL || coords == NULL)
        return VG_LITE_INVALID_ARGUMENT;
    segs = 1;
    if(Relative) {
        endX = EndX + coords->lastX;
        endY = EndY + coords->lastY;
    }
    else {
        endX = EndX;
        endY = EndY;
    }

    /* Determine the segment command. */
    segmentCommand = Relative
                     ? VLC_OP_LINE_REL
                     : VLC_OP_LINE;

    /* Determine the size of the buffer required. */
    bufferSize = (1 + 2) * SIZEOF(vg_lite_float_t) * segs;

    linePath = (char *)vg_lite_os_malloc(*offset + bufferSize + last_size);
    if(linePath == NULL)
        return VG_LITE_OUT_OF_RESOURCES;
#if(CHIPID == 0x355)
    memset(linePath, 0, *offset + bufferSize + last_size);
#endif
    memcpy(linePath, (char *)*path_data, *offset);
    vg_lite_os_free(*path_data);

    *path_data = linePath;
    pchar = linePath + *offset;
    pfloat = (vg_lite_float_t *)pchar;

    while(segs-- > 0) {
        /* Adjust relative coordinates. */
        pchar = (char *)pfloat;
        *pchar = segmentCommand;
        pfloat++;
        if(Relative) {
            *pfloat++ = EndX;
            *pfloat++ = 0;
        }
        else {
            *pfloat++ = EndX;
            *pfloat++ = EndY;
        }
        *offset += (1 + 2) * SIZEOF(vg_lite_float_t);
    }
    /* Update the control coordinates. */
    coords->lastX = endX;
    coords->lastY = endY;
    coords->controlX = endX;
    coords->controlY = endY;
    return VG_LITE_SUCCESS;
}

vg_lite_error_t _convert_vline(
    vg_lite_float_t EndX,
    vg_lite_float_t EndY,
    uint8_t Relative,
    vg_lite_control_coord_t * coords,
    void ** path_data,
    uint32_t * offset,
    uint32_t last_size
)
{
    vg_lite_float_t endX, endY;
    uint8_t segmentCommand;
    int32_t segs;
    uint32_t bufferSize;
    char * pchar, * linePath;
    vg_lite_float_t * pfloat;

    /*******************************************************************
    ** Converting.
    */
    if(path_data == NULL || *path_data == NULL || offset == NULL || coords == NULL)
        return VG_LITE_INVALID_ARGUMENT;
    segs = 1;
    if(Relative) {
        endX = EndX + coords->lastX;
        endY = EndY + coords->lastY;
    }
    else {
        endX = EndX;
        endY = EndY;
    }

    /* Determine the segment command. */
    segmentCommand = Relative
                     ? VLC_OP_LINE_REL
                     : VLC_OP_LINE;

    /* Determine the size of the buffer required. */
    bufferSize = (1 + 2) * SIZEOF(vg_lite_float_t) * segs;

    linePath = (char *)vg_lite_os_malloc(*offset + bufferSize + last_size);
    if(linePath == NULL)
        return VG_LITE_OUT_OF_RESOURCES;
#if(CHIPID == 0x355)
    memset(linePath, 0, *offset + bufferSize + last_size);
#endif
    memcpy(linePath, (char *)*path_data, *offset);
    vg_lite_os_free(*path_data);

    *path_data = linePath;
    pchar = linePath + *offset;
    pfloat = (vg_lite_float_t *)pchar;

    while(segs-- > 0) {
        /* Adjust relative coordinates. */
        pchar = (char *)pfloat;
        *pchar = segmentCommand;
        pfloat++;
        if(Relative) {
            *pfloat++ = 0;
            *pfloat++ = EndY;
        }
        else {
            *pfloat++ = EndX;
            *pfloat++ = EndY;
        }
        *offset += (1 + 2) * SIZEOF(vg_lite_float_t);
    }
    /* Update the control coordinates. */
    coords->lastX = endX;
    coords->lastY = endY;
    coords->controlX = endX;
    coords->controlY = endY;
    return VG_LITE_SUCCESS;
}

vg_lite_error_t _convert_scubic(
    vg_lite_float_t EndX,
    vg_lite_float_t EndY,
    vg_lite_float_t ControlX,
    vg_lite_float_t ControlY,
    uint8_t Relative,
    vg_lite_control_coord_t * coords,
    void ** path_data,
    uint32_t * offset,
    uint32_t last_size
)
{
    vg_lite_float_t endX, endY;
    uint8_t segmentCommand;
    int32_t segs;
    uint32_t bufferSize;
    char * pchar, * cubicPath;
    vg_lite_float_t * pfloat;
    vg_lite_float_t controlX;
    vg_lite_float_t controlY;
    /*******************************************************************
    ** Converting.
    */
    if(path_data == NULL || *path_data == NULL || offset == NULL || coords == NULL)
        return VG_LITE_INVALID_ARGUMENT;
    segs = 1;
    if(Relative) {
        endX = EndX + coords->lastX;
        endY = EndY + coords->lastY;
        controlX = coords->lastX + ControlX;
        controlY = coords->lastY + ControlY;
    }
    else {
        endX = EndX;
        endY = EndY;
        controlX = ControlX;
        controlY = ControlY;
    }

    /* Determine the segment command. */
    segmentCommand = Relative
                     ? VLC_OP_CUBIC_REL
                     : VLC_OP_CUBIC;

    /* Determine the size of the buffer required. */
    bufferSize = (1 + 6) * SIZEOF(vg_lite_float_t) * segs;

    cubicPath = (char *)vg_lite_os_malloc(*offset + bufferSize + last_size);
    if(cubicPath == NULL)
        return VG_LITE_OUT_OF_RESOURCES;
#if(CHIPID == 0x355)
    memset(cubicPath, 0, *offset + bufferSize + last_size);
#endif
    memcpy(cubicPath, (char *)*path_data, *offset);
    vg_lite_os_free(*path_data);

    *path_data = cubicPath;
    pchar = cubicPath + *offset;
    pfloat = (vg_lite_float_t *)pchar;

    while(segs-- > 0) {
        /* Adjust relative coordinates. */
        pchar = (char *)pfloat;
        *pchar = segmentCommand;
        pfloat++;
        if(Relative) {
            /* Calculate the first control point and convert to relative coordinates. */
            *pfloat++ = (2 * coords->lastX - coords->controlX) - coords->lastX;
            *pfloat++ = (2 * coords->lastY - coords->controlY) - coords->lastY;
            *pfloat++ = ControlX;
            *pfloat++ = ControlY;
            *pfloat++ = EndX;
            *pfloat++ = EndY;
            *offset += (1 + 6) * SIZEOF(vg_lite_float_t);
        }
        else {
            *pfloat++ = 2 * coords->lastX - coords->controlX;
            *pfloat++ = 2 * coords->lastY - coords->controlY;
            *pfloat++ = ControlX;
            *pfloat++ = ControlY;
            *pfloat++ = EndX;
            *pfloat++ = EndY;
            *offset += (1 + 6) * SIZEOF(vg_lite_float_t);
        }
    }
    /* Update the control coordinates. */
    coords->lastX = endX;
    coords->lastY = endY;
    coords->controlX = controlX;
    coords->controlY = controlY;

    return VG_LITE_SUCCESS;
}

vg_lite_error_t _convert_squad(
    vg_lite_float_t EndX,
    vg_lite_float_t EndY,
    uint8_t Relative,
    vg_lite_control_coord_t * coords,
    void ** path_data,
    uint32_t * offset,
    uint32_t last_size
)
{
    vg_lite_float_t endX, endY;
    uint8_t segmentCommand;
    int32_t segs;
    uint32_t bufferSize;
    char * pchar, * quadPath;
    vg_lite_float_t * pfloat;
    vg_lite_float_t ControlX1;
    vg_lite_float_t ControlY1;
    vg_lite_float_t controlX;
    vg_lite_float_t controlY;
    /*******************************************************************
    ** Converting.
    */
    if(path_data == NULL || *path_data == NULL || offset == NULL || coords == NULL)
        return VG_LITE_INVALID_ARGUMENT;
    segs = 1;
    if(Relative) {
        endX = EndX + coords->lastX;
        endY = EndY + coords->lastY;
    }
    else {
        endX = EndX;
        endY = EndY;
    }

    /* Determine the segment command. */
    segmentCommand = Relative
                     ? VLC_OP_QUAD_REL
                     : VLC_OP_QUAD;

    /* Determine the size of the buffer required. */
    bufferSize = (1 + 4) * SIZEOF(vg_lite_float_t) * segs;

    quadPath = (char *)vg_lite_os_malloc(*offset + bufferSize + last_size);
    if(quadPath == NULL)
        return VG_LITE_OUT_OF_RESOURCES;
#if(CHIPID == 0x355)
    memset(quadPath, 0, *offset + bufferSize + last_size);
#endif
    memcpy(quadPath, (char *)*path_data, *offset);
    vg_lite_os_free(*path_data);

    *path_data = quadPath;
    pchar = quadPath + *offset;
    pfloat = (vg_lite_float_t *)pchar;

    while(segs-- > 0) {
        /* Adjust relative coordinates. */
        pchar = (char *)pfloat;
        *pchar = segmentCommand;
        pfloat++;
        if(Relative) {
            ControlX1 = *pfloat++ = (2 * coords->lastX - coords->controlX) - coords->lastX;
            ControlY1 = *pfloat++ = (2 * coords->lastY - coords->controlY) - coords->lastY;
            controlX = ControlX1 + coords->lastX;
            controlY = ControlY1 + coords->lastY;
            *pfloat++ = EndX;
            *pfloat++ = EndY;
            *offset += (1 + 4) * SIZEOF(vg_lite_float_t);
        }
        else {
            controlX = *pfloat++ = 2 * coords->lastX - coords->controlX;
            controlY = *pfloat++ = 2 * coords->lastY - coords->controlY;
            *pfloat++ = EndX;
            *pfloat++ = EndY;
            *offset += (1 + 4) * SIZEOF(vg_lite_float_t);
        }
    }
    /* Update the control coordinates. */
    coords->startX = coords->lastX;
    coords->startY = coords->lastY;
    coords->lastX = endX;
    coords->lastY = endY;
    coords->controlX = controlX;
    coords->controlY = controlY;

    return VG_LITE_SUCCESS;
}

/*!
    @discussion
    Convert arc to multi-segment bezier curve.
    @param HorRadius
    Major axis radius.
    @param VerRadius
    minor axis radius.
    @param RotAngle
    Rotation angle.
    @param EndX
    End coordinate x.
    @param EndX
    End coordinate y.
    @param CounterClockwise
    If this is 0,anticlockwise rotation,if this is 1,clockwise rotation.
    @param Large
    1 means big arc,0 means little arc.
    @param Relative
    1 means absolute coordinates,0 means relative coordinates.
    @param coords
    Including the start point coordinates of the path,the control point of the last segment of the path,
    and the end point of the last segment of the path.
    @param path_data
    Path data usr for internal conversion.
    @param offset
    The offset of path_data.
    @param last_size
    The remain unconverted size of the original path data.
    @result
    Error code. VG_LITE_INVALID_ARGUMENTS to indicate the parameters are wrong.
*/
vg_lite_error_t _convert_arc(
    vg_lite_float_t HorRadius,
    vg_lite_float_t VerRadius,
    vg_lite_float_t RotAngle,
    vg_lite_float_t EndX,
    vg_lite_float_t EndY,
    uint8_t CounterClockwise,
    uint8_t Large,
    uint8_t Relative,
    vg_lite_control_coord_t * coords,
    void ** path_data,
    uint32_t * offset,
    uint32_t last_size
)
{
    vg_lite_float_t endX, endY;
    uint8_t segmentCommand;
    vg_lite_float_t phi, cosPhi, sinPhi;
    vg_lite_float_t dxHalf, dyHalf;
    vg_lite_float_t x1Prime, y1Prime;
    vg_lite_float_t rx, ry;
    vg_lite_float_t x1PrimeSquare, y1PrimeSquare;
    vg_lite_float_t lambda;
    vg_lite_float_t rxSquare, rySquare;
    int32_t sign;
    vg_lite_float_t sq, signedSq;
    vg_lite_float_t cxPrime, cyPrime;
    vg_lite_float_t theta1, thetaSpan;
    int32_t segs;
    vg_lite_float_t theta, ax, ay, x, y;
    vg_lite_float_t controlX, controlY, anchorX, anchorY;
    vg_lite_float_t lastX, lastY;
    uint32_t bufferSize;
    char  *  pchar, * arcPath;
    vg_lite_float_t  * pfloat;

    /*******************************************************************
    ** Converting.
    */
    if(path_data == NULL || *path_data == NULL || offset == NULL || coords == NULL)
        return VG_LITE_INVALID_ARGUMENT;

    if(Relative) {
        endX = EndX + coords->lastX;
        endY = EndY + coords->lastY;
    }
    else {
        endX = EndX;
        endY = EndY;
    }

    phi = RotAngle / 180.0f * PI;
    cosPhi = COSF(phi);
    sinPhi = SINF(phi);

    if(Relative) {
        dxHalf = - EndX / 2.0f;
        dyHalf = - EndY / 2.0f;
    }
    else {
        dxHalf = (coords->lastX - endX) / 2.0f;
        dyHalf = (coords->lastY - endY) / 2.0f;
    }

    x1Prime =  cosPhi * dxHalf + sinPhi * dyHalf;
    y1Prime = -sinPhi * dxHalf + cosPhi * dyHalf;

    rx = FABSF(HorRadius);
    ry = FABSF(VerRadius);

    x1PrimeSquare = x1Prime * x1Prime;
    y1PrimeSquare = y1Prime * y1Prime;

    lambda = x1PrimeSquare / (rx * rx) + y1PrimeSquare / (ry * ry);
    if(lambda > 1.0f) {
        rx *= SQRTF(lambda);
        ry *= SQRTF(lambda);
    }

    rxSquare = rx * rx;
    rySquare = ry * ry;

    sign     = (Large == CounterClockwise) ? -1 : 1;
    sq       = (rxSquare * rySquare
                - rxSquare * y1PrimeSquare
                - rySquare * x1PrimeSquare
               )
               /
               (rxSquare * y1PrimeSquare
                + rySquare * x1PrimeSquare
               );
    signedSq = sign * ((sq < 0) ? 0 : SQRTF(sq));
    cxPrime  = signedSq * (rx * y1Prime / ry);
    cyPrime  = signedSq * -(ry * x1Prime / rx);

    theta1 = _angle(1, 0, (x1Prime - cxPrime) / rx, (y1Prime - cyPrime) / ry);
    theta1 = FMODF(theta1, 2 * PI);

    thetaSpan = _angle((x1Prime - cxPrime) / rx, (y1Prime - cyPrime) / ry,
                       (-x1Prime - cxPrime) / rx, (-y1Prime - cyPrime) / ry);

    if(!CounterClockwise && (thetaSpan > 0)) {
        thetaSpan -= 2 * PI;
    }
    else if(CounterClockwise && (thetaSpan < 0)) {
        thetaSpan += 2 * PI;
    }

    thetaSpan = FMODF(thetaSpan, 2 * PI);

    /*******************************************************************
    ** Drawing.
    */
    segs  = (int32_t)(CEILF(FABSF(thetaSpan) / (45.0f / 180.0f * PI)));

    theta = thetaSpan / segs;

    ax = coords->lastX - COSF(theta1) * rx;
    ay = coords->lastY - SINF(theta1) * ry;
    if(FABSF(HorRadius) != 0 &&
       FABSF(VerRadius) != 0 &&
       (endX != coords->lastX || endY != coords->lastY)) {
        /* Determine the segment command. */
        segmentCommand = Relative
                         ? VLC_OP_QUAD_REL
                         : VLC_OP_QUAD;

        /* Determine the size of the buffer required. */
        bufferSize = (1 + 2 * 2) * SIZEOF(vg_lite_float_t) * segs;

        arcPath = (char *)vg_lite_os_malloc(*offset + bufferSize + last_size);
        if(arcPath == NULL)
            return VG_LITE_OUT_OF_RESOURCES;
#if(CHIPID == 0x355)
        memset(arcPath, 0, *offset + bufferSize + last_size);
#endif
        memcpy(arcPath, (char *)*path_data, *offset);
        vg_lite_os_free(*path_data);

        *path_data = arcPath;

        pchar = arcPath + *offset;
        pfloat = (vg_lite_float_t *)pchar;

        /* Set initial last point. */
        lastX = coords->lastX;
        lastY = coords->lastY;

        while(segs-- > 0) {
            theta1 += theta;

            controlX = ax + COSF(theta1 - (theta / 2.0f)) * rx / COSF(theta / 2.0f);
            controlY = ay + SINF(theta1 - (theta / 2.0f)) * ry / COSF(theta / 2.0f);

            anchorX = ax + COSF(theta1) * rx;
            anchorY = ay + SINF(theta1) * ry;

            if(RotAngle != 0) {
                x = coords->lastX + cosPhi * (controlX - coords->lastX) - sinPhi * (controlY - coords->lastY);
                y = coords->lastY + sinPhi * (controlX - coords->lastX) + cosPhi * (controlY - coords->lastY);
                controlX = x;
                controlY = y;

                x = coords->lastX + cosPhi * (anchorX - coords->lastX) - sinPhi * (anchorY - coords->lastY);
                y = coords->lastY + sinPhi * (anchorX - coords->lastX) + cosPhi * (anchorY - coords->lastY);
                anchorX = x;
                anchorY = y;
            }

            if(segs == 0) {
                /* Use end point directly to avoid accumulated errors. */
                anchorX = endX;
                anchorY = endY;
            }

            /* Adjust relative coordinates. */
            if(Relative) {
                vg_lite_float_t nextLastX = anchorX;
                vg_lite_float_t nextLastY = anchorY;

                controlX -= lastX;
                controlY -= lastY;

                anchorX -= lastX;
                anchorY -= lastY;

                lastX = nextLastX;
                lastY = nextLastY;
            }
            pchar = (char *)pfloat;
            *pchar = segmentCommand;
            pfloat++;
            *pfloat++ = controlX;
            *pfloat++ = controlY;
            *pfloat++ = anchorX;
            *pfloat++ = anchorY;
            *offset += (1 + 2 * 2) * SIZEOF(vg_lite_float_t);
        }
    }
    else {
        /* Determine the segment command. */
        segmentCommand = Relative
                         ? VLC_OP_LINE_REL
                         : VLC_OP_LINE;

        /* Determine the size of the buffer required. */
        bufferSize = (1 + 2) * SIZEOF(vg_lite_float_t);

        arcPath = (char *)vg_lite_os_malloc(*offset + bufferSize + last_size);
        if(arcPath == NULL)
            return VG_LITE_OUT_OF_RESOURCES;
        memcpy(arcPath, (char *)*path_data, *offset);
        vg_lite_os_free(*path_data);

        *path_data = arcPath;

        pchar = arcPath + *offset;
        pfloat = (vg_lite_float_t *)pchar;
        pchar = (char *)pfloat;
        *pchar = segmentCommand;
        pfloat++;
        *pfloat++ = Relative ? EndX : endX;
        *pfloat++ = Relative ? EndY : endY;
        *offset += (1 + 2) * SIZEOF(vg_lite_float_t);

    }
    /* Update the control coordinates. */
    coords->lastX    = endX;
    coords->lastY    = endY;
    coords->controlX = endX;
    coords->controlY = endY;

    return VG_LITE_SUCCESS;
}

vg_lite_error_t vg_lite_init_arc_path(vg_lite_path_t * path,
                                      vg_lite_format_t data_format,
                                      vg_lite_quality_t quality,
                                      vg_lite_uint32_t path_length,
                                      vg_lite_pointer path_data,
                                      vg_lite_float_t min_x, vg_lite_float_t min_y,
                                      vg_lite_float_t max_x, vg_lite_float_t max_y)
{
    vg_lite_error_t error = VG_LITE_SUCCESS;
    uint32_t i = 0, j, command = 0, offset = 0;
    vg_lite_float_t moveToX, moveToY, lineToX, lineToY, controlX, controlY, quadToX, quadToY;
    vg_lite_float_t controlX1, controlY1, controlX2, controlY2, cubicToX, cubicToY;
    vg_lite_float_t horRadius, verRadius, rotAngle, endX, endY;
    float * pfloat, * fpath;
    char * cpath, * pathdata;
    vg_lite_control_coord_t coords;
    char add_end = path->add_end;
    vg_lite_int32_t bytes;
    vg_lite_pointer path_data_fp32 = path_data;
    int8_t cmd, * path_data_s8_ptr;
    int16_t * path_data_s16_ptr;
    int32_t * path_data_s32_ptr;
    float * path_data_fp32_ptr;
    int32_t data_size, num = 0;
    memset(&coords, 0, sizeof(vg_lite_control_coord_t));
    coords.lastX = s_context.path_lastX;
    coords.lastY = s_context.path_lastY;

#if gcFEATURE_VG_TRACE_API
    VGLITE_LOG("vg_lite_init_arc_path %p %d %d %d %p %f %f %f %f\n", path, data_format, quality, path_length, path_data,
               min_x, min_y, max_x, max_y);
#endif

    if(path == NULL || path_data == NULL)
        return VG_LITE_INVALID_ARGUMENT;

    /* Path data cannot end with a CLOSE op. Replace CLOSE with END for path_data */
    data_size = get_data_size(data_format);
    num = path_length / data_size;

    switch(data_format) {
        case VG_LITE_S8:
            if(path_data && (*((char *)path_data + num - 1) == VLC_OP_CLOSE)) {
                *(char *)((int *)path_data + num - 1) = VLC_OP_END;
            }
            break;

        case VG_LITE_S16:
            if(path_data && (*(char *)((short *)path_data + num - 1) == VLC_OP_CLOSE)) {
                *(char *)((short *)path_data + num - 1) = VLC_OP_END;
            }
            break;

        case VG_LITE_S32:
            if(path_data && (*(char *)((int *)path_data + num - 1) == VLC_OP_CLOSE)) {
                *(char *)((int *)path_data + num - 1) = VLC_OP_END;
            }
            break;

        case VG_LITE_FP32:
            if(path_data && (*(char *)((float *)path_data + num - 1) == VLC_OP_CLOSE)) {
                *(char *)((float *)path_data + num - 1) = VLC_OP_END;
            }
            break;

        default:
            break;
    }

    /* Convert path format into float. */
    switch(data_format) {
        case VG_LITE_S8:
            /* src_s8, dst_fp32 */
            bytes = path_length * 4;
            path_data_fp32 = vg_lite_os_malloc(bytes);
            if(path_data_fp32 == NULL)
                return VG_LITE_OUT_OF_RESOURCES;
            memset(path_data_fp32, 0, bytes);
            path_data_fp32_ptr = path_data_fp32;
            path_data_s8_ptr = (int8_t *)path_data;
            i = 0;
            while(i < path_length) {
                cmd = *(uint8_t *)path_data_s8_ptr;
                *(uint8_t *)path_data_fp32_ptr = cmd;
                path_data_s8_ptr++;
                path_data_fp32_ptr++;
                for(j = 0; j < _commandSize_float[cmd] / 4 - 1; j++) {
                    *path_data_fp32_ptr = (float)(*path_data_s8_ptr);
                    path_data_fp32_ptr++;
                    path_data_s8_ptr++;
                }
                i += _commandSize_float[cmd] / 4;
            }
            path_length *= 4;
            break;

        case VG_LITE_S16:
            /* src_s16, dst_fp32 */
            bytes = path_length * 2;
            path_data_fp32 = vg_lite_os_malloc(bytes);
            if(path_data_fp32 == NULL)
                return VG_LITE_OUT_OF_RESOURCES;
            memset(path_data_fp32, 0, bytes);
            path_data_fp32_ptr = path_data_fp32;
            path_data_s16_ptr = (int16_t *)path_data;
            i = 0;
            while(i < path_length) {
                cmd = *(uint8_t *)path_data_s16_ptr;
                *(uint8_t *)path_data_fp32_ptr = cmd;
                path_data_s16_ptr++;
                path_data_fp32_ptr++;
                for(j = 0; j < _commandSize_float[cmd] / 4 - 1; j++) {
                    *path_data_fp32_ptr = (float)(*path_data_s16_ptr);
                    path_data_fp32_ptr++;
                    path_data_s16_ptr++;
                }
                i += _commandSize_float[cmd] / 2;
            }
            path_length *= 2;
            break;

        case VG_LITE_S32:
            /* src_s32, dst_fp32 */
            bytes = path_length;
            path_data_fp32 = vg_lite_os_malloc(bytes);
            if(path_data_fp32 == NULL)
                return VG_LITE_OUT_OF_RESOURCES;
            memset(path_data_fp32, 0, bytes);
            path_data_fp32_ptr = path_data_fp32;
            path_data_s32_ptr = (int32_t *)path_data;
            i = 0;
            while(i < path_length) {
                cmd = *(uint8_t *)path_data_s32_ptr;
                *(uint8_t *)path_data_fp32_ptr = cmd;
                path_data_s32_ptr++;
                path_data_fp32_ptr++;
                for(j = 0; j < _commandSize_float[cmd] / 4 - 1; j++) {
                    *path_data_fp32_ptr = (float)(*path_data_s32_ptr);
                    path_data_fp32_ptr++;
                    path_data_s32_ptr++;
                }
                i += _commandSize_float[cmd];
            }
            break;

        case VG_LITE_FP32: {
                /* src_fp32, dst_fp32 */
                bytes = path_length;
                path_data_fp32 = vg_lite_os_malloc(bytes);
                if(path_data_fp32 == NULL)
                    return VG_LITE_OUT_OF_RESOURCES;
#if(CHIPID == 0x355)
                memset(path_data_fp32, 0, bytes);
#endif
                memcpy(path_data_fp32, path_data, bytes);
                break;
            }

        default:
            break;
    }

    vg_lite_clear_path(path);

    data_format = VG_LITE_FP32;
    if(!path_length) {
        path->format = data_format;
        path->quality = quality;
        path->bounding_box[0] = min_x;
        path->bounding_box[1] = min_y;
        path->bounding_box[2] = max_x;
        path->bounding_box[3] = max_y;
        path->path_length = 0;
        path->path = NULL;
        path->pdata_internal = 1;
        path->path_changed = 1;
        path->uploaded.address = 0;
        path->uploaded.bytes = 0;
        path->uploaded.handle = NULL;
        path->uploaded.memory = NULL;
        return VG_LITE_SUCCESS;
    }
    path->add_end = add_end;
    path->bounding_box[0] = min_x;
    path->bounding_box[1] = min_y;
    path->bounding_box[2] = max_x;
    path->bounding_box[3] = max_y;
    pathdata = (char *)vg_lite_os_malloc(path_length);
    if(pathdata == NULL)
        return VG_LITE_OUT_OF_RESOURCES;
#if(CHIPID == 0x355)
    memset(pathdata, 0, path_length);
#endif
    pfloat = (vg_lite_float_t *)path_data_fp32;
    i = 0;
    while(i < path_length) {
        cpath = (char *)pfloat;
        command = (uint32_t) * cpath;
        pfloat++;
#if (CHIPID==0x355)
        uint8_t flag = 0;
        if(command == VLC_OP_CLOSE) {
            if(*((uint32_t *)cpath + 1) == VLC_OP_MOVE || *((uint32_t *)cpath + 1) == VLC_OP_MOVE_REL) {
                flag = 1;
            }
        }
#endif
        switch(command) {
            case VLC_OP_END:
                cpath = (char *)pathdata + offset;
                fpath = (vg_lite_float_t *)cpath;
                *cpath = VLC_OP_END;
                offset += _commandSize_float[VLC_OP_END];
                i += _commandSize_float[VLC_OP_END];
                break;
            case VLC_OP_CLOSE:
                /* Update the control coordinates. */
                coords.lastX = coords.startX;
                coords.lastY = coords.startY;
                coords.controlX = coords.startX;
                coords.controlY = coords.startY;

                cpath = (char *)pathdata + offset;
                fpath = (vg_lite_float_t *)cpath;
#if (CHIPID==0x355)
                if(flag == 1) {
                    if(data_size == 1) {
                        *cpath = VLC_OP_CLOSE;
                    }
                    else if(data_size == 2) {
                        *(uint16_t *)cpath = 0x0101;
                    }
                    else if(data_size == 4) {
                        *(uint32_t *)cpath = 0x01010101;
                    }
                }
                else
#endif
                {
                    *cpath = VLC_OP_CLOSE;
                }
                offset += _commandSize_float[VLC_OP_CLOSE];
                i += _commandSize_float[VLC_OP_CLOSE];
                break;
            case VLC_OP_MOVE:
                moveToX = *pfloat;
                pfloat++;
                moveToY = *pfloat;
                pfloat++;

                /* Update the control coordinates. */
                coords.startX = moveToX;
                coords.startY = moveToY;
                coords.lastX = moveToX;
                coords.lastY = moveToY;
                coords.controlX = moveToX;
                coords.controlY = moveToY;

                cpath = (char *)pathdata + offset;
                fpath = (vg_lite_float_t *)cpath;
                *cpath = VLC_OP_MOVE;
                fpath++;
                *fpath = moveToX;
                fpath++;
                *fpath = moveToY;
                fpath++;
                offset += _commandSize_float[VLC_OP_MOVE];
                i += _commandSize_float[VLC_OP_MOVE];
                break;
            case VLC_OP_MOVE_REL:
                moveToX = *pfloat;
                pfloat++;
                moveToY = *pfloat;
                pfloat++;

                cpath = (char *)pathdata + offset;
                fpath = (vg_lite_float_t *)cpath;
                *cpath = VLC_OP_MOVE_REL;
                fpath++;
                *fpath = moveToX;
                fpath++;
                *fpath = moveToY;
                fpath++;
                offset += _commandSize_float[VLC_OP_MOVE_REL];
                i += _commandSize_float[VLC_OP_MOVE_REL];

                /* Determine the absolute coordinates. */
                moveToX += coords.lastX;
                moveToY += coords.lastY;

                /* Update the control coordinates. */
                coords.startX = moveToX;
                coords.startY = moveToY;
                coords.lastX = moveToX;
                coords.lastY = moveToY;
                coords.controlX = moveToX;
                coords.controlY = moveToY;
                break;
            case VLC_OP_LINE:
                lineToX = *pfloat;
                pfloat++;
                lineToY = *pfloat;
                pfloat++;

                /* Update the control coordinates. */
                coords.lastX = lineToX;
                coords.lastY = lineToY;
                coords.controlX = lineToX;
                coords.controlY = lineToY;

                cpath = (char *)pathdata + offset;
                fpath = (vg_lite_float_t *)cpath;
                *cpath = VLC_OP_LINE;
                fpath++;
                *fpath = lineToX;
                fpath++;
                *fpath = lineToY;
                fpath++;
                offset += _commandSize_float[VLC_OP_LINE];
                i += _commandSize_float[VLC_OP_LINE];
                break;
            case VLC_OP_LINE_REL:
                lineToX = *pfloat;
                pfloat++;
                lineToY = *pfloat;
                pfloat++;

                cpath = (char *)pathdata + offset;
                fpath = (vg_lite_float_t *)cpath;
                *cpath = VLC_OP_LINE_REL;
                fpath++;
                *fpath = lineToX;
                fpath++;
                *fpath = lineToY;
                fpath++;
                offset += _commandSize_float[VLC_OP_LINE_REL];
                i += _commandSize_float[VLC_OP_LINE_REL];

                /* Determine the absolute coordinates. */
                lineToX += coords.lastX;
                lineToY += coords.lastY;

                /* Update the control coordinates. */
                coords.lastX = lineToX;
                coords.lastY = lineToY;
                coords.controlX = lineToX;
                coords.controlY = lineToY;
                break;
            case VLC_OP_QUAD:
                controlX = *pfloat;
                pfloat++;
                controlY = *pfloat;
                pfloat++;
                quadToX = *pfloat;
                pfloat++;
                quadToY = *pfloat;
                pfloat++;
                compute_quadpathbounds(path, coords.lastX, coords.lastY, controlX, controlY, quadToX, quadToY);
                /* Update the control coordinates. */
                coords.lastX = quadToX;
                coords.lastY = quadToY;
                coords.controlX = controlX;
                coords.controlY = controlY;

                cpath = (char *)pathdata + offset;
                fpath = (vg_lite_float_t *)cpath;
                *cpath = VLC_OP_QUAD;
                fpath++;
                *fpath = controlX;
                fpath++;
                *fpath = controlY;
                fpath++;
                *fpath = quadToX;
                fpath++;
                *fpath = quadToY;
                fpath++;
                offset += _commandSize_float[VLC_OP_QUAD];
                i += _commandSize_float[VLC_OP_QUAD];
                break;
            case VLC_OP_SQUAD:
                quadToX = *pfloat;
                pfloat++;
                quadToY = *pfloat;
                pfloat++;
                i += _commandSize_float[VLC_OP_SQUAD];
                /* Update the control coordinates. */
                VG_LITE_ERROR_HANDLER(_convert_squad(quadToX, quadToY, VGL_FALSE, &coords, (void *)&pathdata, &offset,
                                                     path_length - i));
                compute_quadpathbounds(path, coords.startX, coords.startY, coords.controlX, coords.controlY, quadToX, quadToY);
                break;
            case VLC_OP_SQUAD_REL:
                quadToX = *pfloat;
                pfloat++;
                quadToY = *pfloat;
                pfloat++;
                i += _commandSize_float[VLC_OP_SQUAD_REL];
                /* Update the control coordinates. */
                VG_LITE_ERROR_HANDLER(_convert_squad(quadToX, quadToY, VGL_TRUE, &coords, (void *)&pathdata, &offset, path_length - i));
                break;
            case VLC_OP_QUAD_REL:
                controlX = *pfloat;
                pfloat++;
                controlY = *pfloat;
                pfloat++;
                quadToX = *pfloat;
                pfloat++;
                quadToY = *pfloat;
                pfloat++;

                cpath = (char *)pathdata + offset;
                fpath = (vg_lite_float_t *)cpath;
                *cpath = VLC_OP_QUAD_REL;
                fpath++;
                *fpath = controlX;
                fpath++;
                *fpath = controlY;
                fpath++;
                *fpath = quadToX;
                fpath++;
                *fpath = quadToY;
                fpath++;
                offset += _commandSize_float[VLC_OP_QUAD_REL];
                i += _commandSize_float[VLC_OP_QUAD_REL];

                /* Determine the absolute coordinates. */
                controlX += coords.lastX;
                controlY += coords.lastY;
                quadToX += coords.lastX;
                quadToY += coords.lastY;

                /* Update the control coordinates. */
                coords.lastX = quadToX;
                coords.lastY = quadToY;
                coords.controlX = controlX;
                coords.controlY = controlY;
                break;
            case VLC_OP_CUBIC:
                controlX1 = *pfloat;
                pfloat++;
                controlY1 = *pfloat;
                pfloat++;
                controlX2 = *pfloat;
                pfloat++;
                controlY2 = *pfloat;
                pfloat++;
                cubicToX = *pfloat;
                pfloat++;
                cubicToY = *pfloat;
                pfloat++;

                /* Update the control coordinates. */
                coords.lastX = cubicToX;
                coords.lastY = cubicToY;
                coords.controlX = controlX2;
                coords.controlY = controlY2;

                cpath = (char *)pathdata + offset;
                fpath = (vg_lite_float_t *)cpath;
                *cpath = VLC_OP_CUBIC;
                fpath++;
                *fpath = controlX1;
                fpath++;
                *fpath = controlY1;
                fpath++;
                *fpath = controlX2;
                fpath++;
                *fpath = controlY2;
                fpath++;
                *fpath = cubicToX;
                fpath++;
                *fpath = cubicToY;
                fpath++;
                offset += _commandSize_float[VLC_OP_CUBIC];
                i += _commandSize_float[VLC_OP_CUBIC];
                break;
            case VLC_OP_CUBIC_REL:
                controlX1 = *pfloat;
                pfloat++;
                controlY1 = *pfloat;
                pfloat++;
                controlX2 = *pfloat;
                pfloat++;
                controlY2 = *pfloat;
                pfloat++;
                cubicToX = *pfloat;
                pfloat++;
                cubicToY = *pfloat;
                pfloat++;

                cpath = (char *)pathdata + offset;
                fpath = (vg_lite_float_t *)cpath;
                *cpath = VLC_OP_CUBIC_REL;
                fpath++;
                *fpath = controlX1;
                fpath++;
                *fpath = controlY1;
                fpath++;
                *fpath = controlX2;
                fpath++;
                *fpath = controlY2;
                fpath++;
                *fpath = cubicToX;
                fpath++;
                *fpath = cubicToY;
                fpath++;
                offset += _commandSize_float[VLC_OP_CUBIC_REL];
                i += _commandSize_float[VLC_OP_CUBIC_REL];

                /* Determine the absolute coordinates. */
                controlX2 += coords.lastX;
                controlY2 += coords.lastY;
                cubicToX += coords.lastX;
                cubicToY += coords.lastY;

                /* Update the control coordinates. */
                coords.lastX = cubicToX;
                coords.lastY = cubicToY;
                coords.controlX = controlX2;
                coords.controlY = controlY2;
                break;
            case VLC_OP_SCUBIC:
                controlX1 = *pfloat;
                pfloat++;
                controlY1 = *pfloat;
                pfloat++;
                cubicToX = *pfloat;
                pfloat++;
                cubicToY = *pfloat;
                pfloat++;
                i += _commandSize_float[VLC_OP_SCUBIC];
                /* Update the control coordinates. */
                VG_LITE_ERROR_HANDLER(_convert_scubic(cubicToX, cubicToY, controlX1, controlY1, VGL_FALSE, &coords, (void *)&pathdata,
                                                      &offset, path_length - i));
                break;
            case VLC_OP_SCUBIC_REL:
                controlX1 = *pfloat;
                pfloat++;
                controlY1 = *pfloat;
                pfloat++;
                cubicToX = *pfloat;
                pfloat++;
                cubicToY = *pfloat;
                pfloat++;
                i += _commandSize_float[VLC_OP_SCUBIC_REL];
                /* Update the control coordinates. */
                VG_LITE_ERROR_HANDLER(_convert_scubic(cubicToX, cubicToY, controlX1, controlY1, VGL_TRUE, &coords, (void *)&pathdata,
                                                      &offset, path_length - i));
                break;
            case VLC_OP_HLINE:
                lineToX = *pfloat;
                pfloat++;
                lineToY = coords.lastY;
                i += _commandSize_float[VLC_OP_HLINE];
                /* Update the control coordinates. */
                VG_LITE_ERROR_HANDLER(_convert_hline(lineToX, lineToY, VGL_FALSE, &coords, (void *)&pathdata, &offset,
                                                     path_length - i));
                break;
            case VLC_OP_HLINE_REL:
                lineToX = *pfloat;
                pfloat++;
                lineToY = coords.lastY;
                i += _commandSize_float[VLC_OP_HLINE_REL];
                /* Update the control coordinates. */
                VG_LITE_ERROR_HANDLER(_convert_hline(lineToX, lineToY, VGL_TRUE, &coords, (void *)&pathdata, &offset, path_length - i));
                break;
            case VLC_OP_VLINE:
                lineToX = coords.lastX;
                lineToY = *pfloat;
                pfloat++;
                i += _commandSize_float[VLC_OP_VLINE];
                /* Update the control coordinates. */
                VG_LITE_ERROR_HANDLER(_convert_vline(lineToX, lineToY, VGL_FALSE, &coords, (void *)&pathdata, &offset,
                                                     path_length - i));
                break;
            case VLC_OP_VLINE_REL:
                lineToX = coords.lastX;
                lineToY = *pfloat;
                pfloat++;
                i += _commandSize_float[VLC_OP_VLINE_REL];
                /* Update the control coordinates. */
                VG_LITE_ERROR_HANDLER(_convert_vline(lineToX, lineToY, VGL_TRUE, &coords, (void *)&pathdata, &offset, path_length - i));
                break;
            case VLC_OP_SCCWARC:
                horRadius = *pfloat;
                pfloat++;
                verRadius = *pfloat;
                pfloat++;
                rotAngle = *pfloat;
                pfloat++;
                endX = *pfloat;
                pfloat++;
                endY = *pfloat;
                pfloat++;
                i += _commandSize_float[VLC_OP_SCCWARC];
                VG_LITE_ERROR_HANDLER(_convert_arc(horRadius, verRadius, rotAngle, endX, endY, VGL_FALSE, VGL_FALSE, VGL_FALSE, &coords,
                                                   (void *)&pathdata, &offset, path_length - i));
                break;
            case VLC_OP_SCCWARC_REL:
                horRadius = *pfloat;
                pfloat++;
                verRadius = *pfloat;
                pfloat++;
                rotAngle = *pfloat;
                pfloat++;
                endX = *pfloat;
                pfloat++;
                endY = *pfloat;
                pfloat++;
                i += _commandSize_float[VLC_OP_SCCWARC_REL];
                VG_LITE_ERROR_HANDLER(_convert_arc(horRadius, verRadius, rotAngle, endX, endY, VGL_FALSE, VGL_FALSE, VGL_TRUE, &coords,
                                                   (void *)&pathdata, &offset, path_length - i));
                break;
            case VLC_OP_SCWARC:
                horRadius = *pfloat;
                pfloat++;
                verRadius = *pfloat;
                pfloat++;
                rotAngle = *pfloat;
                pfloat++;
                endX = *pfloat;
                pfloat++;
                endY = *pfloat;
                pfloat++;
                i += _commandSize_float[VLC_OP_SCCWARC_REL];
                VG_LITE_ERROR_HANDLER(_convert_arc(horRadius, verRadius, rotAngle, endX, endY, VGL_TRUE, VGL_FALSE, VGL_FALSE, &coords,
                                                   (void *)&pathdata, &offset, path_length - i));
                break;
            case VLC_OP_SCWARC_REL:
                horRadius = *pfloat;
                pfloat++;
                verRadius = *pfloat;
                pfloat++;
                rotAngle = *pfloat;
                pfloat++;
                endX = *pfloat;
                pfloat++;
                endY = *pfloat;
                pfloat++;
                i += _commandSize_float[VLC_OP_SCCWARC_REL];
                VG_LITE_ERROR_HANDLER(_convert_arc(horRadius, verRadius, rotAngle, endX, endY, VGL_TRUE, VGL_FALSE, VGL_TRUE, &coords,
                                                   (void *)&pathdata, &offset, path_length - i));
                break;
            case VLC_OP_LCCWARC:
                horRadius = *pfloat;
                pfloat++;
                verRadius = *pfloat;
                pfloat++;
                rotAngle = *pfloat;
                pfloat++;
                endX = *pfloat;
                pfloat++;
                endY = *pfloat;
                pfloat++;
                i += _commandSize_float[VLC_OP_SCCWARC_REL];
                VG_LITE_ERROR_HANDLER(_convert_arc(horRadius, verRadius, rotAngle, endX, endY, VGL_FALSE, VGL_TRUE, VGL_FALSE, &coords,
                                                   (void *)&pathdata, &offset, path_length - i));
                break;
            case VLC_OP_LCCWARC_REL:
                horRadius = *pfloat;
                pfloat++;
                verRadius = *pfloat;
                pfloat++;
                rotAngle = *pfloat;
                pfloat++;
                endX = *pfloat;
                pfloat++;
                endY = *pfloat;
                pfloat++;
                i += _commandSize_float[VLC_OP_SCCWARC_REL];
                VG_LITE_ERROR_HANDLER(_convert_arc(horRadius, verRadius, rotAngle, endX, endY, VGL_FALSE, VGL_TRUE, VGL_TRUE, &coords,
                                                   (void *)&pathdata, &offset, path_length - i));
                break;
            case VLC_OP_LCWARC:
                horRadius = *pfloat;
                pfloat++;
                verRadius = *pfloat;
                pfloat++;
                rotAngle = *pfloat;
                pfloat++;
                endX = *pfloat;
                pfloat++;
                endY = *pfloat;
                pfloat++;
                i += _commandSize_float[VLC_OP_SCCWARC_REL];
                VG_LITE_ERROR_HANDLER(_convert_arc(horRadius, verRadius, rotAngle, endX, endY, VGL_TRUE, VGL_TRUE, VGL_FALSE, &coords,
                                                   (void *)&pathdata, &offset, path_length - i));
                break;
            case VLC_OP_LCWARC_REL:
                horRadius = *pfloat;
                pfloat++;
                verRadius = *pfloat;
                pfloat++;
                rotAngle = *pfloat;
                pfloat++;
                endX = *pfloat;
                pfloat++;
                endY = *pfloat;
                pfloat++;
                i += _commandSize_float[VLC_OP_SCCWARC_REL];
                VG_LITE_ERROR_HANDLER(_convert_arc(horRadius, verRadius, rotAngle, endX, endY, VGL_TRUE, VGL_TRUE, VGL_TRUE, &coords,
                                                   (void *)&pathdata, &offset, path_length - i));
                break;
            default:
                break;
        }
    }

    if(path_data_fp32 != NULL)
        vg_lite_os_free(path_data_fp32);
    path->format = VG_LITE_FP32;
    path->quality = quality;
    path->path_length = offset;
    path->path = pathdata;
    path->pdata_internal = 1;
    path->path_changed = 1;
    path->uploaded.address = 0;
    path->uploaded.bytes = 0;
    path->uploaded.handle = NULL;
    path->uploaded.memory = NULL;
    s_context.path_lastX = coords.lastX;
    s_context.path_lastY = coords.lastY;
    return VG_LITE_SUCCESS;

ErrorHandler:
    vg_lite_os_free(pathdata);
    pathdata = NULL;
    return error;
}

#else  /* gcFEATURE_VG_ARC_PATH */

vg_lite_error_t vg_lite_init_arc_path(vg_lite_path_t * path,
                                      vg_lite_format_t data_format,
                                      vg_lite_quality_t quality,
                                      vg_lite_uint32_t path_length,
                                      vg_lite_pointer path_data,
                                      vg_lite_float_t min_x, vg_lite_float_t min_y,
                                      vg_lite_float_t max_x, vg_lite_float_t max_y)
{
    return VG_LITE_NOT_SUPPORT;
}

#endif /* gcFEATURE_VG_ARC_PATH */

#endif /* LV_USE_VG_LITE_DRIVER */

