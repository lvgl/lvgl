/**
 * @file lv_uefi_private.h
 *
 */

#ifndef __LV_UEFI_PRIVATE_H__
#define __LV_UEFI_PRIVATE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"

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
 * Internal cache for the image handle (source: application entry point)
 */
extern EFI_HANDLE gLvEfiImageHandle;
/**
 * Internal cache for the system table (source: application entry point)
 */
extern EFI_SYSTEM_TABLE * gLvEfiST;
/**
 * Internal cache for the boot services table (source: gLvEfiST)
 */
extern EFI_BOOT_SERVICES * gLvEfiBS;
/**
 * Internal cache for the boot runtime service table (source: gLvEfiST)
 */
extern EFI_RUNTIME_SERVICES * gLvEfiRT;

/**
 * @brief Test if a protocol is installed at a handle.
 * @param handle The handle on which the protocol might be installed.
 * @param protocol The guid of the protocol.
 * @return TRUE if the protocol is installed, FALSE if not.
*/
bool lv_uefi_protocol_test(EFI_HANDLE handle, EFI_GUID * protocol);

/**
 * @brief Open a protocol.
 * @param handle The handle on which the protocol is installed.
 * @param protocol The guid of the protocol.
 * @return A pointer to the interface, NULL if the protocol couldn't be opened.
*/
void * lv_uefi_protocol_open(EFI_HANDLE handle, EFI_GUID * protocol);

/**
 * @brief Close a protocol.
 * @param handle The handle on which the protocol is installed.
 * @param protocol The guid of the protocol.
*/
void lv_uefi_protocol_close(EFI_HANDLE handle, EFI_GUID * protocol);

/**
 * @brief Convert an UCS-2 string to an ASCII string.
 * The string must contain only characters >= 0x20 and <= 0X7E.
 * @param ucs2 The UCS-2 string.
 * @param ascii The buffer to store the ASCII string.
 * @param ascii_len The size of the buffer in ASCII characters.
 * @return The number of characters written to the buffer or 0 if
 * there was an error.
*/
size_t lv_uefi_ucs2_to_ascii(const CHAR16 * ucs2, char * ascii, size_t ascii_len);

/**
 * @brief Convert an ASCII string to an UCS-2 string.
 * The string must contain only characters >= 0x20 and <= 0X7E.
 * @param ascii The ASCII string.
 * @param ucs2 The buffer to store the UCS-2 string.
 * @param ucs2_len The size of the buffer in UCS-2 characters.
 * @return The number of bytes written to the buffer or 0 if
 * there was an error.
*/
size_t lv_uefi_ascii_to_ucs2(const char * ascii, CHAR16 * ucs2, size_t ucs2_len);

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
}
#endif

#endif //__LV_UEFI_PRIVATE_H__