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
    bool simulator_mode)
{
    lv_windows_create_display_data_t data;

    lv_memzero(&data, sizeof(lv_windows_create_display_data_t));
    data.title = title;
    data.hor_res = hor_res;
    data.ver_res = ver_res;
    data.zoom_level = zoom_level;
    data.allow_dpi_override = allow_dpi_override;
    data.simulator_mode = simulator_mode;
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

void lv_windows_set_top_level(lv_display_t * display, bool top_level)
{
    HWND window_handle = lv_windows_get_display_window_handle(display);

    LONG_PTR ex_style = GetWindowLongPtr(window_handle, GWL_EXSTYLE);
    if(top_level) {
        ex_style |= WS_EX_TOPMOST;
        ex_style |= WS_EX_APPWINDOW;
    }
    else {
        ex_style &= ~WS_EX_TOPMOST;
    }

    SetWindowLongPtr(window_handle, GWL_EXSTYLE, ex_style);
    SetWindowPos(
        window_handle,
        top_level ? HWND_TOPMOST : HWND_NOTOPMOST,
        0,
        0,
        0,
        0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

void lv_windows_set_frameless(lv_display_t * display, bool frameless)
{
    HWND window_handle = lv_windows_get_display_window_handle(display);

    LONG_PTR style = GetWindowLongPtr(window_handle, GWL_STYLE);
    if(frameless) {
        style &= ~WS_OVERLAPPEDWINDOW;
        style |= WS_POPUPWINDOW;
    }
    else {
        style &= ~WS_POPUP;
        style |= WS_OVERLAPPEDWINDOW;
    }

    SetWindowLongPtr(window_handle, GWL_STYLE, style);
    SetWindowPos(
        window_handle,
        NULL,
        0,
        0,
        0,
        0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int32_t lv_windows_center_window(HWND window_handle)
{
    RECT rect = {0};
    if(!GetWindowRect(window_handle, &rect)) {
        return 0;
    }

    int window_width = rect.right - rect.left;
    int window_height = rect.bottom - rect.top;

    RECT screen_rect = {0};
    if(!SystemParametersInfo(SPI_GETWORKAREA, 0, &screen_rect, 0)) {
        return 0;
    }

    int screen_width = screen_rect.right - screen_rect.left;
    int screen_height = screen_rect.bottom - screen_rect.top;

    int x = (screen_width - window_width) / 2;
    int y = (screen_height - window_height) / 2;

    if(!SetWindowPos(
           window_handle,
           NULL,
           x,
           y,
           0,
           0,
           SWP_NOZORDER | SWP_NOSIZE)) {
        return 0;
    }

    return 1;
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

    HWND window_handle = CreateWindowExW(
                             WS_EX_APPWINDOW,
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

    LV_ASSERT(lv_windows_center_window(window_handle));

    lv_windows_window_context_t * context = lv_windows_get_window_context(
                                                window_handle);
    if(!context) {
        return 0;
    }

    data->display = context->display_device_object;

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
