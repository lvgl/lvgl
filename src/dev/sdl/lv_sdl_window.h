/**
 * @file lv_sdl_window.h
 *
 */

#ifndef LV_SDL_DISP_H
#define LV_SDL_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../disp/lv_disp.h"
#include "../../indev/lv_indev.h"

#if LV_USE_SDL

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_SDL_WINDOW_FLASH_CANCEL,                   /**< Cancel any window flash state */
    LV_SDL_WINDOW_FLASH_BRIEFLY,                  /**< Flash the window briefly to get attention */
    LV_SDL_WINDOW_FLASH_UNTIL_FOCUSED             /**< Flash the window until it gets focus */
} lv_sdl_window_flash_t;

typedef enum {
    LV_SDL_WINDOW_FLAGS_FULLSCREEN = 0x00000001,         /**< fullscreen window */
    LV_SDL_WINDOW_FLAGS_OPENGL = 0x00000002,             /**< window usable with OpenGL context */
    LV_SDL_WINDOW_FLAGS_SHOWN = 0x00000004,              /**< window is visible */
    LV_SDL_WINDOW_FLAGS_HIDDEN = 0x00000008,             /**< window is not visible */
    LV_SDL_WINDOW_FLAGS_BORDERLESS = 0x00000010,         /**< no window decoration */
    LV_SDL_WINDOW_FLAGS_RESIZABLE = 0x00000020,          /**< window can be resized */
    LV_SDL_WINDOW_FLAGS_MINIMIZED = 0x00000040,          /**< window is minimized */
    LV_SDL_WINDOW_FLAGS_MAXIMIZED = 0x00000080,          /**< window is maximized */
    LV_SDL_WINDOW_FLAGS_MOUSE_GRABBED = 0x00000100,      /**< window has grabbed mouse input */
    LV_SDL_WINDOW_FLAGS_INPUT_FOCUS = 0x00000200,        /**< window has input focus */
    LV_SDL_WINDOW_FLAGS_MOUSE_FOCUS = 0x00000400,        /**< window has mouse focus */
    LV_SDL_WINDOW_FLAGS_FULLSCREEN_DESKTOP = (LV_SDL_WINDOW_FLAGS_FULLSCREEN | 0x00001000),
    // LV_SDL_WINDOW_FLAGS_FOREIGN = 0x00000800,            /**< window not created by SDL */
    LV_SDL_WINDOW_FLAGS_ALLOW_HIGHDPI = 0x00002000,      /**< window should be created in high-DPI mode if supported.
                                                     On macOS NSHighResolutionCapable must be set true in the
                                                     application's Info.plist for this to have any effect. */
    LV_SDL_WINDOW_FLAGS_MOUSE_CAPTURE    = 0x00004000,   /**< window has mouse captured (unrelated to MOUSE_GRABBED) */
    LV_SDL_WINDOW_FLAGS_ALWAYS_ON_TOP    = 0x00008000,   /**< window should always be above others */
    LV_SDL_WINDOW_FLAGS_SKIP_TASKBAR     = 0x00010000,   /**< window should not be added to the taskbar */
    LV_SDL_WINDOW_FLAGS_UTILITY          = 0x00020000,   /**< window should be treated as a utility window */
    LV_SDL_WINDOW_FLAGS_TOOLTIP          = 0x00040000,   /**< window should be treated as a tooltip */
    LV_SDL_WINDOW_FLAGS_POPUP_MENU       = 0x00080000,   /**< window should be treated as a popup menu */
    LV_SDL_WINDOW_FLAGS_KEYBOARD_GRABBED = 0x00100000,   /**< window has grabbed keyboard input */
    LV_SDL_WINDOW_FLAGS_VULKAN           = 0x10000000,   /**< window usable for Vulkan surface */
    LV_SDL_WINDOW_FLAGS_METAL            = 0x20000000,   /**< window usable for Metal view */

    LV_SDL_WINDOW_FLAGS_CENTERED         = 0x01000000,   /**< window is centered on desktop */

    LV_SDL_WINDOW_FLAGS_INPUT_GRABBED = LV_SDL_WINDOW_FLAGS_MOUSE_GRABBED, /**< equivalent to SDL_WINDOW_MOUSE_GRABBED for compatibility */
    LV_SDL_WINDOW_FLAGS_DEFAULT = (LV_SDL_WINDOW_FLAGS_CENTERED | LV_SDL_WINDOW_FLAGS_RESIZABLE | LV_SDL_WINDOW_FLAGS_SHOWN) /**< window flags default value */
} lv_sdl_window_flags_t;

typedef enum {
    LV_SDL_FULLSCREEN_FLAGS_DESKTOP = LV_SDL_WINDOW_FLAGS_FULLSCREEN_DESKTOP,
    LV_SDL_FULLSCREEN_FLAGS_DEFAULT = LV_SDL_WINDOW_FLAGS_FULLSCREEN
} lv_sdl_fullscreen_flags_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

const char * lv_sdl_window_get_title(lv_disp_t * disp);

void lv_sdl_window_set_pos(lv_disp_t * disp, lv_coord_t x, lv_coord_t y);

lv_coord_t lv_sdl_window_get_x(lv_disp_t * disp);

lv_coord_t lv_sdl_window_get_y(lv_disp_t * disp);

lv_coord_t lv_sdl_window_get_left_border_width(lv_disp_t * disp);

lv_coord_t lv_sdl_window_get_right_border_width(lv_disp_t * disp);

lv_coord_t lv_sdl_window_get_top_border_width(lv_disp_t * disp);

lv_coord_t lv_sdl_window_get_bottom_border_width(lv_disp_t * disp);

void lv_sdl_window_set_minimum_size(lv_disp_t * disp, lv_coord_t width, lv_coord_t height);

lv_coord_t lv_sdl_window_get_minimum_width(lv_disp_t * disp);

lv_coord_t lv_sdl_window_get_minimum_height(lv_disp_t * disp);

void lv_sdl_window_set_maximum_size(lv_disp_t * disp, lv_coord_t width, lv_coord_t height);

lv_coord_t lv_sdl_window_get_maximum_width(lv_disp_t * disp);

lv_coord_t lv_sdl_window_get_maximum_height(lv_disp_t * disp);

void lv_sdl_window_set_bordered(lv_disp_t * disp, bool value);

void lv_sdl_window_set_resizeable(lv_disp_t * disp, bool value);

void lv_sdl_window_set_always_on_top(lv_disp_t * disp, bool value);

void lv_sdl_window_set_show(lv_disp_t * disp, bool value);

void lv_sdl_window_raise(lv_disp_t * disp);

void lv_sdl_window_maximize(lv_disp_t * disp);

void lv_sdl_window_minimize(lv_disp_t * disp);

void lv_sdl_window_restore(lv_disp_t * disp);

void lv_sdl_window_fullscreen(lv_disp_t * disp);

void lv_sdl_window_set_brightness(lv_disp_t * disp, uint8_t level);

uint8_t lv_sdl_window_get_brightness(lv_disp_t * disp);

void lv_sdl_window_set_opacity(lv_disp_t * disp, lv_opa_t opa);

lv_opa_t lv_sdl_window_get_opacity(lv_disp_t * disp);

void lv_sdl_window_flash(lv_disp_t * disp, lv_sdl_window_flash_t flash);

lv_disp_t * lv_sdl_window_create(lv_coord_t hor_res, lv_coord_t ver_res, lv_sdl_window_flags_t flags);

void lv_sdl_window_set_zoom(lv_disp_t * disp, uint8_t zoom);

uint8_t lv_sdl_window_get_zoom(lv_disp_t * disp);

lv_disp_t * _lv_sdl_get_disp_from_win_id(uint32_t win_id);

void lv_sdl_window_set_title(lv_disp_t * disp, const char * title);

void lv_sdl_quit();

/**********************
 *      MACROS
 **********************/

#endif /* LV_DRV_SDL */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_SDL_DISP_H */
