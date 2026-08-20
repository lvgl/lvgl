/**
 * @file lv_uefi_context.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_uefi_private.h"

#if LV_USE_UEFI

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
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief Initialize the UEFI cache variables.
 * @param image_handle The handle of the current image
 * @param system_table Pointer to the system table
 * @remark This has to be called before lv_init().
*/
void lv_uefi_init(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE * system_table)
{
    LV_CHECK_ARG(image_handle != NULL, return);
    LV_CHECK_ARG(system_table != NULL, return);

    gLvEfiImageHandle = image_handle;
    gLvEfiST = system_table;
    gLvEfiBS = gLvEfiST->BootServices;
    gLvEfiRT = gLvEfiST->RuntimeServices;
}

/**
 * @brief Initialize the LVGL UEFI backend.
 * @remark This is a private API which is used for LVGL UEFI backend
 *         implementation. LVGL users shouldn't use that because the
 *         LVGL has already used it in lv_init.
 */
void lv_uefi_platform_init(void)
{
    LV_CHECK_ARG(gLvEfiImageHandle != NULL, return);
    LV_CHECK_ARG(gLvEfiST != NULL, return);
    LV_CHECK_ARG(gLvEfiBS != NULL, return);
    LV_CHECK_ARG(gLvEfiRT != NULL, return);
}

/**
 * @brief Cleanup the LVGL UEFI backend.
 * @remark This is a private API which is used for LVGL UEFI backend
 *         implementation. LVGL users shouldn't use that because the
 *         LVGL has already used it in lv_deinit.
*/
void lv_uefi_platform_deinit(void)
{
    ;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
