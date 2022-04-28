/**
 * @file lv_drv_esp.h
 *
 */

#ifndef LV_DRV_ESP_H
#define LV_DRV_ESP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_drv.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_DRV_ESP_HW_ID_HSPI,
    LV_DRV_ESP_HW_ID_VSPI,
    LV_DRV_ESP_HW_ID_I2C_1,
    LV_DRV_ESP_HW_ID_I2C_2,
} lv_drv_esp_hwid_t;

typedef struct {
    lv_drv_t base;
    lv_drv_esp_hwid_t hwid;
    uint32_t baud;
    uint32_t rst_pin;
    uint32_t cs_pin;    /**< Only for SPI*/
    uint32_t adr;       /**< Only for I2C*/
} lv_drv_esp_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRV_ESP_H*/
