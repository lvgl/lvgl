/**
 * @file lv_irq_none.h
 *
 */

#ifndef LV_IRQ_NONE_H
#define LV_IRQ_NONE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl_public.h"

#if LV_USE_IRQ == LV_IRQ_NONE

/*********************
 *      DEFINES
 *********************/

/* No IRQ backend: it knows no peripherals. Every LV_IRQ_HAS_* is left
 * undefined here and falls back to 0 in lv_irq_private.h, so draw units poll, or
 * fail to compile when they require an interrupt.
 *
 * To supply your own backend, select LV_IRQ_CUSTOM instead (see src/irqal/Kconfig).
 */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_IRQ == LV_IRQ_NONE*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_IRQ_NONE_H*/
