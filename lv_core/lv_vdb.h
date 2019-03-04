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
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../lv_conf.h"
#endif

#if LV_VDB_SIZE != 0

#include "../lv_misc/lv_color.h"
#include "../lv_misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/
/*Can be used in `lv_conf.h` the set an invalid address for the VDB. It should be replaced later by a valid address using `lv_vdb_set_adr()`*/
#define LV_VDB_ADR_INV  8       /*8 is still too small to be valid but it's aligned on 64 bit machines as well*/

#ifndef LV_VDB_PX_BPP
#define LV_VDB_PX_BPP LV_COLOR_SIZE     /* Default is LV_COLOR_SIZE */
#endif


#if LV_VDB_TRUE_DOUBLE_BUFFERED && (LV_VDB_SIZE != LV_HOR_RES * LV_VER_RES || LV_VDB_DOUBLE == 0)
#error "With LV_VDB_TRUE_DOUBLE_BUFFERED: (LV_VDB_SIZE = LV_HOR_RES * LV_VER_RES and LV_VDB_DOUBLE = 1 is required"
#endif


/* The size of VDB in bytes.
 * (LV_VDB_SIZE * LV_VDB_PX_BPP) >> 3): just divide by 8 to convert bits to bytes
 * (((LV_VDB_SIZE * LV_VDB_PX_BPP) & 0x7) ? 1 : 0): add an extra byte to round up.
 *    E.g. if LV_VDB_SIZE = 10 and LV_VDB_PX_BPP = 1 -> 10 bits -> 2 bytes*/
#define LV_VDB_SIZE_IN_BYTES ((LV_VDB_SIZE * LV_VDB_PX_BPP) >> 3) + (((LV_VDB_SIZE * LV_VDB_PX_BPP) & 0x7) ? 1 : 0)

/**********************
 *      TYPEDEFS
 **********************/

typedef struct
{
    lv_area_t area;
    lv_color_t *buf;
} lv_vdb_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Get the 'vdb' variable or allocate one in LV_VDB_DOUBLE mode
 * @return pointer to a 'vdb' variable
 */
lv_vdb_t * lv_vdb_get(void);

/**
 * Flush the content of the vdb
 */
void lv_vdb_flush(void);

/**
 * Set the address of VDB buffer(s) manually. To use this set `LV_VDB_ADR` (and `LV_VDB2_ADR`) to `LV_VDB_ADR_INV` in `lv_conf.h`.
 * It should be called before `lv_init()`. The size of the buffer should be: `LV_VDB_SIZE_IN_BYTES`
 * @param buf1 address of the VDB.
 * @param buf2 address of the second buffer. `NULL` if `LV_VDB_DOUBLE  0`
 */
void lv_vdb_set_adr(void * buf1, void * buf2);

/**
 * Call in the display driver's  'disp_flush' function when the flushing is finished
 */
void lv_flush_ready(void);

/**
 * Get currently active VDB, where the drawing happens. Used with `LV_VDB_DOUBLE  1`
 * @return pointer to the active VDB. If `LV_VDB_DOUBLE  0` give the single VDB
 */
lv_vdb_t * lv_vdb_get_active(void);

/**
 * Get currently inactive VDB, which is being displayed or being flushed. Used with `LV_VDB_DOUBLE  1`
 * @return pointer to the inactive VDB. If `LV_VDB_DOUBLE  0` give the single VDB
 */
lv_vdb_t * lv_vdb_get_inactive(void);

/**
 * Whether the flushing is in progress or not
 * @return true: flushing is in progress; false: flushing ready
 */
bool lv_vdb_is_flushing(void);

/**********************
 *      MACROS
 **********************/

#else /*LV_VDB_SIZE != 0*/

/*Just for compatibility*/
void lv_flush_ready(void);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_VDB_H*/
