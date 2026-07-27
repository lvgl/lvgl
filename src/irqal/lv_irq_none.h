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

/* No hardware IRQ backend is compiled in.
 *
 * The per-peripheral capability macros (LV_IRQ_HAS_*) are left undefined here and
 * default to 0 in lv_irq_private.h, so draw units fall back to polling or emit a
 * compile error when they require an interrupt.
 *
 * This selection is also the "bring your own backend" path: define the needed
 * LV_IRQ_HAS_* macros to 1 in your lv_conf.h and link your own implementation of
 * the matching lv_irq_attach and lv_irq_detach functions from your build. Because
 * no in-repo backend .c is compiled for LV_IRQ_NONE, there is no symbol conflict.
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
