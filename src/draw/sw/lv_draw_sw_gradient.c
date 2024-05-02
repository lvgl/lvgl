/**
 * @file lv_draw_sw_gradient.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_gradient.h"
#if LV_USE_DRAW_SW

#include "../../misc/lv_types.h"
#include "../../osal/lv_os.h"

/*********************
 *      DEFINES
 *********************/
#define GRAD_CM(r,g,b) lv_color_make(r,g,b)
#define GRAD_CONV(t, x) t = x

#undef ALIGN
#if defined(LV_ARCH_64)
    #define ALIGN(X)    (((X) + 7) & ~7)
#else
    #define ALIGN(X)    (((X) + 3) & ~3)
#endif

//#define USE_INCREMENTAL
//#define USE_FLOATING_POINT
//#define USE_LV_SQRT

/**********************
 *      TYPEDEFS
 **********************/

#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS

typedef struct {
    /* w = (-b(xp, yp) + sqrt(sqr(b(xp, yp)) - 4 * a * c(xp, yp))) / (2 * a) */
    int32_t x0;         /* center of the start circle */
    int32_t y0;         /* center of the start circle */
    int32_t r0;         /* radius of the start circle */
    int32_t inv_dr;     /* 1 / (r1 - r0) */
    int32_t a4;         /* 4 * a */
    int32_t inv_a4;     /* 1 / (4 * a) */
    int32_t dx;
    /* b(xp, yp) = xp * bpx + yp * bpy + bc */
    int32_t bpx;
    int32_t bpy;
    int32_t bc;
#ifdef USE_INCREMENTAL
    /*save intermediates for incremental calculation*/
    int32_t x;          /* previous x */
    int32_t y;          /* previous y */
    int32_t b;
    int32_t db;         /* delta b */
    int32_t c;
    int32_t dc;         /* delta c */
#endif
} lv_grad_radial_state_t;

#endif

/**********************
 *  STATIC PROTOTYPES
 **********************/
typedef lv_result_t (*op_cache_t)(lv_grad_t * c, void * ctx);
static lv_grad_t * allocate_item(const lv_grad_dsc_t * g, int32_t w, int32_t h);

#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS

    static inline uint32_t bsr(uint32_t a);
    static inline uint16_t fast_sqrt32(uint32_t val);
    static inline int32_t lv_sqr(int32_t x);

#endif

/**********************
 *   STATIC VARIABLE
 **********************/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_grad_t * allocate_item(const lv_grad_dsc_t * g, int32_t w, int32_t h)
{
    int32_t size;
    switch(g->dir) {
        case LV_GRAD_DIR_RADIAL:
        case LV_GRAD_DIR_HOR:
            size = w;
            break;
        case LV_GRAD_DIR_VER:
            size = h;
            break;
        default:
            size = 64;
    }

    size_t req_size = ALIGN(sizeof(lv_grad_t)) + ALIGN(size * sizeof(lv_color_t)) + ALIGN(size * sizeof(lv_opa_t));
    lv_grad_t * item  = lv_malloc(req_size);
    LV_ASSERT_MALLOC(item);
    if(item == NULL) return NULL;

    uint8_t * p = (uint8_t *)item;
    item->color_map = (lv_color_t *)(p + ALIGN(sizeof(*item)));
    item->opa_map = (lv_opa_t *)(p + ALIGN(sizeof(*item)) + ALIGN(size * sizeof(lv_color_t)));
    item->size = size;
    return item;
}

/**********************
 *     FUNCTIONS
 **********************/

lv_grad_t * lv_gradient_get(const lv_grad_dsc_t * g, int32_t w, int32_t h)
{
    /* No gradient, no cache */
    if(g->dir == LV_GRAD_DIR_NONE) return NULL;

    /* Step 1: Search cache for the given key */
    lv_grad_t * item = allocate_item(g, w, h);
    if(item == NULL) {
        LV_LOG_WARN("Failed to allocate item for the gradient");
        return item;
    }

    /* Step 3: Fill it with the gradient, as expected */
    uint32_t i;
    for(i = 0; i < item->size; i++) {
        lv_gradient_color_calculate(g, item->size, i, &item->color_map[i], &item->opa_map[i]);
    }
    return item;
}

void LV_ATTRIBUTE_FAST_MEM lv_gradient_color_calculate(const lv_grad_dsc_t * dsc, int32_t range,
                                                       int32_t frac, lv_grad_color_t * color_out, lv_opa_t * opa_out)
{
    lv_grad_color_t tmp;
    /*Clip out-of-bounds first*/
    int32_t min = (dsc->stops[0].frac * range) >> 8;
    if(frac <= min) {
        GRAD_CONV(tmp, dsc->stops[0].color);
        *color_out = tmp;
        *opa_out = dsc->stops[0].opa;
        return;
    }

    int32_t max = (dsc->stops[dsc->stops_count - 1].frac * range) >> 8;
    if(frac >= max) {
        GRAD_CONV(tmp, dsc->stops[dsc->stops_count - 1].color);
        *color_out = tmp;
        *opa_out = dsc->stops[dsc->stops_count - 1].opa;
        return;
    }

    /*Find the 2 closest stop now*/
    int32_t d = 0;
    int32_t found_i = 0;
    for(uint8_t i = 1; i < dsc->stops_count; i++) {
        int32_t cur = (dsc->stops[i].frac * range) >> 8;
        if(frac <= cur) {
            found_i = i;
            break;
        }
    }

    LV_ASSERT(found_i != 0);

    lv_color_t one, two;
    one = dsc->stops[found_i - 1].color;
    two = dsc->stops[found_i].color;
    min = (dsc->stops[found_i - 1].frac * range) >> 8;
    max = (dsc->stops[found_i].frac * range) >> 8;
    d = max - min;

    /*Then interpolate*/
    frac -= min;
    lv_opa_t mix = (frac * 255) / d;
    lv_opa_t imix = 255 - mix;

    *color_out = GRAD_CM(LV_UDIV255(two.red * mix   + one.red * imix),
                         LV_UDIV255(two.green * mix + one.green * imix),
                         LV_UDIV255(two.blue * mix  + one.blue * imix));

    *opa_out = LV_UDIV255(dsc->stops[found_i].opa * mix   + dsc->stops[found_i - 1].opa * imix);
}

void lv_gradient_cleanup(lv_grad_t * grad)
{
    lv_free(grad);
}

#ifdef LV_USE_DRAW_SW_COMPLEX_GRADIENTS

/*bit scan reverse*/
static inline uint32_t bsr(uint32_t a)
{
#ifdef __GNUC__
    return __builtin_clz(a) ^ 31;
#elif _WIN32
    unsigned long r = 0;
    _BitScanReverse(&r, a);
    return (uint8_t)r;
#else
#error Need to define bsr() in lv_draw_sw_gradient.c
#endif
}

static inline uint16_t fast_sqrt32(uint32_t val)
{
    unsigned a, b;

    if(val < 2) return val;  /* avoid div/0 */

    /* start with the estimated lower bound */
    a = 1 << (bsr(val) >> 1);

    b = val / a;
    a = (a + b) >> 1;
    b = val / a;
    a = (a + b) >> 1;
    b = val / a;
    a = (a + b) >> 1;
    //    b = val / a; a = (a+b) >> 1;
    //    b = val / a; a = (a+b) >> 1;
    //    b = val / a; a = (a+b) >> 1;

    return a;
}

static inline int32_t lv_sqr(int32_t x)
{
    return x * x;
}

void lv_gradient_radial_setup(lv_grad_dsc_t * dsc)
{
    lv_point_t start = dsc->r.start;
    lv_point_t end = dsc->r.end;
    int16_t r_start = dsc->r.start_radius;
    int16_t r_end = dsc->r.end_radius;
    LV_ASSERT(r_end != 0);
    lv_grad_radial_state_t * grad_r = lv_malloc(sizeof(lv_grad_radial_state_t));
    dsc->state = grad_r;
#ifdef SCALING_SUPPORT
    grad_r->x0 = (float)start.x * GRAD_SCALE_X;
    grad_r->y0 = (float)start.y * GRAD_SCALE_Y;
    grad_r->r0 = (float)r_start * GRAD_SCALE_X;
    float dr = (float)(r_end - r_start) * GRAD_SCALE_X;
#else
    grad_r->x0 = start.x;
    grad_r->y0 = start.y;
    grad_r->r0 = r_start;
    int32_t dr = r_end - r_start;
#endif
    if(end.x == start.x && end.y == start.y) {
        grad_r->a4 = lv_sqr(dr) << 2;
        grad_r->bpx = 0;
        grad_r->bpy = 0;
        grad_r->bc = (grad_r->r0 * dr) << 1;
        grad_r->dx = 0;
        grad_r->inv_dr = (1 << (8 + 16)) / dr;
    }
    else {
#ifdef SCALING_SUPPORT
        float dx = (float)(end.x - start.x) * GRAD_SCALE_X;
        float dy = (float)(end.y - start.y) * GRAD_SCALE_Y;
#else
        int32_t dx = end.x - start.x;
        int32_t dy = end.y - start.y;
#endif
        grad_r->dx = dx;    // needed for incremental calculation
        grad_r->a4 = (lv_sqr(dr) - lv_sqr(dx) - lv_sqr(dy)) << 2;
        /* b(xp, yp) = xp * bpx + yp * bpy + bc */
        grad_r->bpx = dx << 1;
        grad_r->bpy = dy << 1;
        grad_r->bc = (grad_r->r0 * dr - grad_r->x0 * dx - grad_r->y0 * dy) << 1;
    }
    grad_r->inv_a4 = grad_r->a4 != 0 ? (1 << (13 + 16)) / grad_r->a4 : 0;
#ifdef USE_INCREMENTAL
    grad_r->x = 0x7fffffff;
    grad_r->y = 0x7fffffff;
    grad_r->db = grad_r->dx << 1;
#endif
}

void lv_gradient_radial_cleanup(lv_grad_dsc_t * dsc)
{
    lv_free(dsc->state);
}

int32_t LV_ATTRIBUTE_FAST_MEM lv_gradient_radial_get_w(lv_grad_dsc_t * dsc, int32_t xp, int32_t yp)
{
    lv_grad_radial_state_t * grad_r = (lv_grad_radial_state_t *)dsc->state;
    int32_t w;
    int32_t a4, b, c;

    if(grad_r->bpx || grad_r->bpy) {

#ifdef USE_INCREMENTAL
        if(p.x == ++grad_r->x && p.y == grad_r->y) {
            b = grad_r->b + grad_r->db;
            c = grad_r->c - grad_r->dc;
            grad_r->b = b;
            grad_r->c = c;
            grad_r->dc += 2;
        }
        else {
#endif
            /*
                ((r1-r0)^2 - (x1-x0)^2 - (y1-y0)^2) * w^2 + 2*((xp-x0)*(x1-x0) + (yp-y0)*(y1-y0)) * w + (-(xp-x0)^2 - (yp-y0)^) = 0
                w = (-b(xp, yp) + sqrt(sqr(b(xp, yp)) - 4 * a * c(xp, yp))) / (2 * a), where
                    b(xp, yp) = xp * bpx + yp * bpy + bc
                    c(xp, yp) = xp * (xp - cx) + yp * (yp - cy) + cc
             */
            b = xp * grad_r->bpx + yp * grad_r->bpy + grad_r->bc;
            c = lv_sqr(grad_r->r0) - lv_sqr(xp - grad_r->x0) - lv_sqr(yp - grad_r->y0);
#ifdef USE_INCREMENTAL
            grad_r->b = b;
            grad_r->c = c;
            grad_r->dc = ((p.x - grad_r->x0) << 1) + 1;
            grad_r->x = xp;
            grad_r->y = yp;
        }
#endif
        a4 = grad_r->a4;
        if(a4 == 0) {   // solve linear equation: w = -c/b
            w = b == 0 ? 0 /*0x7fffffff*/ : -(c << 8) / b;
        }
        else {                  // solve quadratical equation: w = (-b + sqrt(b^2 - 4ac))/2a
            int32_t sqrb = lv_sqr(b >> 4);                      // b^2 shifted down by 2*4=8
            int32_t det1 = ((a4 >> 4) * (c >> 4));     // 4ac shifted down by 8
            if(sqrb > det1) {    // determinant will be positive
#ifdef USE_LV_SQRT
                lv_sqrt_res_t res;
                lv_sqrt(sqrb - det1, &res, 0x8000);
                //                  w = ((res.i - (b >> 4)) << 13) / a4;        // square root shifted down by 4 (includes *256)
                w = ((res.i - (b >> 4)) * grad_r->inv_a4) >> 16;        // square root shifted down by 4 (includes *256)
#else
                w = ((fast_sqrt32(sqrb - det1) - (b >> 4)) * grad_r->inv_a4) >>
                    16;        // square root shifted down by 4 (includes *256)
#endif
            }
            else {
                return 0; // 0x7fffffff;
            }
        }
    }
    else {  // special case: concentric circles
        c = (lv_sqr(xp - grad_r->x0) + lv_sqr(yp - grad_r->y0));
        w = (((fast_sqrt32(c) - grad_r->r0)) * grad_r->inv_dr) >> 16;
    }
    if(w < 0)
        w = 0;
    switch(dsc->extend) {
        case LV_GRAD_EXTEND_PAD:     /**< Repeat the same color*/
            w = LV_MIN(w, 255);
            break;
        case LV_GRAD_EXTEND_REPEAT:  /**< Repeat the pattern*/
            w &= 255;
            break;
        case LV_GRAD_EXTEND_REFLECT: /**< Repeat the pattern mirrored*/
            w &= 511;
            if(w > 255)
                w ^= 511;   /* 511 - w */
            break;
    }
    return w;
}

void LV_ATTRIBUTE_FAST_MEM lv_gradient_radial_get_line(lv_grad_dsc_t * dsc, lv_grad_t * grad, int32_t x, int32_t y,
                                                       int32_t width, lv_grad_t * result)
{
    int32_t end = x + width;
    lv_color_t * buf = result->color_map;
    lv_opa_t * opa = result->opa_map;
    for(; x < end; x++) {
        int32_t w = lv_gradient_radial_get_w(dsc, x, y);
        if(w == 0x7fffffff) {
            *buf = lv_color_black();
            *opa = LV_OPA_0;
        }
        else {
            *buf = grad->color_map[w];
            *opa = grad->opa_map[w];
        }
        buf++;
        opa++;
    }
}

#endif /* LV_USE_DRAW_SW_COMPLEX_GRADIENTS */

#endif /*LV_USE_DRAW_SW*/
