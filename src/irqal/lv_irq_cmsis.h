/**
 * @file lv_irq_cmsis.h
 *
 */

#ifndef LV_IRQ_CMSIS_H
#define LV_IRQ_CMSIS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl_public.h"

#if LV_USE_IRQ == LV_IRQ_CMSIS

/* Vendor/CMSIS device header providing the IRQn_Type enumerators (e.g. PXP_IRQn,
 * DMA2D_IRQn) and the NVIC_* helpers. Needed both to test peripheral availability
 * below and to register/enable the lines in lv_irq_cmsis.c. */
#if LV_IRQ_USE_CMSIS_INCLUDE
#include LV_IRQ_CMSIS_INCLUDE
#endif

/*********************
 *      DEFINES
 *********************/

/* Per-peripheral capability: available only if the device header exposes the
 * peripheral. Detection uses the peripheral base-address *macro* (DMA2D_BASE,
 * PXP_BASE) rather than the *_IRQn enumerators, because the latter are enum
 * constants and are invisible to the preprocessor. `#ifndef` guards let a user
 * force a value from lv_conf.h when their device header names things differently. */
#ifndef LV_IRQ_HAS_DMA2D
#if defined(DMA2D_BASE)
#define LV_IRQ_HAS_DMA2D 1
#else
#define LV_IRQ_HAS_DMA2D 0
#endif
#endif

#ifndef LV_IRQ_HAS_PXP
#if defined(PXP_BASE)
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

#endif /*LV_USE_IRQ == LV_IRQ_CMSIS*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IRQ_CMSIS_H*/
