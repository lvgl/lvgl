/**
 * @file lv_drv.h
 *
 */

#ifndef LV_DRV_H
#define LV_DRV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../hal/lv_hal_disp.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct _lv_drv_t {
    lv_res_t (*reset_dev_cb)(struct _lv_drv_t * drv);
    lv_res_t (*send_cmd_cb)(struct _lv_drv_t * drv, uint8_t cmd, const uint8_t * params, uint32_t len);
    lv_res_t (*read_data_cb)(struct _lv_drv_t * drv, uint8_t cmd, const uint8_t * params, uint32_t params_len,
                             uint8_t * rxbuf, uint32_t rxbuf_len);
    lv_res_t (*send_image_cb)(struct _lv_drv_t * drv, lv_disp_drv_t * disp_drv, const lv_area_t * area, const void * buf);
} lv_drv_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_res_t lv_drv_reset_dev(lv_drv_t * drv);

lv_res_t lv_drv_send_cmd(lv_drv_t * drv, uint8_t cmd, const void * params, uint32_t len);

lv_res_t lv_drv_read_data(lv_drv_t * drv, uint8_t cmd, const void * params, uint32_t params_len, void * rxbuf,
                          uint32_t rxbuf_len);

lv_res_t lv_drv_send_image(lv_drv_t * drv, lv_disp_drv_t * disp_drv, const lv_area_t * area, const void * buf);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRV_H*/
