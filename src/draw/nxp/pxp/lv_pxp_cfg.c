/**
 * @file lv_pxp_cfg.c
 *
 */

/**
 * Copyright 2020-2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_pxp_cfg.h"

#if LV_USE_DRAW_PXP
#include "lv_pxp_osa.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static pxp_cfg_t * _pxp_cfg;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_pxp_init(void)
{
    _pxp_cfg = pxp_get_default_cfg();

    PXP_Init(PXP_ID);

    PXP_EnableCsc1(PXP_ID, false); /*Disable CSC1, it is enabled by default.*/
    PXP_SetProcessBlockSize(PXP_ID, kPXP_BlockSize16); /*Block size 16x16 for higher performance*/

    PXP_EnableInterrupts(PXP_ID, kPXP_CompleteInterruptEnable);

    _pxp_cfg->pxp_interrupt_init();
}

void lv_pxp_deinit(void)
{
    _pxp_cfg->pxp_interrupt_deinit();
    PXP_DisableInterrupts(PXP_ID, kPXP_CompleteInterruptEnable);
    PXP_Deinit(PXP_ID);
}

void lv_pxp_reset(void)
{
    PXP_ResetControl(PXP_ID);

    PXP_EnableCsc1(PXP_ID, false); /*Disable CSC1, it is enabled by default.*/
    PXP_SetProcessBlockSize(PXP_ID, kPXP_BlockSize16); /*Block size 16x16 for higher performance*/
}

void lv_pxp_run(void)
{
    _pxp_cfg->pxp_run();
    _pxp_cfg->pxp_wait();
}

void lv_pxp_wait(void)
{
    _pxp_cfg->pxp_wait();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_PXP*/
