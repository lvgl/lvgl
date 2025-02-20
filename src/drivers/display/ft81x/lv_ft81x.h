/**
 * @file lv_ft81x.h
 *
 */

#ifndef LV_FT81X_H
#define LV_FT81X_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"
#if LV_USE_FT81X

#include "../../../display/lv_display.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint16_t hor_res;
    uint16_t ver_res;

    uint16_t hcycle;
    uint16_t hoffset;
    uint16_t hsync0;
    uint16_t hsync1;
    uint16_t vcycle;
    uint16_t voffset;
    uint16_t vsync0;
    uint16_t vsync1;
    uint8_t swizzle;
    uint8_t pclkpol;
    uint8_t cspread;
    uint8_t pclk;

    bool has_crystal;
    bool is_bt81x;
} lv_ft81x_parameters_t;

typedef enum {
    LV_FT81X_SPI_OPERATION_CS_ASSERT,
    LV_FT81X_SPI_OPERATION_CS_DEASSERT,
    LV_FT81X_SPI_OPERATION_SEND,
    LV_FT81X_SPI_OPERATION_RECEIVE
} lv_ft81x_spi_operation;

typedef void (*lv_ft81x_spi_cb_t)(lv_display_t * disp, lv_ft81x_spi_operation operation, void * data, uint32_t length);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a framebuffer-based ft81x driver display.
 * @param params      pointer to a struct of display panel properties. does not need to be static.
 * @param partial_buf a single partial buffer
 * @param buf_size    size of the partial buffer
 * @param spi_cb      a callback called by the driver to perform SPI operations
 * @param user_data   use `lv_ft81x_get_user_data` to get this pointer inside the SPI callback
 * @return pointer to the display
 */
lv_display_t * lv_ft81x_create(const lv_ft81x_parameters_t * params, void * partial_buf, uint32_t buf_size,
                               lv_ft81x_spi_cb_t spi_cb, void * user_data);

/**
 * Get the `user_data` parameter that was passed to `lv_ft81x_create`. Useful in the SPI callback.
 * @param disp      pointer to the ft81x display
 * @return          the `user_data` pointer
 */
void * lv_ft81x_get_user_data(lv_display_t * disp);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_FT81X*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FT81X_H*/
