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
 * HAL_<PERIPH>_IRQHandler on these handles, which calls back here. Attach
 * clears cb, stores user_data, then sets cb, so the callback never pairs a
 * callback with another one's user_data. */

#if STM32_DMA2D
    extern DMA2D_HandleTypeDef hdma2d;
    static volatile lv_irq_cb_t dma2d_cb;
    static void * volatile dma2d_user_data;
#endif

#if STM32_NEMAGFX
    extern GPU2D_HandleTypeDef hgpu2d;
    /* Attach runs before the draw unit submits its first command list, so the
    * completion cannot fire while the event's user_data is being set. */
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
    dma2d_cb = NULL;
    dma2d_user_data = user_data;
    dma2d_cb = cb;
    hdma2d.XferCpltCallback = dma2d_hal_trampoline;
    return LV_RESULT_OK;
}

lv_result_t lv_irq_detach_dma2d(void)
{
    hdma2d.XferCpltCallback = NULL;
    dma2d_cb = NULL;
    return LV_RESULT_OK;
}

#endif /*STM32_DMA2D*/

#if STM32_NEMAGFX

lv_result_t lv_irq_attach_nemagfx(lv_irq_cb_t cb, void * user_data)
{
    LV_CHECK_ARG(cb != NULL, return LV_RESULT_INVALID);
    nemagfx_cb = NULL;
    nemagfx_event.user_data = user_data;
    nemagfx_cb = cb;
#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
    HAL_GPU2D_RegisterCommandListCpltCallback(&hgpu2d, nemagfx_hal_trampoline);
#endif
    return LV_RESULT_OK;
}

lv_result_t lv_irq_detach_nemagfx(void)
{
    /* With the callback cleared the HAL callback does nothing, so no HAL
     * unregister API is needed. */
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
