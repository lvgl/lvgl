/**
 * @file lv_irq_nema_stm32.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_irq_private.h"

#if LV_USE_NEMA_GFX && (LV_USE_NEMA_HAL == LV_NEMA_HAL_STM32)

#include LV_NEMA_STM32_HAL_INCLUDE

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
    static void nema_gpu2d_hal_trampoline(GPU2D_HandleTypeDef * hgpu2d, uint32_t CmdListID);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/* The GPU2D completion is delivered through the vendor HAL's command-list-complete
 * callback API, which owns the hardware vector and needs the hgpu2d handle. That
 * makes this a NemaGFX/STM32-specific irqal backend: it lives with the generic
 * backends but is gated on the NemaGFX STM32 HAL rather than on LV_USE_IRQ.
 *
 * nema_event_code holds the last completed command-list id; a pointer to it is
 * forwarded to the callback as user_data (the NemaGFX-specific payload). */
extern GPU2D_HandleTypeDef hgpu2d;

static volatile lv_irq_cb_t nema_gpu2d_cb;
static uint32_t nema_event_code;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_irq_attach_nema_gpu2d(lv_irq_cb_t cb)
{
    LV_CHECK_ARG(cb != NULL, return LV_RESULT_INVALID, "callback must not be NULL");
    nema_gpu2d_cb = cb;
#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
    HAL_GPU2D_RegisterCommandListCpltCallback(&hgpu2d, nema_gpu2d_hal_trampoline);
#endif
    return LV_RESULT_OK;
}

lv_result_t lv_irq_detach_nema_gpu2d(void)
{
    /* Clearing the stored callback makes the HAL callback a no-op; no dependency
     * on a HAL un-register API. */
    nema_gpu2d_cb = NULL;
    return LV_RESULT_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Vendor HAL command-list-complete callback: saves the completed command-list id
 * and forwards a pointer to it as the callback's user_data. Registered at runtime
 * when USE_HAL_GPU2D_REGISTER_CALLBACKS==1, otherwise it overrides the HAL's weak
 * symbol. */
#if (USE_HAL_GPU2D_REGISTER_CALLBACKS == 1)
    static void nema_gpu2d_hal_trampoline(GPU2D_HandleTypeDef * hgpu2d, uint32_t CmdListID)
#else
    void HAL_GPU2D_CommandListCpltCallback(GPU2D_HandleTypeDef * hgpu2d, uint32_t CmdListID)
#endif
{
    lv_irq_cb_t cb = nema_gpu2d_cb;
    LV_UNUSED(hgpu2d);

    nema_event_code = CmdListID;
    if(cb) cb(&nema_event_code);
}

#endif /*LV_USE_NEMA_GFX && LV_USE_NEMA_HAL == LV_NEMA_HAL_STM32*/
