/**
 * @file lv_windows_context.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_windows_context.h"
#if LV_USE_WINDOWS

#ifdef __GNUC__
    #pragma GCC diagnostic ignored "-Wcast-function-type"
#endif

#include "lv_windows_display.h"
#include "lv_windows_input_private.h"
#include "../../osal/lv_os_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static uint32_t lv_windows_tick_count_callback(void);

static void lv_windows_delay_callback(uint32_t ms);

static void lv_windows_check_display_existence_timer_callback(
    lv_timer_t * timer);

static bool lv_windows_window_message_callback_nolock(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    LRESULT * plResult);

static LRESULT CALLBACK lv_windows_window_message_callback(
    HWND   hWnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_windows_platform_init(void)
{
    lv_tick_set_cb(lv_windows_tick_count_callback);

    lv_delay_set_cb(lv_windows_delay_callback);

    lv_timer_create(
        lv_windows_check_display_existence_timer_callback,
        200,
        NULL);

    // Try to ensure the default group exists.
    {
        lv_group_t * default_group = lv_group_get_default();
        if(!default_group) {
            default_group = lv_group_create();
            if(default_group) {
                lv_group_set_default(default_group);
            }
        }
    }

    WNDCLASSEXW window_class;
    lv_memzero(&window_class, sizeof(WNDCLASSEXW));
    window_class.cbSize = sizeof(WNDCLASSEXW);
    window_class.style = 0;
    window_class.lpfnWndProc = lv_windows_window_message_callback;
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 0;
    window_class.hInstance = NULL;
    window_class.hIcon = NULL;
    window_class.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);
    window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    window_class.lpszMenuName = NULL;
    window_class.lpszClassName = L"LVGL.Window";
    window_class.hIconSm = NULL;
    LV_ASSERT(RegisterClassExW(&window_class));
}

lv_windows_window_context_t * lv_windows_get_window_context(
    HWND window_handle)
{
    return (lv_windows_window_context_t *)(
               GetPropW(window_handle, L"LVGL.Window.Context"));
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static uint32_t lv_windows_tick_count_callback(void)
{
    LARGE_INTEGER Frequency;
    if(QueryPerformanceFrequency(&Frequency)) {
        LARGE_INTEGER PerformanceCount;
        if(QueryPerformanceCounter(&PerformanceCount)) {
            return (uint32_t)(PerformanceCount.QuadPart * 1000 / Frequency.QuadPart);
        }
    }

    return (uint32_t)GetTickCount64();
}

static void lv_windows_delay_callback(uint32_t ms)
{
    HANDLE timer_handle = CreateWaitableTimerExW(
                              NULL,
                              NULL,
                              CREATE_WAITABLE_TIMER_MANUAL_RESET |
                              CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                              TIMER_ALL_ACCESS);
    if(timer_handle) {
        LARGE_INTEGER due_time;
        due_time.QuadPart = -((int64_t)ms) * 1000 * 10;
        SetWaitableTimer(timer_handle, &due_time, 0, NULL, NULL, FALSE);
        WaitForSingleObject(timer_handle, INFINITE);

        CloseHandle(timer_handle);
    }
}

static void lv_windows_check_display_existence_timer_callback(
    lv_timer_t * timer)
{
    LV_UNUSED(timer);
    if(!lv_display_get_next(NULL)) {
        // Don't use lv_deinit() due to it will cause exception when parallel
        // rendering is enabled.
        exit(0);
    }
}

static HDC lv_windows_create_frame_buffer(
    HWND window_handle,
    LONG width,
    LONG height,
    UINT32 ** pixel_buffer,
    SIZE_T * pixel_buffer_size)
{
    HDC frame_buffer_dc_handle = NULL;

    LV_ASSERT_NULL(pixel_buffer);
    LV_ASSERT_NULL(pixel_buffer_size);

    HDC window_dc_handle = GetDC(window_handle);
    if(window_dc_handle) {
        frame_buffer_dc_handle = CreateCompatibleDC(window_dc_handle);
        ReleaseDC(window_handle, window_dc_handle);
    }

    if(frame_buffer_dc_handle) {
#if (LV_COLOR_DEPTH == 32) || (LV_COLOR_DEPTH == 24)
        BITMAPINFO bitmap_info = { 0 };
#elif (LV_COLOR_DEPTH == 16)
        typedef struct _BITMAPINFO_16BPP {
            BITMAPINFOHEADER bmiHeader;
            DWORD bmiColorMask[3];
        } BITMAPINFO_16BPP;

        BITMAPINFO_16BPP bitmap_info = { 0 };
#else
#error [lv_windows] Unsupported LV_COLOR_DEPTH.
#endif

        bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bitmap_info.bmiHeader.biWidth = width;
        bitmap_info.bmiHeader.biHeight = -height;
        bitmap_info.bmiHeader.biPlanes = 1;
        bitmap_info.bmiHeader.biBitCount = lv_color_format_get_bpp(
                                               LV_COLOR_FORMAT_NATIVE);
#if (LV_COLOR_DEPTH == 32) || (LV_COLOR_DEPTH == 24)
        bitmap_info.bmiHeader.biCompression = BI_RGB;
#elif (LV_COLOR_DEPTH == 16)
        bitmap_info.bmiHeader.biCompression = BI_BITFIELDS;
        bitmap_info.bmiColorMask[0] = 0xF800;
        bitmap_info.bmiColorMask[1] = 0x07E0;
        bitmap_info.bmiColorMask[2] = 0x001F;
#else
#error [lv_windows] Unsupported LV_COLOR_DEPTH.
#endif

        HBITMAP hBitmap = CreateDIBSection(
                              frame_buffer_dc_handle,
                              (PBITMAPINFO)(&bitmap_info),
                              DIB_RGB_COLORS,
                              (void **)pixel_buffer,
                              NULL,
                              0);
        if(hBitmap) {
            *pixel_buffer_size = width * height;
            *pixel_buffer_size *= lv_color_format_get_size(
                                      LV_COLOR_FORMAT_NATIVE);

            DeleteObject(SelectObject(frame_buffer_dc_handle, hBitmap));
            DeleteObject(hBitmap);
        }
        else {
            DeleteDC(frame_buffer_dc_handle);
            frame_buffer_dc_handle = NULL;
        }
    }

    return frame_buffer_dc_handle;
}

static void lv_windows_display_timer_callback(lv_timer_t * timer)
{
    lv_windows_window_context_t * context = lv_timer_get_user_data(timer);
    LV_ASSERT_NULL(context);

    if(!context->display_resolution_changed) {
        return;
    }

    lv_display_set_resolution(
        context->display_device_object,
        context->requested_display_resolution.x,
        context->requested_display_resolution.y);

    int32_t hor_res = lv_display_get_horizontal_resolution(
                          context->display_device_object);
    int32_t ver_res = lv_display_get_vertical_resolution(
                          context->display_device_object);

    HWND window_handle = lv_windows_get_display_window_handle(
                             context->display_device_object);
    if(window_handle) {
        if(context->display_framebuffer_context_handle) {
            context->display_framebuffer_base = NULL;
            context->display_framebuffer_size = 0;
            DeleteDC(context->display_framebuffer_context_handle);
            context->display_framebuffer_context_handle = NULL;
        }

        context->display_framebuffer_context_handle =
            lv_windows_create_frame_buffer(
                window_handle,
                hor_res,
                ver_res,
                &context->display_framebuffer_base,
                &context->display_framebuffer_size);
        if(context->display_framebuffer_context_handle) {
            lv_display_set_buffers(
                context->display_device_object,
                context->display_framebuffer_base,
                NULL,
                (uint32_t)context->display_framebuffer_size,
                LV_DISPLAY_RENDER_MODE_DIRECT);
        }
    }

    context->display_resolution_changed = false;
    context->requested_display_resolution.x = 0;
    context->requested_display_resolution.y = 0;
}

static void lv_windows_display_driver_flush_callback(
    lv_display_t * display,
    const lv_area_t * area,
    uint8_t * px_map)
{
    LV_UNUSED(area);

    HWND window_handle = lv_windows_get_display_window_handle(display);
    if(!window_handle) {
        lv_display_flush_ready(display);
        return;
    }

    lv_windows_window_context_t * context = lv_windows_get_window_context(
                                                window_handle);
    if(!context) {
        lv_display_flush_ready(display);
        return;
    }

    if(lv_display_flush_is_last(display)) {
#if (LV_COLOR_DEPTH == 32) || \
    (LV_COLOR_DEPTH == 24) || \
    (LV_COLOR_DEPTH == 16)
        UNREFERENCED_PARAMETER(px_map);
#else
#error [lv_windows] Unsupported LV_COLOR_DEPTH.
#endif

        HDC hdc = GetDC(window_handle);
        if(hdc) {
            SetStretchBltMode(hdc, HALFTONE);

            RECT client_rect;
            GetClientRect(window_handle, &client_rect);

            int32_t width = lv_windows_zoom_to_logical(
                                client_rect.right - client_rect.left,
                                context->zoom_level);
            int32_t height = lv_windows_zoom_to_logical(
                                 client_rect.bottom - client_rect.top,
                                 context->zoom_level);
            if(context->simulator_mode) {
                width = lv_windows_dpi_to_logical(width, context->window_dpi);
                height = lv_windows_dpi_to_logical(height, context->window_dpi);
            }

            StretchBlt(
                hdc,
                client_rect.left,
                client_rect.top,
                client_rect.right - client_rect.left,
                client_rect.bottom - client_rect.top,
                context->display_framebuffer_context_handle,
                0,
                0,
                width,
                height,
                SRCCOPY);

            ReleaseDC(window_handle, hdc);
        }
    }

    lv_display_flush_ready(display);
}

static UINT lv_windows_get_dpi_for_window(HWND window_handle)
{
    UINT result = (UINT)(-1);

    HMODULE module_handle = LoadLibraryW(L"SHCore.dll");
    if(module_handle) {
        typedef enum MONITOR_DPI_TYPE_PRIVATE {
            MDT_EFFECTIVE_DPI = 0,
            MDT_ANGULAR_DPI = 1,
            MDT_RAW_DPI = 2,
            MDT_DEFAULT = MDT_EFFECTIVE_DPI
        } MONITOR_DPI_TYPE_PRIVATE;

        typedef HRESULT(WINAPI * function_type)(
            HMONITOR, MONITOR_DPI_TYPE_PRIVATE, UINT *, UINT *);

        function_type function = (function_type)(
                                     GetProcAddress(module_handle, "GetDpiForMonitor"));
        if(function) {
            HMONITOR MonitorHandle = MonitorFromWindow(
                                         window_handle,
                                         MONITOR_DEFAULTTONEAREST);

            UINT dpiX = 0;
            UINT dpiY = 0;
            if(SUCCEEDED(function(
                             MonitorHandle,
                             MDT_EFFECTIVE_DPI,
                             &dpiX,
                             &dpiY))) {
                result = dpiX;
            }
        }

        FreeLibrary(module_handle);
    }

    if(result == (UINT)(-1)) {
        HDC hWindowDC = GetDC(window_handle);
        if(hWindowDC) {
            result = GetDeviceCaps(hWindowDC, LOGPIXELSX);
            ReleaseDC(window_handle, hWindowDC);
        }
    }

    if(result == (UINT)(-1)) {
        result = USER_DEFAULT_SCREEN_DPI;
    }

    return result;
}

static BOOL lv_windows_register_touch_window(
    HWND window_handle,
    ULONG flags)
{
    HMODULE module_handle = GetModuleHandleW(L"user32.dll");
    if(!module_handle) {
        return FALSE;
    }

    typedef BOOL(WINAPI * function_type)(HWND, ULONG);

    function_type function = (function_type)(
                                 GetProcAddress(module_handle, "RegisterTouchWindow"));
    if(!function) {
        return FALSE;
    }

    return function(window_handle, flags);
}

static BOOL lv_windows_enable_child_window_dpi_message(
    HWND WindowHandle)
{
    // The private Per-Monitor DPI Awareness support extension is Windows 10
    // only. We don't need the private Per-Monitor DPI Awareness support
    // extension if the Per-Monitor (V2) DPI Awareness exists.
    OSVERSIONINFOEXW os_version_info_ex = { 0 };
    os_version_info_ex.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
    os_version_info_ex.dwMajorVersion = 10;
    os_version_info_ex.dwMinorVersion = 0;
    os_version_info_ex.dwBuildNumber = 14986;
    if(!VerifyVersionInfoW(
           &os_version_info_ex,
           VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER,
           VerSetConditionMask(
               VerSetConditionMask(
                   VerSetConditionMask(
                       0,
                       VER_MAJORVERSION,
                       VER_GREATER_EQUAL),
                   VER_MINORVERSION,
                   VER_GREATER_EQUAL),
               VER_BUILDNUMBER,
               VER_LESS))) {
        return FALSE;
    }

    HMODULE module_handle = GetModuleHandleW(L"user32.dll");
    if(!module_handle) {
        return FALSE;
    }

    typedef BOOL(WINAPI * function_type)(HWND, BOOL);

    function_type function = (function_type)(
                                 GetProcAddress(module_handle, "EnableChildWindowDpiMessage"));
    if(!function) {
        return FALSE;
    }

    return function(WindowHandle, TRUE);
}

static bool lv_windows_window_message_callback_nolock(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam,
    LRESULT * plResult)
{
    switch(uMsg) {
        case WM_CREATE: {
                // Note: Return -1 directly because WM_DESTROY message will be sent
                // when destroy the window automatically. We free the resource when
                // processing the WM_DESTROY message of this window.

                lv_windows_create_display_data_t * data =
                    (lv_windows_create_display_data_t *)(
                        ((LPCREATESTRUCTW)(lParam))->lpCreateParams);
                if(!data) {
                    return -1;
                }

                lv_windows_window_context_t * context =
                    (lv_windows_window_context_t *)(HeapAlloc(
                                                        GetProcessHeap(),
                                                        HEAP_ZERO_MEMORY,
                                                        sizeof(lv_windows_window_context_t)));
                if(!context) {
                    return -1;
                }

                if(!SetPropW(hWnd, L"LVGL.Window.Context", (HANDLE)(context))) {
                    return -1;
                }

                context->window_dpi = lv_windows_get_dpi_for_window(hWnd);
                context->zoom_level = data->zoom_level;
                context->allow_dpi_override = data->allow_dpi_override;
                context->simulator_mode = data->simulator_mode;

                context->display_timer_object = lv_timer_create(
                                                    lv_windows_display_timer_callback,
                                                    LV_DEF_REFR_PERIOD,
                                                    context);

                context->display_resolution_changed = false;
                context->requested_display_resolution.x = 0;
                context->requested_display_resolution.y = 0;

                context->display_device_object = lv_display_create(0, 0);
                if(!context->display_device_object) {
                    return -1;
                }
                RECT request_content_size;
                GetWindowRect(hWnd, &request_content_size);
                lv_display_set_resolution(
                    context->display_device_object,
                    request_content_size.right - request_content_size.left,
                    request_content_size.bottom - request_content_size.top);
                lv_display_set_flush_cb(
                    context->display_device_object,
                    lv_windows_display_driver_flush_callback);
                lv_display_set_driver_data(
                    context->display_device_object,
                    hWnd);
                if(!context->allow_dpi_override) {
                    lv_display_set_dpi(
                        context->display_device_object,
                        context->window_dpi);
                }

                if(context->simulator_mode) {
                    context->display_resolution_changed = true;
                    context->requested_display_resolution.x =
                        lv_display_get_horizontal_resolution(
                            context->display_device_object);
                    context->requested_display_resolution.y =
                        lv_display_get_vertical_resolution(
                            context->display_device_object);
                }

                lv_windows_register_touch_window(hWnd, 0);

                lv_windows_enable_child_window_dpi_message(hWnd);

                break;
            }
        case WM_SIZE: {
                if(wParam != SIZE_MINIMIZED) {
                    lv_windows_window_context_t * context = (lv_windows_window_context_t *)(
                                                                lv_windows_get_window_context(hWnd));
                    if(context) {
                        if(!context->simulator_mode) {
                            context->display_resolution_changed = true;
                            context->requested_display_resolution.x = LOWORD(lParam);
                            context->requested_display_resolution.y = HIWORD(lParam);
                        }
                        else {
                            int32_t window_width = lv_windows_dpi_to_physical(
                                                       lv_windows_zoom_to_physical(
                                                           lv_display_get_horizontal_resolution(
                                                               context->display_device_object),
                                                           context->zoom_level),
                                                       context->window_dpi);
                            int32_t window_height = lv_windows_dpi_to_physical(
                                                        lv_windows_zoom_to_physical(
                                                            lv_display_get_vertical_resolution(
                                                                context->display_device_object),
                                                            context->zoom_level),
                                                        context->window_dpi);

                            RECT window_rect;
                            GetWindowRect(hWnd, &window_rect);

                            RECT client_rect;
                            GetClientRect(hWnd, &client_rect);

                            int32_t original_window_width =
                                window_rect.right - window_rect.left;
                            int32_t original_window_height =
                                window_rect.bottom - window_rect.top;

                            int32_t original_client_width =
                                client_rect.right - client_rect.left;
                            int32_t original_client_height =
                                client_rect.bottom - client_rect.top;

                            int32_t reserved_width =
                                original_window_width - original_client_width;
                            int32_t reserved_height =
                                original_window_height - original_client_height;

                            SetWindowPos(
                                hWnd,
                                NULL,
                                0,
                                0,
                                reserved_width + window_width,
                                reserved_height + window_height,
                                SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
                        }
                    }
                }
                break;
            }
        case WM_DPICHANGED: {
                lv_windows_window_context_t * context = (lv_windows_window_context_t *)(
                                                            lv_windows_get_window_context(hWnd));
                if(context) {
                    context->window_dpi = HIWORD(wParam);

                    if(!context->allow_dpi_override) {
                        lv_display_set_dpi(
                            context->display_device_object,
                            context->window_dpi);
                    }

                    LPRECT suggested_rect = (LPRECT)lParam;

                    SetWindowPos(
                        hWnd,
                        NULL,
                        suggested_rect->left,
                        suggested_rect->top,
                        suggested_rect->right,
                        suggested_rect->bottom,
                        SWP_NOZORDER | SWP_NOACTIVATE);
                }

                break;
            }
        case WM_ERASEBKGND: {
                return TRUE;
            }
        case WM_DESTROY: {
                lv_windows_window_context_t * context = (lv_windows_window_context_t *)(
                                                            RemovePropW(hWnd, L"LVGL.Window.Context"));
                if(context) {
                    lv_display_t * display_device_object =
                        context->display_device_object;
                    context->display_device_object = NULL;
                    lv_display_delete(display_device_object);
                    DeleteDC(context->display_framebuffer_context_handle);

                    lv_timer_delete(context->display_timer_object);

                    HeapFree(GetProcessHeap(), 0, context);
                }

                PostQuitMessage(0);

                break;
            }
        default: {
                lv_windows_window_context_t * context = (lv_windows_window_context_t *)(
                                                            lv_windows_get_window_context(hWnd));
                if(context) {
                    if(context->pointer.indev &&
                       lv_windows_pointer_device_window_message_handler(
                           hWnd,
                           uMsg,
                           wParam,
                           lParam,
                           plResult)) {
                        // Handled
                        return true;
                    }
                    else if(context->keypad.indev &&
                            lv_windows_keypad_device_window_message_handler(
                                hWnd,
                                uMsg,
                                wParam,
                                lParam,
                                plResult)) {
                        // Handled
                        return true;
                    }
                    else if(context->encoder.indev &&
                            lv_windows_encoder_device_window_message_handler(
                                hWnd,
                                uMsg,
                                wParam,
                                lParam,
                                plResult)) {
                        // Handled
                        return true;
                    }
                }

                // Not Handled
                return false;
            }
    }

    // Handled
    *plResult = 0;
    return true;
}

static LRESULT CALLBACK lv_windows_window_message_callback(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    lv_lock();

    LRESULT lResult = 0;
    bool Handled = lv_windows_window_message_callback_nolock(
                       hWnd,
                       uMsg,
                       wParam,
                       lParam,
                       &lResult);

    lv_unlock();

    return Handled ? lResult : DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

#endif // LV_USE_WINDOWS
