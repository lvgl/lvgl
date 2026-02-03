/**
 * @file lv_uefi_display.h
 *
 */

#ifndef __LV_UEFI_DISPLAY_H__
#define __LV_UEFI_DISPLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../display/lv_display.h"

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
 * @brief Create a LVGL display object.
 * @param handle The handle on which an instance of the EFI_GRAPHICS_OUTPUT_PROTOCOL protocol is installed.
 * @return The created LVGL display object.
*/
lv_display_t * lv_uefi_display_create(void * handle);

/**
 * @brief Try to find the active display handle.
 * @return The handle or NULL if not found.
 * @remark The active display need interfaces for EFI_GRAPHICS_OUTPUT_PROTOCOL and EFI_EDID_ACTIVE_PROTOCOL
*/
void * lv_uefi_display_get_active(void);

/**
 * @brief Try to find any display handle.
 * @return The handle or NULL if not found.
*/
void * lv_uefi_display_get_any(void);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
}
#endif

#endif //__LV_UEFI_DISPLAY_H__
