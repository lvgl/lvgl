/**
 * @file lv_vdb.c
 * 
 */
#include "lv_conf.h"
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
#if LV_VDB_DOUBLE != 0
typedef enum {
    LV_VDB_STATE_FREE = 0,
    LV_VDB_STATE_ACTIVE,
    LV_VDB_STATE_FLUSH,
} lv_vdb_state_t;
#endif
/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
#if LV_VDB_DOUBLE == 0
static lv_vdb_t vdb;
#else
static lv_vdb_t vdb[2];
static volatile lv_vdb_state_t vdb_state[2] = {LV_VDB_STATE_FREE, LV_VDB_STATE_FREE};
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Get the 'vdb' variable or allocate one in LV_VDB_DOUBLE mode
 * @return pointer to the 'vdb' variable
 */
lv_vdb_t * lv_vdb_get(void)
{
#if LV_VDB_DOUBLE == 0
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

#if LV_VDB_DOUBLE != 0
    /* Wait the pending flush before starting this one
     * (Don't forget: 'lv_vdb_flush_ready' has to be called when flushing is ready)*/
    while(vdb_state[0] == LV_VDB_STATE_FLUSH || vdb_state[1] == LV_VDB_STATE_FLUSH);

    /*Turn the active VDB to flushing*/
    if(vdb_state[0] == LV_VDB_STATE_ACTIVE) vdb_state[0] = LV_VDB_STATE_FLUSH;
    if(vdb_state[1] == LV_VDB_STATE_ACTIVE) vdb_state[1] = LV_VDB_STATE_FLUSH;
#endif

#if LV_ANTIALIAS == 0
	disp_map(vdb_act->area.x1, vdb_act->area.y1, vdb_act->area.x2, vdb_act->area.y2, vdb_act->buf);
#else
	/* Get the average of 2x2 pixels and put the result back to the VDB
	 * The reading goes much faster then the write back
	 * so useful data won't be overwritten
	 * Example:
	 * -----------------------------
	 * in1_buf  |2,2|6,8|      3,7
	 * in2_buf  |4,4|7,7|      1,2
	 *          ---------  ==>
	 * in1_buf  |1,1|1,3|
	 * in2_buf  |1,1|1,3|
	 * */
	cord_t x;
	cord_t y;
	cord_t w = area_get_width(&vdb_act->area);
	color_t * in1_buf = vdb_act->buf;      /*Pointer to the first row*/
    color_t * in2_buf = vdb_act->buf + w;  /*Pointer to the second row*/
    color_t * out_buf = vdb_act->buf;      /*Store the result here*/
	for(y = vdb_act->area.y1; y < vdb_act->area.y2; y += 2) {
        for(x = vdb_act->area.x1; x < vdb_act->area.x2; x += 2) {
        
            /*If the pixels are the same do not calculate the average */
            if(in1_buf->full == (in1_buf + 1)->full &&
               in1_buf->full == in2_buf->full &&
               in1_buf->full == (in2_buf + 1)->full) {
                out_buf->full = in1_buf->full;
            } else {
                /*Get the average of 2x2 red*/
                out_buf->red = (in1_buf->red + (in1_buf + 1)->red +
                                in2_buf->red + (in2_buf+ 1)->red) >> 2;
                /*Get the average of 2x2 green*/
                out_buf->green = (in1_buf->green + (in1_buf + 1)->green +
                                  in2_buf->green + (in2_buf + 1)->green) >> 2;
                /*Get the average of 2x2 blue*/
                out_buf->blue = (in1_buf->blue + (in1_buf + 1)->blue +
                                 in2_buf->blue + (in2_buf + 1)->blue) >> 2;
            }
            
			in1_buf += 2; /*Skip the next pixel because it is already used above*/
            in2_buf += 2;
			out_buf ++;
		}
		/*2 row is ready so go the next 2*/
		in1_buf += w; /*Skip the next row because it is processed from in2_buf*/
        in2_buf += w;
	}

	/* Now the full the VDB is filtered and the result is stored in the first quarter of it
	 * Write out the filtered map to the display*/
	lv_disp_map(vdb_act->area.x1 >> 1, vdb_act->area.y1 >> 1, vdb_act->area.x2 >> 1, vdb_act->area.y2 >> 1, vdb_act->buf);
#endif
}

/**
 * In 'LV_VDB_DOUBLE' mode  has to be called when the 'disp_map'
 * is ready with copying the map to a frame buffer.
 */
void lv_vdb_flush_ready(void)
{
#if LV_VDB_DOUBLE != 0
    if(vdb_state[0] == LV_VDB_STATE_FLUSH)  vdb_state[0] = LV_VDB_STATE_FREE;
    if(vdb_state[1] == LV_VDB_STATE_FLUSH)  vdb_state[1] = LV_VDB_STATE_FREE;
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
