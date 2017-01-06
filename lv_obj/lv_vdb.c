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
	disp_area(DISP_ID_ALL, vdb.vdb_area.x1 , vdb.vdb_area.y1, vdb.vdb_area.x2, vdb.vdb_area.y2);
	disp_map(DISP_ID_ALL, vdb.buf);
#else
	color_t row_buf[LV_HOR_RES / LV_DOWNSCALE];
	color_t * row_buf_p;
	cord_t x;
	cord_t y;
	cord_t w = area_get_width(&vdb.vdb_area);
	cord_t i;
	color_t * buf_p = vdb.buf;
	for(y = vdb.vdb_area.y1 >> 1; y <= vdb.vdb_area.y2 >> 1; y ++) {
		i = 0;
		row_buf_p = row_buf;
		for(x = vdb.vdb_area.x1; x < vdb.vdb_area.x2; x += 2, i += 2) {
			row_buf_p->red = (buf_p[i].red +
					          buf_p[i + 1].red +
					          buf_p[i + w].red +
							  buf_p[i + w + 1].red) >> 2;
			row_buf_p->green = (buf_p[i].green +
					            buf_p[i + 1].green +
					            buf_p[i + w].green +
								buf_p[i + w + 1].green) >> 2;
			row_buf_p->blue = (buf_p[i].blue +
					           buf_p[i + 1].blue +
					           buf_p[i + w].blue +
							   buf_p[i + w + 1].blue) >> 2;

			row_buf_p++;
		}
		buf_p += LV_DOWNSCALE * w;

		disp_area(DISP_ID_ALL, vdb.vdb_area.x1 >> 1, y, vdb.vdb_area.x2 >> 1, y);
		disp_map(DISP_ID_ALL, row_buf);
	}
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/



#endif
