/**
 * @file 2d.h
 * 
 */

#ifndef _2D_H
#define _2D_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef LV_CORD_TYPE cord_t;

typedef struct
{
    cord_t x;
    cord_t y;
}point_t;

typedef struct
{
    cord_t x1;
    cord_t y1;        
    cord_t x2;
    cord_t y2;        
}area_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

static void inline area_cpy(area_t * dest, const area_t * src)
{
    memcpy(dest, src, sizeof(area_t));
}

static inline cord_t area_get_width(const area_t * area_p)
{
    return area_p->x2 - area_p->x1 + 1;
}

static inline cord_t area_get_height(const area_t * area_p)
{
    return area_p->y2 - area_p->y1 + 1;
}

void area_set(area_t * area_p, cord_t x1, cord_t y1, cord_t x2, cord_t y2);
uint32_t area_get_size(const area_t * area_p);
bool area_union(area_t * res_p, const area_t * a1_p, const area_t * a2_p);
void area_join(area_t * a_res_p, const area_t * a1_p, const area_t * a2_p);
bool area_is_point_on(const area_t * a_p, const point_t * p_p);
bool area_is_on(const area_t * a1_p, const area_t * a2_p);
bool area_is_in(const area_t * a_in, const area_t * a_holder);

/**********************
 *      MACROS
 **********************/

#endif
