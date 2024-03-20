/**
 * @file lv_draw_sw_gradient.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_sw_gradient.h"
#include "../../misc/lv_gc.h"
#include "../../misc/lv_types.h"

/*********************
 *      DEFINES
 *********************/
#if _DITHER_GRADIENT
    #define GRAD_CM(r,g,b) LV_COLOR_MAKE32(r,g,b)
    #define GRAD_CONV(t, x) t.full = lv_color_to32(x)
#else
    #define GRAD_CM(r,g,b) LV_COLOR_MAKE(r,g,b)
    #define GRAD_CONV(t, x) t = x
#endif

#undef ALIGN
#if defined(LV_ARCH_64)
    #define ALIGN(X)    (((X) + 7) & ~7)
#else
    #define ALIGN(X)    (((X) + 3) & ~3)
#endif

#if LV_GRAD_CACHE_DEF_SIZE != 0 && LV_GRAD_CACHE_DEF_SIZE < 256
    #error "LV_GRAD_CACHE_DEF_SIZE is too small"
#endif

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_grad_t * next_in_cache(lv_grad_t * item);

typedef lv_res_t (*op_cache_t)(lv_grad_t * c, void * ctx);
static lv_res_t iterate_cache(op_cache_t func, void * ctx, lv_grad_t ** out);
static size_t get_cache_item_size(lv_grad_t * c);
static lv_grad_t * allocate_item(const lv_grad_dsc_t * g, lv_coord_t w, lv_coord_t h);
static lv_res_t find_oldest_item_life(lv_grad_t * c, void * ctx);
static lv_res_t kill_oldest_item(lv_grad_t * c, void * ctx);
static lv_res_t find_item(lv_grad_t * c, void * ctx);
static void free_item(lv_grad_t * c);
static  uint32_t compute_key(const lv_grad_dsc_t * g, lv_coord_t w, lv_coord_t h);

/**********************
 *   STATIC VARIABLE
 **********************/
static size_t    grad_cache_size = 0;
static uint8_t * grad_cache_end = 0;

/**********************
 *   STATIC FUNCTIONS
 **********************/
union void_cast {
    const void * ptr;
    const uint32_t value;
};

static uint32_t compute_key(const lv_grad_dsc_t * g, lv_coord_t size, lv_coord_t w)
{
    union void_cast v;
    v.ptr = g;
    return (v.value ^ size ^ (w >> 1)); /*Yes, this is correct, it's like a hash that changes if the width changes*/
}

static size_t get_cache_item_size(lv_grad_t * c)
{
    size_t s = ALIGN(sizeof(*c)) + ALIGN(c->alloc_size * sizeof(lv_color_t));
#if _DITHER_GRADIENT
    s += ALIGN(c->size * sizeof(lv_color32_t));
#if LV_DITHER_ERROR_DIFFUSION == 1
    s += ALIGN(c->w * sizeof(lv_scolor24_t));
#endif
#endif
    return s;
}

static lv_grad_t * next_in_cache(lv_grad_t * item)
{
    if(grad_cache_size == 0) return NULL;

    if(item == NULL)
        return (lv_grad_t *)LV_GC_ROOT(_lv_grad_cache_mem);

    size_t s = get_cache_item_size(item);
    /*Compute the size for this cache item*/
    if((uint8_t *)item + s >= grad_cache_end) return NULL;
    else return (lv_grad_t *)((uint8_t *)item + s);
}

static lv_res_t iterate_cache(op_cache_t func, void * ctx, lv_grad_t ** out)
{
    lv_grad_t * first = next_in_cache(NULL);
    while(first != NULL && first->life) {
        if((*func)(first, ctx) == LV_RES_OK) {
            if(out != NULL) *out = first;
            return LV_RES_OK;
        }
        first = next_in_cache(first);
    }
    return LV_RES_INV;
}

static lv_res_t find_oldest_item_life(lv_grad_t * c, void * ctx)
{
    uint32_t * min_life = (uint32_t *)ctx;
    if(c->life < *min_life) *min_life = c->life;
    return LV_RES_INV;
}

static void free_item(lv_grad_t * c)
{
    size_t size = get_cache_item_size(c);
    size_t next_items_size = (size_t)(grad_cache_end - (uint8_t *)c) - size;
    grad_cache_end -= size;
    if(next_items_size) {
        uint8_t * old = (uint8_t *)c;
        lv_memcpy(c, ((uint8_t *)c) + size, next_items_size);
        /* Then need to fix all internal pointers too */
        while((uint8_t *)c != grad_cache_end) {
            c->map = (lv_color_t *)(((uint8_t *)c->map) - size);
#if _DITHER_GRADIENT
            c->hmap = (lv_color32_t *)(((uint8_t *)c->hmap) - size);
#if LV_DITHER_ERROR_DIFFUSION == 1
            c->error_acc = (lv_scolor24_t *)(((uint8_t *)c->error_acc) - size);
#endif
#endif
            c = (lv_grad_t *)(((uint8_t *)c) + get_cache_item_size(c));
        }
        lv_memset_00(old + next_items_size, size);
    }
}

static lv_res_t kill_oldest_item(lv_grad_t * c, void * ctx)
{
    uint32_t * min_life = (uint32_t *)ctx;
    if(c->life == *min_life) {
        /*Found, let's kill it*/
        free_item(c);
        return LV_RES_OK;
    }
    return LV_RES_INV;
}

static lv_res_t find_item(lv_grad_t * c, void * ctx)
{
    uint32_t * k = (uint32_t *)ctx;
    if(c->key == *k) return LV_RES_OK;
    return LV_RES_INV;
}

static lv_grad_t * allocate_item(const lv_grad_dsc_t * g, lv_coord_t w, lv_coord_t h)
{
    lv_coord_t size = g->dir == LV_GRAD_DIR_HOR ? w : h;
    lv_coord_t map_size = LV_MAX(w, h); /* The map is being used horizontally (width) unless
                                           no dithering is selected where it's used vertically */

    size_t req_size = ALIGN(sizeof(lv_grad_t)) + ALIGN(map_size * sizeof(lv_color_t));
#if _DITHER_GRADIENT
    req_size += ALIGN(size * sizeof(lv_color32_t));
#if LV_DITHER_ERROR_DIFFUSION == 1
    req_size += ALIGN(w * sizeof(lv_scolor24_t));
#endif
#endif

    size_t act_size = (size_t)(grad_cache_end - LV_GC_ROOT(_lv_grad_cache_mem));
    lv_grad_t * item = NULL;
    if(req_size + act_size < grad_cache_size) {
        item = (lv_grad_t *)grad_cache_end;
        item->not_cached = 0;
    }
    else {
        /*Need to evict items from cache until we find enough space to allocate this one */
        if(req_size <= grad_cache_size) {
            while(act_size + req_size > grad_cache_size) {
                uint32_t oldest_life = UINT32_MAX;
                iterate_cache(&find_oldest_item_life, &oldest_life, NULL);
                iterate_cache(&kill_oldest_item, &oldest_life, NULL);
                act_size = (size_t)(grad_cache_end - LV_GC_ROOT(_lv_grad_cache_mem));
            }
            item = (lv_grad_t *)grad_cache_end;
            item->not_cached = 0;
        }
        else {
            /*The cache is too small. Allocate the item manually and free it later.*/
            item = lv_mem_alloc(req_size);
            LV_ASSERT_MALLOC(item);
            if(item == NULL) return NULL;
            item->not_cached = 1;
        }
    }

    item->key = compute_key(g, size, w);
    item->life = 1;
    item->filled = 0;
    item->alloc_size = map_size;
    item->size = size;
    if(item->not_cached) {
        uint8_t * p = (uint8_t *)item;
        item->map = (lv_color_t *)(p + ALIGN(sizeof(*item)));
#if _DITHER_GRADIENT
        item->hmap = (lv_color32_t *)(p + ALIGN(sizeof(*item)) + ALIGN(map_size * sizeof(lv_color_t)));
#if LV_DITHER_ERROR_DIFFUSION == 1
        item->error_acc = (lv_scolor24_t *)(p + ALIGN(sizeof(*item)) + ALIGN(size * sizeof(lv_grad_color_t)) +
                                            ALIGN(map_size * sizeof(lv_color_t)));
        item->w = w;
#endif
#endif
    }
    else {
        item->map = (lv_color_t *)(grad_cache_end + ALIGN(sizeof(*item)));
#if _DITHER_GRADIENT
        item->hmap = (lv_color32_t *)(grad_cache_end + ALIGN(sizeof(*item)) + ALIGN(map_size * sizeof(lv_color_t)));
#if LV_DITHER_ERROR_DIFFUSION == 1
        item->error_acc = (lv_scolor24_t *)(grad_cache_end + ALIGN(sizeof(*item)) + ALIGN(size * sizeof(lv_grad_color_t)) +
                                            ALIGN(map_size * sizeof(lv_color_t)));
        item->w = w;
#endif
#endif
        grad_cache_end += req_size;
    }
    return item;
}

/**********************
 *     FUNCTIONS
 **********************/
void lv_gradient_free_cache(void)
{
    lv_mem_free(LV_GC_ROOT(_lv_grad_cache_mem));
    LV_GC_ROOT(_lv_grad_cache_mem) = grad_cache_end = NULL;
    grad_cache_size = 0;
}

void lv_gradient_set_cache_size(size_t max_bytes)
{
    lv_mem_free(LV_GC_ROOT(_lv_grad_cache_mem));
    grad_cache_end = LV_GC_ROOT(_lv_grad_cache_mem) = lv_mem_alloc(max_bytes);
    LV_ASSERT_MALLOC(LV_GC_ROOT(_lv_grad_cache_mem));
    lv_memset_00(LV_GC_ROOT(_lv_grad_cache_mem), max_bytes);
    grad_cache_size = max_bytes;
}

lv_grad_t * lv_gradient_get(const lv_grad_dsc_t * g, lv_coord_t w, lv_coord_t h)
{
    /* No gradient, no cache */
    if(g->dir == LV_GRAD_DIR_NONE) return NULL;

    /* Step 0: Check if the cache exist (else create it) */
    static bool inited = false;
    if(!inited) {
        lv_gradient_set_cache_size(LV_GRAD_CACHE_DEF_SIZE);
        inited = true;
    }

    /* Step 1: Search cache for the given key */
    lv_coord_t size = g->dir == LV_GRAD_DIR_HOR ? w : h;
    uint32_t key = compute_key(g, size, w);
    lv_grad_t * item = NULL;
    if(iterate_cache(&find_item, &key, &item) == LV_RES_OK) {
        item->life++; /* Don't forget to bump the counter */
        return item;
    }

    /* Step 2: Need to allocate an item for it */
    item = allocate_item(g, w, h);
    if(item == NULL) {
        LV_LOG_WARN("Faild to allcoate item for teh gradient");
        return item;
    }

    /* Step 3: Fill it with the gradient, as expected */
#if _DITHER_GRADIENT
    for(lv_coord_t i = 0; i < item->size; i++) {
        item->hmap[i] = lv_gradient_calculate(g, item->size, i);
    }
#if LV_DITHER_ERROR_DIFFUSION == 1
    lv_memset_00(item->error_acc, w * sizeof(lv_scolor24_t));
#endif
#else
    for(lv_coord_t i = 0; i < item->size; i++) {
        item->map[i] = lv_gradient_calculate(g, item->size, i);
    }
#endif

    return item;
}

lv_grad_color_t LV_ATTRIBUTE_FAST_MEM lv_gradient_calculate(const lv_grad_dsc_t * dsc, lv_coord_t range,
                                                            lv_coord_t frac)
{
    lv_grad_color_t tmp;
    lv_color32_t one, two;
    /*Clip out-of-bounds first*/
    int32_t min = (dsc->stops[0].frac * range) >> 8;
    if(frac <= min) {
        GRAD_CONV(tmp, dsc->stops[0].color);
        return tmp;
    }

    int32_t max = (dsc->stops[dsc->stops_count - 1].frac * range) >> 8;
    if(frac >= max) {
        GRAD_CONV(tmp, dsc->stops[dsc->stops_count - 1].color);
        return tmp;
    }

    /*Find the 2 closest stop now*/
    int32_t d = 0;
    for(uint8_t i = 1; i < dsc->stops_count; i++) {
        int32_t cur = (dsc->stops[i].frac * range) >> 8;
        if(frac <= cur) {
            one.full = lv_color_to32(dsc->stops[i - 1].color);
            two.full = lv_color_to32(dsc->stops[i].color);
            min = (dsc->stops[i - 1].frac * range) >> 8;
            max = (dsc->stops[i].frac * range) >> 8;
            d = max - min;
            break;
        }
    }

    LV_ASSERT(d != 0);

    /*Then interpolate*/
    frac -= min;
    lv_opa_t mix = (frac * 255) / d;
    lv_opa_t imix = 255 - mix;

    lv_grad_color_t r = GRAD_CM(LV_UDIV255(two.ch.red * mix   + one.ch.red * imix),
                                LV_UDIV255(two.ch.green * mix + one.ch.green * imix),
                                LV_UDIV255(two.ch.blue * mix  + one.ch.blue * imix));
    return r;
}

void lv_gradient_cleanup(lv_grad_t * grad)
{
    if(grad->not_cached) {
        lv_mem_free(grad);
    }
}
