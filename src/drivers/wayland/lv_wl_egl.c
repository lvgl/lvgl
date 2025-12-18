/**
 * @file lv_wl_egl.c
 *
 */

/**
 * Modified by Renesas in 2025
 */

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include "lv_wl_egl.h"

#if LV_USE_WAYLAND && LV_WAYLAND_USE_EGL
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <time.h>
#include <sys/time.h>

#if LV_WAYLAND_USE_EGL_DMABUF
    #include "lv_wl_egl_dmabuf.h"
#endif

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
#define BYTES_PER_PIXEL ((LV_COLOR_DEPTH + 7) / 8)

/*****************************************************************************
 * TYPEDEFS
 *****************************************************************************/


/*****************************************************************************
 * STATIC VARIABLES
 *****************************************************************************/

static lv_wl_egl_window_t * g_window = NULL;

/* Vertex shader source */
static const char * vertex_shader_src =
    "attribute vec2 a_position;\n"
    "attribute vec2 a_texcoord;\n"
    "varying vec2 v_texcoord;\n"
    "void main() {\n"
    "    gl_Position = vec4(a_position, 0.0, 1.0);\n"
    "    v_texcoord = a_texcoord;\n"
    "}\n";

/* Fragment shader source */
static const char * fragment_shader_src =
    "precision mediump float;\n"
    "varying vec2 v_texcoord;\n"
    "uniform sampler2D u_texture;\n"
    "void main() {\n"
    "    gl_FragColor = texture2D(u_texture, v_texcoord);\n"
    "}\n";

/*****************************************************************************
 * STATIC FUNCTION PROTOTYPES
 *****************************************************************************/
/* Wayland functions */
static bool init_wayland(lv_wl_egl_window_t * window);
static bool create_surface(lv_wl_egl_window_t * window);
static void cleanup_wayland(lv_wl_egl_window_t * window);

/* EGL functions */
static bool init_egl(lv_wl_egl_window_t * window);
static void cleanup_egl(lv_wl_egl_window_t * window);

/* OpenGL ES functions */
static GLuint compile_shader(const char * source, GLenum type);
static GLuint create_shader_program(void);
static bool setup_opengl_resources(lv_wl_egl_context_t * ctx);
static void render_texture(lv_wl_egl_window_t * window);
static lv_wl_egl_context_t * init_opengles(int32_t width, int32_t height);
static void cleanup_opengles(lv_wl_egl_context_t * ctx);

/* LVGL functions */
static uint32_t lv_wl_egl_tick_count_cb(void);
static bool create_lvgl_display(lv_wl_egl_window_t * window);
static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

/* Wayland listeners */
static void xdg_surface_handle_configure(void * data, struct xdg_surface * xdg_surface, uint32_t serial);
static void xdg_toplevel_handle_configure(void * data, struct xdg_toplevel * xdg_toplevel, int32_t width,
                                          int32_t height, struct wl_array * states);
static void xdg_wm_base_ping(void * data, struct xdg_wm_base * xdg_wm_base, uint32_t serial);

static void registry_handler(void * data, struct wl_registry * registry,
                             uint32_t id, const char * interface, uint32_t version);
static void registry_remover(void * data, struct wl_registry * registry, uint32_t id);

/*****************************************************************************
 * WAYLAND LISTENER IMPLEMENTATIONS
 *****************************************************************************/

static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_handle_configure,
};

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    .configure = xdg_toplevel_handle_configure,
};

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    .ping = xdg_wm_base_ping
};

static const struct wl_registry_listener registry_listener = {
    .global = registry_handler,
    .global_remove = registry_remover,
};

static void registry_handler(void * data, struct wl_registry * registry,
                             uint32_t id, const char * interface, uint32_t version)
{
    lv_wl_egl_window_t * window = (lv_wl_egl_window_t *)data;
    LV_UNUSED(version);

    if(strcmp(interface, "wl_compositor") == 0) {
        window->compositor = wl_registry_bind(registry, id,
                                              &wl_compositor_interface, 4);
    }
    else if(strcmp(interface, "wl_shm") == 0) {
        window->shm = wl_registry_bind(registry, id, &wl_shm_interface, 1);
    }
    else if(strcmp(interface, "wl_seat") == 0) {
        window->wl_seat = wl_registry_bind(registry, id,
                                           &wl_seat_interface, 1);
        wl_seat_add_listener(window->wl_seat, lv_wl_egl_seat_get_listener(), window);
    }
    else if(strcmp(interface, "xdg_wm_base") == 0) {
        // Try xdg_wm_base first (modern protocol)
        window->xdg_wm_base = wl_registry_bind(registry, id,
                                               &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(window->xdg_wm_base, &xdg_wm_base_listener, window);
    }
#if LV_WAYLAND_USE_EGL_DMABUF
    lv_wl_egl_dmabuf_registry_handler(window->dmabuf_ctx, registry, id, interface, version);
#endif
}

static void registry_remover(void * data, struct wl_registry * registry, uint32_t id)
{
    (void)data;
    (void)registry;
    (void)id;
}

static void xdg_wm_base_ping(void * data, struct xdg_wm_base * xdg_wm_base, uint32_t serial)
{
    LV_UNUSED(data);
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static void xdg_surface_handle_configure(void * data, struct xdg_surface * xdg_surface, uint32_t serial)
{
    lv_wl_egl_window_t * window = (lv_wl_egl_window_t *)data;

    LV_LOG_INFO("XDG surface configure, serial: %u", serial);

    /* Acknowledge the configure event */
    xdg_surface_ack_configure(xdg_surface, serial);

    /* Commit the surface to apply the configuration */
    if(window && window->surface) {
        wl_surface_commit(window->surface);
    }
}

static void xdg_toplevel_handle_configure(void * data, struct xdg_toplevel * xdg_toplevel,
                                          int32_t width, int32_t height, struct wl_array * states)
{
    lv_wl_egl_window_t * window = (lv_wl_egl_window_t *)data;

    LV_UNUSED(xdg_toplevel);
    LV_UNUSED(states);

    /* Handle resize if dimensions are provided (0 means compositor doesn't care) */
    if(width > 0 && height > 0 && window) {
        if(width != window->width || height != window->height) {
            LV_LOG_INFO("Window resize requested: %dx%d -> %dx%d",
                        window->width, window->height, width, height);

            window->width = width;
            window->height = height;

            /* Resize EGL window if it exists */
            if(window->egl_window) {
                wl_egl_window_resize(window->egl_window, width, height, 0, 0);
            }

            /* Update OpenGL viewport */
            if(window->gl_context) {
                glViewport(0, 0, width, height);

                /* Recreate texture with new size */
                if(window->gl_context->texture_id) {
                    glBindTexture(GL_TEXTURE_2D, window->gl_context->texture_id);
#if LV_COLOR_DEPTH == 16
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
                                 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
#elif LV_COLOR_DEPTH == 32
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
                                 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
#endif
                    glBindTexture(GL_TEXTURE_2D, 0);
                }
            }

            /* Update LVGL display resolution */
            if(window->lv_display) {
                lv_display_set_resolution(window->lv_display, width, height);

                /* Reallocate buffers */
                size_t buf_size = width * height * BYTES_PER_PIXEL;
                if(window->gl_context->fb1) {
                    free(window->gl_context->fb1);
                    window->gl_context->fb1 = (uint8_t *)malloc(buf_size);
                }
                if(window->gl_context->fb2) {
                    free(window->gl_context->fb2);
                    window->gl_context->fb2 = (uint8_t *)malloc(buf_size);
                }
#if LV_WAYLAND_USE_EGL_DMABUF
                uint8_t render_mode = LV_DISPLAY_RENDER_MODE_FULL;
#else
                uint8_t render_mode = LV_DISPLAY_RENDER_MODE_PARTIAL;
#endif
                lv_display_set_buffers(window->lv_display,
                                       window->gl_context->fb1,
                                       window->gl_context->fb2,
                                       buf_size,
                                       render_mode);
            }

            /* Update opaque region */
            if(window->region) {
                wl_region_destroy(window->region);
                window->region = wl_compositor_create_region(window->compositor);
                wl_region_add(window->region, 0, 0, width, height);
                wl_surface_set_opaque_region(window->surface, window->region);
            }
        }
    }
}


/*****************************************************************************
 * OPENGL ES SHADER FUNCTIONS
 *****************************************************************************/
static GLuint compile_shader(const char * source, GLenum type)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        LV_LOG_ERROR("Shader compilation failed: %s", info_log);
        return 0;
    }

    return shader;
}

static GLuint create_shader_program(void)
{
    GLuint vertex_shader = compile_shader(vertex_shader_src, GL_VERTEX_SHADER);
    GLuint fragment_shader = compile_shader(fragment_shader_src, GL_FRAGMENT_SHADER);

    if(!vertex_shader || !fragment_shader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glBindAttribLocation(program, 0, "a_position");
    glBindAttribLocation(program, 1, "a_texcoord");

    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        char info_log[512];
        glGetProgramInfoLog(program, 512, NULL, info_log);
        LV_LOG_ERROR("Program linking failed: %s", info_log);
        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}

/*****************************************************************************
 * OPENGL ES RESOURCE FUNCTIONS
 *****************************************************************************/
static bool setup_opengl_resources(lv_wl_egl_context_t * ctx)
{
    ctx->shader_program = create_shader_program();
    if(!ctx->shader_program) {
        return false;
    }

    float vertices[] = {
        -1.0f,  1.0f,  0.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenBuffers(1, &ctx->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, ctx->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &ctx->ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenTextures(1, &ctx->texture_id);
    glBindTexture(GL_TEXTURE_2D, ctx->texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

#if LV_COLOR_DEPTH == 16
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, g_window->width, g_window->height,
                 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL);
#elif LV_COLOR_DEPTH == 32
    glTexImage2D(GL_TEXTURE_2D, 0, GL_BGRA, g_window->width, g_window->height,
                 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
#else
#error "Unsupported color depth"
#endif

    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

static void render_texture(lv_wl_egl_window_t * window)
{
    lv_wl_egl_context_t * ctx = window->gl_context;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(ctx->shader_program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ctx->texture_id);
    glUniform1i(glGetUniformLocation(ctx->shader_program, "u_texture"), 0);

    glBindBuffer(GL_ARRAY_BUFFER, ctx->vbo);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx->ebo);
    glDrawElements(GL_TRIANGLES, LV_WL_EGL_INDEX_COUNT, GL_UNSIGNED_INT, NULL);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

/*****************************************************************************
 * LVGL CALLBACK FUNCTIONS
 *****************************************************************************/

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    lv_wl_egl_window_t * window = (lv_wl_egl_window_t *)lv_display_get_user_data(disp);
    lv_wl_egl_context_t * ctx = window->gl_context;

    int32_t x = area->x1;
    int32_t y = area->y1;
    int32_t w = lv_area_get_width(area);
    int32_t h = lv_area_get_height(area);

    glBindTexture(GL_TEXTURE_2D, ctx->texture_id);

#if LV_COLOR_DEPTH == 16
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h,
                    GL_RGB, GL_UNSIGNED_SHORT_5_6_5, px_map);
#elif LV_COLOR_DEPTH == 32
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h,
                    GL_BGRA, GL_UNSIGNED_BYTE, px_map);
#endif

    glBindTexture(GL_TEXTURE_2D, 0);

    if(lv_display_flush_is_last(disp)) {
        render_texture(window);
        eglSwapBuffers(window->egl_display, window->egl_surface);
    }

    lv_display_flush_ready(disp);
}

/*****************************************************************************
 * WAYLAND INITIALIZATION FUNCTIONS
 *****************************************************************************/
static bool init_wayland(lv_wl_egl_window_t * window)
{
    LV_LOG_INFO("Connecting to Wayland display...");

    window->display = wl_display_connect(NULL);
    if(!window->display) {
        LV_LOG_ERROR("Failed to connect to Wayland display");
        return false;
    }

    window->xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    if(!window->xkb_context) {
        LV_LOG_ERROR("Failed to create XKB context");
        return false;
    }

#if LV_WAYLAND_USE_EGL_DMABUF
    lv_wl_egl_dmabuf_initalize_context(window);
#endif

    window->registry = wl_display_get_registry(window->display);
    if(!window->registry) {
        LV_LOG_ERROR("Failed to get Wayland registry");
        return false;
    }

    wl_registry_add_listener(window->registry, &registry_listener, window);

    wl_display_dispatch(window->display);
    wl_display_roundtrip(window->display);

    if(!window->compositor) {
        LV_LOG_ERROR("Failed to get Wayland compositor");
        return false;
    }

    if(!window->xdg_wm_base) {
        LV_LOG_ERROR("Failed to get Wayland XDG shell");
        return false;
    }

    if(!window->shm) {
        LV_LOG_WARN("wl_shm not available - hardware cursor will not work");
    }

    if(!window->wl_seat) {
        LV_LOG_WARN("wl_seat not available - input devices will not work");
    }

    LV_LOG_INFO("Wayland initialization successful");
    return true;
}

static bool create_surface(lv_wl_egl_window_t * window)
{
    LV_LOG_INFO("Creating Wayland surface...");

    window->surface = wl_compositor_create_surface(window->compositor);
    if(!window->surface) {
        LV_LOG_ERROR("Failed to create Wayland surface");
        return false;
    }

    window->xdg_surface = xdg_wm_base_get_xdg_surface(window->xdg_wm_base, window->surface);
    if(!window->xdg_surface) {
        LV_LOG_ERROR("Failed to create xdg_surface");
        return false;
    }
    xdg_surface_add_listener(window->xdg_surface, &xdg_surface_listener, window);

    window->xdg_toplevel = xdg_surface_get_toplevel(window->xdg_surface);
    if(!window->xdg_toplevel) {
        LV_LOG_ERROR("Failed to create xdg_toplevel");
        return false;
    }
    xdg_toplevel_add_listener(window->xdg_toplevel, &xdg_toplevel_listener, window);

    if(window->title) {
        xdg_toplevel_set_title(window->xdg_toplevel, window->title);
    }

    // Configure XDG surfaces event
    wl_surface_commit(window->surface);
    wl_display_roundtrip(window->display);

    LV_LOG_INFO("Creating EGL window Wayland Region...");
    window->region = wl_compositor_create_region(window->compositor);
    if(!window->region) {
        LV_LOG_ERROR("Failed to create wayland region");
        return false;
    }
    wl_region_add(window->region, 0, 0, window->width, window->height);
    wl_surface_set_opaque_region(window->surface, window->region);

    LV_LOG_INFO("Creating EGL window...");

    window->egl_window = wl_egl_window_create(window->surface, window->width, window->height);
    if(!window->egl_window) {
        LV_LOG_ERROR("Failed to create EGL window");
        return false;
    }

    window->egl_surface = eglCreateWindowSurface(window->egl_display,
                                                 window->egl_config,
                                                 (EGLNativeWindowType)window->egl_window,
                                                 NULL);
    if(window->egl_surface == EGL_NO_SURFACE) {
        LV_LOG_ERROR("Failed to create EGL surface: 0x%x", eglGetError());
        return false;
    }

    if(!eglMakeCurrent(window->egl_display, window->egl_surface,
                       window->egl_surface, window->egl_context)) {
        LV_LOG_ERROR("Failed to make EGL context current: 0x%x", eglGetError());
        return false;
    }

    eglSwapInterval(window->egl_display, 1);

    LV_LOG_INFO("Surface creation successful");
    return true;
}

static void cleanup_wayland(lv_wl_egl_window_t * window)
{
    if(window->indev.xkb.state) {
        xkb_state_unref(window->indev.xkb.state);
        window->indev.xkb.state = NULL;
    }
    if(window->indev.xkb.keymap) {
        xkb_keymap_unref(window->indev.xkb.keymap);
        window->indev.xkb.keymap = NULL;
    }
    if(window->xkb_context) {
        xkb_context_unref(window->xkb_context);
        window->xkb_context = NULL;
    }

    if(window->indev.wl_pointer) {
        wl_pointer_destroy(window->indev.wl_pointer);
        window->indev.wl_pointer = NULL;
    }
    if(window->indev.wl_keyboard) {
        wl_keyboard_destroy(window->indev.wl_keyboard);
        window->indev.wl_keyboard = NULL;
    }
#if LV_USE_GESTURE_RECOGNITION
    if(window->indev.wl_touch) {
        wl_touch_destroy(window->indev.wl_touch);
        window->indev.wl_touch = NULL;
    }
#endif

    if(window->wl_seat) {
        wl_seat_destroy(window->wl_seat);
        window->wl_seat = NULL;
    }

    if(window->cursor_theme) {
        wl_shm_destroy(window->shm);
        window->cursor_theme = NULL;
    }

    if(window->cursor_surface) {
        wl_surface_destroy(window->cursor_surface);
        window->cursor_surface = NULL;
    }

    if(window->egl_window) {
        wl_egl_window_destroy(window->egl_window);
        window->egl_window = NULL;
    }

    if(window->region) {
        wl_region_destroy(window->region);
        window->region = NULL;
    }

    if(window->xdg_toplevel) {
        xdg_toplevel_destroy(window->xdg_toplevel);
        window->xdg_toplevel = NULL;
    }
    if(window->xdg_surface) {
        xdg_surface_destroy(window->xdg_surface);
        window->xdg_surface = NULL;
    }
    if(window->xdg_wm_base) {
        xdg_wm_base_destroy(window->xdg_wm_base);
        window->xdg_wm_base = NULL;
    }

    if(window->surface) {
        wl_surface_destroy(window->surface);
        window->surface = NULL;
    }
    if(window->compositor) {
        wl_compositor_destroy(window->compositor);
        window->compositor = NULL;
    }
    if(window->registry) {
        wl_registry_destroy(window->registry);
        window->registry = NULL;
    }
    if(window->display) {
        wl_display_disconnect(window->display);
        window->display = NULL;
    }
}


/*****************************************************************************
 * EGL INITIALIZATION FUNCTIONS
 *****************************************************************************/
static bool init_egl(lv_wl_egl_window_t * window)
{
    EGLint major, minor;

    LV_LOG_INFO("Getting EGL display...");

    window->egl_display = eglGetDisplay((EGLNativeDisplayType)window->display);
    if(window->egl_display == EGL_NO_DISPLAY) {
        LV_LOG_ERROR("Failed to get EGL display");
        return false;
    }

    if(!eglInitialize(window->egl_display, &major, &minor)) {
        LV_LOG_ERROR("Failed to initialize EGL");
        return false;
    }

    if(!eglBindAPI(EGL_OPENGL_ES_API)) {
        LV_LOG_ERROR("Failed to bind OpenGL ES API");
        return false;
    }

    EGLint config_attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
#if LV_COLOR_DEPTH == 32
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
#elif LV_COLOR_DEPTH == 16
        EGL_RED_SIZE, 5,
        EGL_GREEN_SIZE, 6,
        EGL_BLUE_SIZE, 5,
        EGL_ALPHA_SIZE, 0,
#endif
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE
    };

    EGLint num_configs;
    if(!eglChooseConfig(window->egl_display, config_attribs,
                        &window->egl_config, 1, &num_configs) || num_configs == 0) {
        LV_LOG_ERROR("Failed to choose EGL config");
        return false;
    }

    EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    window->egl_context = eglCreateContext(window->egl_display,
                                           window->egl_config,
                                           EGL_NO_CONTEXT,
                                           context_attribs);
    if(window->egl_context == EGL_NO_CONTEXT) {
        LV_LOG_ERROR("Failed to create EGL context: 0x%x", eglGetError());
        return false;
    }

    LV_LOG_INFO("EGL initialization successful");
    return true;
}

static void cleanup_egl(lv_wl_egl_window_t * window)
{
    if(!window) return;

    if(window->egl_display != EGL_NO_DISPLAY) {
        eglMakeCurrent(window->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if(window->egl_surface != EGL_NO_SURFACE) {
            eglDestroySurface(window->egl_display, window->egl_surface);
            window->egl_surface = EGL_NO_SURFACE;
        }

        if(window->egl_context != EGL_NO_CONTEXT) {
            eglDestroyContext(window->egl_display, window->egl_context);
            window->egl_context = EGL_NO_CONTEXT;
        }

        eglTerminate(window->egl_display);
        window->egl_display = EGL_NO_DISPLAY;
    }
}

/*****************************************************************************
 * OPENGL ES CONTEXT FUNCTIONS
 *****************************************************************************/
static lv_wl_egl_context_t * init_opengles(int32_t width, int32_t height)
{
    lv_wl_egl_context_t * ctx = (lv_wl_egl_context_t *)calloc(1, sizeof(lv_wl_egl_context_t));
    if(!ctx) {
        return NULL;
    }

    if(!setup_opengl_resources(ctx)) {
        free(ctx);
        return NULL;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, width, height);

    return ctx;
}

static void cleanup_opengles(lv_wl_egl_context_t * ctx)
{
    if(!ctx) return;

    if(ctx->texture_id) {
        glDeleteTextures(1, &ctx->texture_id);
    }
    if(ctx->shader_program) {
        glDeleteProgram(ctx->shader_program);
    }
    if(ctx->vbo) {
        glDeleteBuffers(1, &ctx->vbo);
    }
    if(ctx->ebo) {
        glDeleteBuffers(1, &ctx->ebo);
    }
    if(ctx->fb1) {
        free(ctx->fb1);
    }
    if(ctx->fb2) {
        free(ctx->fb2);
    }
    free(ctx);
}

/*****************************************************************************
 * LVGL DISPLAY CREATION FUNCTIONS
 *****************************************************************************/

static uint32_t lv_wl_egl_tick_count_cb(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000000000 + t.tv_nsec) / 1000000;
}

static bool create_lvgl_display(lv_wl_egl_window_t * window)
{
    lv_wl_egl_context_t * ctx = window->gl_context;

    lv_tick_set_cb(lv_wl_egl_tick_count_cb);

    window->lv_display = lv_display_create(window->width, window->height);
    if(!window->lv_display) {
        return false;
    }

    size_t buf_size = window->width * window->height * BYTES_PER_PIXEL;
    ctx->fb1 = (uint8_t *)malloc(buf_size);
    ctx->fb2 = (uint8_t *)malloc(buf_size);
    if(!ctx->fb1 || !ctx->fb2) {
        LV_LOG_ERROR("Failed to allocate framebuffers");
        if(ctx->fb1) free(ctx->fb1);
        if(ctx->fb2) free(ctx->fb2);
        lv_display_delete(window->lv_display);
        return false;
    }

#if LV_WAYLAND_USE_EGL_DMABUF
    if(window->use_dmabuf) {
        lv_display_set_buffers(window->lv_display, ctx->fb1, ctx->fb2,
                               buf_size, LV_DISPLAY_RENDER_MODE_FULL);
        lv_display_set_flush_cb(window->lv_display, lv_wl_egl_dmabuf_flush_cb);
    }
    else
#endif
    {
        lv_display_set_buffers(window->lv_display, ctx->fb1, ctx->fb2,
                               buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
        lv_display_set_flush_cb(window->lv_display, flush_cb);
    }

    lv_display_set_user_data(window->lv_display, window);

    /* Register input device */
    if(!lv_wl_egl_indev_init(window)) {
        LV_LOG_ERROR("Failed to initialize input devices");
        lv_display_delete(window->lv_display);
        return false;
    }

    lv_group_t * default_group = lv_group_create();
    lv_group_set_default(default_group);
    lv_indev_set_group(window->lv_indev_keyboard, default_group);

    return window->lv_display != NULL;
}

/*****************************************************************************
 * GLOBAL FUNCTION IMPLEMENTATIONS
 *****************************************************************************/
lv_display_t * lv_wayland_egl_window_create(int32_t width, int32_t height, const char * title)
{
    LV_LOG_INFO("Creating Wayland LVGL window %dx%d", width, height);

    /* Check if window already exists */
    if(g_window) {
        LV_LOG_ERROR("Window already exists. Only one window is supported.");
        return NULL;
    }

    /* Allocate window structure */
    g_window = lv_malloc_zeroed(sizeof(lv_wl_egl_window_t));
    if(!g_window) {
        LV_LOG_ERROR("Failed to allocate window structure");
        return NULL;
    }

    g_window->width = width;
    g_window->height = height;
    g_window->title = title ? strdup(title) : strdup("LVGL OpenGL ES");
    g_window->cursor_flush_pending = false;

    if(!init_wayland(g_window)) {
        LV_LOG_ERROR("Wayland initialization failed");
        goto error;
    }

    if(!init_egl(g_window)) {
        LV_LOG_ERROR("EGL initialization failed");
        goto error;
    }

    if(!create_surface(g_window)) {
        LV_LOG_ERROR("Surface creation failed");
        goto error;
    }

    g_window->gl_context = init_opengles(width, height);
    if(!g_window->gl_context) {
        LV_LOG_ERROR("Failed to initialize OpenGL ES context");
        goto error;
    }

#if LV_WAYLAND_USE_EGL_DMABUF
    if(!lv_wl_egl_dmabuf_create_draw_buffers(g_window)) {
        g_window->use_dmabuf = false;
        goto error;
    }
#endif

    if(!create_lvgl_display(g_window)) {
        LV_LOG_ERROR("Failed to create LVGL display");
        goto error;
    }

#if LV_WAYLAND_USE_EGL_DMABUF
    if(g_window->use_dmabuf) {
        dmabuf_ctx_t * dmabuf_ctx = (dmabuf_ctx_t *)g_window->dmabuf_ctx;
        if(dmabuf_ctx && dmabuf_ctx->buffers) {
            struct buffer * first_buffer = dmabuf_ctx->buffers;
            wl_surface_attach(g_window->surface, first_buffer->wl_buffer, 0, 0);
            wl_surface_damage_buffer(g_window->surface, 0, 0, width, height);
            wl_surface_commit(g_window->surface);
            first_buffer->busy = true;
        }
        wl_display_roundtrip(g_window->display);
    }
    else
#endif
    {
        eglSwapBuffers(g_window->egl_display, g_window->egl_surface);
    }

    LV_LOG_INFO("Window creation successful!");
    return g_window->lv_display;

error:
    lv_wayland_egl_window_destroy();
    return NULL;
}

int lv_wayland_egl_process_events(void)
{
    if(!g_window || !g_window->display) {
        return -1;
    }

    // Read events from the display file descriptor
    struct pollfd fds[1];
    fds[0].fd = wl_display_get_fd(g_window->display);
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    // Check if there are events to read (non-blocking)
    int poll_ret = poll(fds, 1, 0);

    if(poll_ret > 0 && (fds[0].revents & POLLIN)) {
        // Read events from the file descriptor
        if(wl_display_dispatch(g_window->display) < 0) {
            LV_LOG_ERROR("wl_display_dispatch failed: %s", strerror(errno));
            return -1;
        }
    }
    else if(poll_ret < 0) {
        LV_LOG_ERROR("poll failed: %s", strerror(errno));
        return -1;
    }

    // Dispatch any pending events
    int ret = wl_display_dispatch_pending(g_window->display);
    if(ret < 0) {
        LV_LOG_ERROR("Failed to dispatch pending Wayland events: %s", strerror(errno));
        return -1;
    }

    // Flush requests to the server
    int flush_ret = wl_display_flush(g_window->display);
    if(flush_ret < 0) {
        if(errno == EAGAIN) {
            return ret;
        }
        else {
            LV_LOG_ERROR("Failed to flush Wayland display: %s (errno=%d)",
                         strerror(errno), errno);
            return -1;
        }
    }
    else {
        g_window->cursor_flush_pending = false;
    }

    return ret;
}

void lv_wayland_egl_window_destroy(void)
{
    if(!g_window) return;

    if(g_window->lv_indev_pointer) {
        lv_indev_delete(g_window->lv_indev_pointer);
        g_window->lv_indev_pointer = NULL;
    }
    if(g_window->lv_indev_pointeraxis) {
        lv_indev_delete(g_window->lv_indev_pointeraxis);
        g_window->lv_indev_pointeraxis = NULL;
    }
    if(g_window->lv_indev_keyboard) {
        lv_indev_delete(g_window->lv_indev_keyboard);
        g_window->lv_indev_keyboard = NULL;
    }
#if LV_USE_GESTURE_RECOGNITION
    if(g_window->lv_indev_touch) {
        lv_indev_delete(g_window->lv_indev_touch);
        g_window->lv_indev_touch = NULL;
    }
#endif

    if(g_window->lv_display) {
        lv_display_delete(g_window->lv_display);
        g_window->lv_display = NULL;
    }

#if LV_WAYLAND_USE_EGL_DMABUF
    /* Cleanup DMA-BUF buffers if in DMA-BUF mode */
    if(g_window->dmabuf_ctx && g_window->use_dmabuf) {
        lv_wl_egl_dmabuf_cleanup(g_window);
    }
#endif

    /* Cleanup OpenGL ES */
    if(g_window->gl_context) {
        cleanup_opengles(g_window->gl_context);
    }

    /* Cleanup EGL */
    cleanup_egl(g_window);

    /* Cleanup Wayland */
    cleanup_wayland(g_window);

    /* Free title */
    if(g_window->title) {
        lv_free(g_window->title);
        g_window->title = NULL;
    }

    /* Free window structure */
    lv_free(g_window);
    g_window = NULL;
}

#endif
