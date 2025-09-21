/**
 * @file lv_egl_adapter.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"
#include "../../../misc/lv_log.h"

#include "private/glad/include/glad/egl.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../egl_adapter_runtime_defaults.h"
#include "../egl_adapter_runtime_defaults_internal.h"

#include "lv_egl_adapter.h"
#include "private/__lv_egl_adapter.h"
#include "private/lv_egl_adapter_typedefs.h"
#include "private/lv_egl_adapter_output_core.h"


#if defined(WIN32)
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

/*********************
*      DEFINES
*********************/

#if  LV_ADAPTED_ON_WAYLAND
    #define GLMARK2_NATIVE_EGL_DISPLAY_ENUM EGL_PLATFORM_WAYLAND_KHR
#elif  LV_ADAPTED_ON_DRM
    #define GLMARK2_NATIVE_EGL_DISPLAY_ENUM EGL_PLATFORM_GBM_KHR
#else
    // Platforms not in the above platform enums (GBM) fall back to eglGetDisplay.
    #define GLMARK2_NATIVE_EGL_DISPLAY_ENUM 0
#endif /* LV_ADAPTED_ON_WAYLAND */

#if LV_EGL_ADAPTED_WITH_GLESV2
    #define DEFINED_EGL_OPENGL_BIT EGL_OPENGL_ES2_BIT
    GLAD_API_CALL int gladLoadGLES2UserPtr(GLADuserptrloadfunc load, void * userptr);
    GLAD_API_CALL int gladLoadGLES2(GLADloadfunc load);
#elif LV_EGL_ADAPTED_WITH_GL
    #define DEFINED_EGL_OPENGL_BIT EGL_OPENGL_BIT
#else
    #define DEFINED_EGL_OPENGL_BIT 0
#endif /* LV_EGL_ADAPTED_WITH_GLESV2 */

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

bool lv_egl_adapter_init_extensions(void * state_ptr);
bool set_zero_swap_interval(void * adapter_ptr);
void interface_destroy_internal(void ** cnvs_ptr);
void get_glvisualconfig(void * adapter_ptr, EGLConfig config, lv_egl_adapter_config_t * visual_config);
EGLConfig select_best_config(void * adapter_ptr, size_t num_configs, EGLConfig * configs);
GLADapiproc load_proc(void * userdata, const char * name);
bool egl_display_is_valid(void * adapter_ptr);
bool egl_config_is_valid(void * adapter_ptr);
bool egl_surface_is_valid(void * adapter_ptr);
bool egl_context_is_valid(void * adapter_ptr);

void * external_open_path(const char * external_path);
bool external_open(void ** extern_handle_ptr, const char ** alt_names, size_t count);
void external_close(void ** extern_handle_ptr);
void * external_load(void * extern_handle, const char * symbol);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_egl_adapter_init_display(void * adapter_ptr, void * native_display)
{
    lv_egl_adapter_t * adapter_ref = (lv_egl_adapter_t *)(adapter_ptr);

    const char * libnames[] = { "libEGL.so", "libEGL.so.1" };
    if(!external_open(&adapter_ref->egl_extern_handle, libnames, 2)) {
        LV_LOG_ERROR("Unable to load EGL library");
        return false;
    }
    adapter_ref->egl_native_display = (EGLNativeDisplayType)native_display;
    return egl_display_is_valid(adapter_ref);
}
bool lv_egl_adapter_init_surface(void * adapter_ptr, void * native_window)
{
    lv_egl_adapter_t * adapter_ref = (lv_egl_adapter_t *)(adapter_ptr);
    adapter_ref->egl_native_window = (EGLNativeWindowType)native_window;
    return egl_surface_is_valid(adapter_ref);
}
bool lv_egl_adapter_init_extensions(void * adapter_ptr)
{
    lv_egl_adapter_t * adapter_ref = (lv_egl_adapter_t *)(adapter_ptr);
#if LV_EGL_ADAPTED_WITH_GLESV2
    int version_result = gladLoadGLES2UserPtr(load_proc, adapter_ref->gl_extern_handle);
    if(!version_result) {
        LV_LOG_ERROR("Loading GLESv2 entry points failed.");
        return false;
    }
    else {
        LV_LOG_INFO("GLES2 loaded with version result: %d.", version_result);
    }
#elif LV_EGL_ADAPTED_WITH_GL
    if(!gladLoadGLUserPtr(load_proc, adapter_ref->gl_extern_handle)) {
        LV_LOG_ERROR("Loading GL entry points failed.");
        return false;
    }
#endif
    return true;
}
lv_egl_adapter_output_core_t * lv_egl_adapter_get_output_core(void * adapter_ptr)
{
    lv_egl_adapter_t * adapter_ref = (lv_egl_adapter_t *)(adapter_ptr);
    return adapter_ref->output_core;
}
bool lv_egl_adapter_is_valid(void * adapter_ptr)
{
    lv_egl_adapter_t * adapter_ref = (lv_egl_adapter_t *)(adapter_ptr);
    if(!egl_display_is_valid(adapter_ref)) return false;
    if(!egl_config_is_valid(adapter_ref)) return false;
    if(!egl_surface_is_valid(adapter_ref)) return false;
    if(!egl_context_is_valid(adapter_ref)) return false;
    if(eglGetCurrentContext && adapter_ref->egl_context == eglGetCurrentContext()) return true;
    if(!eglMakeCurrent(adapter_ref->egl_display, adapter_ref->egl_surface, adapter_ref->egl_surface,
                       adapter_ref->egl_context)) {
        LV_LOG_ERROR("eglMakeCurrent failed with error: 0x%x", eglGetError());
        return false;
    }
    if(!set_zero_swap_interval(adapter_ref))
        LV_LOG_WARN("Failed to set swap interval. Results may be bounded above by refresh rate.\n");
    if(!lv_egl_adapter_init_extensions(adapter_ref)) return false;
    return true;
}
void lv_egl_adapter_set_output_core(void * adapter_ptr, lv_egl_adapter_output_core_t * core_ptr)
{
    lv_egl_adapter_t * adapter_ref = (lv_egl_adapter_t *)(adapter_ptr);
    adapter_ref->output_core = core_ptr;
}
bool lv_egl_adapter_reset(void * adapter_ptr)
{
    lv_egl_adapter_t * adapter_ref = (lv_egl_adapter_t *)(adapter_ptr);
    if(!egl_display_is_valid(adapter_ref)) return false;
    if(!adapter_ref->egl_context) return true;
    if(eglGetCurrentContext &&
       adapter_ref->egl_context == eglGetCurrentContext()) eglMakeCurrent(adapter_ref->egl_display, 0, 0, 0);
    if(EGL_FALSE == eglDestroyContext(adapter_ref->egl_display,
                                      adapter_ref->egl_context)) LV_LOG_WARN("eglDestroyContext failed with error: 0x%x", eglGetError());
    adapter_ref->egl_context = 0;
    return true;
}
void lv_egl_adapter_swap(void * adapter_ptr)
{
    lv_egl_adapter_t * adapter_ref = (lv_egl_adapter_t *)(adapter_ptr);
    eglSwapBuffers(adapter_ref->egl_display, adapter_ref->egl_surface);
}
void lv_egl_adapter_got_visual_config(void * adapter_ptr, lv_egl_adapter_config_t * vc)
{
    lv_egl_adapter_t * adapter_ref = (lv_egl_adapter_t *)(adapter_ptr);
    if(!egl_config_is_valid(adapter_ref)) return;
    get_glvisualconfig(adapter_ref, adapter_ref->egl_config, vc);
}
void lv_egl_adapter_init(void * adapter_ptr, lv_egl_adapter_config_t * config)
{
    lv_egl_adapter_t * adapter_ref = (lv_egl_adapter_t *)(adapter_ptr);
    adapter_ref->egl_native_display = 0;
    adapter_ref->egl_native_window = 0;
    adapter_ref->egl_display = 0;
    adapter_ref->egl_config = 0;
    adapter_ref->egl_context = 0;
    adapter_ref->egl_surface = 0;
    adapter_ref->output_core = 0;
    adapter_ref->owns_config = false;
    adapter_ref->requested_visual_config = config;
    adapter_ref->egl_extern_handle = NULL;
    adapter_ref->gl_extern_handle = NULL;
}
bool lv_egl_adapter_supports_sync()
{
    return eglCreateSync && eglClientWaitSync && eglDestroySync;
}
bool lv_egl_adapter_prefers_sync(void * adapter_ptr)
{
    return lv_egl_adapter_config_get_vsync(((lv_egl_adapter_t *)adapter_ptr)->requested_visual_config);
}
bool lv_egl_adapter_got_native_config(void * adapter_ptr, EGLint * vid, lv_array_t * mods)
{
    lv_array_deinit(mods);
    lv_egl_adapter_t * adapter_ref = (lv_egl_adapter_t *)(adapter_ptr);
    if(!egl_config_is_valid(adapter_ref)) return false;
    EGLint native_id;
    if(!eglGetConfigAttrib(adapter_ref->egl_display, adapter_ref->egl_config, EGL_NATIVE_VISUAL_ID, &native_id)) {
        LV_LOG_INFO("Failed to get native visual id for EGLConfig %x", lv_egl_adapter_mode_get_id(adapter_ref->egl_config));
        return false;
    }
    if(eglQueryDmaBufModifiersEXT) {
        EGLint num_mods = 0;
        if(eglQueryDmaBufModifiersEXT(adapter_ref->egl_display, native_id, 0, NULL, NULL, &num_mods) && num_mods > 0) {
            uint64_t mods_buffer[num_mods];
            if(!eglQueryDmaBufModifiersEXT(adapter_ref->egl_display, native_id, num_mods, mods_buffer, NULL, &num_mods) ||
               num_mods <= 0) {
                LV_LOG_INFO("Driver does not support eglQueryDmaBufModifiersEXT.");
            }
            else {
                if(mods_buffer[0] != 0) {
                    lv_array_init_from_buf(mods, mods_buffer, num_mods, sizeof(uint64_t));
                    LV_LOG_INFO("This driver supports eglQueryDmaBufModifiersEXT, with %lu defined modifiers:", (size_t)num_mods);
                    for(size_t i = 0; i < (size_t)num_mods; i++) LV_LOG_INFO("+->#%lu = %lu", i, mods_buffer[i]);
                }
            }
        }
    }
    *vid = native_id;
    return true;
}
void lv_egl_adapter_cleanup(void ** adapter_ptr, void ** config_ptr, void ** cnvs_ptr)
{
    lv_egl_adapter_t * adapter_ref = * (lv_egl_adapter_t * *)adapter_ptr;
    if(adapter_ref) {
        lv_egl_adapter_reset(adapter_ref);
        free(adapter_ref->best_config);
        if(adapter_ref->egl_display != NULL)
            if(!eglTerminate(adapter_ref->egl_display))
                LV_LOG_ERROR("eglTerminate failed");
        if(adapter_ref->owns_config) {
            lv_egl_adapter_config_cleanup((void **) & (adapter_ref->requested_visual_config));
        }
    }
    if(eglReleaseThread && !eglReleaseThread()) LV_LOG_ERROR("eglReleaseThread failed");
    if(cnvs_ptr) interface_destroy_internal(cnvs_ptr);
    if(config_ptr) lv_egl_adapter_config_cleanup(config_ptr);
    if(adapter_ref) free(adapter_ref);
    *adapter_ptr = NULL;
}
lv_egl_adapter_t * lv_egl_adapter_create(lv_egl_adapter_config_t * config)
{
    lv_egl_adapter_t * adapter = (lv_egl_adapter_t *)malloc(sizeof(struct lv_egl_adapter));
    if(adapter) lv_egl_adapter_init(adapter, config);
    return adapter;
}
lv_egl_adapter_t * lv_egl_adapter_create_auto(void)
{
    lv_egl_adapter_config_t * _egl_options = lv_egl_adapter_config_by_id(LV_EGL_BUFFER_MODE);
    lv_egl_adapter_config_set_vsync(_egl_options, LV_EGL_SYNC);
    lv_egl_adapter_t * adapter = (lv_egl_adapter_t *)malloc(sizeof(struct lv_egl_adapter));
    if(adapter) {
        lv_egl_adapter_init(adapter, _egl_options);
        adapter->owns_config = true;
    }
    return adapter;
}
struct lv_egl_adapter_sync * lv_egl_adapter_create_sync(void * adapter_ptr)
{
    lv_egl_adapter_t * adapter_ref = (lv_egl_adapter_t *)adapter_ptr;
    struct lv_egl_adapter_sync * s = malloc(sizeof(*s));
    if(!s) return NULL;
    s->display = adapter_ref->egl_display;
    s->sync = eglCreateSync(s->display, EGL_SYNC_FENCE, NULL);
    return s;
}
void lv_egl_adapter_sync_wait(void * lv_egl_adapter_sync_ptr)
{
    lv_egl_adapter_sync_t * sync_ref = (lv_egl_adapter_sync_t *)lv_egl_adapter_sync_ptr;
    if(sync_ref && sync_ref->sync)
        eglClientWaitSync(sync_ref->display, sync_ref->sync, EGL_SYNC_FLUSH_COMMANDS_BIT, EGL_FOREVER);
}
void lv_egl_adapter_destroy_sync(struct lv_egl_adapter_sync * s)
{
    if(!s) return;
    if(s->sync) {
        eglDestroySync(s->display, s->sync);
        s->sync = NULL;
    }
    free(s);
}
bool set_zero_swap_interval(void * adapter_ptr)
{
    lv_egl_adapter_t * adapter_ref = (lv_egl_adapter_t *)adapter_ptr;
    if(!lv_egl_adapter_config_get_vsync(adapter_ref->requested_visual_config) &&
       (!eglSwapInterval || !eglSwapInterval(adapter_ref->egl_display, 0))) return false;
    return true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

void get_glvisualconfig(void * adapter_ptr, EGLConfig config, lv_egl_adapter_config_t * visual_config)
{
    lv_egl_adapter_t * adapter_ref = (lv_egl_adapter_t *)(adapter_ptr);
    eglGetConfigAttrib(adapter_ref->egl_display, config, EGL_CONFIG_ID, &visual_config->id);
    eglGetConfigAttrib(adapter_ref->egl_display, config, EGL_BUFFER_SIZE, &visual_config->buffer);
    eglGetConfigAttrib(adapter_ref->egl_display, config, EGL_RED_SIZE, &visual_config->red);
    eglGetConfigAttrib(adapter_ref->egl_display, config, EGL_GREEN_SIZE, &visual_config->green);
    eglGetConfigAttrib(adapter_ref->egl_display, config, EGL_BLUE_SIZE, &visual_config->blue);
    eglGetConfigAttrib(adapter_ref->egl_display, config, EGL_ALPHA_SIZE, &visual_config->alpha);
    eglGetConfigAttrib(adapter_ref->egl_display, config, EGL_DEPTH_SIZE, &visual_config->depth);
    eglGetConfigAttrib(adapter_ref->egl_display, config, EGL_STENCIL_SIZE, &visual_config->stencil);
    eglGetConfigAttrib(adapter_ref->egl_display, config, EGL_SAMPLES, &visual_config->samples);
}
EGLConfig select_best_config(void * adapter_ptr, size_t num_configs, EGLConfig * configs)
{
    lv_egl_adapter_t * adapter_ref = (lv_egl_adapter_t *)(adapter_ptr);
    int best_score = -111111;
    EGLConfig best_config = 0;
    for(size_t i = 0; i < num_configs; i++) {
        const EGLConfig config = configs[i];
        lv_egl_adapter_config_t * vc = lv_egl_adapter_config_create();
        int score;
        get_glvisualconfig(adapter_ref, config, vc);
        score = lv_egl_adapter_config_match_score(vc, adapter_ref->requested_visual_config);
        if(score > best_score) {
            best_score = score;
            best_config = config;
        }
        free(vc);
    }
    if(best_score <= 0) {
#ifdef LV_EGL_ADAPTER_STRICT_MATCH
        return NULL;
#endif
        if(adapter_ref->requested_visual_config->id != 0x0)
            LV_LOG_WARN("No perfect match for the specified EGL config #%d, using best match.",
                        adapter_ref->requested_visual_config->id);
    }
    return best_config;
}
GLADapiproc load_proc(void * userdata, const char * name)
{
    if(eglGetProcAddress) {
        GLADapiproc sym = (GLADapiproc)eglGetProcAddress(name);
        if(sym) {
            return sym;
        }
    }
    return (GLADapiproc)external_load(userdata, name);
}
bool egl_display_is_valid(void * adapter_ptr)
{
    lv_egl_adapter_t * adapter_ref = (lv_egl_adapter_t *)(adapter_ptr);
    if(adapter_ref->egl_display)
        return true;

    PFNEGLQUERYSTRINGPROC    egl_query_string     = (PFNEGLQUERYSTRINGPROC)(external_load(
                                                                                adapter_ref->egl_extern_handle, "eglQueryString"));
    PFNEGLGETPROCADDRESSPROC egl_get_proc_address = (PFNEGLGETPROCADDRESSPROC)(external_load(
                                                                                   adapter_ref->egl_extern_handle, "eglGetProcAddress"));
    PFNEGLGETERRORPROC       egl_get_error        = (PFNEGLGETERRORPROC)(external_load(adapter_ref->egl_extern_handle,
                                                                                       "eglGetError"));
    PFNEGLGETDISPLAYPROC     egl_get_display      = (PFNEGLGETDISPLAYPROC)(external_load(
                                                                               adapter_ref->egl_extern_handle, "eglGetDisplay"));
    PFNEGLINITIALIZEPROC     egl_initialize       = (PFNEGLINITIALIZEPROC)(external_load(
                                                                               adapter_ref->egl_extern_handle, "eglInitialize"));

    if(!egl_query_string || !egl_get_proc_address || !egl_get_error || !egl_get_display || !egl_initialize) return false;

    char const * __restrict const supported_extensions = egl_query_string(EGL_NO_DISPLAY, EGL_EXTENSIONS);

    bool extensions_supported = false;
    if(GLMARK2_NATIVE_EGL_DISPLAY_ENUM != 0 && supported_extensions &&
       strstr(supported_extensions, "EGL_EXT_platform_base")) {
        extensions_supported = true;
        PFNEGLGETPLATFORMDISPLAYEXTPROC egl_get_platform_display =
            (PFNEGLGETPLATFORMDISPLAYEXTPROC)
            egl_get_proc_address("eglGetPlatformDisplayEXT");

        if(egl_get_platform_display != NULL) adapter_ref->egl_display = egl_get_platform_display(
                                                                                GLMARK2_NATIVE_EGL_DISPLAY_ENUM,
                                                                                (void *)adapter_ref->egl_native_display, NULL);

        if(!adapter_ref->egl_display) {
            LV_LOG_WARN("eglGetPlatformDisplayEXT() failed with error: 0x%x",
                        egl_get_error());
        }
    }
    if(extensions_supported)
        LV_LOG_USER("EGL Platform Display Extensions Valid: [ Yes ]");
    else
        LV_LOG_USER("EGL Platform Display Extensions Valid: [ No ]");

    if(!adapter_ref->egl_display) {
        LV_LOG_INFO("Falling back to eglGetDisplay()");
        adapter_ref->egl_display = egl_get_display(adapter_ref->egl_native_display);
    }

    if(!adapter_ref->egl_display) {
        LV_LOG_ERROR("eglGetDisplay() failed with error: 0x%x", egl_get_error());
        return false;
    }

    int egl_major = -1;
    int egl_minor = -1;
    if(!egl_initialize(adapter_ref->egl_display, &egl_major, &egl_minor)) {
        LV_LOG_ERROR("eglInitialize() failed with error: 0x%x", egl_get_error());
        adapter_ref->egl_display = 0;
        return false;
    }

    if(gladLoadEGLUserPtr(adapter_ref->egl_display, load_proc, adapter_ref->egl_extern_handle) == 0) {
        LV_LOG_ERROR("Loading EGL entry points failed");
        return false;
    }

#if LV_EGL_ADAPTED_WITH_GL
    EGLenum apiType = EGL_OPENGL_API;
    const char * libNames[] = { "libGL.so", "libGL.so.1" };
    if(!GLAD_EGL_VERSION_1_4) {
        LV_LOG_ERROR("EGL version %d.%d does not support the OpenGL API", egl_major, egl_minor);
        return false;
    }
#else // LV_EGL_ADAPTED_WITH_GLESV2
    EGLenum apiType = EGL_OPENGL_ES_API;
    const char * libNames[] = { "libGLESv2.so", "libGLESv2.so.2" };
#endif

    if(eglBindAPI && !eglBindAPI(apiType)) {
        LV_LOG_ERROR("Failed to bind api");
        return false;
    }

    if(!external_open(&adapter_ref->gl_extern_handle, libNames, 2)) {
        LV_LOG_ERROR("Unable to load GL library");
        return false;
    }

    return true;
}
bool egl_config_is_valid(void * adapter_ptr)
{
    lv_egl_adapter_t * adapter_ref = (lv_egl_adapter_t *)(adapter_ptr);
    if(adapter_ref->egl_config) {
        return true;
    }
    if(!egl_display_is_valid(adapter_ref)) {
        return false;
    }

    const EGLint config_attribs[] = {
        EGL_RENDERABLE_TYPE,
        DEFINED_EGL_OPENGL_BIT,
        EGL_NONE
    };

    // Find out how many configs match the attributes.
    EGLint num_configs = 0;
    if(!eglChooseConfig(adapter_ref->egl_display, config_attribs, 0, 0, &num_configs)) {
        LV_LOG_ERROR("eglChooseConfig() (count query) failed with error: %d",
                     eglGetError());
        return false;
    }

    if(num_configs == 0) {
        LV_LOG_ERROR("eglChooseConfig() didn't return any configs");
        return false;
    }

    // Get all the matching configs
    EGLConfig _EGLConfig_struct_buffer[num_configs];

    if(!eglChooseConfig(adapter_ref->egl_display, config_attribs, _EGLConfig_struct_buffer,
                        num_configs, &num_configs)) {
        LV_LOG_ERROR("eglChooseConfig() failed with error: %d",
                     eglGetError());
        return false;
    }

    // Select the best matching config
    adapter_ref->egl_config = select_best_config(adapter_ref, (size_t)num_configs, &_EGLConfig_struct_buffer[0]);
    if(!adapter_ref->egl_config) {
        LV_LOG_ERROR("Failed to find suitable EGL config");
        return false;
    }

    for(size_t i = 0; i < (size_t)num_configs; i++) {
        if(_EGLConfig_struct_buffer[i] == adapter_ref->egl_config) {
            lv_egl_adapter_mode_t * cfg = lv_egl_adapter_mode_create(adapter_ref->egl_display, _EGLConfig_struct_buffer[i]);
            adapter_ref->best_config = cfg;
            break;
        }
    }
    //#ifdef LV_EXTRA_EGL_INFO
    if((adapter_ref->requested_visual_config->id == 0x0) ||
       (lv_egl_adapter_mode_get_id(adapter_ref->best_config) != adapter_ref->requested_visual_config->id)) {
        LV_LOG_USER("Found (%u) buffer configs. Best match: [ 0x%x ]", num_configs,
                    lv_egl_adapter_mode_get_id(adapter_ref->best_config));
        unsigned int lineNumber = 0;
        bool is_best = false;
        for(size_t i = 0; i < (size_t)num_configs; i++) {
            lv_egl_adapter_mode_t * cfg = lv_egl_adapter_mode_create(adapter_ref->egl_display, _EGLConfig_struct_buffer[i]);
            if(!lineNumber) lv_egl_adapter_mode_print_header();
            is_best = lv_egl_adapter_mode_get_id(adapter_ref->best_config) == lv_egl_adapter_mode_get_id(cfg);
            if(is_best && (lineNumber > 0)) lv_egl_adapter_mode_print_bar();
            lv_egl_adapter_mode_print(cfg, is_best);
            if(is_best && (lineNumber > 0)) lv_egl_adapter_mode_print_bar();
            lineNumber++;
            free(cfg);
        }
        if(!is_best) lv_egl_adapter_mode_print_bar();
    }
    else {
        LV_LOG_USER("Using specified buffer config id: [ 0x%x ]", lv_egl_adapter_mode_get_id(adapter_ref->best_config));
    }
    /*#ifndef LV_EXTRA_EGL_INFO_MORE
        lv_egl_adapter_mode_print_header();
        lv_egl_adapter_mode_print(adapter_ref->best_config, false);
        lv_egl_adapter_mode_print_bar();
    #else
        unsigned int lineNumber = 0;
        bool is_best = false;
        for(size_t i = 0; i < (size_t)num_configs; i++) {
            lv_egl_adapter_mode_t * cfg = lv_egl_adapter_mode_create(adapter_ref->egl_display, _EGLConfig_struct_buffer[i]);
            if(!lineNumber) lv_egl_adapter_mode_print_header();
            is_best = lv_egl_adapter_mode_get_id(adapter_ref->best_config) == lv_egl_adapter_mode_get_id(cfg);
            if(is_best && (lineNumber > 0)) lv_egl_adapter_mode_print_bar();
            lv_egl_adapter_mode_print(cfg, is_best);
            if(is_best && (lineNumber > 0)) lv_egl_adapter_mode_print_bar();
            lineNumber++;
            free(cfg);
        }
        if(!is_best) lv_egl_adapter_mode_print_bar();
        LV_LOG_INFO("    [*] = Best EGLConfig ID: 0x%x\n", lv_egl_adapter_mode_get_id(adapter_ref->best_config));
    #endif
    #endif */

    return true;
}
bool egl_surface_is_valid(void * adapter_ptr)
{
    lv_egl_adapter_t * adapter_ref = (lv_egl_adapter_t *)(adapter_ptr);
    if(adapter_ref->egl_surface) return true;
    if(!egl_display_is_valid(adapter_ref)) return false;
    if(!egl_config_is_valid(adapter_ref)) return false;
    adapter_ref->egl_surface = eglCreateWindowSurface(adapter_ref->egl_display, adapter_ref->egl_config,
                                                      adapter_ref->egl_native_window, 0);
    if(!adapter_ref->egl_surface) {
        LV_LOG_ERROR("eglCreateWindowSurface failed with error: 0x%x\n", eglGetError());
        return false;
    }
    return true;
}
bool egl_context_is_valid(void * adapter_ptr)
{
    lv_egl_adapter_t * adapter_ref = (lv_egl_adapter_t *)(adapter_ptr);

    if(adapter_ref->egl_context)
        return true;

    if(!egl_display_is_valid(adapter_ref))
        return false;

    if(!egl_config_is_valid(adapter_ref))
        return false;

    static const EGLint context_attribs[] = {
#ifdef LV_EGL_ADAPTED_WITH_GLESV2
        EGL_CONTEXT_CLIENT_VERSION, 2,
#endif
        EGL_NONE
    };

    adapter_ref->egl_context = eglCreateContext(adapter_ref->egl_display, adapter_ref->egl_config,
                                                EGL_NO_CONTEXT, context_attribs);
    if(!adapter_ref->egl_context) {
        LV_LOG_ERROR("eglCreateContext() failed with error: 0x%x",
                     eglGetError());
        return false;
    }

    return true;
}

void * external_open_path(const char * external_path)
{
    void * extern_handle;
#if defined(WIN32)
    extern_handle = LoadLibraryA(external_path);
#else
    extern_handle = dlopen(external_path, RTLD_NOW | RTLD_NODELETE);
#endif
    return extern_handle;
}
bool external_open(void ** extern_handle_ptr, const char ** alt_names, size_t count)
{
    for(size_t i = 0; i < count; ++i) {
        *extern_handle_ptr = external_open_path(alt_names[i]);
        if(*extern_handle_ptr) {
            return true;
        }
    }
    return false;
}
void external_close(void ** extern_handle_ptr)
{
    if(*extern_handle_ptr) {
#if defined(WIN32)
        FreeLibrary((HMODULE)(*extern_handle_ptr));
#else
        dlclose(*extern_handle_ptr);
#endif
        *extern_handle_ptr = NULL;
    }
}
void * external_load(void * extern_handle, const char * symbol)
{
    if(!extern_handle) return NULL;

#if defined(WIN32)
    return (void *)GetProcAddress((HMODULE)extern_handle, symbol);
#else
    return dlsym(extern_handle, symbol);
#endif
}
