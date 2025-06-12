/**
 * @file lv_uefi_indev_pointer.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"
#include "../../stdlib/lv_mem.h"
#include "../../misc/lv_types.h"
#include "../../misc/lv_text.h"

#if LV_USE_UEFI

#include "lv_uefi_indev.h"
#include "lv_uefi_private.h"

/*********************
 *      DEFINES
 *********************/

#define SIMPLE_POINTER_INDEV_SIGNATURE 0x53505449

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_uefi_simple_pointer_handle_context_t {
    EFI_HANDLE handle;
    EFI_SIMPLE_POINTER_PROTOCOL * interface;
    lv_point_t pixel_per_step_8;    /**< How many pixels does the mouse cursor move in one step*/
} lv_uefi_simple_pointer_handle_context_t;

typedef struct _lv_uefi_simple_pointer_context_t {
    uint32_t signature; /**< Has to be checked to avoid access to a different indev*/
    lv_point_t display_res;
    lv_point_t position;
    lv_ll_t handles;
} lv_uefi_simple_pointer_context_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void _simple_pointer_indev_event_cb(lv_event_t * e);
static void _simple_pointer_read_cb(lv_indev_t * indev, lv_indev_data_t * data);
static void _simple_pointer_handle_context_free(void * ptr);
static void _simple_pointer_context_free(lv_uefi_simple_pointer_context_t * indev_ctx);
static bool _simple_pointer_interface_is_valid(const EFI_SIMPLE_POINTER_PROTOCOL * interface);
static void _simple_pointer_read(lv_uefi_simple_pointer_context_t * indev_ctx,
                                 lv_uefi_simple_pointer_handle_context_t * handle_ctx, bool * was_pressed);

/**********************
 *  STATIC VARIABLES
 **********************/

static EFI_GUID _uefi_guid_simple_pointer = EFI_SIMPLE_POINTER_PROTOCOL_GUID;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief Create an indev object.
 * @param display_res The resolution of the display in pixels, needed to scale the input.
 * If NULL the resolution of the current default display will be used.
 * @return The created LVGL indev object.
*/
lv_indev_t * lv_uefi_simple_pointer_indev_create(lv_point_t * display_res)
{
    lv_indev_t * indev = NULL;
    lv_uefi_simple_pointer_context_t * indev_ctx = NULL;

    indev_ctx = lv_calloc(1, sizeof(lv_uefi_simple_pointer_context_t));
    LV_ASSERT_MALLOC(indev_ctx);

    indev_ctx->signature = SIMPLE_POINTER_INDEV_SIGNATURE;

    if(display_res != NULL) {
        indev_ctx->display_res.x = display_res->x;
        indev_ctx->display_res.y = display_res->y;
    }
    else {
        indev_ctx->display_res.x = lv_display_get_horizontal_resolution(lv_display_get_default());
        indev_ctx->display_res.y = lv_display_get_vertical_resolution(lv_display_get_default());
    }

    lv_ll_init(&indev_ctx->handles, sizeof(lv_uefi_simple_pointer_handle_context_t));

    indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_user_data(indev, indev_ctx);
    lv_indev_add_event_cb(indev, _simple_pointer_indev_event_cb, LV_EVENT_DELETE, indev);
    lv_indev_set_read_cb(indev, _simple_pointer_read_cb);

    return indev;
}

/**
 * @brief Add an EFI_SIMPLE_POINTER_PROTOCOL interface to the indev.
 * @param indev Indev that was created with lv_uefi_simple_pointer_indev_create.
 * @param handle The handle on which an instance of the EFI_SIMPLE_POINTER_PROTOCOL protocol is installed.
 * @return True if the interface was added.
*/
bool lv_uefi_simple_pointer_indev_add_handle(lv_indev_t * indev, EFI_HANDLE handle)
{
    EFI_SIMPLE_POINTER_PROTOCOL * interface = NULL;
    lv_uefi_simple_pointer_handle_context_t * handle_ctx = NULL;

    lv_uefi_simple_pointer_context_t * indev_ctx = (lv_uefi_simple_pointer_context_t *)lv_indev_get_user_data(indev);
    LV_ASSERT_NULL(indev_ctx);

    if(indev_ctx->signature != SIMPLE_POINTER_INDEV_SIGNATURE) return false;

    interface = (EFI_SIMPLE_POINTER_PROTOCOL *)lv_uefi_protocol_open(handle, &_uefi_guid_simple_pointer);
    if(!_simple_pointer_interface_is_valid(interface)) {
        lv_uefi_protocol_close(handle, &_uefi_guid_simple_pointer);
        LV_LOG_WARN("[lv_uefi] The SIMPLE_POINTER interface is not valid.");
        return false;
    }

    handle_ctx = (lv_uefi_simple_pointer_handle_context_t *) lv_ll_ins_head(&indev_ctx->handles);
    LV_ASSERT_MALLOC(handle_ctx);

    handle_ctx->handle = handle;
    handle_ctx->interface = interface;
    handle_ctx->pixel_per_step_8.x = (((indev_ctx->display_res.x) << 8) / 50) /
                                     interface->Mode->ResolutionX;
    handle_ctx->pixel_per_step_8.y = (((indev_ctx->display_res.y) << 8) / 50) /
                                     interface->Mode->ResolutionY;

    return true;
}

/**
 * @brief Add all available EFI_SIMPLE_POINTER_PROTOCOL interfaces to the indev.
 * @param indev Indev that was created with lv_uefi_simple_pointer_indev_create.
*/
void lv_uefi_simple_pointer_indev_add_all(lv_indev_t * indev)
{
    EFI_STATUS status;
    EFI_HANDLE * handles = NULL;
    UINTN no_handles;
    UINTN index;

    lv_uefi_simple_pointer_context_t * indev_ctx = (lv_uefi_simple_pointer_context_t *)lv_indev_get_user_data(indev);
    LV_ASSERT_NULL(indev_ctx);

    if(indev_ctx->signature != SIMPLE_POINTER_INDEV_SIGNATURE) return;

    status = gLvEfiBS->LocateHandleBuffer(ByProtocol, &_uefi_guid_simple_pointer, NULL, &no_handles, &handles);
    if(status != EFI_SUCCESS) {
        LV_LOG_ERROR("[lv_uefi] LocateHandleBuffer(SIMPLE_POINTER) failed with error code %llx.", status);
        return;
    }

    for(index = 0; index < no_handles; index++) {
        lv_uefi_simple_pointer_indev_add_handle(indev, handles[index]);
    }

    if(handles != NULL) gLvEfiBS->FreePool(handles);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _simple_pointer_indev_event_cb(lv_event_t * e)
{
    lv_indev_t * indev;
    lv_uefi_simple_pointer_context_t * indev_ctx;

    if(lv_event_get_code(e) != LV_EVENT_DELETE) return;

    indev = (lv_indev_t *)lv_event_get_user_data(e);
    if(indev == NULL) return;

    indev_ctx = (lv_uefi_simple_pointer_context_t *)lv_indev_get_user_data(indev);
    lv_indev_set_user_data(indev, NULL);

    if(indev_ctx != NULL) _simple_pointer_context_free(indev_ctx);
}

static void _simple_pointer_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    void * node = NULL;

    lv_uefi_simple_pointer_context_t * indev_ctx = (lv_uefi_simple_pointer_context_t *)lv_indev_get_user_data(indev);
    LV_ASSERT_NULL(indev_ctx);

    /* Read from all registered devices */
    for(node = lv_ll_get_head(&indev_ctx->handles); node != NULL; node = lv_ll_get_next(&indev_ctx->handles, node)) {
        lv_uefi_simple_pointer_handle_context_t * handle_ctx = (lv_uefi_simple_pointer_handle_context_t *) node;
        bool was_pressed = false;

        _simple_pointer_read(indev_ctx, handle_ctx, &was_pressed);

        data->state |= was_pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    }

    /* Sanitize the events position */
    if(indev_ctx->position.x < 0) {
        indev_ctx->position.x = 0;
    }
    else if(indev_ctx->position.x > indev_ctx->display_res.x - 1) {
        indev_ctx->position.x = indev_ctx->display_res.x - 1;
    }

    if(indev_ctx->position.y < 0) {
        indev_ctx->position.y = 0;
    }
    else if(indev_ctx->position.y > indev_ctx->display_res.y - 1) {
        indev_ctx->position.y = indev_ctx->display_res.y - 1;
    }

    data->point.x = indev_ctx->position.x;
    data->point.y = indev_ctx->position.y;

    data->continue_reading = FALSE;
}

static void _simple_pointer_context_free(lv_uefi_simple_pointer_context_t * indev_ctx)
{
    if(indev_ctx == NULL) return;
    lv_ll_clear_custom(&indev_ctx->handles, _simple_pointer_handle_context_free);
    lv_free(indev_ctx);
}

static bool _simple_pointer_interface_is_valid(const EFI_SIMPLE_POINTER_PROTOCOL * interface)
{
    if(interface == NULL) return FALSE;
    if(interface->Mode == NULL) return FALSE;
    if(interface->Mode->ResolutionX == 0) return FALSE;
    if(interface->Mode->ResolutionX >= 256) return FALSE;
    if(interface->Mode->ResolutionY == 0) return FALSE;
    if(interface->Mode->ResolutionY >= 256) return FALSE;
    return TRUE;
}

static void _simple_pointer_handle_context_free(void * ptr)
{
    lv_uefi_simple_pointer_handle_context_t * handle_ctx = (lv_uefi_simple_pointer_handle_context_t *)ptr;

    if(handle_ctx == NULL) return;
    if(handle_ctx->interface) lv_uefi_protocol_close(handle_ctx->handle, &_uefi_guid_simple_pointer);
    lv_free(handle_ctx);
}

static void _simple_pointer_read(lv_uefi_simple_pointer_context_t * indev_ctx,
                                 lv_uefi_simple_pointer_handle_context_t * handle_ctx, bool * was_pressed)
{
    EFI_STATUS status;
    EFI_SIMPLE_POINTER_STATE state;
    lv_point_t pointer_mov;

    LV_ASSERT_NULL(indev_ctx);
    LV_ASSERT_NULL(handle_ctx);
    LV_ASSERT_NULL(was_pressed);

    status = handle_ctx->interface->GetState(
                     handle_ctx->interface,
                     &state);
    if(status == EFI_NOT_READY) return;
    if(status != EFI_SUCCESS) {
        LV_LOG_ERROR("[lv_uefi] GetState failed.");
        return;
    }

    pointer_mov.x = (state.RelativeMovementX * handle_ctx->pixel_per_step_8.x) >> 8;
    pointer_mov.y = (state.RelativeMovementY * handle_ctx->pixel_per_step_8.y) >> 8;

    indev_ctx->position.x += pointer_mov.x;
    indev_ctx->position.y += pointer_mov.y;

    /* Set the state to pressed if one of the interfaces reports a press */
    *was_pressed = state.LeftButton;
}

#endif