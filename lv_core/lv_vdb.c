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

/**********************
 *      TYPEDEFS
 **********************/
enum {
    LV_VDB_STATE_FREE = 0,      /*Not used*/
    LV_VDB_STATE_ACTIVE,        /*Being used to render*/
    LV_VDB_STATE_FLUSH,         /*Flushing pixels from it*/
};
typedef uint8_t lv_vdb_state_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

#if LV_VDB_DOUBLE == 0
/*Simple VDB*/
static volatile lv_vdb_state_t vdb_state = LV_VDB_STATE_ACTIVE;
#  if LV_VDB_ADR == 0
/*If the buffer address is not specified  simply allocate it*/
static uint8_t vdb_buf[LV_VDB_SIZE_IN_BYTES];
static lv_vdb_t vdb = {.buf = (lv_color_t *)vdb_buf};
#  else     /*LV_VDB_ADR != 0*/
/*If the buffer address is specified use that address*/
static lv_vdb_t vdb = {.buf = (lv_color_t *)LV_VDB_ADR};
#  endif
#else       /*LV_VDB_DOUBLE != 0*/
/*Double VDB*/
static volatile lv_vdb_state_t vdb_state[2] = {LV_VDB_STATE_FREE, LV_VDB_STATE_FREE};
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
    /* Wait until VDB become ACTIVE from FLUSH by the
     * user call of 'lv_flush_ready()' in display drivers's flush function*/
    while(vdb_state != LV_VDB_STATE_ACTIVE);

    if(vdb.buf == (void *)LV_VDB_ADR_INV) {
        LV_LOG_ERROR("VDB address is invalid. Use `lv_vdb_set_adr` to set a valid address or use LV_VDB_ADR = 0 in lv_conf.h");
        return NULL;
    }
    return &vdb;
#else
    /*If already there is an active do nothing*/
    if(vdb_state[0] == LV_VDB_STATE_ACTIVE) return &vdb[0];
    if(vdb_state[1] == LV_VDB_STATE_ACTIVE) return &vdb[1];

    /*Try to allocate a free VDB*/
    if(vdb_state[0] == LV_VDB_STATE_FREE) {
        vdb_state[0] = LV_VDB_STATE_ACTIVE;
        return &vdb[0];
    }

    if(vdb_state[1] == LV_VDB_STATE_FREE) {
        vdb_state[1] = LV_VDB_STATE_ACTIVE;
        return &vdb[1];
    }

    return NULL;   /*There wasn't free VDB (never happen)*/
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
#if LV_VDB_DOUBLE == 0
    vdb_state = LV_VDB_STATE_FLUSH;     /*User call to 'lv_flush_ready()' will set to ACTIVE 'disp_flush'*/
#else
    /* Wait the pending flush before starting this one
     * (Don't forget: 'lv_flush_ready()' has to be called when flushing is ready)*/
    while(vdb_state[0] == LV_VDB_STATE_FLUSH || vdb_state[1] == LV_VDB_STATE_FLUSH);

    /*Turn the active VDB to flushing*/
    if(vdb_state[0] == LV_VDB_STATE_ACTIVE) vdb_state[0] = LV_VDB_STATE_FLUSH;
    if(vdb_state[1] == LV_VDB_STATE_ACTIVE) vdb_state[1] = LV_VDB_STATE_FLUSH;
#endif

    /*Flush the rendered content to the display*/
    lv_disp_flush(vdb_act->area.x1, vdb_act->area.y1, vdb_act->area.x2, vdb_act->area.y2, vdb_act->buf);

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
void lv_flush_ready(void)
{
#if LV_VDB_DOUBLE == 0
    vdb_state = LV_VDB_STATE_ACTIVE;

#if LV_COLOR_SCREEN_TRANSP
    memset(vdb_buf, 0x00, LV_VDB_SIZE_IN_BYTES);
#endif

#else
    if(vdb_state[0] == LV_VDB_STATE_FLUSH) {
#if LV_COLOR_SCREEN_TRANSP
        memset(vdb_buf[0], 0x00, LV_VDB_SIZE_IN_BYTES);
#endif
        vdb_state[0] = LV_VDB_STATE_FREE;
    }
    if(vdb_state[1] == LV_VDB_STATE_FLUSH) {
#if LV_COLOR_SCREEN_TRANSP
        memset(vdb_buf[1], 0x00, LV_VDB_SIZE_IN_BYTES);
#endif
        vdb_state[1] = LV_VDB_STATE_FREE;
    }
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
