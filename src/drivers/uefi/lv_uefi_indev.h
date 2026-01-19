/**
 * @file lv_uefi_indev.h
 *
 */

#ifndef __LV_UEFI_INDEV_H__
#define __LV_UEFI_INDEV_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../indev/lv_indev.h"

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
 * @brief Create an indev object.
 * @param display_res The resolution of the display in pixels, needed to scale the input.
 * If NULL the resolution of the current default display will be used.
 * @return The created LVGL indev object.
*/
lv_indev_t * lv_uefi_simple_pointer_indev_create(lv_point_t * display_res);

/**
 * @brief Add an EFI_SIMPLE_POINTER_PROTOCOL interface to the indev.
 * @param indev Indev that was created with lv_uefi_simple_pointer_indev_create.
 * @param handle The handle on which an instance of the EFI_SIMPLE_POINTER_PROTOCOL protocol is installed.
 * @return True if the interface was added.
*/
bool lv_uefi_simple_pointer_indev_add_handle(lv_indev_t * indev, EFI_HANDLE handle);

/**
 * @brief Add all available EFI_SIMPLE_POINTER_PROTOCOL interfaces to the indev.
 * @param indev Indev that was created with lv_uefi_simple_pointer_indev_create.
*/
void lv_uefi_simple_pointer_indev_add_all(lv_indev_t * indev);

/**
 * @brief Create a LVGL indev object.
 * @param display_res The resolution of the display in pixels, needed to scale the input.
 * @return The created LVGL indev object.
*/
lv_indev_t * lv_uefi_absolute_pointer_indev_create(lv_point_t * display_res);

/**
 * @brief Add an EFI_ABSOLUTE_POINTER_PROTOCOL interface to the indev.
 * @param indev Indev that was created with lv_uefi_absolute_pointer_indev_create.
 * @param handle The handle on which an instance of the EFI_ABSOLUTE_POINTER_PROTOCOL protocol is installed.
 * @return True if the interface was added.
*/
bool lv_uefi_absolute_pointer_indev_add_handle(lv_indev_t * indev, EFI_HANDLE handle);

/**
 * @brief Add all available EFI_ABSOLUTE_POINTER_PROTOCOL interfaces to the indev.
 * @param indev Indev that was created with lv_uefi_absolute_pointer_indev_create.
*/
void lv_uefi_absolute_pointer_indev_add_all(lv_indev_t * indev);

/**
 * @brief Create an indev object.
 * @return The created LVGL indev object.
*/
lv_indev_t * lv_uefi_simple_text_input_indev_create(void);

/**
 * @brief Add an EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL interface to the indev.
 * @param indev Indev that was created with lv_uefi_simple_text_input_indev_create.
 * @param handle The handle on which an instance of the EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL protocol is installed.
 * @return True if the interface was added.
*/
bool lv_uefi_simple_text_input_indev_add_handle(lv_indev_t * indev, EFI_HANDLE handle);

/**
 * @brief Add all available EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL interfaces to the indev.
 * @param indev Indev that was created with lv_uefi_simple_text_input_indev_create.
*/
void lv_uefi_simple_text_input_indev_add_all(lv_indev_t * indev);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
}
#endif

#endif //__LV_UEFI_INDEV_H__
