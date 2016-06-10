/**
 * @file lv_vdb.h
 * 
 */

#ifndef LV_VDB_H
#define LV_VDB_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"

#if LV_VDB_SIZE != 0

#include "misc/others/color.h"
#include <lvgl/lv_misc/2d.h>
#include "../lv_misc/font.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct
{
    area_t vdb_area;
    color_t buf[LV_VDB_SIZE];
}lv_vdb_t;



/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_vdb_t * lv_vdb_get(void);
void lv_vdb_flush(void);
/**********************
 *      MACROS
 **********************/

#endif

#endif
