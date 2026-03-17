/**
 * @file lv_uefi_context.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"

#if LV_USE_UEFI

#include "lv_uefi_context.h"
#include "lv_uefi_private.h"

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
    LV_ASSERT_NULL(image_handle);
    LV_ASSERT_NULL(system_table);

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
    LV_ASSERT_NULL(gLvEfiImageHandle);
    LV_ASSERT_NULL(gLvEfiST);
    LV_ASSERT_NULL(gLvEfiBS);
    LV_ASSERT_NULL(gLvEfiRT);
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
