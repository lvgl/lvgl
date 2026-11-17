/**
 * @file lv_irq_stm32.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lvgl_public.h"

#if LV_IRQ_PROVIDER == LV_IRQ_PROVIDER_STM32

#include LV_IRQ_STM32_HAL_INCLUDE

/* A source is served when it is enabled, the device has it and its HAL module
 * is enabled. */
#if LV_IRQ_PROVIDES_DMA2D && defined(DMA2D_BASE) && defined(HAL_DMA2D_MODULE_ENABLED)
    #define STM32_DMA2D 1
#else
    #define STM32_DMA2D 0
#endif
#if LV_IRQ_PROVIDES_NEMAGFX && defined(GPU2D_BASE) && defined(HAL_GPU2D_MODULE_ENABLED)
    #define STM32_NEMAGFX 1
#else
    #define STM32_NEMAGFX 0
#endif

#if STM32_DMA2D || STM32_NEMAGFX

/**********************
 *  STATIC PROTOTYPES
 **********************/

#if STM32_DMA2D
    static void dma2d_hal_trampoline(DMA2D_HandleTypeDef * handle);
#endif
#if STM32_NEMAGFX && (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
    static void nemagfx_hal_trampoline(GPU2D_HandleTypeDef * hgpu2d, uint32_t CmdListID);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/* The HAL owns the vectors: the generated <PERIPH>_IRQHandler calls
 * HAL_<PERIPH>_IRQHandler on these handles, which calls back here. Attach masks
 * the line while it changes the callback, so a completion arriving meanwhile
 * stays pending instead of being lost. The priority is the one the HAL init
 * (CubeMX) set. */

#if STM32_DMA2D
    extern DMA2D_HandleTypeDef hdma2d;
    static volatile lv_irq_cb_t dma2d_cb;
    static void * volatile dma2d_user_data;
#endif

#if STM32_NEMAGFX
    extern GPU2D_HandleTypeDef hgpu2d;
    static volatile lv_irq_cb_t nemagfx_cb;
    static lv_irq_nemagfx_event_t nemagfx_event;
#endif

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#if STM32_DMA2D

lv_result_t lv_irq_attach_dma2d(lv_irq_cb_t cb, void * user_data)
{
    LV_CHECK_ARG(cb != NULL, return LV_RESULT_INVALID);
    NVIC_DisableIRQ(DMA2D_IRQn);
    dma2d_user_data = user_data;
    dma2d_cb = cb;
    hdma2d.XferCpltCallback = dma2d_hal_trampoline;
    NVIC_EnableIRQ(DMA2D_IRQn);
    return LV_RESULT_OK;
}

lv_result_t lv_irq_detach_dma2d(void)
{
    NVIC_DisableIRQ(DMA2D_IRQn);
    hdma2d.XferCpltCallback = NULL;
    dma2d_cb = NULL;
    return LV_RESULT_OK;
}

#endif /*STM32_DMA2D*/

#if STM32_NEMAGFX

lv_result_t lv_irq_attach_nemagfx(lv_irq_cb_t cb, void * user_data)
{
    LV_CHECK_ARG(cb != NULL, return LV_RESULT_INVALID);
    NVIC_DisableIRQ(GPU2D_IRQn);
    nemagfx_event.user_data = user_data;
    nemagfx_cb = cb;
#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
    HAL_GPU2D_RegisterCommandListCpltCallback(&hgpu2d, nemagfx_hal_trampoline);
#endif
    NVIC_EnableIRQ(GPU2D_IRQn);
    return LV_RESULT_OK;
}

lv_result_t lv_irq_detach_nemagfx(void)
{
    /* With the line disabled and the callback cleared, no HAL unregister API
     * is needed. */
    NVIC_DisableIRQ(GPU2D_IRQn);
    nemagfx_cb = NULL;
    return LV_RESULT_OK;
}

#endif /*STM32_NEMAGFX*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if STM32_DMA2D

static void dma2d_hal_trampoline(DMA2D_HandleTypeDef * handle)
{
    lv_irq_cb_t cb = dma2d_cb;
    void * user_data = dma2d_user_data;
    LV_UNUSED(handle);

    if(cb) cb(user_data);
}

#endif /*STM32_DMA2D*/

#if STM32_NEMAGFX

/* Registered at runtime when USE_HAL_GPU2D_REGISTER_CALLBACKS == 1; otherwise
 * it overrides the HAL's weak callback. */
#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
    static void nemagfx_hal_trampoline(GPU2D_HandleTypeDef * hgpu2d, uint32_t CmdListID)
#else
    void HAL_GPU2D_CommandListCpltCallback(GPU2D_HandleTypeDef * hgpu2d, uint32_t CmdListID)
#endif
{
    lv_irq_cb_t cb = nemagfx_cb;
    LV_UNUSED(hgpu2d);

    if(cb) {
        nemagfx_event.cmd_list_id = CmdListID;
        cb(&nemagfx_event);
    }
}

#endif /*STM32_NEMAGFX*/

#endif /*STM32_DMA2D || STM32_NEMAGFX*/

#endif /*LV_IRQ_PROVIDER == LV_IRQ_PROVIDER_STM32*/
