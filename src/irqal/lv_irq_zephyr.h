/**
 * @file lv_irq_zephyr.h
 *
 */

#ifndef LV_IRQ_ZEPHYR_H
#define LV_IRQ_ZEPHYR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl_public.h"

#if LV_USE_IRQ == LV_IRQ_ZEPHYR

#include <zephyr/devicetree.h>

/*********************
 *      DEFINES
 *********************/

/* Per-peripheral capability: available only if the device tree has the node.
 * `#ifndef` guards let a user force a value from lv_conf.h. */
#ifndef LV_IRQ_HAS_DMA2D
#if DT_NODE_EXISTS(DT_NODELABEL(dma2d))
#define LV_IRQ_HAS_DMA2D 1
#else
#define LV_IRQ_HAS_DMA2D 0
#endif
#endif

#ifndef LV_IRQ_HAS_PXP
#if DT_NODE_EXISTS(DT_NODELABEL(pxp))
#define LV_IRQ_HAS_PXP 1
#else
#define LV_IRQ_HAS_PXP 0
#endif
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_IRQ == LV_IRQ_ZEPHYR*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IRQ_ZEPHYR_H*/
