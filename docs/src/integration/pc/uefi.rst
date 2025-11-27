====
UEFI
====

Overview
********

The **UEFI** display/input `driver <https://github.com/lvgl/lvgl/src/drivers/uefi>`__ offers support
for using LVGL to create UIs in UEFI environments.

Prerequisites
*************

You need the following UEFI protocols available:

- ``EFI_LOADED_IMAGE_PROTOCOL_GUID``: for file system support (used to determine the file system used to load the application)
- ``EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID``: for file system support
- ``EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID``: for keyboard support
- ``EFI_SIMPLE_POINTER_PROTOCOL_GUID``: for mouse support
- ``EFI_ABSOLUTE_POINTER_PROTOCOL_GUID``: for touch support
- ``EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID``: for drawing
- ``EFI_EDID_ACTIVE_PROTOCOL_GUID``: for drawing (optional)

Configuration
*************

1. Enable the UEFI driver support in ``lv_conf.h``:

.. code-block:: c

    #define LV_USE_UEFI  1

- Enable the memory core functions, which are wrappers around ``AllocatePool`` and ``FreePool`` (using memory of type *EfiBootServicesData*) if you do not wan't to use your own implementations

.. code-block:: c

    #define LV_UEFI_USE_MEMORY_SERVICES  1

3. Enable file system support for the volume the application was loaded from (defaults to drive letter ``E``):

.. code-block:: c

    #define LV_USE_FS_UEFI  1

4. Include UEFI type definitions. There are predefined includes for EDK2 and GNU-EFI:

.. code-block:: c

    #define LV_USE_UEFI_INCLUDE <lvgl/src/drivers/uefi/lv_uefi_edk2.h>
    // or
    #define LV_USE_UEFI_INCLUDE <lvgl/src/drivers/uefi/lv_uefi_gnu_efi.h>

Usage
*****

A minimal example:

.. code-block:: c

    #include "lvgl/lvgl.h"
    #include "lvgl/examples/lv_examples.h"
    #include "lvgl/demos/lv_demos.h"

    EFI_STATUS EFIAPI EfiMain(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
    {
        lv_uefi_init(ImageHandle, SystemTable);
        lv_init();

        if (!lv_is_initialized()) return EFI_NOT_READY;

        EFI_HANDLE handle = lv_uefi_display_get_active();
        if (!handle) handle = lv_uefi_display_get_any();
        if (!handle) {
            lv_deinit();
            return EFI_UNSUPPORTED;
        }

        lv_display_t *display = lv_uefi_display_create(handle);
        lv_display_set_default(display);

        lv_group_t *group = lv_group_create();
        lv_group_set_default(group);

        lv_obj_t *cursor = lv_image_create(lv_layer_top());
        lv_image_set_src(cursor, "E:cursor.png");

        lv_indev_t *indev;

        indev = lv_uefi_simple_text_input_indev_create();
        lv_indev_set_group(indev, group);
        lv_uefi_simple_text_input_indev_add_all(indev);

        indev = lv_uefi_simple_pointer_indev_create(NULL);
        lv_uefi_simple_pointer_indev_add_all(indev);
        lv_indev_set_cursor(indev, cursor);

        indev = lv_uefi_absolute_pointer_indev_create(NULL);
        lv_uefi_absolute_pointer_indev_add_all(indev);

        lv_demo_widgets();

        size_t counter = 0;
        while (counter < 10000) {
            counter++;
            gBS->Stall(1000);
            lv_tick_inc(1);
            lv_timer_handler();
        }

        return EFI_SUCCESS;
    }
