/**
 * @file lv_irq_redox.h
 *
 * The Redox IRQ provider's ID and settings. LVGL's generated config does not
 * know a provider from outside its tree, so the provider defines them itself.
 * With a hand-written lv_conf.h, include this header there before picking the
 * provider, e.g.
 *     #include "lv_irq_redox.h"
 *     #define LV_IRQ_DMA2D_PROVIDER LV_IRQ_PROVIDER_REDOX
 *     #define LV_IRQ_REDOX_DMA2D_IRQ 42
 */

#ifndef LV_IRQ_REDOX_H
#define LV_IRQ_REDOX_H

#define LV_IRQ_PROVIDER_REDOX 128

#if !defined(LV_IRQ_REDOX_DMA2D_IRQ) && defined(CONFIG_LV_IRQ_REDOX_DMA2D_IRQ)
#define LV_IRQ_REDOX_DMA2D_IRQ CONFIG_LV_IRQ_REDOX_DMA2D_IRQ
#endif

#endif /*LV_IRQ_REDOX_H*/
