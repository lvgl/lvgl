/**
 * @file lv_egl_adapter_outmod_wayland.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#if LV_USE_LINUX_WAYLAND && LV_LINUX_WAYLAND_USE_EGL

#include "lv_egl_adapter_outmod_wayland.h"
#include "../../opengles/egl_adapter/private/lv_egl_adapter_output_core.h"

#include <linux/input.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#include <wayland-cursor.h>
#include "xdg-shell-client-protocol.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

struct my_output {
    struct wl_output * output;
    int32_t width;
    int32_t height;
    int32_t refresh;
    int32_t scale;
};

typedef struct my_output * OutputsVector;

typedef struct lv_egl_adapter_outmod_wayland {
    lv_egl_adapter_output_core_t core;

    struct my_display {
        struct wl_display * display;
        struct wl_registry * registry;
        struct wl_compositor * compositor;
        struct wl_shm * shm;
        struct wl_seat * seat;
        struct wl_pointer * pointer;
        struct wl_keyboard * keyboard;
        struct xdg_wm_base * xdg_wm_base;
        OutputsVector outputs;
        size_t outputs_count;  // Count of added outputs
    } * display_;

    struct my_cursor {
        struct wl_cursor_theme * cursor_theme;
        struct wl_cursor * default_cursor;
        struct wl_surface * cursor_surface;
    } * cursor_;

    struct my_window {
        struct native_window_properties properties;
        bool waiting_for_configure;
        struct wl_surface * surface;
        struct wl_egl_window * native;
        struct xdg_surface * xdg_surface;
        struct xdg_toplevel * xdg_toplevel;
    } * window_;

    volatile bool should_quit_;
} lv_egl_adapter_outmod_wayland;


/**********************
 *  STATIC PROTOTYPES
 **********************/

static void populate_output_core(void * outmod_ptr);
static void pointer_handle_enter(void * data, struct wl_pointer * pointer, uint32_t serial, struct wl_surface * surface,
                                 wl_fixed_t sx, wl_fixed_t sy);
static void pointer_handle_leave(void * data, struct wl_pointer * pointer, uint32_t serial,
                                 struct wl_surface * surface);
static void pointer_handle_motion(void * data, struct wl_pointer * pointer, uint32_t time, wl_fixed_t sx,
                                  wl_fixed_t sy);
static void pointer_handle_button(void * data, struct wl_pointer * wl_pointer, uint32_t serial, uint32_t time,
                                  uint32_t button,
                                  uint32_t state);
static void pointer_handle_axis(void * data, struct wl_pointer * wl_pointer, uint32_t time, uint32_t axis,
                                wl_fixed_t value);
static void keyboard_handle_keymap(void * data, struct wl_keyboard * keyboard, uint32_t format, int fd, uint32_t size);
static void keyboard_handle_enter(void * data, struct wl_keyboard * keyboard, uint32_t serial,
                                  struct wl_surface * surface,
                                  struct wl_array * keys);
static void keyboard_handle_leave(void * data, struct wl_keyboard * keyboard, uint32_t serial,
                                  struct wl_surface * surface);
static void keyboard_handle_key(void * data, struct wl_keyboard * keyboard, uint32_t serial, uint32_t time,
                                uint32_t key,
                                uint32_t state);
static void keyboard_handle_modifiers(void * data, struct wl_keyboard * keyboard, uint32_t serial,
                                      uint32_t mods_depressed,
                                      uint32_t mods_latched, uint32_t mods_locked, uint32_t group);
static void seat_handle_capabilities(void * data, struct wl_seat * seat, uint32_t caps);
static void xdg_wm_base_handle_ping(void * data, struct xdg_wm_base * xdg_wm_base, uint32_t serial);
static void output_handle_geometry(void * data, struct wl_output * wl_output,
                                   int32_t x, int32_t y, int32_t physical_width,
                                   int32_t physical_height, int32_t subpixel,
                                   const char * make, const char * model,
                                   int32_t transform);
static void output_handle_mode(void * data, struct wl_output * wl_output,
                               uint32_t flags, int32_t width, int32_t height,
                               int32_t refresh);
static void output_handle_done(void * data, struct wl_output * wl_output);
static void output_handle_scale(void * data, struct wl_output * wl_output,
                                int32_t factor);
// Define the wl_registry_listener
static void registry_handle_global(void * data, struct wl_registry * registry,
                                   uint32_t id, const char * interface,
                                   uint32_t version);
static void registry_handle_global_remove(void * data, struct wl_registry * registry, uint32_t name);
static void xdg_toplevel_handle_configure(void * data, struct xdg_toplevel * xdg_toplevel,
                                          int32_t width, int32_t height,
                                          struct wl_array * states);
static void xdg_toplevel_handle_close(void * data, struct xdg_toplevel * xdg_toplevel);
static void xdg_surface_handle_configure(void * data, struct xdg_surface * xdg_surface, uint32_t serial);

/**********************
*  STATIC VARIABLES
**********************/

static const struct wl_pointer_listener pointer_listener;
static const struct wl_keyboard_listener keyboard_listener;
static const struct wl_seat_listener seat_listener;
static const struct xdg_wm_base_listener xdg_wm_base_listener;
static const struct wl_output_listener output_listener;
static const struct wl_registry_listener registry_listener;
static const struct xdg_toplevel_listener xdg_toplevel_listener;
static const struct xdg_surface_listener xdg_surface_listener;
static volatile bool should_quit = false;
static struct my_output * my_main_output;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

struct my_output * get_output(void * void_self, size_t index)
{
    lv_egl_adapter_outmod_wayland_t state = (lv_egl_adapter_outmod_wayland_t)void_self;
    if(index < state->display_->outputs_count) {
        return &(state->display_->outputs[index]); // Return the output at the specified index
    }
    return NULL; // Index out of bounds
}

struct my_output * add_output(void * void_self, struct wl_output * output)
{
    lv_egl_adapter_outmod_wayland_t state = (lv_egl_adapter_outmod_wayland_t)void_self;
    if(state->display_->outputs_count >= 10) return false;  // Limit reached
    struct my_output new_output;
    new_output.output = output;
    new_output.width = 64;
    new_output.height = 64;
    new_output.refresh = 60;
    new_output.scale = 1;
    state->display_->outputs[state->display_->outputs_count++] = new_output;
    return get_output(state, state->display_->outputs_count - 1);
}

lv_egl_adapter_output_core_t lv_egl_adapter_outmod_wayland_get_core(void * void_self)
{
    lv_egl_adapter_outmod_wayland_t wayland_out = (lv_egl_adapter_outmod_wayland_t) void_self;
    return wayland_out ? wayland_out->core : NULL;
}

// Destructor for lv_egl_adapter_outmod_wayland
void lv_egl_adapter_outmod_wayland_destroy(void ** nativewayland_ptr)
{
    lv_egl_adapter_outmod_wayland_t state = *(lv_egl_adapter_outmod_wayland_t *)nativewayland_ptr;
    if(state) {
        if(state->window_) {
            if(state->window_->xdg_toplevel)
                xdg_toplevel_destroy(state->window_->xdg_toplevel);
            if(state->window_->xdg_surface)
                xdg_surface_destroy(state->window_->xdg_surface);
            if(state->window_->native)
                wl_egl_window_destroy(state->window_->native);
            if(state->window_->surface)
                wl_surface_destroy(state->window_->surface);
            free(state->window_);
        }

        if(state->cursor_) {
            if(state->cursor_->cursor_surface)
                wl_surface_destroy(state->cursor_->cursor_surface);
            if(state->cursor_->cursor_theme)
                wl_cursor_theme_destroy(state->cursor_->cursor_theme);
            free(state->cursor_);
        }

        if(state->display_) {

            if(state->display_->xdg_wm_base)
                xdg_wm_base_destroy(state->display_->xdg_wm_base);

            for(size_t i = 0; i < state->display_->outputs_count; i++) {
                wl_output_destroy(state->display_->outputs[i].output);
                //free(state->display_->outputs[i]);
            }
            free(state->display_->outputs);

            /*
                        if (state->display_->pointer) {
                            wl_pointer_destroy(state->display_->pointer);
                            state->display_->pointer = NULL;
                            //wl_pointer_release(state->display_->pointer);
                        }
                        if (state->display_->keyboard) {
                            wl_keyboard_destroy(state->display_->keyboard);
                            state->display_->keyboard = NULL;
                            //wl_keyboard_release(state->display_->keyboard);
                        }
            */
            if(state->display_->seat)
                wl_seat_release(state->display_->seat);
            if(state->display_->shm)
                wl_shm_destroy(state->display_->shm);
            if(state->display_->compositor)
                wl_compositor_destroy(state->display_->compositor);
            if(state->display_->registry)
                wl_registry_destroy(state->display_->registry);
            if(state->display_->display) {
                wl_display_flush(state->display_->display);
                //wl_display_disconnect(state->display_->display);
            }
            free(state->display_);
        }
        free(state);
    }
    *nativewayland_ptr = NULL;
}

void lv_egl_adapter_outmod_wayland_flush(void * void_self)
{
    lv_egl_adapter_outmod_wayland_t state = (lv_egl_adapter_outmod_wayland_t)void_self;
    if(state->window_->surface) {
        struct my_output * temp_output = get_output(state, 0);
        wl_surface_damage(state->window_->surface, 0, 0,
                          temp_output->width, temp_output->height);
        wl_surface_commit(state->window_->surface);
    }

    if(state->display_->display) {
        //wl_display_flush(state->display_->display);
        wl_display_roundtrip(state->display_->display);
    }
}

// Quit handler
void lv_egl_adapter_outmod_wayland_quit_handler(int /*signum*/)
{
    should_quit = true;
}

// Initialize display
bool lv_egl_adapter_outmod_wayland_init_display(void * void_self, int * x_res, int * y_res, float refr_rate)
{
    lv_egl_adapter_outmod_wayland_t wayland_out = (lv_egl_adapter_outmod_wayland_t)void_self;
    struct sigaction sa;
    sa.sa_handler = &lv_egl_adapter_outmod_wayland_quit_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    wayland_out->display_ = malloc(sizeof(struct my_display));
    if(!wayland_out->display_) {
        return false;
    }

    wayland_out->display_->outputs = malloc(10 * sizeof(struct my_output *)); // Allocate space for 10 outputs
    wayland_out->display_->outputs_count = 0; // Initialize count
    wayland_out->display_->pointer = NULL;

    wayland_out->display_->display = wl_display_connect(NULL);
    if(!wayland_out->display_->display) {
        free(wayland_out->display_);
        return false;
    }

    wayland_out->display_->registry = wl_display_get_registry(wayland_out->display_->display);
    wl_registry_add_listener(wayland_out->display_->registry, &registry_listener, wayland_out);
    wl_display_roundtrip(wayland_out->display_->display);
    //setup_cursor(wayland_out);

    return true;
}

// Return the display
void * lv_egl_adapter_outmod_wayland_display(void * void_self)
{
    lv_egl_adapter_outmod_wayland_t wayland_out = (lv_egl_adapter_outmod_wayland_t)void_self;
    return (void *)(wayland_out->display_->display);
}

// Create a window
bool lv_egl_adapter_outmod_wayland_create_window(void * void_self, native_window_properties_t properties)
{
    lv_egl_adapter_outmod_wayland_t wayland_out = (lv_egl_adapter_outmod_wayland_t)void_self;
    struct my_output * output = NULL;
    if(!wayland_out->display_->outputs_count == 0) output = get_output(wayland_out, 0);

    wayland_out->window_ = malloc(sizeof(struct my_window));
    wayland_out->window_->surface = NULL;
    wayland_out->window_->native = NULL;
    wayland_out->window_->xdg_surface = NULL;
    wayland_out->window_->xdg_toplevel = NULL;
    wayland_out->window_->waiting_for_configure = true;

    wayland_out->window_->properties = *properties;

    /* Force fullscreen initialization mode */
    wayland_out->window_->properties.width = -1;
    wayland_out->window_->properties.height = -1;
    wayland_out->window_->properties.fullscreen = true;

    wayland_out->window_->surface = wl_compositor_create_surface(wayland_out->display_->compositor);
    wayland_out->window_->xdg_surface = xdg_wm_base_get_xdg_surface(wayland_out->display_->xdg_wm_base,
                                                                    wayland_out->window_->surface);
    xdg_surface_add_listener(wayland_out->window_->xdg_surface, &xdg_surface_listener, wayland_out);
    wayland_out->window_->xdg_toplevel = xdg_surface_get_toplevel(wayland_out->window_->xdg_surface);
    xdg_toplevel_add_listener(wayland_out->window_->xdg_toplevel, &xdg_toplevel_listener, wayland_out);

    xdg_toplevel_set_app_id(wayland_out->window_->xdg_toplevel, "com.github.lvgl.simulator");
    xdg_toplevel_set_title(wayland_out->window_->xdg_toplevel, "LVGL Simulator");
    if(wayland_out->window_->properties.fullscreen && output)
        xdg_toplevel_set_fullscreen(wayland_out->window_->xdg_toplevel, output->output);
    wl_surface_commit(wayland_out->window_->surface);

    uint32_t passnum = 0;
    while(wayland_out->window_->waiting_for_configure) {
        wl_display_roundtrip(wayland_out->display_->display);
        passnum++;
    }
    return true;
}

// Return the window
void * lv_egl_adapter_outmod_wayland_window(void * void_self, native_window_properties_t properties)
{
    lv_egl_adapter_outmod_wayland_t wayland_out = (lv_egl_adapter_outmod_wayland_t)void_self;
    if(wayland_out->window_) {
        //*properties = wayland_out->window_->properties;
        properties->width = wayland_out->window_->properties.width;
        properties->height = wayland_out->window_->properties.height;
        properties->fullscreen = true;//wayland_out->window_->properties.fullscreen;
        properties->visual_id = wayland_out->window_->properties.visual_id;
        properties->modifiers = wayland_out->window_->properties.modifiers;
        return wayland_out->window_->native;
    }
    return NULL;
}

// Set visibility
void lv_egl_adapter_outmod_wayland_visible(void * void_self, bool v)
{
    // Implementation for visibility (if needed)
    //lv_egl_adapter_outmod_wayland_t wayland_out = (lv_egl_adapter_outmod_wayland_t )void_self;
}

// Check if should quit
bool lv_egl_adapter_outmod_wayland_should_quit(void * void_self)
{
    //lv_egl_adapter_outmod_wayland_t wayland_out = (lv_egl_adapter_outmod_wayland_t )void_self;
    return should_quit;
}

// Flip the display
void lv_egl_adapter_outmod_wayland_flip(void * void_self, bool sync)
{
    lv_egl_adapter_outmod_wayland_t wayland_out = (lv_egl_adapter_outmod_wayland_t)void_self;
    int ret = wl_display_roundtrip(wayland_out->display_->display);
    should_quit = (ret == -1) || should_quit;
}

// Handle seat capabilities
void lv_egl_adapter_outmod_wayland_seat_handle_capabilities(void * data, struct wl_seat * seat, uint32_t caps)
{
    lv_egl_adapter_outmod_wayland_t that = (lv_egl_adapter_outmod_wayland_t)data;

    if((caps & WL_SEAT_CAPABILITY_POINTER) && !that->display_->pointer) {
        that->display_->pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(that->display_->pointer, &pointer_listener, that);
    }
    else if(!(caps & WL_SEAT_CAPABILITY_POINTER) && that->display_->pointer) {
        wl_pointer_destroy(that->display_->pointer);
        that->display_->pointer = NULL;
    }

    if((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !that->display_->keyboard) {
        that->display_->keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(that->display_->keyboard, &keyboard_listener, that);
    }
    else if(!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && that->display_->keyboard) {
        wl_keyboard_destroy(that->display_->keyboard);
        that->display_->keyboard = NULL;
    }
}

// Constructor for lv_egl_adapter_outmod_wayland
lv_egl_adapter_outmod_wayland_t lv_egl_adapter_outmod_wayland_create()
{
    lv_egl_adapter_outmod_wayland_t state = malloc(sizeof(lv_egl_adapter_outmod_wayland));
    if(state) {
        state->cursor_  = NULL;
        state->display_ = NULL;
        state->window_  = NULL;
        populate_output_core(state);
    }
    return state;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void populate_output_core(void * outmod_ptr)
{
    lv_egl_adapter_outmod_wayland_t wayland_out = (lv_egl_adapter_outmod_wayland_t)outmod_ptr;
    wayland_out->core = (lv_egl_adapter_output_core_t)malloc(sizeof(*(wayland_out->core)));
    wayland_out->core->destroy            = lv_egl_adapter_outmod_wayland_destroy;
    wayland_out->core->init_display       = lv_egl_adapter_outmod_wayland_init_display;
    wayland_out->core->display            = lv_egl_adapter_outmod_wayland_display;
    wayland_out->core->create_window      = lv_egl_adapter_outmod_wayland_create_window;
    wayland_out->core->window             = lv_egl_adapter_outmod_wayland_window;
    wayland_out->core->visible            = lv_egl_adapter_outmod_wayland_visible;
    wayland_out->core->should_quit        = lv_egl_adapter_outmod_wayland_should_quit;
    wayland_out->core->flip               = lv_egl_adapter_outmod_wayland_flip;
}

// Define the wl_pointer_listener
static void pointer_handle_enter(void * data, struct wl_pointer * pointer, uint32_t serial, struct wl_surface * surface,
                                 wl_fixed_t sx, wl_fixed_t sy)
{
    lv_egl_adapter_outmod_wayland_t that = (lv_egl_adapter_outmod_wayland_t)data;

    struct wl_buffer * buffer;
    struct my_cursor * my_cursor = that->cursor_;
    struct wl_cursor * cursor;
    struct wl_cursor_image * image;

    if(!my_cursor)
        return;

    cursor = my_cursor->default_cursor;

    if(that->window_->properties.fullscreen) {
        wl_pointer_set_cursor(pointer, serial, NULL, 0, 0);
    }
    else {
        if(cursor) {
            image = my_cursor->default_cursor->images[0];
            buffer = wl_cursor_image_get_buffer(image);
            if(!buffer)
                return;

            wl_pointer_set_cursor(pointer, serial,
                                  my_cursor->cursor_surface,
                                  image->hotspot_x,
                                  image->hotspot_y);
            wl_surface_attach(my_cursor->cursor_surface, buffer, 0, 0);
            wl_surface_damage(my_cursor->cursor_surface, 0, 0,
                              image->width, image->height);
            wl_surface_commit(my_cursor->cursor_surface);
        }
    }
}
static void pointer_handle_leave(void * data, struct wl_pointer * pointer, uint32_t serial, struct wl_surface * surface)
{
    // No operation
}
static void pointer_handle_motion(void * data, struct wl_pointer * pointer, uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
    lv_egl_adapter_outmod_wayland_t that = (lv_egl_adapter_outmod_wayland_t)data;
    //that->pointer_x = (int)sx >> 8;
    //that->pointer_y = (int)sy >> 8;
}

static void pointer_handle_button(void * data, struct wl_pointer * wl_pointer, uint32_t serial, uint32_t time,
                                  uint32_t button,
                                  uint32_t state)
{
    // Cast data to lv_egl_adapter_outmod_wayland
    lv_egl_adapter_outmod_wayland_t that = (lv_egl_adapter_outmod_wayland_t)data;

    if(!that->window_)  // Ensure window_ is not NULL
        return;

    if(state == WL_POINTER_BUTTON_STATE_PRESSED && button == BTN_LEFT) {
        xdg_toplevel_move(that->window_->xdg_toplevel, that->display_->seat, serial);
    }
}
static void pointer_handle_axis(void * data, struct wl_pointer * wl_pointer, uint32_t time, uint32_t axis,
                                wl_fixed_t value)
{
    // No operation
}
static const struct wl_pointer_listener pointer_listener = {
    pointer_handle_enter,
    pointer_handle_leave,
    pointer_handle_motion,
    pointer_handle_button,
    pointer_handle_axis,
};

// Define the wl_keyboard_listener
static void keyboard_handle_keymap(void * data, struct wl_keyboard * keyboard, uint32_t format, int fd, uint32_t size)
{
    close(fd);
}
static void keyboard_handle_enter(void * data, struct wl_keyboard * keyboard, uint32_t serial,
                                  struct wl_surface * surface,
                                  struct wl_array * keys)
{
    // No operation
}
static void keyboard_handle_leave(void * data, struct wl_keyboard * keyboard, uint32_t serial,
                                  struct wl_surface * surface)
{
    // No operation
}
static void keyboard_handle_key(void * data, struct wl_keyboard * keyboard, uint32_t serial, uint32_t time,
                                uint32_t key,
                                uint32_t state)
{
    if(state == WL_KEYBOARD_KEY_STATE_PRESSED &&
       (key == KEY_ESC || key == KEY_Q))
        should_quit = true;
}
static void keyboard_handle_modifiers(void * data, struct wl_keyboard * keyboard, uint32_t serial,
                                      uint32_t mods_depressed,
                                      uint32_t mods_latched, uint32_t mods_locked, uint32_t group)
{
    // No operation
}
static const struct wl_keyboard_listener keyboard_listener = {
    keyboard_handle_keymap,
    keyboard_handle_enter,
    keyboard_handle_leave,
    keyboard_handle_key,
    keyboard_handle_modifiers,
};

// Define the wl_seat_listener
static void seat_handle_capabilities(void * data, struct wl_seat * seat, uint32_t caps)
{
    lv_egl_adapter_outmod_wayland_t that = (lv_egl_adapter_outmod_wayland_t)data;
    if((caps & WL_SEAT_CAPABILITY_POINTER) && !that->display_->pointer) {
        that->display_->pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(that->display_->pointer, &pointer_listener, that);
    }
    else if(!(caps & WL_SEAT_CAPABILITY_POINTER) && that->display_->pointer) {
        if(that->display_->pointer != NULL) {
            wl_pointer_destroy(that->display_->pointer);
            that->display_->pointer = NULL;
        }
    }

    if((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !that->display_->keyboard) {
        that->display_->keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(that->display_->keyboard, &keyboard_listener, that);
    }
    else if(!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && that->display_->keyboard) {
        wl_keyboard_destroy(that->display_->keyboard);
        that->display_->keyboard = NULL;
    }
}
static const struct wl_seat_listener seat_listener = {
    seat_handle_capabilities,
};

// Define the xdg_wm_base_listener
static void xdg_wm_base_handle_ping(void * data, struct xdg_wm_base * xdg_wm_base, uint32_t serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);
}
static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    xdg_wm_base_handle_ping
};

static void output_handle_geometry(void * data, struct wl_output * wl_output,
                                   int32_t x, int32_t y, int32_t physical_width,
                                   int32_t physical_height, int32_t subpixel,
                                   const char * make, const char * model,
                                   int32_t transform)
{
    // Implementation for handling output geometry
}
static void output_handle_mode(void * data, struct wl_output * wl_output,
                               uint32_t flags, int32_t width, int32_t height,
                               int32_t refresh)
{
    if(flags & WL_OUTPUT_MODE_CURRENT) {
        struct my_output * my_output = (struct my_output *)data;
        my_output->width = width;
        my_output->height = height;
        my_output->refresh = refresh;
    }
}
static void output_handle_done(void * data, struct wl_output * wl_output)
{
    // Implementation for handling output done
}
static void output_handle_scale(void * data, struct wl_output * wl_output,
                                int32_t factor)
{
    struct my_output * my_output = (struct my_output *)data;
    my_output->scale = factor;
}
static const struct wl_output_listener output_listener = {
    output_handle_geometry,
    output_handle_mode,
    output_handle_done,
    output_handle_scale
};

// Define the wl_registry_listener
static void registry_handle_global(void * data, struct wl_registry * registry,
                                   uint32_t id, const char * interface,
                                   uint32_t version)
{
    lv_egl_adapter_outmod_wayland_t that = (lv_egl_adapter_outmod_wayland_t)data;

    if(strcmp(interface, "wl_compositor") == 0) {
        that->display_->compositor =
            (struct wl_compositor *)wl_registry_bind(registry, id, &wl_compositor_interface, version < 4 ? version : 4);
    }
    else if(strcmp(interface, "xdg_wm_base") == 0) {
        that->display_->xdg_wm_base =
            (struct xdg_wm_base *)wl_registry_bind(registry, id, &xdg_wm_base_interface, version < 2 ? version : 2);
        xdg_wm_base_add_listener(that->display_->xdg_wm_base, &xdg_wm_base_listener, that);
    }
    else if(strcmp(interface, "wl_output") == 0) {
        my_main_output = add_output(that, (struct wl_output *)wl_registry_bind(registry, id, &wl_output_interface,
                                                                               version < 2 ? version : 2));
        wl_output_add_listener(my_main_output->output, &output_listener, my_main_output);
        wl_display_roundtrip(that->display_->display);
    }
    else if(strcmp(interface, "wl_seat") == 0) {
        that->display_->seat =
            (struct wl_seat *)wl_registry_bind(registry, id, &wl_seat_interface, 1);
        wl_seat_add_listener(that->display_->seat, &seat_listener, that);
    }
    else if(strcmp(interface, "wl_shm") == 0) {
        that->display_->shm =
            (struct wl_shm *)wl_registry_bind(registry, id, &wl_shm_interface, 1);
    }
}
static void registry_handle_global_remove(void * data, struct wl_registry * registry, uint32_t name)
{
    // No action needed for removal
}
static const struct wl_registry_listener registry_listener = {
    registry_handle_global,
    registry_handle_global_remove
};

// Define the xdg_toplevel_listener
static void xdg_toplevel_handle_configure(void * data, struct xdg_toplevel * xdg_toplevel,
                                          int32_t width, int32_t height,
                                          struct wl_array * states)
{
    lv_egl_adapter_outmod_wayland_t that = (lv_egl_adapter_outmod_wayland_t)data;
    uint32_t * state;
    bool want_fullscreen = true; //false;
    bool want_maximized = false;
    uint32_t scale = 1;

    that->window_->waiting_for_configure = false;

    if(!that->display_->outputs_count == 0) scale = get_output(that, 0)->scale;

    for(state = (uint32_t *)states->data;
        state < (uint32_t *)((char *)states->data + states->size);
        state++) {
        if(*state == XDG_TOPLEVEL_STATE_FULLSCREEN)
            want_fullscreen = true;
        else if(*state == XDG_TOPLEVEL_STATE_MAXIMIZED)
            want_maximized = true;
    }

    if(want_maximized) {
        that->window_->properties.width = width * scale;
        that->window_->properties.height = height * scale;
    }
    else if(that->window_->properties.fullscreen) {
        if(want_fullscreen) {
            that->window_->properties.width = width * scale;
            that->window_->properties.height = height * scale;
        }
        else if(!that->display_->outputs_count == 0) {
            that->window_->properties.width = get_output(that, 0)->width;
            that->window_->properties.height = get_output(that, 0)->height;
        }
    }

    width = that->window_->properties.width;
    height = that->window_->properties.height;

    if(!that->window_->native) {
        that->window_->native = wl_egl_window_create(that->window_->surface, width, height);
    }
    else {
        wl_egl_window_resize(that->window_->native, width, height, 0, 0);
    }

    struct wl_region * opaque_region = wl_compositor_create_region(that->display_->compositor);
    wl_region_add(opaque_region, 0, 0, width, height);
    wl_surface_set_opaque_region(that->window_->surface, opaque_region);
    wl_region_destroy(opaque_region);

    if(wl_surface_get_version(that->window_->surface) >= WL_SURFACE_SET_BUFFER_SCALE_SINCE_VERSION) {
        wl_surface_set_buffer_scale(that->window_->surface, scale);
    }
}
static void xdg_toplevel_handle_close(void * data, struct xdg_toplevel * xdg_toplevel)
{
    should_quit = true;
}
static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    xdg_toplevel_handle_configure,
    xdg_toplevel_handle_close
};

/*
void setup_cursor(void *void_self) {
    lv_egl_adapter_outmod_wayland_t wayland_out = (lv_egl_adapter_outmod_wayland_t )void_self;
    if (!wayland_out->display_->shm)
        return;

    struct my_cursor *my_cursor = malloc(sizeof(struct my_cursor));
    if (!my_cursor) {
        return;
    }

    my_cursor->cursor_surface =
         wl_compositor_create_surface(wayland_out->display_->compositor);
    my_cursor->cursor_theme = wl_cursor_theme_load(NULL, 32, wayland_out->display_->shm);

    if (!my_cursor->cursor_theme) {
        fprintf(stderr, "unable to load default theme\n");
        wl_surface_destroy(my_cursor->cursor_surface);
        free(my_cursor);
        return;
    }

    my_cursor->default_cursor =
        wl_cursor_theme_get_cursor(my_cursor->cursor_theme, "left_ptr");

    if (!my_cursor->default_cursor) {
        wl_surface_destroy(my_cursor->cursor_surface);
        // assume above cursor_theme was set
        wl_cursor_theme_destroy(my_cursor->cursor_theme);
        free(my_cursor);
        return;
    }

    *wayland_out->cursor_ = *my_cursor;
}
*/
// Define the xdg_surface_listener
static void xdg_surface_handle_configure(void * data, struct xdg_surface * xdg_surface, uint32_t serial)
{
    xdg_surface_ack_configure(xdg_surface, serial);
}
static const struct xdg_surface_listener xdg_surface_listener = {
    xdg_surface_handle_configure
};

#endif /* LV_USE_LINUX_WAYLAND && LV_LINUX_WAYLAND_USE_EGL */