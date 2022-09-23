/**
 * @file lv_drv_esp.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_drv_esp.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t reset_dev(lv_drv_t * drv, uint32_t t);
static lv_res_t send_cmd(lv_drv_t * drv, uint8_t cmd, const void * params, uint32_t len);
static lv_res_t read_data(lv_drv_t * drv, uint8_t cmd, const void * params, uint32_t params_len, void * rxbuf,
                          uint32_t rxbuf_len);
static lv_res_t disp_flush(lv_drv_t * drv, lv_disp_t * disp, const lv_area_t * area, const void * buf);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_drv_esp_init(lv_drv_esp_t * drv)
{
    lv_memzero(drv, sizeof(*drv));
    drv->base.send_cmd_cb = reset_dev;
    drv->base.send_cmd_cb = send_cmd;
    drv->base.read_data_cb = read_data;
    drv->base.send_image_cb = disp_flush;
}

void lv_drv_esp_create(lv_drv_esp_t * drv)
{
    /*Perform some initializations*/
    switch(drv->hwid) {
        case LV_DRV_ESP_HW_ID_HSPI:
            break;
        case LV_DRV_ESP_HW_ID_VSPI:
            break;
        case LV_DRV_ESP_HW_ID_I2C_1:
            break;
        case LV_DRV_ESP_HW_ID_I2C_2:
            break;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_res_t reset_dev(lv_drv_t * drv, uint32_t t)
{
    lv_drv_esp_t * esp_drv = (lv_drv_esp_t *) drv;
    //    esp_drv->rst_pin to 1
    //    wait(1);
    //    esp_drv->rst_pin to 0
    //    wait(t);
    //    esp_drv->rst_pin to 1
    //    wait(1);

    return LV_RES_OK;
}

static lv_res_t send_cmd(lv_drv_t * drv, uint8_t cmd, const void * params, uint32_t len)
{
    lv_drv_esp_t * esp_drv = (lv_drv_esp_t *) drv;

    switch(esp_drv->hwid) {
        case LV_DRV_ESP_HW_ID_HSPI:
            break;
        case LV_DRV_ESP_HW_ID_VSPI:
            break;
        case LV_DRV_ESP_HW_ID_I2C_1:
            break;
        case LV_DRV_ESP_HW_ID_I2C_2:
            break;
    }
    return LV_RES_OK;
}

static lv_res_t read_data(lv_drv_t * drv, uint8_t cmd, const void * params, uint32_t params_len, void * rxbuf,
                          uint32_t rxbuf_len)
{

    lv_drv_esp_t * esp_drv = (lv_drv_esp_t *) drv;

    switch(esp_drv->hwid) {
        case LV_DRV_ESP_HW_ID_HSPI:
            break;
        case LV_DRV_ESP_HW_ID_VSPI:
            break;
        case LV_DRV_ESP_HW_ID_I2C_1:
            break;
        case LV_DRV_ESP_HW_ID_I2C_2:
            break;
    }
    return LV_RES_OK;
}

static lv_res_t disp_flush(lv_drv_t * drv, lv_disp_t * disp, const lv_area_t * area, const void * buf)
{
    lv_drv_esp_t * esp_drv = (lv_drv_esp_t *) drv;

    switch(esp_drv->hwid) {
        case LV_DRV_ESP_HW_ID_HSPI:
            break;
        case LV_DRV_ESP_HW_ID_VSPI:
            break;
        case LV_DRV_ESP_HW_ID_I2C_1:
            break;
        case LV_DRV_ESP_HW_ID_I2C_2:
            break;
    }

    lv_disp_flush_ready(disp);
    return LV_RES_OK;

}
