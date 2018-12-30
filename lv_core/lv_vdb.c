/**
 * @file lv_vdb.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_vdb.h"
#if LV_VDB_SIZE != 0

#include "../lv_hal/lv_hal_disp.h"
#include "../lv_misc/lv_log.h"
#include <stddef.h>

/*********************
 *      DEFINES
 *********************/
#ifndef LV_ATTRIBUTE_FLUSH_READY
#define LV_ATTRIBUTE_FLUSH_READY
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/*Simple VDB*/
#if LV_VDB_DOUBLE == 0
#  if LV_VDB_ADR == 0
/*If the buffer address is not specified  simply allocate it*/
static uint8_t vdb_buf[LV_VDB_SIZE_IN_BYTES];
static lv_vdb_t vdb = {.buf = (lv_color_t *)vdb_buf};
#  else     /*LV_VDB_ADR != 0*/
/*If the buffer address is specified use that address*/
static lv_vdb_t vdb = {.buf = (lv_color_t *)LV_VDB_ADR};
#  endif

/*LV_VDB_DOUBLE != 0*/
#else
/*Double VDB*/
static uint8_t vdb_active = 0;
#  if LV_VDB_ADR == 0
/*If the buffer address is not specified  simply allocate it*/
static uint8_t vdb_buf1[LV_VDB_SIZE_IN_BYTES];
static uint8_t vdb_buf2[LV_VDB_SIZE_IN_BYTES];
static lv_vdb_t vdb[2] = {{.buf = (lv_color_t *) vdb_buf1}, {.buf = (lv_color_t *) vdb_buf2}};
#  else /*LV_VDB_ADR != 0*/
/*If the buffer address is specified use that address*/
static lv_vdb_t vdb[2] = {{.buf = (lv_color_t *)LV_VDB_ADR}, {.buf = (lv_color_t *)LV_VDB2_ADR}};
#  endif
#endif

static volatile bool vdb_flushing = false;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Get the 'vdb' variable or allocate one in LV_VDB_DOUBLE mode
 * @return pointer to a 'vdb' variable
 */
lv_vdb_t * lv_vdb_get(void)
{
#if LV_VDB_DOUBLE == 0
    /* Wait until VDB is flushing.
     * (Until this user calls of 'lv_flush_ready()' in the display drivers's flush function*/
    while(vdb_flushing);

    return &vdb;
#else
    /*If already there is an active do nothing*/
    return &vdb[vdb_active];
#endif
}

/**
 * Flush the content of the VDB
 */
void lv_vdb_flush(void)
{
    lv_vdb_t * vdb_act = lv_vdb_get();
    if(!vdb_act) {
        LV_LOG_WARN("Invalid VDB pointer");
        return;
    }

    /*Don't start a new flush while the previous is not finished*/
#if LV_VDB_DOUBLE
    while(vdb_flushing);
#endif  /*LV_VDB_DOUBLE*/

    vdb_flushing = true;

    /*Flush the rendered content to the display*/
    lv_disp_flush(vdb_act->area.x1, vdb_act->area.y1, vdb_act->area.x2, vdb_act->area.y2, vdb_act->buf);


#if LV_VDB_DOUBLE
    /*Make the other VDB active. The content of the current will be kept until the next flush*/
    vdb_active++;
    vdb_active &= 0x1;

    /*If the screen is transparent initialize it when the new VDB is selected*/
#  if LV_COLOR_SCREEN_TRANSP
        memset(vdb[vdb_active].buf, 0x00, LV_VDB_SIZE_IN_BYTES);
#  endif  /*LV_COLOR_SCREEN_TRANSP*/

    /* With true double buffering the flushing should be only the address change of the current frame buffer
     * Wait until the address change is ready and copy the active content to the other frame buffer (new active VDB)
     * The changes will be written to the new VDB.*/
#  if LV_VDB_TRUE_DOUBLE_BUFFERED
        while(vdb_flushing);
        memcpy(vdb[vdb_active].buf, vdb[(vdb_active + 1) & 0x1].buf, LV_VDB_SIZE_IN_BYTES);
#  endif

#endif  /*#if LV_VDB_DOUBLE*/

}

/**
 * Set the address of VDB buffer(s) manually. To use this set `LV_VDB_ADR` (and `LV_VDB2_ADR`) to `LV_VDB_ADR_INV` in `lv_conf.h`.
 * It should be called before `lv_init()`. The size of the buffer should be: `LV_VDB_SIZE_IN_BYTES`
 * @param buf1 address of the VDB.
 * @param buf2 address of the second buffer. `NULL` if `LV_VDB_DOUBLE  0`
 */
void lv_vdb_set_adr(void * buf1, void * buf2)
{
#if LV_VDB_DOUBLE == 0
    (void) buf2;   /*unused*/
    vdb.buf = buf1;
#else
    vdb[0].buf = buf1;
    vdb[1].buf = buf2;
#endif
}

/**
 * Call in the display driver's  'disp_flush' function when the flushing is finished
 */
LV_ATTRIBUTE_FLUSH_READY void lv_flush_ready(void)
{
    vdb_flushing = false;

    /*If the screen is transparent initialize it when the flushing is ready*/
#if LV_VDB_DOUBLE == 0 && LV_COLOR_SCREEN_TRANSP
    memset(vdb_buf, 0x00, LV_VDB_SIZE_IN_BYTES);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#else

/**
 * Just for compatibility
 */
void lv_flush_ready(void)
{
    /*Do nothing. It is used only for VDB*/
}
#endif
