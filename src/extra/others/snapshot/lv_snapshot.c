/**
 * @file lv_snapshot.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_snapshot.h"
#if LV_USE_SNAPSHOT

#include <stdbool.h>
#include "../../../core/lv_disp.h"
#include "../../../core/lv_refr.h"
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

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/** Get the buffer needed for object snapshot image.
 *
 * @param obj    The object to generate snapshot.
 * @param cf     color format for generated image.
 *
 * @return the buffer size needed in bytes
 */
uint32_t lv_snapshot_buf_size_needed(lv_obj_t * obj, lv_img_cf_t cf)
{
    switch(cf) {
        case LV_IMG_CF_TRUE_COLOR_ALPHA:
        case LV_IMG_CF_ALPHA_1BIT:
        case LV_IMG_CF_ALPHA_2BIT:
        case LV_IMG_CF_ALPHA_4BIT:
        case LV_IMG_CF_ALPHA_8BIT:
            break;
        default:
            return 0;
    }

    lv_obj_update_layout(obj);

    /*Width and height determine snapshot image size.*/
    lv_coord_t w = lv_obj_get_width(obj);
    lv_coord_t h = lv_obj_get_height(obj);
    lv_coord_t ext_size = _lv_obj_get_ext_draw_size(obj);
    w += ext_size * 2;
    h += ext_size * 2;

    uint8_t px_size = lv_img_cf_get_px_size(cf);
    return w * h * ((px_size + 7) >> 3);
}

/** Take snapshot for object with its children, save image info to provided buffer.
 *
 * @param obj    The object to generate snapshot.
 * @param cf     color format for generated image.
 * @param dsc    image descriptor to store the image result.
 * @param buf    the buffer to store image data.
 * @param buff_size provided buffer size in bytes.
 *
 * @return LV_RES_OK on success, LV_RES_INV on error.
 */
lv_res_t lv_snapshot_take_to_buf(lv_obj_t * obj, lv_img_cf_t cf, lv_img_dsc_t * dsc, void * buf, uint32_t buff_size)
{
    LV_ASSERT(dsc);
    LV_ASSERT(buf);

    switch(cf) {
        case LV_IMG_CF_TRUE_COLOR_ALPHA:
        case LV_IMG_CF_ALPHA_1BIT:
        case LV_IMG_CF_ALPHA_2BIT:
        case LV_IMG_CF_ALPHA_4BIT:
        case LV_IMG_CF_ALPHA_8BIT:
            break;
        default:
            return LV_RES_INV;
    }

    if(lv_snapshot_buf_size_needed(obj, cf) > buff_size)
        return LV_RES_INV;

    /*Width and height determine snapshot image size.*/
    lv_coord_t w = lv_obj_get_width(obj);
    lv_coord_t h = lv_obj_get_height(obj);
    lv_coord_t ext_size = _lv_obj_get_ext_draw_size(obj);
    w += ext_size * 2;
    h += ext_size * 2;

    /*Backup obj original info.*/
    lv_obj_t * parent_old = lv_obj_get_parent(obj);
    lv_area_t coords_bkp;
    lv_area_copy(&coords_bkp, &obj->coords);

    lv_memset(buf, 0x00, buff_size);
    lv_memset_00(dsc, sizeof(lv_img_dsc_t));

    /*We are safe to use stack for below variables since disp will be
     * unregistered when function returns. */
    lv_disp_t * disp;
    lv_disp_drv_t driver;
    lv_disp_draw_buf_t draw_buf;

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, w * h);

    lv_disp_drv_init(&driver);
    driver.draw_buf = &draw_buf;
    driver.hor_res = w;
    driver.ver_res = h;
    lv_disp_drv_use_generic_set_px_cb(&driver, cf);

    disp = lv_disp_drv_register(&driver);
    if(disp == NULL) {
        return LV_RES_INV;
    }

    /*Make background transparent */
    lv_disp_set_bg_opa(disp, LV_OPA_TRANSP);

    /*Move obj to newly created disp and refresh it. */
    lv_obj_t * screen = lv_disp_get_scr_act(disp);
    lv_obj_remove_style_all(screen);
    lv_obj_allocate_spec_attr(screen);
    screen->spec_attr->child_cnt = 1;
    screen->spec_attr->children = &obj;

    obj->parent = screen;

    disp->inv_p = 0;

    obj->coords.x2 = w - ext_size - 1;
    obj->coords.x1 = ext_size;
    obj->coords.y2 = h - ext_size - 1;
    obj->coords.y1 = ext_size;

    lv_obj_invalidate(obj);

    /*Don't call lv_refr_now to avoid animation disruption */
    _lv_disp_refr_timer(disp->refr_timer);

    /*Restore obj original parameters and clean up*/
    obj->parent = parent_old;
    screen->spec_attr->child_cnt = 0;
    screen->spec_attr->children = NULL;

    lv_disp_remove(disp);

    lv_area_copy(&obj->coords, &coords_bkp);

    dsc->data = buf;
    dsc->header.w = lv_area_get_width(&draw_buf.area);
    dsc->header.h = lv_area_get_height(&draw_buf.area);
    dsc->header.cf = cf;
    return LV_RES_OK;
}

/** Take snapshot for object with its children, alloc the memory needed.
 *
 * @param obj    The object to generate snapshot.
 * @param cf     color format for generated image.
 *
 * @return a pointer to a image descriptor, or NULL if failed.
 */
lv_img_dsc_t * lv_snapshot_take(lv_obj_t * obj, lv_img_cf_t cf)
{
    uint32_t buff_size = lv_snapshot_buf_size_needed(obj, cf);

    void * buf = lv_mem_alloc(buff_size);
    if(buf == NULL) {
        return NULL;
    }

    lv_img_dsc_t * dsc = lv_mem_alloc(sizeof(lv_img_dsc_t));
    if(dsc == NULL) {
        lv_mem_free(buf);
        return NULL;
    }

    if(lv_snapshot_take_to_buf(obj, cf, dsc, buf, buff_size) == LV_RES_INV) {
        lv_mem_free(buf);
        lv_mem_free(dsc);
        return NULL;
    }

    return dsc;
}

/** Free the snapshot image returned by @ref lv_snapshot_take
 *
 * It will firstly free the data image takes, then the image descriptor.
 *
 * @param dsc    The image descriptor generated by lv_snapshot_take.
 *
 */
void lv_snapshot_free(lv_img_dsc_t * dsc)
{
    if(!dsc)
        return;

    if(dsc->data)
        lv_mem_free((void *)dsc->data);

    lv_mem_free(dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_SNAPSHOT*/
