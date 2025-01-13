/**
 * @file lv_uefi.h
 *
 */

#ifndef LV_UEFI_H
#define LV_UEFI_H

#if LV_USE_UEFI

    #include LV_USE_UEFI_INCLUDE

    #if defined(__clang__) || defined(__GNUC__)
        #if defined(__x86_64__)
            #define __LV_UEFI_ARCH_X64__
            #define __LV_UEFI_64BIT__
        #elif defined(__i386__)
            #define __LV_UEFI_ARCH_X86__
            #define __LV_UEFI_32BIT__
        #elif defined(__aarch64__)
            #define __LV_UEFI_ARCH_AARCH64__
            #define __LV_UEFI_64BIT__
        #else
            #error Architecture is not supported
        #endif
        #define LV_UEFI_STATIC_ASSERT _Static_assert
    #elif defined(_MSC_VER)
        #if defined(_M_AMD64) && !defined(_M_ARM64)
            #define __LV_UEFI_ARCH_X64__
            #define __LV_UEFI_64BIT__
        #elif defined(_M_IX86)
            #define __LV_UEFI_ARCH_X86__
            #define __LV_UEFI_32BIT__
        #elif defined(_M_ARM64)
            #define __LV_UEFI_ARCH_AARCH64__
            #define __LV_UEFI_64BIT__
        #else
            #error Architecture is not supported
        #endif
        #define LV_UEFI_STATIC_ASSERT static_assert
    #else
        #error Your compiler is not supported
    #endif

    #ifdef LV_USE_UEFI_INCLUDE
        #include LV_USE_UEFI_INCLUDE
    #else
        #error No UEFI headers available
    #endif

    // Verify that all required protocols are known
    #if !defined(EFI_LOADED_IMAGE_PROTOCOL_GUID)
        #error Missing support for EFI_LOADED_IMAGE_PROTOCOL
    #endif
    #if !defined(EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID)
        #error Missing support for EFI_SIMPLE_FILE_SYSTEM_PROTOCOL
    #endif
    #if !defined(EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID)
        #error Missing support for EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL
    #endif
    #if !defined(EFI_SIMPLE_TEXT_INPUT_PROTOCOL_GUID)
        #error Missing support for EFI_SIMPLE_TEXT_INPUT_PROTOCOL
    #endif
    #if !defined(EFI_SIMPLE_POINTER_PROTOCOL_GUID)
        #error Missing support for EFI_SIMPLE_POINTER_PROTOCOL
    #endif
    #if !defined(EFI_ABSOLUTE_POINTER_PROTOCOL_GUID)
        #error Missing support for EFI_ABSOLUTE_POINTER_PROTOCOL
    #endif
    #if !defined(EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID)
        #error Missing support for EFI_GRAPHICS_OUTPUT_PROTOCOL
    #endif
    #if !defined(EFI_EDID_ACTIVE_PROTOCOL_GUID)
        #error Missing support for EFI_EDID_ACTIVE_PROTOCOL
    #endif
    #if !defined(EFI_FILE_INFO_ID)
        #error Missing support for EFI_FILE_INFO
    #endif
    #if !defined(EFI_TIMESTAMP_PROTOCOL_GUID)
        #error Missing support for EFI_TIMESTAMP_PROTOCOL_GUID
    #endif

    // Verify that all types have the correct size
    LV_UEFI_STATIC_ASSERT(sizeof(BOOLEAN)  == 1, "Size check for 'BOOLEAN' failed.");
    LV_UEFI_STATIC_ASSERT(sizeof(INT8)     == 1, "Size check for 'INT8' failed.");
    LV_UEFI_STATIC_ASSERT(sizeof(UINT8)    == 1, "Size check for 'UINT8' failed.");
    LV_UEFI_STATIC_ASSERT(sizeof(INT16)    == 2, "Size check for 'INT16' failed.");
    LV_UEFI_STATIC_ASSERT(sizeof(UINT16)   == 2, "Size check for 'UINT16' failed.");
    LV_UEFI_STATIC_ASSERT(sizeof(INT32)    == 4, "Size check for 'INT32' failed.");
    LV_UEFI_STATIC_ASSERT(sizeof(UINT32)   == 4, "Size check for 'UINT32' failed.");
    LV_UEFI_STATIC_ASSERT(sizeof(INT64)    == 8, "Size check for 'INT64' failed.");
    LV_UEFI_STATIC_ASSERT(sizeof(UINT64)   == 8, "Size check for 'UINT64' failed.");
    LV_UEFI_STATIC_ASSERT(sizeof(CHAR8)    == 1, "Size check for 'CHAR8' failed.");
    LV_UEFI_STATIC_ASSERT(sizeof(CHAR16)   == 2, "Size check for 'CHAR16' failed.");

    #ifdef __LV_UEFI_32BIT__
        LV_UEFI_STATIC_ASSERT(sizeof(INTN)   == 4, "Size check for 'INTN' failed.");
        LV_UEFI_STATIC_ASSERT(sizeof(UINTN)  == 4, "Size check for 'UINTN' failed.");
    #else
        LV_UEFI_STATIC_ASSERT(sizeof(INTN)   == 8, "Size check for 'INTN' failed.");
        LV_UEFI_STATIC_ASSERT(sizeof(UINTN)  == 8, "Size check for 'UINTN' failed.");
    #endif

#endif

#endif