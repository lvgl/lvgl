/**
 * @file lv_draw_gradient.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_gradient.h"


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

#define MAX_WIN_RES     1024 /**TODO: Find a way to get this information: max(horz_res, vert_res)*/

#if _DITHER_GRADIENT
    #if LV_DITHER_ERROR_DIFFUSION == 1
        #define LV_DEFAULT_GRAD_CACHE_SIZE  sizeof(lv_gradient_cache_t) + MAX_WIN_RES * sizeof(lv_grad_color_t) + MAX_WIN_RES * sizeof(lv_color_t) + MAX_WIN_RES * sizeof(lv_scolor24_t)
    #else
        #define LV_DEFAULT_GRAD_CACHE_SIZE  sizeof(lv_gradient_cache_t) + MAX_WIN_RES * sizeof(lv_grad_color_t) + MAX_WIN_RES * sizeof(lv_color_t)
    #endif /* LV_DITHER_ERROR_DIFFUSION */
#else
    #define LV_DEFAULT_GRAD_CACHE_SIZE  sizeof(lv_gradient_cache_t) + MAX_WIN_RES * sizeof(lv_grad_color_t)
#endif /* _DITHER_GRADIENT */

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_gradient_cache_t * next_in_cache(lv_gradient_cache_t * first);

typedef lv_res_t (*op_cache_t)(lv_gradient_cache_t * c, void * ctx);
static lv_res_t iterate_cache(op_cache_t func, void * ctx, lv_gradient_cache_t ** out);
static size_t get_cache_item_size(lv_gradient_cache_t * c);
static lv_gradient_cache_t * allocate_item(const lv_gradient_t * g, lv_coord_t w, lv_coord_t h);
static lv_res_t find_oldest_item_life(lv_gradient_cache_t * c, void * ctx);
static lv_res_t kill_oldest_item(lv_gradient_cache_t * c, void * ctx);
static lv_res_t find_item(lv_gradient_cache_t * c, void * ctx);
static void free_item(lv_gradient_cache_t * c);
static  uint32_t compute_key(const lv_gradient_t * g, lv_coord_t w, lv_coord_t h);


/**********************
 *   STATIC VARIABLE
 **********************/
static uint8_t * grad_cache_mem = 0;
static size_t    grad_cache_size = 0;
static uint8_t * grad_cache_end = 0;

/**********************
 *   STATIC FUNCTIONS
 **********************/
union void_cast {
    const void * ptr;
    const uint32_t value;
};

static uint32_t compute_key(const lv_gradient_t * g, lv_coord_t size, lv_coord_t w)
{
    union void_cast v;
    v.ptr = g;
    return (v.value ^ size ^ (w >> 1)); /*Yes, this is correct, it's like a hash that changes if the width changes*/
}


static size_t get_cache_item_size(lv_gradient_cache_t * c)
{
    size_t s = sizeof(*c) + c->size * sizeof(lv_color_t)
#if _DITHER_GRADIENT
               + c->hmap_size * sizeof(lv_color32_t)
#if LV_DITHER_ERROR_DIFFUSION == 1
               + c->size * sizeof(lv_scolor24_t)
#endif
#endif
               ;
    return s; /* TODO: Should we align this ? */
}

static lv_gradient_cache_t * next_in_cache(lv_gradient_cache_t * first)
{
    if(first == NULL)
        return (lv_gradient_cache_t *)grad_cache_mem;
    if(first == NULL)
        return NULL;

    size_t s = get_cache_item_size(first);
    /*Compute the size for this cache item*/
    if((uint8_t *)first + s > grad_cache_end)
        return NULL;
    return (lv_gradient_cache_t *)((uint8_t *)first + s);
}

static lv_res_t iterate_cache(op_cache_t func, void * ctx, lv_gradient_cache_t ** out)
{
    lv_gradient_cache_t * first = next_in_cache(NULL);
    while(first != NULL) {
        if((*func)(first, ctx) == LV_RES_OK) {
            if(out != NULL) *out = first;
            return LV_RES_OK;
        }
        first = next_in_cache(first);
    }
    return LV_RES_INV;
}

static lv_res_t find_oldest_item_life(lv_gradient_cache_t * c, void * ctx)
{
    uint32_t * min_life = (uint32_t *)ctx;
    if(c->life < *min_life) *min_life = c->life;
    return LV_RES_INV;
}

static void free_item(lv_gradient_cache_t * c)
{
    size_t size = get_cache_item_size(c);
    size_t next_items_size = (size_t)(grad_cache_end - (uint8_t *)c) - size;
    grad_cache_end -= size;
    if(next_items_size) {
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
            c = (lv_gradient_cache_t *)(((uint8_t *)c) + get_cache_item_size(c));
        }
    }
}

static lv_res_t kill_oldest_item(lv_gradient_cache_t * c, void * ctx)
{
    uint32_t * min_life = (uint32_t *)ctx;
    if(c->life == *min_life) {
        /*Found, let's kill it*/
        free_item(c);
        return LV_RES_OK;
    }
    return LV_RES_INV;
}
static lv_res_t find_item(lv_gradient_cache_t * c, void * ctx)
{
    uint32_t * k = (uint32_t *)ctx;
    if(c->key == *k) return LV_RES_OK;
    return LV_RES_INV;
}


static lv_gradient_cache_t * allocate_item(const lv_gradient_t * g, lv_coord_t w, lv_coord_t h)
{
    lv_coord_t size = g->dir == LV_GRAD_DIR_HOR ? w : h;
    size_t req_size = sizeof(lv_gradient_cache_t) + w * sizeof(lv_color_t)
#if _DITHER_GRADIENT
                      + size * sizeof(lv_color32_t)
#if LV_DITHER_ERROR_DIFFUSION == 1
                      + w * sizeof(lv_scolor24_t)
#endif
#endif
                      ;
    size_t act_size = (size_t)(grad_cache_end - grad_cache_mem);
    if(req_size + act_size > grad_cache_size) {
        /*Need to evict items from cache until we find enough space to allocate this one */
        if(req_size > grad_cache_size) {
            LV_LOG_WARN("Gradient cache too small, failed to allocate");
            return NULL; /*No magic here, if the empty cache is still too small*/
        }
        while(act_size + req_size < grad_cache_size) {
            uint32_t oldest_life = UINT32_MAX;
            iterate_cache(&find_oldest_item_life, &oldest_life, NULL);
            iterate_cache(&kill_oldest_item, &oldest_life, NULL);
            act_size = (size_t)(grad_cache_end - grad_cache_mem);
        }
        /*Ok, now we have space to allocate*/
    }
    lv_gradient_cache_t * item = (lv_gradient_cache_t *)grad_cache_end;
    item->key = compute_key(g, size, w);
    item->life = 1;
    item->filled = 0;
    item->size = w;
    item->map = (lv_color_t *)(grad_cache_end + sizeof(*item));
#if _DITHER_GRADIENT
    item->hmap = (lv_color32_t *)(grad_cache_end + sizeof(*item) + w * sizeof(lv_color_t));
    item->hmap_size = size;
#if LV_DITHER_ERROR_DIFFUSION == 1
    item->error_acc = (lv_scolor24_t *)(grad_cache_end + sizeof(*item) + size * sizeof(lv_grad_color_t) + w * sizeof(
                                            lv_color_t));
#endif
#endif
    grad_cache_end += req_size;
    return item;
}


/**********************
 *     FUNCTIONS
 **********************/
void lv_grad_free_cache()
{
    lv_mem_free(grad_cache_mem);
    grad_cache_mem = grad_cache_end = NULL;
    grad_cache_size = 0;
}

void lv_grad_set_cache_size(size_t max_bytes)
{
    lv_mem_free(grad_cache_mem);
    grad_cache_end = grad_cache_mem = lv_mem_alloc(max_bytes);
    LV_ASSERT_MALLOC(grad_cache_mem);
    grad_cache_size = max_bytes;
}

lv_gradient_cache_t * lv_grad_get_from_cache(const lv_gradient_t * g, lv_coord_t w, lv_coord_t h)
{
    /* No gradient, no cache */
    if(g->dir == LV_GRAD_DIR_NONE) return NULL;

    /* Step 0: Check if the cache exist (else create it) */
    if(!grad_cache_size) {
        lv_grad_set_cache_size(LV_DEFAULT_GRAD_CACHE_SIZE);
    }

    /* Step 1: Search cache for the given key */
    lv_coord_t size = g->dir == LV_GRAD_DIR_HOR ? w : h;
    uint32_t key = compute_key(g, size, w);
    lv_gradient_cache_t * item = NULL;
    if(iterate_cache(&find_item, &key, &item) == LV_RES_OK) {
        item->life++; /* Don't forget to bump the counter */
        return item;
    }

    /* Step 2: Need to allocate an item for it */
    item = allocate_item(g, w, h);
    if(item == NULL) return item;

    /* Step 3: Fill it with the gradient, as expected */
#if _DITHER_GRADIENT
    for(lv_coord_t i = 0; i < item->hmap_size; i++) {
        item->hmap[i] = lv_grad_get(g, item->hmap_size, i);
    }
#if LV_DITHER_ERROR_DIFFUSION == 1
    lv_memset_00(item->error_acc, w * sizeof(lv_scolor24_t));
#endif
#else
    for(lv_coord_t i = 0; i < item->size; i++) {
        item->map[i] = lv_grad_get(g, item->size, i);
    }
#endif

    return item;
}


void lv_grad_pop_from_cache(const lv_gradient_t * g, lv_coord_t w, lv_coord_t h)
{
    lv_coord_t size = g->dir == LV_GRAD_DIR_HOR ? w : h;
    uint32_t key = compute_key(g, size, w);
    lv_gradient_cache_t * item = NULL;
    if(iterate_cache(&find_item, &key, &item) == LV_RES_OK) {
        free_item(item);
    }
}



LV_ATTRIBUTE_FAST_MEM lv_grad_color_t lv_grad_get(const lv_gradient_t * dsc, lv_coord_t range, lv_coord_t frac)
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

    /*Then interpolate*/
    frac -= min;
    lv_opa_t mix = (frac * 255) / d;
    lv_opa_t imix = 255 - mix;

    lv_grad_color_t r = GRAD_CM(LV_UDIV255(two.ch.red * mix   + one.ch.red * imix),
                                LV_UDIV255(two.ch.green * mix + one.ch.green * imix),
                                LV_UDIV255(two.ch.blue * mix  + one.ch.blue * imix));
    return r;
}
