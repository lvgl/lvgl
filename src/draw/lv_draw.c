/**
 * @file lv_draw.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw.h"

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
}

void lv_draw_backend_init(lv_draw_backend_t * backend)
{
    lv_memset_00(backend, sizeof(lv_draw_backend_t));
}

void lv_draw_backend_add(lv_draw_backend_t * backend)
{
    backend->base = backend_head;
    backend_head = backend;
}

const lv_draw_backend_t * lv_draw_backend_get(void)
{
    return backend_head;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
