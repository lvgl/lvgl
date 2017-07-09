/**
 * @file lv_vdb.h
 * 
 */

#ifndef LV_VDB_H
#define LV_VDB_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"

#if LV_VDB_SIZE != 0

#include "misc/gfx/color.h"
#include "misc/gfx/area.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct
{
    area_t area;
    color_t buf[LV_VDB_SIZE];
}lv_vdb_t;



/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Get the vdb variable
 * @return pointer to the vdb variable
 */
lv_vdb_t * lv_vdb_get(void);

/**
 * Flush the content of the vdb
 */
void lv_vdb_flush(void);

/**********************
 *      MACROS
 **********************/

#endif /*LV_VDB_SIZE != 0*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_VDB_H*/
