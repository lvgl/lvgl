/**
 * @file lv_vdb.c
 * 
 */
#include "lv_conf.h"
#if LV_VDB_SIZE != 0

#include "hal/disp/disp.h"
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

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_vdb_t vdb;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Get the vdb variable
 * @return pointer to the vdb variable
 */
lv_vdb_t * lv_vdb_get(void)
{
    return &vdb;
}

/**
 * Flush the content of the vdb
 */
void lv_vdb_flush(void)
{
#if LV_ANTIALIAS == 0
	disp_map(vdb.area.x1, vdb.area.y1, vdb.area.x2, vdb.area.y2, vdb.buf);
#else
	/* Get the average of 2x2 pixels and put the result back to the VDB
	 * The reading goes much faster then the write back
	 * so useful data won't be overwritten
	 * Example:
	 * -----------------------------
	 * in1_buf  |2,2|6,8|      3,7
	 * in2_buf  |4,4|7,7|      1,2
	 *           ---------  ==>
	 * in1_buf  |1,1|1,3|
	 * in2_buf  |1,1|1,3|
	 * */
	cord_t x;
	cord_t y;
	cord_t w = area_get_width(&vdb.area);
	color_t * in1_buf = vdb.buf;      /*Pointer to the first row*/
    color_t * in2_buf = vdb.buf + w;  /*Pointer to the second row*/
    color_t * out_buf = vdb.buf;      /*Store the result here*/
	for(y = vdb.area.y1; y < vdb.area.y2; y += 2) {
        for(x = vdb.area.x1; x < vdb.area.x2; x += 2) {
        
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
    disp_map(vdb.area.x1 >> 1, vdb.area.y1 >> 1, vdb.area.x2 >> 1, vdb.area.y2 >> 1, vdb.buf);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
