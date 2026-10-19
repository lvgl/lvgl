/**
 * @file lv_test_dma2d_hal.c
 *
 */
#include <lvgl/lvgl.h>

#if LV_BUILD_TEST && LV_USE_DRAW_DMA2D

#include "unity/unity.h"
#include "lv_test_dma2d_hal.h"

static DMA2D_TypeDef regs;
static DMA2D_TypeDef transfers[LV_TEST_DMA2D_LOG_MAX];
static uint32_t transfer_cnt;

RCC_TypeDef lv_test_rcc_regs;

DMA2D_TypeDef * lv_test_dma2d_regs(void)
{
    /*The unit polls CR until START clears, which ends the transfer*/
    if(regs.CR & DMA2D_CR_START) {
        if(transfer_cnt < LV_TEST_DMA2D_LOG_MAX) transfers[transfer_cnt] = regs;
        transfer_cnt++;
        regs.CR &= ~DMA2D_CR_START;
    }
    return &regs;
}

void lv_test_dma2d_reset(void)
{
    lv_memzero(&regs, sizeof(regs));
    lv_memzero(transfers, sizeof(transfers));
    transfer_cnt = 0;
}

uint32_t lv_test_dma2d_count(void)
{
    return transfer_cnt;
}

const DMA2D_TypeDef * lv_test_dma2d_get(uint32_t i)
{
    TEST_ASSERT_TRUE(i < transfer_cnt && i < LV_TEST_DMA2D_LOG_MAX);
    return &transfers[i];
}

#endif
