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

#define USE_INCREMENTAL
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
    lv_area_t clip_area;
} lv_grad_radial_state_t;

#endif

/**********************
 *  STATIC PROTOTYPES
 **********************/
typedef lv_result_t (*op_cache_t)(lv_grad_t * c, void * ctx);
static lv_grad_t * allocate_item(const lv_grad_dsc_t * g, int32_t w, int32_t h);

#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS

    static inline uint32_t bsr(uint32_t a);
    static inline uint32_t fast_sqrt32(uint32_t val);
    static inline int32_t sqr32(int32_t x);

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

#if 0
static inline uint32_t fast_sqrt32(uint32_t val)
{
    uint32_t a, b;

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
#elif 1
/*
// Integer Square Root function
// Contributors include Arne Steinarson for the basic approximation idea,
// Dann Corbit and Mathew Hendry for the first cut at the algorithm,
// Lawrence Kirby for the rearrangement, improvments and range optimization
// and Paul Hsieh for the round-then-adjust idea.
*/
static inline uint32_t fast_sqrt32(uint32_t x)
{
    static const unsigned char sqq_table[] = {
       0,  16,  22,  27,  32,  35,  39,  42,  45,  48,  50,  53,  55,  57,
      59,  61,  64,  65,  67,  69,  71,  73,  75,  76,  78,  80,  81,  83,
      84,  86,  87,  89,  90,  91,  93,  94,  96,  97,  98,  99, 101, 102,
     103, 104, 106, 107, 108, 109, 110, 112, 113, 114, 115, 116, 117, 118,
     119, 120, 121, 122, 123, 124, 125, 126, 128, 128, 129, 130, 131, 132,
     133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 144, 145,
     146, 147, 148, 149, 150, 150, 151, 152, 153, 154, 155, 155, 156, 157,
     158, 159, 160, 160, 161, 162, 163, 163, 164, 165, 166, 167, 167, 168,
     169, 170, 170, 171, 172, 173, 173, 174, 175, 176, 176, 177, 178, 178,
     179, 180, 181, 181, 182, 183, 183, 184, 185, 185, 186, 187, 187, 188,
     189, 189, 190, 191, 192, 192, 193, 193, 194, 195, 195, 196, 197, 197,
     198, 199, 199, 200, 201, 201, 202, 203, 203, 204, 204, 205, 206, 206,
     207, 208, 208, 209, 209, 210, 211, 211, 212, 212, 213, 214, 214, 215,
     215, 216, 217, 217, 218, 218, 219, 219, 220, 221, 221, 222, 222, 223,
     224, 224, 225, 225, 226, 226, 227, 227, 228, 229, 229, 230, 230, 231,
     231, 232, 232, 233, 234, 234, 235, 235, 236, 236, 237, 237, 238, 238,
     239, 240, 240, 241, 241, 242, 242, 243, 243, 244, 244, 245, 245, 246,
     246, 247, 247, 248, 248, 249, 249, 250, 250, 251, 251, 252, 252, 253,
     253, 254, 254, 255
    };

    unsigned long xn;

    if (x >= 0x10000)
        if (x >= 0x1000000)
            if (x >= 0x10000000)
                if (x >= 0x40000000) {
                    if (x >= 65535UL*65535UL)
                        return 65535;
                    xn = sqq_table[x>>24] << 8;
                } else
                    xn = sqq_table[x>>22] << 7;
            else
                if (x >= 0x4000000)
                    xn = sqq_table[x>>20] << 6;
                else
                    xn = sqq_table[x>>18] << 5;
        else {
            if (x >= 0x100000)
                if (x >= 0x400000)
                    xn = sqq_table[x>>16] << 4;
                else
                    xn = sqq_table[x>>14] << 3;
            else
                if (x >= 0x40000)
                    xn = sqq_table[x>>12] << 2;
                else
                    xn = sqq_table[x>>10] << 1;

            goto nr1;
        }
    else
        if (x >= 0x100) {
            if (x >= 0x1000)
                if (x >= 0x4000)
                    xn = (sqq_table[x>>8] >> 0) + 1;
                else
                    xn = (sqq_table[x>>6] >> 1) + 1;
            else
                if (x >= 0x400)
                    xn = (sqq_table[x>>4] >> 2) + 1;
                else
                    xn = (sqq_table[x>>2] >> 3) + 1;

            goto adj;
        } else
            return sqq_table[x] >> 4;

/* Run two iterations of the standard convergence formula */

    xn = (xn + 1 + x / xn) / 2;
nr1:
    xn = (xn + 1 + x / xn) / 2;
adj:

    if (xn * xn > x) /* Correct rounding if necessary */
       xn--;

    return xn;
}
#elif 1

#endif

static inline int32_t sqr32(int32_t x)
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
        grad_r->a4 = sqr32(dr) << 2;
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
        grad_r->a4 = (sqr32(dr) - sqr32(dx) - sqr32(dy)) << 2;
        /* b(xp, yp) = xp * bpx + yp * bpy + bc */
        grad_r->bpx = dx << 1;
        grad_r->bpy = dy << 1;
        grad_r->bc = (grad_r->r0 * dr - grad_r->x0 * dx - grad_r->y0 * dy) << 1;
    }
#if 0
    if (grad_r->a4 == 0) {
    	ESP_LOGI("lv_gradient_radial_setup", "Degenerate case");
    }
	if(!(grad_r->bpx || grad_r->bpy)) {
    	ESP_LOGI("lv_gradient_radial_setup", "Concentric");
	}
#endif
    grad_r->inv_a4 = grad_r->a4 != 0 ? (1 << (13 + 16)) / grad_r->a4 : 0;
    /* check for possible clipping */
    if (dsc->extend == LV_GRAD_EXTEND_PAD &&	/* if extend mode is 'pad', then we can clip to the end circle's bounding box, if the start circle is within the end circle */
    	(sqr32(start.x - end.x) + sqr32(start.y - end.y) < sqr32(r_end - r_start))) {
    	if (r_end > r_start) {
    		lv_area_set(&grad_r->clip_area, end.x - r_end, end.y - r_end, end.x  + r_end, end.y + r_end);
    	}
    	else {
    		lv_area_set(&grad_r->clip_area, start.x - r_start, start.y - r_start, start.x  + r_start, start.y + r_start);
    	}
//    	ESP_LOGI("lv_gradient_radial_setup", "Clip area: x1 = %ld, y1 = %ld, x2 = %ld, y2 = %ld", grad_r->clip_area.x1, grad_r->clip_area.y1, grad_r->clip_area.x2, grad_r->clip_area.y2);
    } else {
		grad_r->clip_area.x1 = -0x7fffffff;
    }
}

void lv_gradient_radial_cleanup(lv_grad_dsc_t * dsc)
{
    lv_free(dsc->state);
}

static inline int32_t extend_w(int32_t w, lv_grad_extend_t extend)
{
	if(w < 0)
		return 0;
	if(extend == LV_GRAD_EXTEND_PAD) {     			/**< Repeat the same color*/
		return LV_MIN(w, 255);
	}
	if(extend == LV_GRAD_EXTEND_REPEAT) {  	/**< Repeat the pattern*/
		return w & 255;
	}
	w &= 511;
	if(w > 255)
		w ^= 511;   /* 511 - w */
	return w;
}

void LV_ATTRIBUTE_FAST_MEM lv_gradient_radial_get_line(lv_grad_dsc_t * dsc, lv_grad_t * grad, int32_t xp, int32_t yp,
                                                          int32_t width, lv_grad_t * result)
{
    lv_grad_radial_state_t * grad_r = (lv_grad_radial_state_t *)dsc->state;
    lv_color_t * buf = result->color_map;
    lv_opa_t * opa = result->opa_map;

    int32_t w;
    int32_t b, db, c, dc;

    /* check for possible clipping */
    if (grad_r->clip_area.x1 != -0x7fffffff) {
		/* fill line with starting color */
        lv_color_t * _buf = buf;
        lv_opa_t * _opa = opa;
        lv_color_t _c = grad->color_map[255];
        lv_opa_t _o = grad->opa_map[255]; //LV_OPA_0;	//
        int32_t _w = width;
		for(; _w > 0; _w--) {
			*_buf++ = _c;
			*_opa++ = _o;
		}
		/* is this line fully outside the clip area? */
    	if (yp < grad_r->clip_area.y1 ||
    		yp >= grad_r->clip_area.y2 ||
			xp >= grad_r->clip_area.x2 ||
			xp + width < grad_r->clip_area.x1) {
    		return;
    	} else {
    		int32_t _x1 = LV_MAX(xp, grad_r->clip_area.x1);
    		int32_t _x2 = LV_MIN(xp + width, grad_r->clip_area.x2);
    		buf += _x1 - xp;
    		opa += _x1 - xp;
    		xp = _x1;
    		width = _x2 - _x1;
    	}
    }

    /*
        ((r1-r0)^2 - (x1-x0)^2 - (y1-y0)^2) * w^2 + 2*((xp-x0)*(x1-x0) + (yp-y0)*(y1-y0)) * w + (-(xp-x0)^2 - (yp-y0)^) = 0
        w = (-b(xp, yp) + sqrt(sqr(b(xp, yp)) - 4 * a * c(xp, yp))) / (2 * a), where

            b(xp, yp) = xp * bpx + yp * bpy + bc
            c(xp, yp) = xp * (xp - cx) + yp * (yp - cy) + cc
     */
	b = xp * grad_r->bpx + yp * grad_r->bpy + grad_r->bc;
    c = sqr32(grad_r->r0) - sqr32(xp - grad_r->x0) - sqr32(yp - grad_r->y0);
    db = grad_r->dx << 1;
    dc = ((xp - grad_r->x0) << 1) + 1;

    if(grad_r->a4 == 0) {   /* not a quadratic equation: solve linear equation: w = -c/b */
		for(; width > 0; width--) {
			w = extend_w(b == 0 ? 0 : -(c << 8) / b, dsc->extend);
			*buf++ = grad->color_map[w];
			*opa++ = grad->opa_map[w];
            b += db;
            c -= dc;
            dc += 2;
	   }
    }
    else {                  /* solve quadratical equation */
		if(grad_r->bpx || grad_r->bpy) {	/* general case (circles are not concentric): w = (-b + sqrt(b^2 - 4ac))/2a (we only need the more positive root)*/
			int32_t a4 = grad_r->a4 >> 4;
			for(; width > 0; width--) {
				int32_t det = sqr32(b >> 4) - (a4 * (c >> 4));     /* b^2 shifted down by 2*4=8, 4ac shifted down by 8 */
				/* check determinant: if negative, then there is no solution */
				w = det < 0 ? 0 : extend_w(((fast_sqrt32(det) - (b >> 4)) * grad_r->inv_a4) >>	16, dsc->extend);        /* square root shifted down by 4 (includes *256 to set output range) */
				*buf++ = grad->color_map[w];
				*opa++ = grad->opa_map[w];
				b += db;
				c -= dc;
				dc += 2;
			}
    	}
		else {  /* special case: concentric circles: w = (sqrt((xp-x0)^2 + (yx-y0)^2)-r0)/(r1-r0) */
	        c = sqr32(xp - grad_r->x0) + sqr32(yp - grad_r->y0);
			for(; width > 0; width--) {
				w = extend_w((((fast_sqrt32(c) - grad_r->r0)) * grad_r->inv_dr) >> 16, dsc->extend);
				*buf++ = grad->color_map[w];
				*opa++ = grad->opa_map[w];
				c += dc;
				dc += 2;
			}
		}
	}
}

#endif /* LV_USE_DRAW_SW_COMPLEX_GRADIENTS */

#endif /*LV_USE_DRAW_SW*/
