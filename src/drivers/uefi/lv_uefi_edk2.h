/**
 * @file lv_uefi_edk2.h
 *
 */

#ifndef LV_UEFI_EDK2_H
#define LV_UEFI_EDK2_H

#if LV_USE_UEFI

    #define LV_UEFI_EDK2_HEADERS    1

    #include <Uefi.h>
    #include <Base.h>
    #include <Protocol/SimplePointer.h>
    #include <Protocol/SimpleFileSystem.h>
    #include <Protocol/AbsolutePointer.h>
    #include <Protocol/GraphicsOutput.h>
    #include <Protocol/SimpleTextIn.h>
    #include <Protocol/SimpleTextOut.h>
    #include <Protocol/Timestamp.h>
    #include <Protocol/LoadedImage.h>
    #include <Protocol/EdidActive.h>
    #include <Guid/FileInfo.h>

#endif

#endif