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

/**********************
 *   STATIC FUNCTIONS
 **********************/

static unsigned int __stdcall lv_windows_display_thread_entrypoint(
    void * parameter)
{
    lv_windows_create_display_data_t * data = parameter;
    LV_ASSERT_NULL(data);

    DWORD window_style = WS_OVERLAPPEDWINDOW;
    DWORD ext_window_style = WS_EX_CLIENTEDGE;
    RECT rect = { 0, 0, data->hor_res, data->ver_res };

    if(data->simulator_mode) {
        window_style &= ~(WS_SIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);
    }
    else {
        /* Have Windows compute window size so, regardless of window style,
         * the CLIENT AREA has dimensions [data->hor_res, data->ver_res].
         * This is the area needed for LVGL to render to. */
        AdjustWindowRectEx(&rect, window_style, false, ext_window_style);
    }

    HWND window_handle = CreateWindowExW(
                             ext_window_style,
                             L"LVGL.Window",
                             data->title,
                             window_style,
                             CW_USEDEFAULT,
                             0,
                             rect.right - rect.left,
                             rect.bottom - rect.top,
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
