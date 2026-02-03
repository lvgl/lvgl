/**
 * @file lv_uefi_display.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"

#if LV_USE_UEFI

#include "lv_uefi_display.h"
#include "lv_uefi_private.h"

#if LV_COLOR_DEPTH != 32
    #error [lv_uefi] Unsupported LV_COLOR_DEPTH.
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct _lv_uefi_display_context_t {
    EFI_HANDLE handle;
    EFI_GRAPHICS_OUTPUT_PROTOCOL * gop_protocol;
    void * buffer;
    size_t buffer_size;
} lv_uefi_display_context_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void _display_event_cb(lv_event_t * e);
static void _display_flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map);

static void _display_ctx_free(lv_uefi_display_context_t * display_ctx);
static bool _display_interface_is_valid(const EFI_GRAPHICS_OUTPUT_PROTOCOL * interface);

/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static EFI_GUID _uefi_guid_graphics_output = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
static EFI_GUID _uefi_guid_edid_active = EFI_EDID_ACTIVE_PROTOCOL_GUID;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief Create a LVGL display object.
 * @param handle The handle on which an instance of the EFI_GRAPHICS_OUTPUT_PROTOCOL protocol is installed.
 * @return The created LVGL display object.
 */
lv_display_t * lv_uefi_display_create(void * handle)
{
    lv_display_t * display = NULL;
    lv_uefi_display_context_t * display_ctx;

    if(!lv_uefi_protocol_test(handle, &_uefi_guid_graphics_output)) return NULL;

    display_ctx = lv_calloc(1, sizeof(lv_uefi_display_context_t));
    LV_ASSERT_MALLOC(display_ctx);

    display_ctx->handle = handle;
    display_ctx->gop_protocol = (EFI_GRAPHICS_OUTPUT_PROTOCOL *)lv_uefi_protocol_open(handle, &_uefi_guid_graphics_output);
    if(!_display_interface_is_valid(display_ctx->gop_protocol)) {
        LV_LOG_WARN("[lv_uefi] The GOP interface is not valid.");
        goto error;
    }

    // 4 bytes per pixel
    display_ctx->buffer_size = 4 * display_ctx->gop_protocol->Mode->Info->HorizontalResolution *
                               display_ctx->gop_protocol->Mode->Info->VerticalResolution;
    display_ctx->buffer = lv_malloc(display_ctx->buffer_size);
    LV_ASSERT_MALLOC(display_ctx->buffer);

    display = lv_display_create(display_ctx->gop_protocol->Mode->Info->HorizontalResolution,
                                display_ctx->gop_protocol->Mode->Info->VerticalResolution);
    lv_display_add_event_cb(display, _display_event_cb, LV_EVENT_DELETE, display);
    lv_display_set_flush_cb(display, _display_flush_cb);
    lv_display_set_buffers(display, display_ctx->buffer, NULL, (uint32_t)display_ctx->buffer_size,
                           LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_user_data(display, display_ctx);

    goto finish;

error:
    if(display != NULL) {
        lv_display_set_user_data(display, NULL);
        lv_display_delete(display);
        display = NULL;
    }

    if(display_ctx != NULL) _display_ctx_free(display_ctx);

finish:
    return display;
}

/**
 * @brief Try to find the active display handle.
 * @return The handle or NULL if not found.
 * @remark The active display need interfaces for EFI_GRAPHICS_OUTPUT_PROTOCOL and EFI_EDID_ACTIVE_PROTOCOL
*/
void * lv_uefi_display_get_active(void)
{
    EFI_STATUS status;
    EFI_HANDLE active_handle = NULL;
    EFI_HANDLE * handles = NULL;
    UINTN no_handles;
    UINTN index;

    status = gLvEfiBS->LocateHandleBuffer(
                 ByProtocol,
                 &_uefi_guid_graphics_output,
                 NULL,
                 &no_handles,
                 &handles);
    if(status != EFI_SUCCESS) goto error;

    for(index = 0; index < no_handles; index++) {
        if(!lv_uefi_protocol_test(handles[index], &_uefi_guid_edid_active)) continue;
        if(!lv_uefi_protocol_test(handles[index], &_uefi_guid_graphics_output)) continue;
        active_handle = handles[index];
        break;
    }

    goto finish;

error:

finish:
    if(handles != NULL) gLvEfiBS->FreePool(handles);

    return active_handle;
}

/**
 * @brief Try to find any display handle.
 * @return The handle or NULL if not found.
*/
void * lv_uefi_display_get_any(void)
{
    EFI_STATUS status;
    EFI_HANDLE active_handle = NULL;
    EFI_HANDLE * handles = NULL;
    UINTN no_handles;
    UINTN index;

    status = gLvEfiBS->LocateHandleBuffer(
                 ByProtocol,
                 &_uefi_guid_graphics_output,
                 NULL,
                 &no_handles,
                 &handles);
    if(status != EFI_SUCCESS) goto error;

    for(index = 0; index < no_handles; index++) {
        if(!lv_uefi_protocol_test(handles[index], &_uefi_guid_graphics_output)) continue;
        active_handle = handles[index];
        break;
    }

    goto finish;

error:

finish:
    if(handles != NULL) gLvEfiBS->FreePool(handles);

    return active_handle;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _display_event_cb(lv_event_t * e)
{
    lv_display_t * display;
    lv_uefi_display_context_t * display_ctx;

    if(lv_event_get_code(e) != LV_EVENT_DELETE) return;

    display = (lv_display_t *)lv_event_get_user_data(e);
    if(display == NULL) return;

    display_ctx = (lv_uefi_display_context_t *)lv_display_get_user_data(display);
    lv_display_set_user_data(display, NULL);

    if(display_ctx != NULL) _display_ctx_free(display_ctx);
}

static void _display_flush_cb(lv_display_t * display, const lv_area_t * area, uint8_t * px_map)
{
    EFI_STATUS status;
    int32_t w;
    int32_t h;

    lv_uefi_display_context_t * display_ctx = (lv_uefi_display_context_t *)lv_display_get_user_data(display);
    LV_ASSERT_NULL(display_ctx);

    w = (int32_t)area->x2 - (int32_t)area->x1 + 1;
    h = (int32_t)area->y2 - (int32_t)area->y1 + 1;

    if(w < 0 || h < 0) {
        LV_LOG_ERROR("[lv_uefi] Invalid lv_display_flush_cb call (invalid rect).");
        goto error;
    }

    if((uint32_t)(area->x1 + w) > display_ctx->gop_protocol->Mode->Info->HorizontalResolution) {
        LV_LOG_ERROR("[lv_uefi] Invalid lv_display_flush_cb call (invalid width).");
        goto error;
    }

    if((uint32_t)(area->y1 + h) > display_ctx->gop_protocol->Mode->Info->HorizontalResolution) {
        LV_LOG_ERROR("[lv_uefi] Invalid lv_display_flush_cb call (invalid height).");
        goto error;
    }

    status = display_ctx->gop_protocol->Blt(
                 display_ctx->gop_protocol,
                 (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)px_map,
                 EfiBltBufferToVideo,
                 area->x1,
                 area->y1,
                 area->x1,
                 area->y1,
                 w,
                 h,
                 display_ctx->gop_protocol->Mode->Info->HorizontalResolution * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));
    if(status != EFI_SUCCESS) {
        LV_LOG_ERROR("[lv_uefi] Blt failed with error code: %llx.", status);
        goto error;
    }

    goto finish;

error:

finish:
    lv_display_flush_ready(display);
}

static void _display_ctx_free(lv_uefi_display_context_t * display_ctx)
{
    if(display_ctx == NULL) {
        return;
    }

    if(display_ctx->gop_protocol != NULL) lv_uefi_protocol_close(display_ctx->handle, &_uefi_guid_graphics_output);
    if(display_ctx->buffer != NULL) lv_free(display_ctx->buffer);

    lv_free(display_ctx);
}

static bool _display_interface_is_valid(const EFI_GRAPHICS_OUTPUT_PROTOCOL * interface)
{
    if(interface == NULL) return FALSE;
    if(interface->Mode == NULL) return FALSE;
    if(interface->Mode->Info == NULL) return FALSE;
    if(interface->Mode->Info->HorizontalResolution == 0) return FALSE;
    if(interface->Mode->Info->HorizontalResolution >= 32767) return FALSE;
    if(interface->Mode->Info->VerticalResolution == 0) return FALSE;
    if(interface->Mode->Info->VerticalResolution >= 32767) return FALSE;

    return TRUE;
}
#endif
