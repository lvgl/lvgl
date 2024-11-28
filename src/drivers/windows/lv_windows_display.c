/**
 * @file lv_windows_display.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_windows_display.h"
#if LV_USE_WINDOWS

#include "lv_windows_context.h"

#include <process.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static unsigned int __stdcall lv_windows_display_thread_entrypoint(
    void * parameter);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_windows_create_display(
    const wchar_t * title,
    int32_t hor_res,
    int32_t ver_res,
    int32_t zoom_level,
    bool allow_dpi_override,
    bool simulator_mode,
    bool top_level,
    bool frameless)
{
    lv_windows_create_display_data_t data;

    lv_memzero(&data, sizeof(lv_windows_create_display_data_t));
    data.title = title;
    data.hor_res = hor_res;
    data.ver_res = ver_res;
    data.zoom_level = zoom_level;
    data.allow_dpi_override = allow_dpi_override;
    data.simulator_mode = simulator_mode;
    data.top_level = top_level;
    data.frameless = frameless;
    data.mutex = CreateEventExW(NULL, NULL, 0, EVENT_ALL_ACCESS);
    data.display = NULL;
    if(!data.mutex) {
        return NULL;
    }

    HANDLE thread = (HANDLE)_beginthreadex(
                        NULL,
                        0,
                        lv_windows_display_thread_entrypoint,
                        &data,
                        0,
                        NULL);
    LV_ASSERT(thread);

    WaitForSingleObjectEx(data.mutex, INFINITE, FALSE);

    if(thread) {
        CloseHandle(thread);
    }

    if(data.mutex) {
        CloseHandle(data.mutex);
    }

    return data.display;
}

HWND lv_windows_get_display_window_handle(lv_display_t * display)
{
    return (HWND)lv_display_get_driver_data(display);
}

int32_t lv_windows_zoom_to_logical(int32_t physical, int32_t zoom_level)
{
    return MulDiv(physical, LV_WINDOWS_ZOOM_BASE_LEVEL, zoom_level);
}

int32_t lv_windows_zoom_to_physical(int32_t logical, int32_t zoom_level)
{
    return MulDiv(logical, zoom_level, LV_WINDOWS_ZOOM_BASE_LEVEL);
}

int32_t lv_windows_dpi_to_logical(int32_t physical, int32_t dpi)
{
    return MulDiv(physical, USER_DEFAULT_SCREEN_DPI, dpi);
}

int32_t lv_windows_dpi_to_physical(int32_t logical, int32_t dpi)
{
    return MulDiv(logical, dpi, USER_DEFAULT_SCREEN_DPI);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void __stdcall lv_windows_center_window(HWND hwnd)
{
    RECT rect = {0};
    GetWindowRect(hwnd, &rect);

    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;

    RECT screenRect = {0};
    SystemParametersInfo(SPI_GETWORKAREA, 0, &screenRect, 0);

    int screenWidth = screenRect.right - screenRect.left;
    int screenHeight = screenRect.bottom - screenRect.top;

    int x = (screenWidth - windowWidth) / 2;
    int y = (screenHeight - windowHeight) / 2;

    SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
}

static unsigned int __stdcall lv_windows_display_thread_entrypoint(
    void * parameter)
{
    lv_windows_create_display_data_t * data = parameter;
    LV_ASSERT_NULL(data);

    DWORD window_style = WS_OVERLAPPEDWINDOW;
    if(data->simulator_mode) {
        window_style &= ~(WS_SIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);
    }

    if(data->frameless) {
        window_style = WS_POPUPWINDOW;
    }

    // WS_EX_TOPMOST
    // Makes the window always stay above other non-topmost windows. Even if the window loses focus, it remains on top.

    // WS_EX_APPWINDOW
    // Forces the window to appear in the taskbar. Typically used for top-level windows to ensure they are displayed in the taskbar.

    DWORD window_ex_style =  WS_EX_APPWINDOW;
    if(data->top_level) {
        window_ex_style |= WS_EX_TOPMOST;
    }

    HWND window_handle = CreateWindowExW(
                             window_ex_style,
                             L"LVGL.Window",
                             data->title,
                             window_style,
                             0,
                             0,
                             data->hor_res,
                             data->ver_res,
                             NULL,
                             NULL,
                             NULL,
                             data);
    if(!window_handle) {
        return 0;
    }

    lv_windows_window_context_t * context = lv_windows_get_window_context(
                                                window_handle);
    if(!context) {
        return 0;
    }

    data->display = context->display_device_object;

    lv_windows_center_window(window_handle);
    ShowWindow(window_handle, SW_SHOW);
    UpdateWindow(window_handle);

    LV_ASSERT(SetEvent(data->mutex));

    data = NULL;

    MSG message;
    while(GetMessageW(&message, NULL, 0, 0)) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    return 0;
}

#endif // LV_USE_WINDOWS
