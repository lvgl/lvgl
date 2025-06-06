/**
 * @file lv_uefi_context.h
 *
 */

#ifndef __LV_UEFI_CONTEXT_H__
#define __LV_UEFI_CONTEXT_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_UEFI

#include "lv_uefi.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Initialize the UEFI cache variables.
 * @param image_handle The handle of the current image
 * @param system_table Pointer to the system table
 * @remark This has to be called before lv_init().
*/
void lv_uefi_init(
    EFI_HANDLE image_handle,
    EFI_SYSTEM_TABLE * system_table);

/**
 * @brief Initialize the LVGL UEFI backend.
 * @remark This is a private API which is used for LVGL UEFI backend
 *         implementation. LVGL users shouldn't use that because the
 *         LVGL has already used it in lv_init.
*/
void lv_uefi_platform_init(void);

/**
 * @brief Cleanup the LVGL UEFI backend.
 * @remark This is a private API which is used for LVGL UEFI backend
 *         implementation. LVGL users shouldn't use that because the
 *         LVGL has already used it in lv_deinit.
*/
void lv_uefi_platform_deinit(void);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
}
#endif

#endif //__LV_UEFI_CONTEXT_H__