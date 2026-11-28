/**
 * @file lv_gd_tli.h
 *
 */

#ifndef LV_GD_TLI_H
#define LV_GD_TLI_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../config/lv_conf_internal.h"
#if LV_USE_GD_TLI

#include "../../display/lv_display.h"

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a direct render mode display bound to a TLI layer.
 * @param fb_adr_1      The TLI layer's framebuffer memory address.
 * @param fb_adr_2      An additional framebuffer-sized buffer to use for double buffering.
 *                      @nullable When NULL a single buffer is used.
 * @param layer_idx     The TLI layer to bind the display to: 0 (LAYER0) or 1 (LAYER1).
 * @return              The display, or NULL if the TLI layer's pixel format is not supported.
 */
lv_display_t * lv_gd_tli_create_direct(void * fb_adr_1, void * fb_adr_2, uint32_t layer_idx);

/**
 * Create a partial render mode display bound to a TLI layer. The layer's framebuffer is flushed to internally.
 * Enable `LV_GD_TLI_USE_IPA_FLUSH` for parallel flushing.
 * @param render_buf_1  A render buffer.
 * @param render_buf_2  An additional render buffer for double-buffering. @nullable When
 *                      NULL a single buffer is used.
 * @param buf_size      The size of the buffer(s) in bytes.
 * @param layer_idx     The TLI layer to bind the display to: 0 (LAYER0) or 1 (LAYER1).
 * @return              The display, or NULL if the TLI layer's pixel format is not supported
 *                      (with `LV_GD_TLI_USE_IPA_FLUSH`, L8 and AL88 are not supported either).
 */
lv_display_t * lv_gd_tli_create_partial(void * render_buf_1, void * render_buf_2, uint32_t buf_size,
                                        uint32_t layer_idx);

/**
 * Handle the TLI layer-reload-finished interrupt.
 * Call it from TLI_IRQHandler() when `LV_GD_TLI_DEFINE_IRQ_HANDLERS` is 0.
 */
void lv_gd_tli_irq_handler(void);

#if LV_GD_TLI_USE_IPA_FLUSH
/**
 * Handle the IPA transfer-finished interrupt.
 * Call it from IPA_IRQHandler() when `LV_GD_IPA_DEFINE_IRQ_HANDLERS` is 0.
 */
void lv_gd_tli_ipa_irq_handler(void);
#endif /*LV_GD_TLI_USE_IPA_FLUSH*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GD_TLI*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_GD_TLI_H*/
