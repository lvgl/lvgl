/**
 * @file lv_irq_nema_stm32.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

#if LV_IRQ_NEMA_GPU2D_PROVIDER == LV_IRQ_PROVIDER_NEMA_STM32

#include LV_NEMA_STM32_HAL_INCLUDE

/**********************
 *  STATIC PROTOTYPES
 **********************/

#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
    static void nema_gpu2d_hal_trampoline(GPU2D_HandleTypeDef * hgpu2d, uint32_t CmdListID);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/* The GPU2D completion arrives through the vendor HAL's command-list-complete
 * callback, which owns the hardware vector and needs the hgpu2d handle. */
extern GPU2D_HandleTypeDef hgpu2d;

/* Attach runs before the draw unit submits its first command list, so the
 * completion cannot fire while the event's user_data is being set. */
static volatile lv_irq_cb_t nema_gpu2d_cb;
static lv_irq_nema_gpu2d_event_t nema_gpu2d_event;

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_irq_attach_nema_gpu2d(lv_irq_cb_t cb, void * user_data)
{
    LV_CHECK_ARG(cb != NULL, return LV_RESULT_INVALID);
    nema_gpu2d_event.user_data = user_data;
    nema_gpu2d_cb = cb;
#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
    HAL_GPU2D_RegisterCommandListCpltCallback(&hgpu2d, nema_gpu2d_hal_trampoline);
#endif
    return LV_RESULT_OK;
}

lv_result_t lv_irq_detach_nema_gpu2d(void)
{
    /* With the callback cleared the HAL callback does nothing, so no HAL
     * unregister API is needed. */
    nema_gpu2d_cb = NULL;
    return LV_RESULT_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Registered at runtime when USE_HAL_GPU2D_REGISTER_CALLBACKS == 1; otherwise
 * it overrides the HAL's weak callback. */
#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
    static void nema_gpu2d_hal_trampoline(GPU2D_HandleTypeDef * hgpu2d, uint32_t CmdListID)
#else
    void HAL_GPU2D_CommandListCpltCallback(GPU2D_HandleTypeDef * hgpu2d, uint32_t CmdListID)
#endif
{
    lv_irq_cb_t cb = nema_gpu2d_cb;
    LV_UNUSED(hgpu2d);

    if(cb) {
        nema_gpu2d_event.cmd_list_id = CmdListID;
        cb(&nema_gpu2d_event);
    }
}

#endif /*LV_IRQ_NEMA_GPU2D_PROVIDER == LV_IRQ_PROVIDER_NEMA_STM32*/
