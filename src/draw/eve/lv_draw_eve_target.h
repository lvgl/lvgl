/**
 * @file lv_draw_eve_target.h
 *
 */

#ifndef LV_DRAW_EVE_TARGET_H
#define LV_DRAW_EVE_TARGET_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"
#if LV_USE_DRAW_EVE

#include "../../misc/lv_types.h"
#include LV_STDBOOL_INCLUDE
#include LV_STDINT_INCLUDE

typedef struct {
    uint16_t hor_res;          /**< active display width */
    uint16_t ver_res;          /**< active display height */
    uint16_t hcycle;           /**< total number of clocks per line, incl front/back porch */
    uint16_t hoffset;          /**< start of active line */
    uint16_t hsync0;           /**< start of horizontal sync pulse */
    uint16_t hsync1;           /**< end of horizontal sync pulse */
    uint16_t vcycle;           /**< total number of lines per screen, including pre/post */
    uint16_t voffset;          /**< start of active screen */
    uint16_t vsync0;           /**< start of vertical sync pulse */
    uint16_t vsync1;           /**< end of vertical sync pulse */
    uint8_t swizzle;           /**< FT8xx output to LCD - pin order */
    uint8_t pclkpol;           /**< LCD data is clocked in on this PCLK edge */
    uint8_t cspread;           /**< helps with noise, when set to 1 fewer signals are changed simultaneously, reset-default: 1 */
    uint8_t pclk;              /**< 60MHz / pclk = pclk frequency */
    bool has_crystal;          /**< has an external clock crystal */
    bool has_gt911;            /**< has a touch controller */
    uint8_t backlight_pwm;     /**< backlight PWM duty cycle 0 = off, 128 = max */
    uint16_t backlight_freq;   /**< backlight PWM frequency. try 4000 if unsure */
} lv_draw_eve_parameters_t;

typedef enum {
    LV_DRAW_EVE_OPERATION_POWERDOWN_SET,   /**< set the "PD_N" pin low */
    LV_DRAW_EVE_OPERATION_POWERDOWN_CLEAR, /**< set the "PD_N" pin high */
    LV_DRAW_EVE_OPERATION_CS_ASSERT,       /**< set the "CS_N" pin low */
    LV_DRAW_EVE_OPERATION_CS_DEASSERT,     /**< set the "CS_N" pin high */
    LV_DRAW_EVE_OPERATION_SPI_SEND,        /**< send `length` bytes of `data` over SPI */
    LV_DRAW_EVE_OPERATION_SPI_RECEIVE      /**< receive `length` bytes into `data` from SPI */
} lv_draw_eve_operation_t;

typedef void (*lv_draw_eve_operation_cb_t)(lv_display_t * disp, lv_draw_eve_operation_t operation, void * data,
                                           uint32_t length);

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DRAW_EVE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_EVE_TARGET_H*/
