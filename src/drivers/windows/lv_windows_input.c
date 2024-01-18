/**
 * @file lv_windows_input.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_windows_input.h"
#if LV_USE_WINDOWS

#include "lv_windows_context.h"
#include "lv_windows_display.h"

#include <windowsx.h>

#include "../../widgets/textarea/lv_textarea.h"
#include "../../widgets/keyboard/lv_keyboard.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_windows_pointer_driver_read_callback(
    lv_indev_t * indev,
    lv_indev_data_t * data);

static void lv_windows_release_pointer_device_event_callback(lv_event_t * e);

static void lv_windows_keypad_driver_read_callback(
    lv_indev_t * indev,
    lv_indev_data_t * data);

static void lv_windows_release_keypad_device_event_callback(lv_event_t * e);

static void lv_windows_encoder_driver_read_callback(
    lv_indev_t * indev,
    lv_indev_data_t * data);

static void lv_windows_release_encoder_device_event_callback(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

HWND lv_windows_get_indev_window_handle(lv_indev_t * indev)
{
    return lv_windows_get_display_window_handle(lv_indev_get_display(indev));
}

lv_indev_t * lv_windows_acquire_pointer_indev(lv_display_t * display)
{
    HWND window_handle = lv_windows_get_display_window_handle(display);
    if(!window_handle) {
        return NULL;
    }

    lv_windows_window_context_t * context = lv_windows_get_window_context(
                                                window_handle);
    if(!context) {
        return NULL;
    }

    if(!context->pointer.indev) {
        context->pointer.state = LV_INDEV_STATE_RELEASED;
        context->pointer.point.x = 0;
        context->pointer.point.y = 0;

        context->pointer.indev = lv_indev_create();
        if(context->pointer.indev) {
            lv_indev_set_type(
                context->pointer.indev,
                LV_INDEV_TYPE_POINTER);
            lv_indev_set_read_cb(
                context->pointer.indev,
                lv_windows_pointer_driver_read_callback);
            lv_indev_set_display(
                context->pointer.indev,
                context->display_device_object);
            lv_indev_add_event_cb(
                context->pointer.indev,
                lv_windows_release_pointer_device_event_callback,
                LV_EVENT_DELETE,
                context->pointer.indev);
            lv_indev_set_group(
                context->pointer.indev,
                lv_group_get_default());
        }
    }

    return context->pointer.indev;
}

lv_indev_t * lv_windows_acquire_keypad_indev(lv_display_t * display)
{
    HWND window_handle = lv_windows_get_display_window_handle(display);
    if(!window_handle) {
        return NULL;
    }

    lv_windows_window_context_t * context = lv_windows_get_window_context(
                                                window_handle);
    if(!context) {
        return NULL;
    }

    if(!context->keypad.indev) {
        InitializeCriticalSection(&context->keypad.mutex);
        _lv_ll_init(
            &context->keypad.queue,
            sizeof(lv_windows_keypad_queue_item_t));
        context->keypad.utf16_high_surrogate = 0;
        context->keypad.utf16_low_surrogate = 0;

        context->keypad.indev = lv_indev_create();
        if(context->keypad.indev) {
            lv_indev_set_type(
                context->keypad.indev,
                LV_INDEV_TYPE_KEYPAD);
            lv_indev_set_read_cb(
                context->keypad.indev,
                lv_windows_keypad_driver_read_callback);
            lv_indev_set_display(
                context->keypad.indev,
                context->display_device_object);
            lv_indev_add_event_cb(
                context->keypad.indev,
                lv_windows_release_keypad_device_event_callback,
                LV_EVENT_DELETE,
                context->keypad.indev);
            lv_indev_set_group(
                context->keypad.indev,
                lv_group_get_default());
        }
    }

    return context->keypad.indev;
}

lv_indev_t * lv_windows_acquire_encoder_indev(lv_display_t * display)
{
    HWND window_handle = lv_windows_get_display_window_handle(display);
    if(!window_handle) {
        return NULL;
    }

    lv_windows_window_context_t * context = lv_windows_get_window_context(
                                                window_handle);
    if(!context) {
        return NULL;
    }

    if(!context->encoder.indev) {
        context->encoder.state = LV_INDEV_STATE_RELEASED;
        context->encoder.enc_diff = 0;

        context->encoder.indev = lv_indev_create();
        if(context->encoder.indev) {
            lv_indev_set_type(
                context->encoder.indev,
                LV_INDEV_TYPE_ENCODER);
            lv_indev_set_read_cb(
                context->encoder.indev,
                lv_windows_encoder_driver_read_callback);
            lv_indev_set_display(
                context->encoder.indev,
                context->display_device_object);
            lv_indev_add_event_cb(
                context->encoder.indev,
                lv_windows_release_encoder_device_event_callback,
                LV_EVENT_DELETE,
                context->encoder.indev);
            lv_indev_set_group(
                context->encoder.indev,
                lv_group_get_default());
        }
    }

    return context->encoder.indev;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_windows_pointer_driver_read_callback(
    lv_indev_t * indev,
    lv_indev_data_t * data)
{
    lv_windows_window_context_t * context = lv_windows_get_window_context(
                                                lv_windows_get_indev_window_handle(indev));
    if(!context) {
        return;
    }

    data->state = context->pointer.state;
    data->point = context->pointer.point;
}

static void lv_windows_release_pointer_device_event_callback(lv_event_t * e)
{
    lv_indev_t * indev = (lv_indev_t *)lv_event_get_user_data(e);
    if(!indev) {
        return;
    }

    HWND window_handle = lv_windows_get_indev_window_handle(indev);
    if(!window_handle) {
        return;
    }

    lv_windows_window_context_t * context = lv_windows_get_window_context(
                                                window_handle);
    if(!context) {
        return;
    }

    context->pointer.state = LV_INDEV_STATE_RELEASED;
    context->pointer.point.x = 0;
    context->pointer.point.y = 0;

    context->pointer.indev = NULL;
}

static BOOL lv_windows_get_touch_input_info(
    HTOUCHINPUT touch_input_handle,
    UINT input_count,
    PTOUCHINPUT inputs,
    int item_size)
{
    HMODULE module_handle = GetModuleHandleW(L"user32.dll");
    if(!module_handle) {
        return FALSE;
    }

    typedef BOOL(WINAPI * function_type)(HTOUCHINPUT, UINT, PTOUCHINPUT, int);

    function_type function = (function_type)(
                                 GetProcAddress(module_handle, "GetTouchInputInfo"));
    if(!function) {
        return FALSE;
    }

    return function(touch_input_handle, input_count, inputs, item_size);
}

static BOOL lv_windows_close_touch_input_handle(
    HTOUCHINPUT touch_input_handle)
{
    HMODULE module_handle = GetModuleHandleW(L"user32.dll");
    if(!module_handle) {
        return FALSE;
    }

    typedef BOOL(WINAPI * function_type)(HTOUCHINPUT);

    function_type function = (function_type)(
                                 GetProcAddress(module_handle, "CloseTouchInputHandle"));
    if(!function) {
        return FALSE;
    }

    return function(touch_input_handle);
}

bool lv_windows_pointer_device_window_message_handler(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    LRESULT * plResult)
{
    switch(uMsg) {
        case WM_MOUSEMOVE: {
                lv_windows_window_context_t * context = (lv_windows_window_context_t *)(
                                                            lv_windows_get_window_context(hWnd));
                if(context) {
                    int32_t hor_res = lv_display_get_horizontal_resolution(
                                          context->display_device_object);
                    int32_t ver_res = lv_display_get_vertical_resolution(
                                          context->display_device_object);

                    context->pointer.point.x = lv_windows_zoom_to_logical(
                                                   GET_X_LPARAM(lParam),
                                                   context->zoom_level);
                    context->pointer.point.y = lv_windows_zoom_to_logical(
                                                   GET_Y_LPARAM(lParam),
                                                   context->zoom_level);
                    if(context->simulator_mode) {
                        context->pointer.point.x = lv_windows_dpi_to_logical(
                                                       context->pointer.point.x,
                                                       context->window_dpi);
                        context->pointer.point.y = lv_windows_dpi_to_logical(
                                                       context->pointer.point.y,
                                                       context->window_dpi);
                    }
                    if(context->pointer.point.x < 0) {
                        context->pointer.point.x = 0;
                    }
                    if(context->pointer.point.x > hor_res - 1) {
                        context->pointer.point.x = hor_res - 1;
                    }
                    if(context->pointer.point.y < 0) {
                        context->pointer.point.y = 0;
                    }
                    if(context->pointer.point.y > ver_res - 1) {
                        context->pointer.point.y = ver_res - 1;
                    }
                }

                break;
            }
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP: {
                lv_windows_window_context_t * context = (lv_windows_window_context_t *)(
                                                            lv_windows_get_window_context(hWnd));
                if(context) {
                    context->pointer.state = (
                                                 uMsg == WM_LBUTTONDOWN
                                                 ? LV_INDEV_STATE_PRESSED
                                                 : LV_INDEV_STATE_RELEASED);
                }

                break;
            }
        case WM_TOUCH: {
                lv_windows_window_context_t * context = (lv_windows_window_context_t *)(
                                                            lv_windows_get_window_context(hWnd));
                if(context) {
                    UINT input_count = LOWORD(wParam);
                    HTOUCHINPUT touch_input_handle = (HTOUCHINPUT)(lParam);

                    PTOUCHINPUT inputs = malloc(input_count * sizeof(TOUCHINPUT));
                    if(inputs) {
                        if(lv_windows_get_touch_input_info(
                               touch_input_handle,
                               input_count,
                               inputs,
                               sizeof(TOUCHINPUT))) {
                            for(UINT i = 0; i < input_count; ++i) {
                                POINT Point;
                                Point.x = TOUCH_COORD_TO_PIXEL(inputs[i].x);
                                Point.y = TOUCH_COORD_TO_PIXEL(inputs[i].y);
                                if(!ScreenToClient(hWnd, &Point)) {
                                    continue;
                                }

                                context->pointer.point.x = lv_windows_zoom_to_logical(
                                                               Point.x,
                                                               context->zoom_level);
                                context->pointer.point.y = lv_windows_zoom_to_logical(
                                                               Point.y,
                                                               context->zoom_level);
                                if(context->simulator_mode) {
                                    context->pointer.point.x = lv_windows_dpi_to_logical(
                                                                   context->pointer.point.x,
                                                                   context->window_dpi);
                                    context->pointer.point.y = lv_windows_dpi_to_logical(
                                                                   context->pointer.point.y,
                                                                   context->window_dpi);
                                }

                                DWORD MousePressedMask =
                                    TOUCHEVENTF_MOVE | TOUCHEVENTF_DOWN;

                                context->pointer.state = (
                                                             inputs[i].dwFlags & MousePressedMask
                                                             ? LV_INDEV_STATE_PRESSED
                                                             : LV_INDEV_STATE_RELEASED);
                            }
                        }

                        free(inputs);
                    }

                    lv_windows_close_touch_input_handle(touch_input_handle);
                }

                break;
            }
        default:
            // Not Handled
            return false;
    }

    // Handled
    *plResult = 0;
    return true;
}

static void lv_windows_keypad_driver_read_callback(
    lv_indev_t * indev,
    lv_indev_data_t * data)
{
    lv_windows_window_context_t * context = lv_windows_get_window_context(
                                                lv_windows_get_indev_window_handle(indev));
    if(!context) {
        return;
    }

    EnterCriticalSection(&context->keypad.mutex);

    lv_windows_keypad_queue_item_t * current = (lv_windows_keypad_queue_item_t *)(
                                                   _lv_ll_get_head(&context->keypad.queue));
    if(current) {
        data->key = current->key;
        data->state = current->state;

        _lv_ll_remove(&context->keypad.queue, current);
        lv_free(current);

        data->continue_reading = true;
    }

    LeaveCriticalSection(&context->keypad.mutex);
}

static void lv_windows_release_keypad_device_event_callback(lv_event_t * e)
{
    lv_indev_t * indev = (lv_indev_t *)lv_event_get_user_data(e);
    if(!indev) {
        return;
    }

    HWND window_handle = lv_windows_get_indev_window_handle(indev);
    if(!window_handle) {
        return;
    }

    lv_windows_window_context_t * context = lv_windows_get_window_context(
                                                window_handle);
    if(!context) {
        return;
    }

    DeleteCriticalSection(&context->keypad.mutex);
    _lv_ll_clear(&context->keypad.queue);
    context->keypad.utf16_high_surrogate = 0;
    context->keypad.utf16_low_surrogate = 0;

    context->keypad.indev = NULL;
}

static void lv_windows_push_key_to_keyboard_queue(
    lv_windows_window_context_t * context,
    uint32_t key,
    lv_indev_state_t state)
{
    lv_windows_keypad_queue_item_t * current = (lv_windows_keypad_queue_item_t *)(
                                                   _lv_ll_ins_tail(&context->keypad.queue));
    if(current) {
        current->key = key;
        current->state = state;
    }
}

static HIMC lv_windows_imm_get_context(
    HWND window_handle)
{
    HMODULE module_handle = GetModuleHandleW(L"imm32.dll");
    if(!module_handle) {
        return FALSE;
    }

    typedef HIMC(WINAPI * function_type)(HWND);

    function_type function = (function_type)(
                                 GetProcAddress(module_handle, "ImmGetContext"));
    if(!function) {
        return FALSE;
    }

    return function(window_handle);
}

static BOOL lv_windows_imm_release_context(
    HWND window_handle,
    HIMC imm_context_handle)
{
    HMODULE module_handle = GetModuleHandleW(L"imm32.dll");
    if(!module_handle) {
        return FALSE;
    }

    typedef BOOL(WINAPI * function_type)(HWND, HIMC);

    function_type function = (function_type)(
                                 GetProcAddress(module_handle, "ImmReleaseContext"));
    if(!function) {
        return FALSE;
    }

    return function(window_handle, imm_context_handle);
}

static HIMC lv_windows_imm_associate_context(
    HWND window_handle,
    HIMC imm_context_handle)
{
    HMODULE module_handle = GetModuleHandleW(L"imm32.dll");
    if(!module_handle) {
        return FALSE;
    }

    typedef HIMC(WINAPI * function_type)(HWND, HIMC);

    function_type function = (function_type)(
                                 GetProcAddress(module_handle, "ImmAssociateContext"));
    if(!function) {
        return FALSE;
    }

    return function(window_handle, imm_context_handle);
}

static BOOL lv_windows_imm_set_composition_window(
    HIMC imm_context_handle,
    LPCOMPOSITIONFORM composition_form)
{
    HMODULE module_handle = GetModuleHandleW(L"imm32.dll");
    if(!module_handle) {
        return FALSE;
    }

    typedef BOOL(WINAPI * function_type)(HIMC, LPCOMPOSITIONFORM);

    function_type function = (function_type)(
                                 GetProcAddress(module_handle, "ImmSetCompositionWindow"));
    if(!function) {
        return FALSE;
    }

    return function(imm_context_handle, composition_form);
}

bool lv_windows_keypad_device_window_message_handler(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    LRESULT * plResult)
{
    switch(uMsg) {
        case WM_KEYDOWN:
        case WM_KEYUP: {
                lv_windows_window_context_t * context = (lv_windows_window_context_t *)(
                                                            lv_windows_get_window_context(hWnd));
                if(context) {
                    EnterCriticalSection(&context->keypad.mutex);

                    bool skip_translation = false;
                    uint32_t translated_key = 0;

                    switch(wParam) {
                        case VK_UP:
                            translated_key = LV_KEY_UP;
                            break;
                        case VK_DOWN:
                            translated_key = LV_KEY_DOWN;
                            break;
                        case VK_LEFT:
                            translated_key = LV_KEY_LEFT;
                            break;
                        case VK_RIGHT:
                            translated_key = LV_KEY_RIGHT;
                            break;
                        case VK_ESCAPE:
                            translated_key = LV_KEY_ESC;
                            break;
                        case VK_DELETE:
                            translated_key = LV_KEY_DEL;
                            break;
                        case VK_BACK:
                            translated_key = LV_KEY_BACKSPACE;
                            break;
                        case VK_RETURN:
                            translated_key = LV_KEY_ENTER;
                            break;
                        case VK_TAB:
                        case VK_NEXT:
                            translated_key = LV_KEY_NEXT;
                            break;
                        case VK_PRIOR:
                            translated_key = LV_KEY_PREV;
                            break;
                        case VK_HOME:
                            translated_key = LV_KEY_HOME;
                            break;
                        case VK_END:
                            translated_key = LV_KEY_END;
                            break;
                        default:
                            skip_translation = true;
                            break;
                    }

                    if(!skip_translation) {
                        lv_windows_push_key_to_keyboard_queue(
                            context,
                            translated_key,
                            ((uMsg == WM_KEYUP)
                             ? LV_INDEV_STATE_RELEASED
                             : LV_INDEV_STATE_PRESSED));
                    }

                    LeaveCriticalSection(&context->keypad.mutex);
                }

                break;
            }
        case WM_CHAR: {
                lv_windows_window_context_t * context = (lv_windows_window_context_t *)(
                                                            lv_windows_get_window_context(hWnd));
                if(context) {
                    EnterCriticalSection(&context->keypad.mutex);

                    uint16_t raw_code_point = (uint16_t)(wParam);

                    if(raw_code_point >= 0x20 && raw_code_point != 0x7F) {
                        if(IS_HIGH_SURROGATE(raw_code_point)) {
                            context->keypad.utf16_high_surrogate = raw_code_point;
                        }
                        else if(IS_LOW_SURROGATE(raw_code_point)) {
                            context->keypad.utf16_low_surrogate = raw_code_point;
                        }

                        uint32_t code_point = raw_code_point;

                        if(context->keypad.utf16_high_surrogate &&
                           context->keypad.utf16_low_surrogate) {
                            uint16_t high_surrogate =
                                context->keypad.utf16_high_surrogate;
                            uint16_t low_surrogate =
                                context->keypad.utf16_low_surrogate;

                            code_point = (low_surrogate & 0x03FF);
                            code_point += (((high_surrogate & 0x03FF) + 0x40) << 10);

                            context->keypad.utf16_high_surrogate = 0;
                            context->keypad.utf16_low_surrogate = 0;
                        }

                        uint32_t lvgl_code_point =
                            _lv_text_unicode_to_encoded(code_point);

                        lv_windows_push_key_to_keyboard_queue(
                            context,
                            lvgl_code_point,
                            LV_INDEV_STATE_PRESSED);
                        lv_windows_push_key_to_keyboard_queue(
                            context,
                            lvgl_code_point,
                            LV_INDEV_STATE_RELEASED);
                    }

                    LeaveCriticalSection(&context->keypad.mutex);
                }

                break;
            }
        case WM_IME_SETCONTEXT: {
                if(wParam == TRUE) {
                    HIMC imm_context_handle = lv_windows_imm_get_context(hWnd);
                    if(imm_context_handle) {
                        lv_windows_imm_associate_context(
                            hWnd,
                            imm_context_handle);
                        lv_windows_imm_release_context(
                            hWnd,
                            imm_context_handle);
                    }
                }

                *plResult = DefWindowProcW(hWnd, uMsg, wParam, wParam);
                break;
            }
        case WM_IME_STARTCOMPOSITION: {
                HIMC imm_context_handle = lv_windows_imm_get_context(hWnd);
                if(imm_context_handle) {
                    lv_obj_t * textarea_object = NULL;
                    lv_obj_t * focused_object = lv_group_get_focused(
                                                    lv_group_get_default());
                    if(focused_object) {
                        const lv_obj_class_t * object_class = lv_obj_get_class(
                                                                  focused_object);

                        if(object_class == &lv_textarea_class) {
                            textarea_object = focused_object;
                        }
                        else if(object_class == &lv_keyboard_class) {
                            textarea_object = lv_keyboard_get_textarea(focused_object);
                        }
                    }

                    COMPOSITIONFORM composition_form;
                    composition_form.dwStyle = CFS_POINT;
                    composition_form.ptCurrentPos.x = 0;
                    composition_form.ptCurrentPos.y = 0;

                    if(textarea_object) {
                        lv_textarea_t * textarea = (lv_textarea_t *)(textarea_object);
                        lv_obj_t * label_object = lv_textarea_get_label(textarea_object);

                        composition_form.ptCurrentPos.x =
                            label_object->coords.x1 + textarea->cursor.area.x1;
                        composition_form.ptCurrentPos.y =
                            label_object->coords.y1 + textarea->cursor.area.y1;
                    }

                    lv_windows_imm_set_composition_window(
                        imm_context_handle,
                        &composition_form);
                    lv_windows_imm_release_context(
                        hWnd,
                        imm_context_handle);
                }

                *plResult = DefWindowProcW(hWnd, uMsg, wParam, wParam);
                break;
            }
        default:
            // Not Handled
            return false;
    }

    // Handled
    *plResult = 0;
    return true;
}

static void lv_windows_encoder_driver_read_callback(
    lv_indev_t * indev,
    lv_indev_data_t * data)
{
    lv_windows_window_context_t * context = lv_windows_get_window_context(
                                                lv_windows_get_indev_window_handle(indev));
    if(!context) {
        return;
    }

    data->state = context->encoder.state;
    data->enc_diff = context->encoder.enc_diff;
    context->encoder.enc_diff = 0;
}

static void lv_windows_release_encoder_device_event_callback(lv_event_t * e)
{
    lv_indev_t * indev = (lv_indev_t *)lv_event_get_user_data(e);
    if(!indev) {
        return;
    }

    HWND window_handle = lv_windows_get_indev_window_handle(indev);
    if(!window_handle) {
        return;
    }

    lv_windows_window_context_t * context = lv_windows_get_window_context(
                                                window_handle);
    if(!context) {
        return;
    }

    context->encoder.state = LV_INDEV_STATE_RELEASED;
    context->encoder.enc_diff = 0;

    context->encoder.indev = NULL;
}

bool lv_windows_encoder_device_window_message_handler(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    LRESULT * plResult)
{
    switch(uMsg) {
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP: {
                lv_windows_window_context_t * context = (lv_windows_window_context_t *)(
                                                            lv_windows_get_window_context(hWnd));
                if(context) {
                    context->encoder.state = (
                                                 uMsg == WM_MBUTTONDOWN
                                                 ? LV_INDEV_STATE_PRESSED
                                                 : LV_INDEV_STATE_RELEASED);
                }

                break;
            }
        case WM_MOUSEWHEEL: {
                lv_windows_window_context_t * context = (lv_windows_window_context_t *)(
                                                            lv_windows_get_window_context(hWnd));
                if(context) {
                    context->encoder.enc_diff =
                        -(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
                }

                break;
            }
        default:
            // Not Handled
            return false;
    }

    // Handled
    *plResult = 0;
    return true;
}

#endif // LV_USE_WINDOWS
