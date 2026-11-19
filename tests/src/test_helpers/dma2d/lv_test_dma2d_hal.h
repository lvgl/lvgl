/**
 * @file lv_test_dma2d_hal.h
 *
 * Stand-in for the STM32 HAL header, so that the DMA2D draw unit runs on the host.
 * See tests/configs/dma2d.defconfig.
 */

#ifndef LV_TEST_DMA2D_HAL_H
#define LV_TEST_DMA2D_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define STM32H7

typedef struct {
    uint32_t CR;
    uint32_t AMTCR;
    uint32_t NLR;
    uint32_t OMAR;
    uint32_t OOR;
    uint32_t OPFCCR;
    uint32_t OCOLR;
    uint32_t FGMAR;
    uint32_t FGOR;
    uint32_t FGCOLR;
    uint32_t FGPFCCR;
    uint32_t BGMAR;
    uint32_t BGOR;
    uint32_t BGCOLR;
    uint32_t BGPFCCR;
} DMA2D_TypeDef;

typedef struct {
    uint32_t AHB3ENR;
    uint32_t AHB3LPENR;
} RCC_TypeDef;

/*As in the CMSIS device header stm32h743xx.h*/
#define DMA2D_CR_START_Pos          (0U)
#define DMA2D_CR_START              (1UL << DMA2D_CR_START_Pos)
#define DMA2D_CR_MODE_Pos           (16U)
#define DMA2D_NLR_PL_Pos            (16U)
#define DMA2D_NLR_NL_Pos            (0U)
#define DMA2D_OPFCCR_CM_Pos         (0U)
#define DMA2D_FGPFCCR_CM_Pos        (0U)
#define DMA2D_FGPFCCR_AM_Pos        (16U)
#define DMA2D_FGPFCCR_ALPHA_Pos     (24U)
#define DMA2D_BGPFCCR_CM_Pos        (0U)
#define DMA2D_BGPFCCR_AM_Pos        (16U)
#define DMA2D_BGPFCCR_ALPHA_Pos     (24U)

#define RCC_AHB3ENR_DMA2DEN         (1UL << 4)
#define RCC_AHB3LPENR_DMA2DLPEN     (1UL << 4)

#define DMA2D_IRQn                  90

#define LV_TEST_DMA2D_LOG_MAX       32

/*Every access to DMA2D goes through here. A transfer started by the previous access is logged and START is cleared*/
DMA2D_TypeDef * lv_test_dma2d_regs(void);

void lv_test_dma2d_reset(void);

uint32_t lv_test_dma2d_count(void);

const DMA2D_TypeDef * lv_test_dma2d_get(uint32_t i);

extern RCC_TypeDef lv_test_rcc_regs;

#define DMA2D (lv_test_dma2d_regs())
#define RCC   (&lv_test_rcc_regs)

static inline void NVIC_EnableIRQ(int irqn)
{
    (void)irqn;
}

static inline void NVIC_DisableIRQ(int irqn)
{
    (void)irqn;
}

static inline void __DSB(void)
{
}

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEST_DMA2D_HAL_H*/
