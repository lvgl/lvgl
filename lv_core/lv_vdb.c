/**
 * @file lv_vdb.c
 * 
 */
#include "../../lv_conf.h"
#if LV_VDB_SIZE != 0

#include "../lv_hal/lv_hal_disp.h"
#include <stddef.h>
#include "lv_vdb.h"

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_VDB_STATE_FREE = 0,
    LV_VDB_STATE_ACTIVE,
    LV_VDB_STATE_FLUSH,
} lv_vdb_state_t;

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
     static lv_color_t vdb_buf[LV_VDB_SIZE];
     static lv_vdb_t vdb = {.buf = vdb_buf};
#  else
     /*If the buffer address is specified use that address*/
     static lv_vdb_t vdb = {.buf = (lv_color_t *)LV_VDB_ADR};
#  endif
#else
   /*Double VDB*/
   static volatile lv_vdb_state_t vdb_state[2] = {LV_VDB_STATE_FREE, LV_VDB_STATE_FREE};
#  if LV_VDB_ADR == 0
   /*If the buffer address is not specified  simply allocate it*/
   static lv_color_t vdb_buf1[LV_VDB_SIZE];
   static lv_color_t vdb_buf2[LV_VDB_SIZE];
   static lv_vdb_t vdb[2] = {{.buf = vdb_buf1}, {.buf = vdb_buf2}};
#  else
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
    if(vdb_act == NULL) return;

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
 * Call in the display driver's  'disp_flush' function when the flushing is finished
 */
void lv_flush_ready(void)
{
#if LV_VDB_DOUBLE == 0
    vdb_state = LV_VDB_STATE_ACTIVE;
#else
    if(vdb_state[0] == LV_VDB_STATE_FLUSH)  vdb_state[0] = LV_VDB_STATE_FREE;
    if(vdb_state[1] == LV_VDB_STATE_FLUSH)  vdb_state[1] = LV_VDB_STATE_FREE;
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
