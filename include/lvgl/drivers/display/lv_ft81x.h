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

#include "../../config/lv_conf_internal.h"
#if LV_USE_FT81X

#include "../../display/lv_display.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    uint16_t hor_res;   /**< active display width */
    uint16_t ver_res;   /**< active display height */

    uint16_t hcycle;    /**< total number of clocks per line, incl front/back porch */
    uint16_t hoffset;   /**< start of active line */
    uint16_t hsync0;    /**< start of horizontal sync pulse */
    uint16_t hsync1;    /**< end of horizontal sync pulse */
    uint16_t vcycle;    /**< total number of lines per screen, including pre/post */
    uint16_t voffset;   /**< start of active screen */
    uint16_t vsync0;    /**< start of vertical sync pulse */
    uint16_t vsync1;    /**< end of vertical sync pulse */
    uint8_t swizzle;    /**< FT8xx output to LCD - pin order */
    uint8_t pclkpol;    /**< LCD data is clocked in on this PCLK edge */
    uint8_t cspread;    /**< helps with noise, when set to 1 fewer signals are changed simultaneously, reset-default: 1 */
    uint8_t pclk;       /**< 60MHz / pclk = pclk frequency */

    bool has_crystal;   /**< has an external clock crystal */
    bool is_bt81x;      /**< is a BT series model, not FT */
} lv_ft81x_parameters_t;

typedef enum {
    LV_FT81X_SPI_OPERATION_CS_ASSERT,
    LV_FT81X_SPI_OPERATION_CS_DEASSERT,
    LV_FT81X_SPI_OPERATION_SEND,
    LV_FT81X_SPI_OPERATION_RECEIVE
} lv_ft81x_spi_operation_t;

typedef void (*lv_ft81x_spi_cb_t)(lv_display_t * disp, lv_ft81x_spi_operation_t operation, void * data,
                                  uint32_t length);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a framebuffer-based ft81x driver display. The display is unusable until
 * `lv_ft81x_init()` runs, so it almost always follows on the next line. Only what the SPI
 * callback needs goes in between, such as `lv_display_set_user_data()`.
 * @param params      pointer to a struct of display panel properties. does not need to be static.
 * @param partial_buf a single partial buffer
 * @param buf_size    size of the partial buffer
 * @return pointer to the display
 */
lv_display_t * lv_ft81x_create(const lv_ft81x_parameters_t * params, void * partial_buf, uint32_t buf_size);

/**
 * Set the function that performs the SPI operations. Required before init.
 * @param disp      pointer to the ft81x display
 * @param spi_cb    a callback called by the driver to perform SPI operations. Use
 *                  `lv_display_get_user_data()` to reach your own context inside it.
 */
void lv_ft81x_set_spi_cb(lv_display_t * disp, lv_ft81x_spi_cb_t spi_cb);

/**
 * Initialize the ft81x chip of a display created with `lv_ft81x_create()`.
 * Fails if the SPI callback has not been set.
 * The display is not flushed before this function returns.
 * @param disp      pointer to the ft81x display
 * @return          LV_RESULT_OK on success
 */
lv_result_t lv_ft81x_init(lv_display_t * disp);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_FT81X*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FT81X_H*/
