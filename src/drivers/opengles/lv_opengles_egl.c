/**
 * @file lv_opengles_egl.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_opengles_egl.h"
#include LV_STDBOOL_INCLUDE

#if LV_USE_EGL

#include <dlfcn.h>

#include <string.h>
#include "lv_opengles_debug.h"

#include "../../lvgl_public.h"
#include "glad/include/glad/egl.h"
#include "lv_opengles_private.h"
#include "lv_opengles_egl_private.h"

/*********************
*      DEFINES
*********************/

/**********************
*      TYPEDEFS
**********************/

/**********************
*  STATIC PROTOTYPES
**********************/

static lv_result_t load_egl(lv_opengles_egl_t * ctx);
static EGLDisplay create_egl_display(lv_opengles_egl_t * ctx);
static EGLSurface create_egl_surface(lv_opengles_egl_t * ctx);
static EGLContext create_egl_context(lv_opengles_egl_t * ctx);
static EGLConfig create_egl_config(lv_opengles_egl_t * ctx);
static lv_result_t lv_egl_config_from_egl_config(lv_opengles_egl_t * ctx, lv_egl_config_t * lv_egl_config,
                                                 EGLConfig egl_config);
static void * create_native_window(lv_opengles_egl_t * ctx);
static lv_result_t get_native_config(lv_opengles_egl_t * ctx, EGLint * native_id, uint64_t ** mods, size_t * count);
static GLADapiproc glad_egl_load_cb(void * userdata, const char * name);

/**********************
*  STATIC VARIABLES
**********************/

/**********************
*      MACROS
**********************/

/**********************
*   GLOBAL FUNCTIONS
**********************/

lv_opengles_egl_t * lv_opengles_egl_context_create(const lv_egl_interface_t * interface)
{
    lv_opengles_egl_t * ctx = lv_zalloc(sizeof(*ctx));
    LV_ASSERT_MALLOC(ctx);
    if(!ctx) {
        LV_LOG_ERROR("Failed to create egl context");
        return NULL;
    }
    ctx->interface = *interface;
    ctx->vsync = false;

    lv_result_t res = load_egl(ctx);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to load egl ");
        lv_free(ctx);
        return NULL;
    }
    lv_opengles_init();
    return ctx;
}

void lv_opengles_egl_context_destroy(lv_opengles_egl_t * ctx)
{
    if(!ctx) {
        return;
    }
    if(ctx->egl_display) {
        eglMakeCurrent(ctx->egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if(ctx->egl_context) {
            eglDestroyContext(ctx->egl_display, ctx->egl_context);
        }
        ctx->egl_context = EGL_NO_CONTEXT;
    }
    if(ctx->egl_surface && ctx->egl_display) {
        if(ctx->interface.destroy_surface_cb) {
            ctx->interface.destroy_surface_cb(ctx->interface.driver_data, ctx->egl_surface);
        }
        else {
            eglDestroySurface(ctx->egl_display, ctx->egl_surface);
        }
        ctx->egl_surface = EGL_NO_SURFACE;
    }

    if(ctx->native_window && ctx->interface.destroy_window_cb) {
        ctx->interface.destroy_window_cb(ctx->interface.driver_data, (void *)ctx->native_window);
        ctx->native_window = 0;
    }

    eglReleaseThread();
    if(ctx->egl_display) {
        eglTerminate(ctx->egl_display);
        ctx->egl_display = EGL_NO_DISPLAY;
    }

    if(ctx->egl_lib_handle) {
        dlclose(ctx->egl_lib_handle);
    }
    if(ctx->opengl_lib_handle) {
        dlclose(ctx->opengl_lib_handle);
    }
    ctx->egl_config = NULL;
    lv_free(ctx);
}

uint8_t lv_opengles_egl_get_gles_version(lv_opengles_egl_t * ctx)
{
    LV_ASSERT_NULL(ctx);
    return ctx->gles_version;
}

void lv_opengles_egl_clear(lv_opengles_egl_t * ctx)
{
    LV_UNUSED(ctx);
    GL_CALL(glClearColor(0.f, 0.f, 0.f, 1.0f));
    GL_CALL(glDepthRangef(-1.0, 1.0));
    GL_CALL(glDepthFunc(GL_ALWAYS));
    GL_CALL(glClearDepthf(1.0f));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void lv_opengles_egl_update(lv_opengles_egl_t * ctx)
{
    eglSwapBuffers(ctx->egl_display, ctx->egl_surface);
    ctx->interface.flip_cb(ctx->interface.driver_data, ctx->vsync);
}

size_t lv_opengles_egl_display_select_config(lv_display_t * display, const lv_egl_config_t * configs,
                                             size_t config_count)
{
    int32_t target_w = lv_display_get_horizontal_resolution(display);
    int32_t target_h = lv_display_get_vertical_resolution(display);

#if LV_COLOR_DEPTH == 16
    lv_color_format_t target_cf = LV_COLOR_FORMAT_RGB565;
#elif LV_COLOR_DEPTH == 32
    lv_color_format_t target_cf = LV_COLOR_FORMAT_ARGB8888;
#else
#error("Unsupported color format")
#endif


    for(size_t i = 0; i < config_count; ++i) {
        LV_LOG_TRACE("Got config %zu %#x %dx%d %d %d %d %d buffer size %d depth %d  samples %d stencil %d surface type %d",
                     i, configs[i].id,
                     configs[i].max_width, configs[i].max_height, configs[i].r_bits, configs[i].g_bits, configs[i].b_bits, configs[i].a_bits,
                     configs[i].buffer_size, configs[i].depth, configs[i].samples, configs[i].stencil, configs[i].surface_type);
    }

    for(size_t i = 0; i < config_count; ++i) {
        lv_color_format_t config_cf = lv_opengles_egl_color_format_from_egl_config(&configs[i]);
        const bool resolution_matches = configs[i].max_width >= target_w &&
                                        configs[i].max_height >= target_h;
        const bool is_nanovg_compatible = (configs[i].renderable_type & EGL_OPENGL_ES2_BIT) != 0 &&
                                          configs[i].stencil == 8 && configs[i].samples == 4;
        const bool is_window = (configs[i].surface_type & EGL_WINDOW_BIT) != 0;
        const bool is_compatible_with_draw_unit = is_nanovg_compatible || !LV_USE_DRAW_NANOVG;

        if(is_window && resolution_matches && config_cf == target_cf && is_compatible_with_draw_unit) {
            LV_LOG_TRACE("Choosing config %zu", i);
            return i;
        }
    }
    return config_count;
}


/**********************
*   STATIC FUNCTIONS
**********************/

static void * load_lib(const char ** libs, size_t count)
{
    const int mode = RTLD_NOW | RTLD_NODELETE;
    for(size_t i = 0; i < count; ++i) {

        void * handle = dlopen(libs[i], mode);
        if(handle) {
            return handle;
        }
    }
    return NULL;
}
static void * load_egl_lib(void)
{
    const char * egl_libs[] = {"libEGL.so", "libEGL.so.1"};
    return load_lib(egl_libs, sizeof(egl_libs) / sizeof(egl_libs[0]));
}
static void * load_gl_lib(void)
{
    const char * gl_libs[] = {"libGLESv2.so", "libGLESv2.so.2"};
    return load_lib(gl_libs, sizeof(gl_libs) / sizeof(gl_libs[0]));
}

static lv_result_t load_egl(lv_opengles_egl_t * ctx)
{
    ctx->egl_lib_handle = load_egl_lib();
    if(!ctx->egl_lib_handle) {
        LV_LOG_ERROR("Failed to load egl shared lib: %s", dlerror());
        goto err;
    }

    ctx->egl_display = create_egl_display(ctx);
    if(!ctx->egl_display) {
        LV_LOG_ERROR("Failed to create egl display");
        goto egl_display_err;
    }

    if(!gladLoadEGLUserPtr(ctx->egl_display, glad_egl_load_cb, ctx->egl_lib_handle)) {
        LV_LOG_ERROR("Failed to load EGL entry points");
        goto load_egl_functions_err;
    }

    if(eglBindAPI && !eglBindAPI(EGL_OPENGL_ES_API)) {
        LV_LOG_ERROR("Failed to bind api");
        goto load_egl_functions_err;
    }

    ctx->opengl_lib_handle = load_gl_lib();
    if(!ctx->opengl_lib_handle) {
        LV_LOG_ERROR("Failed to load OpenGL library. %s", dlerror());
        goto opengl_lib_err;
    }

    ctx->egl_config = create_egl_config(ctx);
    if(!ctx->egl_config) {
        LV_LOG_ERROR("Failed to create EGL config. Error code: %#x", eglGetError());
        goto egl_config_err;
    }

    if(ctx->interface.create_window_cb) {
        ctx->native_window = (EGLNativeWindowType) create_native_window(ctx);
        if(!ctx->native_window) {
            LV_LOG_ERROR("Failed to create native window");
            goto create_window_err;
        }
    }
    ctx->egl_surface = create_egl_surface(ctx);
    if(!ctx->egl_surface) {
        LV_LOG_ERROR("Failed to create EGL surface. Error code: %#x", eglGetError());
        goto egl_surface_err;
    }

    ctx->egl_context = create_egl_context(ctx);
    if(!ctx->egl_context) {
        LV_LOG_ERROR("Failed to create EGL context. Error code: %#x", eglGetError());
        goto egl_context_err;
    }

    if(!eglMakeCurrent(ctx->egl_display, ctx->egl_surface, ctx->egl_surface, ctx->egl_context)) {
        LV_LOG_ERROR("Failed to set current egl context. Error code: %#x", eglGetError());
        goto egl_make_current_context_err;
    }

    /* The swap interval only applies to window surfaces. Calling it on an offscreen
     * surface latches an EGL_BAD_SURFACE error for no reason, so skip it there. */
    if(ctx->native_window && !eglSwapInterval(ctx->egl_display, 0)) {
        LV_LOG_WARN("Can't set egl swap interval");
    }

    if(!gladLoadGLES2UserPtr(glad_egl_load_cb, ctx->opengl_lib_handle)) {
        LV_LOG_ERROR("Failed to load load OpenGL entry points");
        goto load_opengl_functions_err;
    }

    return LV_RESULT_OK;

load_opengl_functions_err:
    eglMakeCurrent(ctx->egl_display, NULL, NULL, NULL);
    eglDestroyContext(ctx->egl_display, ctx->egl_context);
egl_make_current_context_err:
    ctx->egl_context = NULL;
egl_context_err:
    ctx->egl_surface = NULL;
egl_surface_err:
    if(ctx->native_window && ctx->interface.destroy_window_cb) {
        ctx->interface.destroy_window_cb(ctx->interface.driver_data, (void *)ctx->native_window);
        ctx->native_window = 0;
    }
create_window_err:
    ctx->egl_config = NULL;
egl_config_err:
    dlclose(ctx->opengl_lib_handle);
    ctx->opengl_lib_handle = NULL;
opengl_lib_err:
    ctx->egl_display = NULL;
load_egl_functions_err:
egl_display_err:
    dlclose(ctx->egl_lib_handle);
    ctx->egl_lib_handle = NULL;
err:
    return LV_RESULT_INVALID;
}

static EGLDisplay create_egl_display(lv_opengles_egl_t * ctx)
{
    union {
        PFNEGLQUERYSTRINGPROC fn;
        void * ptr;
    } egl_query_string;

    union {
        PFNEGLGETPROCADDRESSPROC fn;
        void * ptr;
    } egl_get_proc_address;

    union {
        PFNEGLGETERRORPROC fn;
        void * ptr;
    } egl_get_error;

    union {
        PFNEGLGETDISPLAYPROC fn;
        void * ptr;
    } egl_get_display;

    union {
        PFNEGLINITIALIZEPROC fn;
        void * ptr;
    } egl_initialize;

    EGLDisplay display = NULL;

    egl_get_proc_address.ptr = dlsym(ctx->egl_lib_handle, "eglGetProcAddress");
    if(!egl_get_proc_address.ptr) {
        LV_LOG_ERROR("Failed to load eglGetProcAddress");
        return NULL;
    }

    egl_query_string.ptr = dlsym(ctx->egl_lib_handle, "eglQueryString");
    if(!egl_query_string.ptr) {
        LV_LOG_ERROR("Failed to load eglQueryString");
        return NULL;
    }

    egl_get_display.ptr = dlsym(ctx->egl_lib_handle, "eglGetDisplay");
    if(!egl_get_display.ptr) {
        LV_LOG_ERROR("Failed to load eglGetDisplay");
        return NULL;
    }

    egl_get_error.ptr = dlsym(ctx->egl_lib_handle, "eglGetError");
    if(!egl_get_error.ptr) {
        LV_LOG_ERROR("Failed to load eglGetError");
        return NULL;
    }

    egl_initialize.ptr = dlsym(ctx->egl_lib_handle, "eglInitialize");
    if(!egl_initialize.ptr) {
        LV_LOG_ERROR("Failed to load eglInitialize");
        return NULL;
    }

    char const * supported_extensions = egl_query_string.fn(EGL_NO_DISPLAY, EGL_EXTENSIONS);

    bool has_platform_display_ext_support = ctx->interface.egl_platform != 0 && supported_extensions &&
                                            strstr(supported_extensions, "EGL_EXT_platform_base");
    if(has_platform_display_ext_support) {
        PFNEGLGETPLATFORMDISPLAYEXTPROC egl_get_platform_display = (PFNEGLGETPLATFORMDISPLAYEXTPROC)
                                                                   egl_get_proc_address.fn("eglGetPlatformDisplayEXT");
        if(egl_get_platform_display) {
            display = egl_get_platform_display(ctx->interface.egl_platform, (void *)ctx->interface.native_display, NULL);
        }
        if(!display) {
            LV_LOG_WARN("Failed to get egl display from eglGetPlatformDisplay. Error code: %#x", egl_get_error.fn());
        }
    }

    if(!display) {
        LV_LOG_INFO("Falling back to eglGetDisplay()");
        display = egl_get_display.fn(ctx->interface.native_display);
    }

    if(!display) {
        LV_LOG_ERROR("Failed to get egl display from eglGetDisplay. Error code: %#x", egl_get_error.fn());
        return NULL;
    }

    EGLint egl_major;
    EGLint egl_minor;
    if(!egl_initialize.fn(display, &egl_major, &egl_minor)) {
        LV_LOG_ERROR("Failed to initialize egl. Error code: %#x", egl_get_error.fn());
        return NULL;
    }
    LV_LOG_INFO("Egl version %d.%d", egl_major, egl_minor);

    return display;
}

static GLADapiproc glad_egl_load_cb(void * userdata, const char * name)
{
    union {
        GLADapiproc fn;
        void * ptr;
    } result;

    if(eglGetProcAddress) {
        GLADapiproc sym = (GLADapiproc)eglGetProcAddress(name);
        if(sym) {
            return sym;
        }
    }
    result.ptr = dlsym(userdata, name);
    return result.fn;
}

static EGLConfig create_egl_config(lv_opengles_egl_t * ctx)
{
    const EGLint config_attribs[] = {
        EGL_RENDERABLE_TYPE,
        EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE,
        EGL_DONT_CARE,
        EGL_NONE
    };

    EGLint num_configs = 0;
    if(!eglChooseConfig(ctx->egl_display, config_attribs, 0, 0, &num_configs)) {
        LV_LOG_ERROR("Failed to get number of configs: %d", eglGetError());
        return NULL;
    }

    if(num_configs == 0) {
        LV_LOG_ERROR("No valid configs");
        return NULL;
    }

    EGLConfig * egl_configs = lv_malloc(num_configs * sizeof(*egl_configs));
    LV_ASSERT_MALLOC(egl_configs);
    if(!egl_configs) {
        LV_LOG_ERROR("Failed to allocate memory for possible configs");
        return NULL;
    }

    if(!eglChooseConfig(ctx->egl_display, config_attribs, egl_configs, num_configs, &num_configs)) {
        LV_LOG_ERROR("Failed to get configs: %d", eglGetError());
        lv_free(egl_configs);
        return NULL;
    }

    lv_egl_config_t * configs = lv_malloc(num_configs * sizeof(*configs));
    LV_ASSERT_MALLOC(configs);
    if(!configs) {
        LV_LOG_ERROR("Failed to allocate memory for configs");
        lv_free(egl_configs);
        return NULL;
    }

    size_t valid_config_count = 0;
    for(size_t i = 0; i < (size_t)num_configs; ++i) {
        lv_result_t err = lv_egl_config_from_egl_config(ctx, configs + valid_config_count, egl_configs[i]);
        if(err == LV_RESULT_OK) {
            egl_configs[valid_config_count] = egl_configs[i];
            valid_config_count ++;
        }
    }

    if(valid_config_count == 0) {
        LV_LOG_ERROR("Failed to parse available EGL configs");
        lv_free(egl_configs);
        lv_free(configs);
        return NULL;
    }

    size_t config_id = ctx->interface.select_config(ctx->interface.driver_data, configs, valid_config_count);

    if(config_id >= valid_config_count) {
        LV_LOG_ERROR("Failed to find suitable EGL config");
        lv_free(egl_configs);
        lv_free(configs);
        return NULL;
    }
    EGLConfig config = egl_configs[config_id];
    lv_free(configs);
    lv_free(egl_configs);
    return config;
}

static EGLSurface create_egl_surface(lv_opengles_egl_t * ctx)
{
    LV_ASSERT(ctx != NULL);
    LV_ASSERT(ctx->egl_display != NULL);
    LV_ASSERT(ctx->egl_config != NULL);

    if(ctx->interface.create_surface_cb) {
        lv_egl_create_surface_params_t params = {
            .config = ctx->egl_config,
            .display = ctx->egl_display,
        };
        return ctx->interface.create_surface_cb(ctx->interface.driver_data, &params);
    }

    LV_ASSERT(ctx->native_window != 0);
    return eglCreateWindowSurface(ctx->egl_display, ctx->egl_config, ctx->native_window, NULL);
}

static EGLContext create_egl_context(lv_opengles_egl_t * ctx)
{
    /* Try ES3 first */
    EGLint context_attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };

    EGLContext context = eglCreateContext(ctx->egl_display, ctx->egl_config,
                                          EGL_NO_CONTEXT, context_attribs);
    if(context != EGL_NO_CONTEXT) {
        ctx->gles_version = 3;
        return context;
    }

    LV_LOG_INFO("Failed to create an OpenGL ES 3 context (error %#x). Falling back to ES 2", eglGetError());

    /* fallback to ES2 */
    context_attribs[1] = 2;
    context = eglCreateContext(ctx->egl_display, ctx->egl_config,
                               EGL_NO_CONTEXT, context_attribs);
    if(context != EGL_NO_CONTEXT) {
        ctx->gles_version = 2;
    }
    return context;
}

lv_color_format_t lv_opengles_egl_color_format_from_egl_config(const lv_egl_config_t * config)
{
    if(config->r_bits == 5 && config->g_bits == 6 && config->b_bits == 5) {
        if(config->a_bits == 8) {
            return LV_COLOR_FORMAT_RGB565A8;
        }
        else {
            return LV_COLOR_FORMAT_RGB565;
        }
    }
    if(config->r_bits == 8 && config->g_bits == 8 && config->b_bits == 8) {
        if(config->a_bits == 8) {
            return LV_COLOR_FORMAT_ARGB8888;
        }
        else {
            return LV_COLOR_FORMAT_RGB888;
        }
    }
    LV_LOG_INFO("Unhandled color format (RGBA) (%d %d %d %d)", config->r_bits, config->g_bits, config->b_bits,
                config->a_bits);
    return LV_COLOR_FORMAT_UNKNOWN;
}

static lv_result_t lv_egl_config_from_egl_config(lv_opengles_egl_t * ctx, lv_egl_config_t * lv_egl_config,
                                                 EGLConfig egl_config)
{
    int res = 1;
    res &= eglGetConfigAttrib(ctx->egl_display, egl_config, EGL_CONFIG_ID, &lv_egl_config->id);
    res &= eglGetConfigAttrib(ctx->egl_display, egl_config, EGL_RED_SIZE, &lv_egl_config->r_bits);
    res &= eglGetConfigAttrib(ctx->egl_display, egl_config, EGL_GREEN_SIZE, &lv_egl_config->g_bits);
    res &= eglGetConfigAttrib(ctx->egl_display, egl_config, EGL_BLUE_SIZE, &lv_egl_config->b_bits);
    res &= eglGetConfigAttrib(ctx->egl_display, egl_config, EGL_ALPHA_SIZE, &lv_egl_config->a_bits);
    res &= eglGetConfigAttrib(ctx->egl_display, egl_config, EGL_MAX_PBUFFER_WIDTH, &lv_egl_config->max_width);
    res &= eglGetConfigAttrib(ctx->egl_display, egl_config, EGL_MAX_PBUFFER_HEIGHT, &lv_egl_config->max_height);
    res &= eglGetConfigAttrib(ctx->egl_display, egl_config, EGL_BUFFER_SIZE, &lv_egl_config->buffer_size);
    res &= eglGetConfigAttrib(ctx->egl_display, egl_config, EGL_DEPTH_SIZE, &lv_egl_config->depth);
    res &= eglGetConfigAttrib(ctx->egl_display, egl_config, EGL_STENCIL_SIZE, &lv_egl_config->stencil);
    res &= eglGetConfigAttrib(ctx->egl_display, egl_config, EGL_SAMPLES, &lv_egl_config->samples);
    res &= eglGetConfigAttrib(ctx->egl_display, egl_config, EGL_SURFACE_TYPE, &lv_egl_config->surface_type);
    res &= eglGetConfigAttrib(ctx->egl_display, egl_config, EGL_RENDERABLE_TYPE, &lv_egl_config->renderable_type);

    if(!res) {
        LV_LOG_WARN("Failed to fetch egl config properties");
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

static void * create_native_window(lv_opengles_egl_t * ctx)
{
    EGLint native_config_id;
    uint64_t * mods = NULL;
    size_t mod_count = 0;
    lv_result_t res = get_native_config(ctx, &native_config_id, &mods, &mod_count);

    if(res == LV_RESULT_INVALID) {
        LV_LOG_ERROR("Failed to get native config");
        return NULL;
    }

    lv_egl_native_window_properties_t properties = { .visual_id = native_config_id };

    void * native_window = ctx->interface.create_window_cb(ctx->interface.driver_data, &properties);
    if(!native_window) {
        LV_LOG_ERROR("Failed to create window");
    }
    lv_free(mods);
    return native_window;
}

static lv_result_t get_native_config(lv_opengles_egl_t * ctx, EGLint * native_id, uint64_t ** mods, size_t * count)
{
    EGLint num_mods;

    if(!eglGetConfigAttrib(ctx->egl_display, ctx->egl_config, EGL_NATIVE_VISUAL_ID, native_id)) {
        LV_LOG_ERROR("Failed to get native visual id for egl config");
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;

    if(!eglQueryDmaBufModifiersEXT || !eglQueryDmaBufModifiersEXT(ctx->egl_display, *native_id, 0, NULL, NULL, &num_mods)) {
        LV_LOG_WARN("Failed to get native modifiers");
        return LV_RESULT_OK;
    }

    if(num_mods <= 0) {
        LV_LOG_INFO("No native modifiers");
        return LV_RESULT_OK;
    }

    *mods = lv_malloc(num_mods * sizeof(*mods));
    LV_ASSERT_MALLOC(mods);
    eglQueryDmaBufModifiersEXT(ctx->egl_display, *native_id, num_mods, *mods, NULL, &num_mods);
    if(*mods[0] == 0) {
        lv_free(mods);
        return LV_RESULT_OK;
    }
    *count = (size_t) num_mods;
    return LV_RESULT_OK;
}

#endif /*LV_USE_EGL*/
