/**
 * @file lv_uefi_indev_keyboard.c
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

#define SIMPLE_TEXT_INPUT_INDEV_SIGNATURE 0x53495449

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_uefi_simple_text_input_key_cache_t {
    uint32_t key;   /**< Key code*/
    bool pressed;   /**< If true this is a pressed entry if false this is a release entry*/
} lv_uefi_simple_text_input_key_cache_t;

typedef struct _lv_uefi_simple_text_input_handle_context_t {
    EFI_HANDLE handle;
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL * interface;
} lv_uefi_simple_text_input_handle_context_t;

typedef struct _lv_uefi_simple_text_input_context_t {
    uint32_t signature; /**< Has to be checked to avoid access to a different indev*/
    lv_ll_t handles;
    lv_ll_t key_cache;
} lv_uefi_simple_text_input_context_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void _simple_text_input_event_cb(lv_event_t * e);
static void _simple_text_input_read_cb(lv_indev_t * indev, lv_indev_data_t * data);

static void _simple_text_input_handle_context_free(void * ptr);
static void _simple_text_input_context_free(lv_uefi_simple_text_input_context_t * indev_ctx);

static bool _simple_text_input_interface_is_valid(const EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL * interface);

static void _simple_text_input_read(lv_uefi_simple_text_input_context_t * indev_ctx,
                                    lv_uefi_simple_text_input_handle_context_t * handle_ctx);

static uint32_t _utf8_from_unicode(UINT32 unicode);
static uint32_t _key_from_uefi_key(const EFI_KEY_DATA * key);

/**********************
 *  STATIC VARIABLES
 **********************/

static EFI_GUID _uefi_guid_simple_text_input = EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief Create an indev object.
 * @return The created LVGL indev object.
*/
lv_indev_t * lv_uefi_simple_text_input_indev_create(void)
{
    lv_indev_t * indev = NULL;
    lv_uefi_simple_text_input_context_t * indev_ctx = NULL;

    indev_ctx = lv_calloc(1, sizeof(lv_uefi_simple_text_input_context_t));
    LV_ASSERT_MALLOC(indev_ctx);

    indev_ctx->signature = SIMPLE_TEXT_INPUT_INDEV_SIGNATURE;

    lv_ll_init(&indev_ctx->handles, sizeof(lv_uefi_simple_text_input_handle_context_t));
    lv_ll_init(&indev_ctx->key_cache, sizeof(lv_uefi_simple_text_input_key_cache_t));

    indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_user_data(indev, indev_ctx);
    lv_indev_add_event_cb(indev, _simple_text_input_event_cb, LV_EVENT_DELETE, indev);
    lv_indev_set_read_cb(indev, _simple_text_input_read_cb);

    return indev;
}

/**
 * @brief Add an EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL interface to the indev.
 * @param indev Indev that was created with lv_uefi_simple_text_input_indev_create.
 * @param handle The handle on which an instance of the EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL protocol is installed.
 * @return True if the interface was added.
*/
bool lv_uefi_simple_text_input_indev_add_handle(lv_indev_t * indev, EFI_HANDLE handle)
{
    EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL * interface = NULL;
    lv_uefi_simple_text_input_handle_context_t * handle_ctx = NULL;

    lv_uefi_simple_text_input_context_t * indev_ctx = (lv_uefi_simple_text_input_context_t *)lv_indev_get_user_data(indev);
    LV_ASSERT_NULL(indev_ctx);

    if(indev_ctx->signature != SIMPLE_TEXT_INPUT_INDEV_SIGNATURE) return false;

    interface = (EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *)lv_uefi_protocol_open(handle, &_uefi_guid_simple_text_input);
    if(!_simple_text_input_interface_is_valid(interface)) {
        lv_uefi_protocol_close(handle, &_uefi_guid_simple_text_input);
        LV_LOG_WARN("[lv_uefi] The SIMPLE_TEXT_INPUT interface is not valid.");
        return false;
    }

    handle_ctx = (lv_uefi_simple_text_input_handle_context_t *) lv_ll_ins_head(&indev_ctx->handles);
    LV_ASSERT_MALLOC(handle_ctx);

    handle_ctx->handle = handle;
    handle_ctx->interface = interface;

    return true;
}

/**
 * @brief Add all available EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL interfaces to the indev.
 * @param indev Indev that was created with lv_uefi_simple_text_input_indev_create.
*/
void lv_uefi_simple_text_input_indev_add_all(lv_indev_t * indev)
{
    EFI_STATUS status;
    EFI_HANDLE * handles = NULL;
    UINTN no_handles;
    UINTN index;

    lv_uefi_simple_text_input_context_t * indev_ctx = (lv_uefi_simple_text_input_context_t *)lv_indev_get_user_data(indev);
    LV_ASSERT_NULL(indev_ctx);

    if(indev_ctx->signature != SIMPLE_TEXT_INPUT_INDEV_SIGNATURE) return;

    status = gLvEfiBS->LocateHandleBuffer(ByProtocol, &_uefi_guid_simple_text_input, NULL, &no_handles, &handles);
    if(status != EFI_SUCCESS) {
        LV_LOG_ERROR("[lv_uefi] LocateHandleBuffer(SIMPLE_TEXT_INPUT_EX) failed with error code %llx.", status);
        return;
    }

    for(index = 0; index < no_handles; index++) {
        lv_uefi_simple_text_input_indev_add_handle(indev, handles[index]);
    }

    if(handles != NULL) gLvEfiBS->FreePool(handles);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void _simple_text_input_event_cb(lv_event_t * e)
{
    lv_indev_t * indev;
    lv_uefi_simple_text_input_context_t * indev_ctx;

    if(lv_event_get_code(e) != LV_EVENT_DELETE) return;

    indev = (lv_indev_t *)lv_event_get_user_data(e);
    if(indev == NULL) return;

    indev_ctx = (lv_uefi_simple_text_input_context_t *)lv_indev_get_user_data(indev);
    lv_indev_set_user_data(indev, NULL);

    if(indev_ctx != NULL) _simple_text_input_context_free(indev_ctx);
}

static void _simple_text_input_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    lv_uefi_simple_text_input_handle_context_t * handle_ctx = NULL;
    lv_uefi_simple_text_input_key_cache_t * key_cache = NULL;
    void * node = NULL;

    lv_uefi_simple_text_input_context_t * indev_ctx = (lv_uefi_simple_text_input_context_t *)lv_indev_get_user_data(indev);
    LV_ASSERT_NULL(indev_ctx);

    /* Empty the buffer before reading new values */
    if(lv_ll_is_empty(&indev_ctx->key_cache)) {
        // Read from all registered devices
        for(node = lv_ll_get_head(&indev_ctx->handles); node != NULL; node = lv_ll_get_next(&indev_ctx->handles, node)) {
            handle_ctx = (lv_uefi_simple_text_input_handle_context_t *) node;
            _simple_text_input_read(indev_ctx, handle_ctx);
        }
    }

    /* Return the first value */
    node = lv_ll_get_head(&indev_ctx->key_cache);
    if(node != NULL) {
        key_cache = (lv_uefi_simple_text_input_key_cache_t *)node;
        data->state = key_cache->pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
        data->key = key_cache->key;
        lv_ll_remove(&indev_ctx->key_cache, node);
        lv_free(key_cache);
    }

    /* Continue reading if there are more values in the buffer */
    data->continue_reading = !lv_ll_is_empty(&indev_ctx->key_cache);
}

static void _simple_text_input_context_free(lv_uefi_simple_text_input_context_t * indev_ctx)
{
    if(indev_ctx == NULL) return;
    lv_ll_clear_custom(&indev_ctx->handles, _simple_text_input_handle_context_free);
    lv_ll_clear(&indev_ctx->key_cache);
    lv_free(indev_ctx);
}

static bool _simple_text_input_interface_is_valid(const EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL * interface)
{
    if(interface == NULL) return FALSE;
    return TRUE;
}

static void _simple_text_input_handle_context_free(void * ptr)
{
    lv_uefi_simple_text_input_handle_context_t * handle_ctx = (lv_uefi_simple_text_input_handle_context_t *)ptr;

    if(handle_ctx == NULL) return;
    if(handle_ctx->interface) lv_uefi_protocol_close(handle_ctx->handle, &_uefi_guid_simple_text_input);
    lv_free(handle_ctx);
}

static void _simple_text_input_read(lv_uefi_simple_text_input_context_t * indev_ctx,
                                    lv_uefi_simple_text_input_handle_context_t * handle_ctx)
{
    EFI_STATUS status;
    EFI_KEY_DATA state;
    uint32_t key;
    lv_uefi_simple_text_input_key_cache_t * cache = NULL;

    LV_ASSERT_NULL(indev_ctx);
    LV_ASSERT_NULL(handle_ctx);

    status = handle_ctx->interface->ReadKeyStrokeEx(
                     handle_ctx->interface,
                     &state);
    if(status == EFI_NOT_READY) return;
    if(status != EFI_SUCCESS) {
        LV_LOG_ERROR("[lv_uefi] ReadKeyStrokeEx failed.");
        return;
    }

    key = _key_from_uefi_key(&state);

    /* insert the press */
    cache = (lv_uefi_simple_text_input_key_cache_t *) lv_ll_ins_tail(&indev_ctx->key_cache);
    LV_ASSERT_MALLOC(cache);
    cache->key = key;
    cache->pressed = true;

    /* insert the release */
    cache = (lv_uefi_simple_text_input_key_cache_t *) lv_ll_ins_tail(&indev_ctx->key_cache);
    LV_ASSERT_MALLOC(cache);
    cache->key = key;
    cache->pressed = false;
}

static uint32_t _utf8_from_unicode(UINT32 unicode)
{
    uint8_t bytes[4] = {0, 0, 0, 0};

    /* unicode < 128 -> 1 byte */
    if(unicode < 128) {
        bytes[0] |= unicode;
    }
    /* unicode < 2048 -> 2 byte */
    else if(unicode < 2048) {
        bytes[0] = 0xC0;
        bytes[0] |= unicode >> 6;
        bytes[1] = 0x80;
        bytes[1] |= (unicode & 0x003F);
    }
    /* unicode < 65536 -> 3 byte */
    else if(unicode < 65536) {
        bytes[0] = 0xE0;
        bytes[0] |= unicode >> 12;
        bytes[1] = 0x80;
        bytes[1] |= ((unicode >> 6) & 0x003F);
        bytes[2] = 0x80;
        bytes[2] |= (unicode & 0x003F);
    }

    return *((uint32_t *)bytes);
}

static uint32_t _key_from_uefi_key(const EFI_KEY_DATA * key)
{
    LV_ASSERT_NULL(key);

    switch(key->Key.ScanCode) {
        case 0x01:
            return LV_KEY_UP;
        case 0x02:
            return LV_KEY_DOWN;
        case 0x04:
            return LV_KEY_LEFT;
        case 0x03:
            return LV_KEY_RIGHT;
        case 0x08:
            return LV_KEY_DEL;
        case 0x05:
            return LV_KEY_HOME;
        case 0x06:
            return LV_KEY_END;
        case 0x17:
            return LV_KEY_ESC;
        /* ignore all other scan codes */
        default:
            break;
    }

    switch(key->Key.UnicodeChar) {
        case 0x09:
            return (key->KeyState.KeyShiftState & EFI_SHIFT_STATE_VALID) &&
                   (key->KeyState.KeyShiftState & (EFI_RIGHT_SHIFT_PRESSED | EFI_LEFT_SHIFT_PRESSED)) ?
                   LV_KEY_PREV :
                   LV_KEY_NEXT;
        case 0x08:
            return LV_KEY_BACKSPACE;
        case 0x0D:
            return LV_KEY_ENTER;
        case 0x18:
            return LV_KEY_ESC;
        default:
            return _utf8_from_unicode(key->Key.UnicodeChar);
    }
}

#endif