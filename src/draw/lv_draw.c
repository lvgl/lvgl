/**
 * @file lv_draw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"
#include "sw/lv_draw_sw.h"

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
const lv_draw_backend_class_t lv_draw_backend = {
    .constructor_cb = lv_draw_backend_constructor,
    .destructor_cb = lv_draw_backend_destructor,
    .instance_size = sizeof(lv_draw_backend_t),
    .base_class = NULL
};

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_draw_backend_t * backend_head;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_init(void)
{
    backend_head = NULL;
    lv_draw_sw_init();

#if LV_USE_GPU_STM32_DMA2D == 0
    lv_gpu_stm32_dma2d_init();
#endif
}

void lv_draw_backend_init(lv_draw_backend_t * backend)
{
    lv_memset_00(backend, sizeof(lv_draw_backend_t));
}

const lv_draw_backend_t * lv_draw_backend_get(void)
{
    return backend_head;
}


lv_draw_backend_t * lv_draw_backend_create(const lv_draw_backend_class_t * class_p)
{
    uint32_t s = get_instance_size(class_p);
    lv_draw_backend_t * backend = lv_mem_alloc(s);
    if(backend == NULL) return NULL;
    lv_memset_00(backend, s);
    backend->class_p = class_p;

    lv_draw_backend_construct(backend);

    return backend;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_draw_backend_construct(lv_draw_backend_t * backend)
{
    const lv_obj_class_t * original_class_p = backend->class_p;

    if(backend->class_p->base_class) {
        /*Don't let the descendant methods run during constructing the ancestor type*/
        backend->class_p = backend->class_p->base_class;

        /*Construct the base first*/
        lv_draw_backend_construct(backend);
    }

    /*Restore the original class*/
    backend->class_p = original_class_p;

    if(backend->class_p->constructor_cb) backend->class_p->constructor_cb(backend->class_p, backend);
}


void lv_draw_backend_destruct(lv_draw_backend_t * backend)
{
    if(backend->class_p->destructor_cb) backend->class_p->destructor_cb(backend->class_p, backend);

    if(backend->class_p->base_class) {
        /*Don't let the descendant methods run during destructing the ancestor type*/
        backend->class_p = backend->class_p->base_class;

        /*Call the base class's destructor too*/
        lv_draw_backend_destruct(backend);
    }
}

static uint32_t get_instance_size(const lv_draw_backend_class_t * class_p)
{
    /*Find a base in which instance size is set*/
    const lv_obj_class_t * base = class_p;
    while(base && base->instance_size == 0) base = base->base_class;

    if(base == NULL) return 0;  /*Never happens: set at least in `lv_obj` class*/

    return base->instance_size;
}

void lv_draw_backend_constructor(const struct _lv_draw_backend_class_t * class_p, struct _lv_draw_backend_t * backend)
{
    LV_UNUSED(class_p);

}

void lv_draw_backend_destructor(const struct _lv_draw_backend_class_t * class_p, struct _lv_draw_backend_t * backend)
{
    LV_UNUSED(class_p);
}
