/**
 * @file lv_egl_adapter_interface.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"
#include "../../../misc/lv_array.h"
#include "../../../misc/lv_log.h"

#include "lv_egl_adapter_interface.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "lv_egl_adapter.h"
#include __LV_OUTMOD_CLASS_INCLUDE_T


/*********************
*      DEFINES
*********************/

/**********************
 *      TYPEDEFS
 **********************/

struct lv_egl_adapter_fbo {
    GLuint color_renderbuffer;
    GLuint depth_renderbuffer;
    GLuint fbo;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static GLenum determine_color_format(const lv_egl_adapter_config_t * vc, bool supports_rgba8, bool supports_rgb8);
static GLenum determine_depth_format(const lv_egl_adapter_config_t * vc, bool supports_depth32, bool supports_depth24);
static bool determine_gl_extension_support(const char * ext);
static const char * interface_gl_format_str(GLenum f);
static void interface_fbos_reset(void * fboarray_ptr);
static bool interface_confirm_gl2_support(void);
static bool interface_confirm_pixel_format(void * cnvs_ptr);
static bool interface_fbos_confirm(void * cnvs_ptr);
static bool interface_fbos_apply(void * cnvs_ptr);
static void interface_fbos_deinit(void * cnvs_ptr);
static bool interface_resize_output(void * cnvs_ptr, int width, int height);
static inline lv_egl_adapter_interface_t * CAST(void * _cnvs_ptr);
static inline lv_egl_adapter_output_core_t * CORE(void * cnvs_ptr);
static bool FALSE_ERROR(const char * desc);
/* static void unproject_pixel_world(float mouse_x, float mouse_y, float w, float h,
 *                                 const float invVP[16], float nd, float out[3]);
 */

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_egl_adapter_interface_init(void * cnvs_ptr)
{
    lv_egl_adapter_interface_t * interface = CAST(cnvs_ptr);
#if LV_ADAPTED_ON_WAYLAND
    lv_egl_adapter_output_core_t * core = lv_egl_adapter_outmod_wayland_get_core(interface->output_module);
#else
    lv_egl_adapter_output_core_t * core = lv_egl_adapter_outmod_drm_get_core(interface->output_module);
#endif
    lv_egl_adapter_set_output_core(interface->egl_adapter, core);
    if(!core->init_display(interface->output_module, &(interface->width), &(interface->height),
                           interface->refresh_rate)) FALSE_ERROR("Canvas init() -> ERROR INITIALIzING DRM");
    if(!lv_egl_adapter_init_display(interface->egl_adapter,
                                    core->display(interface->output_module))) FALSE_ERROR("Canvas init() -> ERROR INITIALIzING GL");
    return lv_egl_adapter_interface_reset(interface);
}
bool lv_egl_adapter_interface_reset(void * cnvs_ptr)
{
    lv_egl_adapter_interface_t * interface = CAST(cnvs_ptr);
    interface_fbos_deinit(interface);
    if(!lv_egl_adapter_reset(interface->egl_adapter)) return false;
    if(!interface_resize_output(interface, interface->width, interface->height)) return false;
    if(!interface_fbos_apply(interface)) return false;
    if(!interface_confirm_gl2_support()) {
        FALSE_ERROR("OpenGL(ES) >= 2.0 required, but version string is:");
        LV_LOG_ERROR("%s", (const char *)glGetString(GL_VERSION));
        return false;
    }
    glViewport(0, 0, interface->width, interface->height);
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LEQUAL);
    glDisable(GL_DEPTH_TEST);

    //glDepthMask(GL_TRUE);
    //glPixelStorei(GL_PACK_ALIGNMENT, 1);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
    lv_egl_adapter_interface_clear();
    return true;
}
void lv_egl_adapter_interface_visible(void * cnvs_ptr, bool visible)
{
    lv_egl_adapter_interface_t * interface = CAST(cnvs_ptr);
    if(visible && !interface->offscreen_fbo_count) CORE(interface)->visible(interface->output_module, visible);
}
void lv_egl_adapter_interface_clear()
{
    glClearColor(0.19f, 0.195f, 0.2f, 1.0f);
    glDepthRangef(-1.0, 1.0);
    glDepthFunc(GL_ALWAYS);

#if LV_EGL_ADAPTED_WITH_GL
    glClearDepth(1.0f);
#elif LV_EGL_ADAPTED_WITH_GLESV2
    glClearDepthf(1.0f);
#endif
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void lv_egl_adapter_interface_print_info(void * cnvs_ptr)
{
    interface_fbos_apply(cnvs_ptr);
    const char * gl_vendor = (const char *)(glGetString(GL_VENDOR));
    const char * gl_renderer = (const char *)(glGetString(GL_RENDERER));
    const char * gl_version = (const char *)(glGetString(GL_VERSION));
    LV_LOG_INFO("GL_VENDOR:   %s", gl_vendor);
    LV_LOG_INFO("GL_RENDERER: %s", gl_renderer);
    LV_LOG_INFO("GL_VERSION:  %s", gl_version);
}
void lv_egl_adapter_interface_read_pixel(int x, int y, uint8_t * r, uint8_t * g, uint8_t * b, uint8_t * a)
{
    uint8_t pixel[4];
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
    *r = pixel[0];
    *g = pixel[1];
    *b = pixel[2];
    *a = pixel[3];
}
float lv_egl_adapter_interface_read_depth(int x, int y)
{
    uint8_t pixel[4];
    glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
    return (float)(pixel[0] + (pixel[1] << 8));
}
lv_egl_adapter_interface_t * interface_create_internal(lv_egl_adapter_t * egl_adapter, int width, int height,
                                                       float refresh_rate)
{
    lv_egl_adapter_interface_t * interface;
    interface = (lv_egl_adapter_interface_t *)malloc(sizeof(*interface));

    /* NOTE: do not try the GBM option yet, the GBM class has not been converted */
    interface->output_module =
#if LV_ADAPTED_ON_GBM
            NativeStateGBM _egl_output
#elif LV_ADAPTED_ON_WAYLAND
            lv_egl_adapter_outmod_wayland_create
#else // LV_ADAPTED_ON_DRM
            lv_egl_adapter_outmod_drm_create
#endif
            ();
    interface->width = width;
    interface->height = height;
    interface->refresh_rate = refresh_rate;
    interface->offscreen_fbo_count = 0;
    interface->egl_adapter = egl_adapter;
    interface->output_internal_data = 0;
    interface->format_color = 0;
    interface->format_depth = 0;
    interface->offscreen_fbo_index = 0;
    interface->is_window_initialized = false;
    interface->fbos = &interface->fbos_data;
    interface->fbos_syncs = &interface->fbos_syncs_data;
    interface->owns_adapter = false;

    lv_array_init(interface->fbos, 1, sizeof(lv_egl_adapter_fbo_t *));
    lv_array_init(interface->fbos_syncs, 1, sizeof(lv_egl_adapter_sync_t *));

    return interface;
}
lv_egl_adapter_interface_t * lv_egl_adapter_interface_create(lv_egl_adapter_t * egl_adapter, int width, int height,
                                                             float refresh_rate)
{
    lv_egl_adapter_interface_t * interface = interface_create_internal(egl_adapter, width, height, refresh_rate);
    interface->owns_adapter = false;
    return interface;
}
lv_egl_adapter_interface_t * lv_egl_adapter_interface_auto(void)
{
    lv_egl_adapter_config_t * _egl_options = lv_egl_adapter_config_by_id(LV_EGL_BUFFER_MODE);
    lv_egl_adapter_config_set_vsync(_egl_options, LV_EGL_SYNC);
    lv_egl_adapter_t * _egl_adapter = lv_egl_adapter_create(_egl_options);
    lv_egl_adapter_interface_t * interface = interface_create_internal(_egl_adapter, LV_EGL_HOR_RES, LV_EGL_VER_RES,
                                                                           LV_EGL_REFR);
    interface->owns_adapter = true;
    if(!lv_egl_adapter_interface_init(interface)) {
        lv_egl_adapter_interface_destroy((void **)&interface);
        return 0;
    }
    return interface;
}
void interface_destroy_internal(void ** cnvs_ptr)
{
    lv_egl_adapter_interface_t * interface = *((lv_egl_adapter_interface_t **)(cnvs_ptr));
    if(interface) {
        interface_fbos_reset(interface->fbos);
        lv_array_deinit(interface->fbos);
        lv_array_deinit(interface->fbos_syncs);
        CORE(interface)->destroy((void **)&interface->output_module);
        free(interface);
    }
    *cnvs_ptr = 0;
}
void lv_egl_adapter_interface_destroy(void ** cnvs_ptr)
{
    lv_egl_adapter_interface_t * interface = *((lv_egl_adapter_interface_t **)(cnvs_ptr));
    if(interface) {
        if(interface->owns_adapter) {
            lv_egl_adapter_t * this_adapter = interface->egl_adapter;
            interface_destroy_internal(cnvs_ptr);
            lv_egl_adapter_cleanup((void **)&this_adapter, 0, 0);
        }
        else {
            interface_destroy_internal(cnvs_ptr);
        }
    }
}
void lv_egl_adapter_interface_update(void * cnvs_ptr)
{
    lv_egl_adapter_interface_t * interface = CAST(cnvs_ptr);
    if(interface->offscreen_fbo_count <= 0) {
        lv_egl_adapter_swap(interface->egl_adapter);
        CORE(interface)->flip(interface->output_module, lv_egl_adapter_prefers_sync(interface->egl_adapter));
        return;
    }
    if(interface->is_sync_supported) {
        lv_egl_adapter_destroy_sync((lv_egl_adapter_sync_t *)lv_array_at(interface->fbos_syncs,
                                                                         interface->offscreen_fbo_index));
        lv_array_assign(interface->fbos_syncs, interface->offscreen_fbo_index,
                        lv_egl_adapter_create_sync(interface->egl_adapter));
    }
    else {
        glFinish();
    }
    //interface->offscreen_fbo_index = (interface->offscreen_fbo_index + 1) % lv_array_size(interface->fbos);
    interface->offscreen_fbo_index = (interface->offscreen_fbo_index + 1) % interface->offscreen_fbo_count;
    lv_egl_adapter_sync_t * current_sync = (lv_egl_adapter_sync_t *)lv_array_at(interface->fbos_syncs,
                                                                                interface->offscreen_fbo_index);
    if(current_sync) {
        lv_egl_adapter_sync_wait(current_sync);
        lv_egl_adapter_destroy_sync(current_sync);
        current_sync = NULL;
        lv_array_assign(interface->fbos_syncs, interface->offscreen_fbo_index,
                        lv_egl_adapter_create_sync(interface->egl_adapter));
    }
    glBindFramebuffer(GL_FRAMEBUFFER, ((lv_egl_adapter_fbo_t *)lv_array_at(interface->fbos,
                                                                           interface->offscreen_fbo_index))->fbo);
}
int lv_egl_adapter_interface_width(void * cnvs_ptr)
{
    return CAST(cnvs_ptr)->width;
}
int lv_egl_adapter_interface_height(void * cnvs_ptr)
{
    return CAST(cnvs_ptr)->height;
}
void lv_egl_adapter_interface_switch_to_primary_fbo(void * cnvs_ptr)
{
    lv_egl_adapter_interface_t * interface = CAST(cnvs_ptr);
    (void)interface;
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    //glDepthMask(GL_TRUE);
    glDepthMask(GL_FALSE);
    //glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glDisable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glDisable(GL_CULL_FACE);
}
void lv_egl_adapter_interface_offscreen(void * cnvs_ptr, unsigned int offscreen)
{
    CAST(cnvs_ptr)->offscreen_fbo_count = offscreen;
}
lv_egl_adapter_fbo_t * lv_egl_adapter_fbo_create(GLsizei width, GLsizei height, GLuint color_format,
                                                 GLuint depth_format)
{
    lv_egl_adapter_fbo_t * interface = (lv_egl_adapter_fbo_t *)malloc(sizeof(struct lv_egl_adapter_fbo));
    interface->color_renderbuffer = 0;
    interface->depth_renderbuffer = 0;
    interface->fbo = 0;

    glGenRenderbuffers(1, &(interface->color_renderbuffer));
    glBindRenderbuffer(GL_RENDERBUFFER, interface->color_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, color_format, width, height);

    /* Create a renderbuffer for the depth attachment */
    glGenRenderbuffers(1, &(interface->depth_renderbuffer));
    glBindRenderbuffer(GL_RENDERBUFFER, interface->depth_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, depth_format, width, height);

    /* Create a FBO and set it up */
    glGenFramebuffers(1, &(interface->fbo));
    glBindFramebuffer(GL_FRAMEBUFFER, interface->fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, interface->color_renderbuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, interface->depth_renderbuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the framebuffer
    return interface;
}
void lv_egl_adapter_fbo_destroy(void ** fbostruct_ptr)
{
    lv_egl_adapter_fbo_t * fbostruct = *(lv_egl_adapter_fbo_t **)fbostruct_ptr;
    if(fbostruct) {
        if(fbostruct->fbo) glDeleteFramebuffers(1, &(fbostruct->fbo));
        if(fbostruct->color_renderbuffer) glDeleteRenderbuffers(1, &(fbostruct->color_renderbuffer));
        if(fbostruct->depth_renderbuffer) glDeleteRenderbuffers(1, &(fbostruct->depth_renderbuffer));
        free(fbostruct);
    }
    *fbostruct_ptr = 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static GLenum determine_color_format(const lv_egl_adapter_config_t * vc, bool supports_rgba8, bool supports_rgb8)
{
    if(vc->buffer == 32) {
        return supports_rgba8 ? GL_RGBA8 : GL_RGBA4;
    }
    else if(vc->buffer == 24) {
        return supports_rgb8 ? GL_RGB8 : GL_RGB565;
    }
    else if(vc->buffer == 16) {
        if(vc->red == 4 && vc->green == 4 && vc->blue == 4 && vc->alpha == 4) {
            return GL_RGBA4;
        }
        else if(vc->red == 5 && vc->green == 5 && vc->blue == 5 && vc->alpha == 1) {
            return GL_RGB5_A1;
        }
        else if(vc->red == 5 && vc->green == 6 && vc->blue == 5 && vc->alpha == 0) {
            return GL_RGB565;
        }
    }
    return 0;
}
static GLenum determine_depth_format(const lv_egl_adapter_config_t * vc, bool supports_depth32, bool supports_depth24)
{
    if(vc->depth == 32 && supports_depth32)
        return GL_DEPTH_COMPONENT32;
    else if(vc->depth >= 24 && supports_depth24)
        return GL_DEPTH_COMPONENT24;
    else if(vc->depth == 16)
        return GL_DEPTH_COMPONENT16;
    return 0;
}
static bool determine_gl_extension_support(const char * ext)
{
    const char * exts = (const char *)glGetString(GL_EXTENSIONS);
    if(!exts)  return false;

    size_t ext_size = strlen(ext);
    const char * pos = strstr(exts, ext);

    while(pos) {
        char next_char = pos[ext_size];
        if(next_char == ' ' || next_char == '\0') return true;
        pos = strstr(pos + 1, ext);
    }

    return false; // Extension not found
}
static const char * interface_gl_format_str(GLenum f)
{
    const char * str;
    switch(f) {
        case GL_RGBA8:
            str = "GL_RGBA8";
            break;
        case GL_RGB8:
            str = "GL_RGB8";
            break;
        case GL_RGBA4:
            str = "GL_RGBA4";
            break;
        case GL_RGB5_A1:
            str = "GL_RGB5_A1";
            break;
        case GL_RGB565:
            str = "GL_RGB565";
            break;
        case GL_DEPTH_COMPONENT16:
            str = "GL_DEPTH_COMPONENT16";
            break;
        case GL_DEPTH_COMPONENT24:
            str = "GL_DEPTH_COMPONENT24";
            break;
        case GL_DEPTH_COMPONENT32:
            str = "GL_DEPTH_COMPONENT32";
            break;
        case GL_NONE:
            str = "GL_NONE";
            break;
        default:
            str = "Unknown";
            break;
    }
    return str;
}
static void interface_fbos_reset(void * fboarray_ptr)
{
    lv_array_t * _array = (lv_array_t *)fboarray_ptr;
    if(_array) {
        if(!lv_array_is_empty(_array)) {
            for(uint32_t i = 0; i < lv_array_size(_array); i++) {
                lv_egl_adapter_fbo_t * _tfbo = (lv_egl_adapter_fbo_t *)lv_array_at(_array, i);
                lv_egl_adapter_fbo_destroy((void **)&_tfbo);
            }
            lv_array_clear(_array);
        }
    }
}
static bool interface_confirm_gl2_support(void)
{
    const char * ver = (const char *)glGetString(GL_VERSION);
    if(!ver) return 0;

    char buf[16];
    size_t bi = 0;
    int started = 0;

    for(; *ver && bi + 1 < sizeof(buf); ++ver) {
        if(!started) {
            if(isdigit((unsigned char) * ver)) {
                started = 1;
                buf[bi++] = *ver;
            }
        }
        else {
            if(*ver == '.' || *ver == ' ') break;
            if(isdigit((unsigned char) * ver)) buf[bi++] = *ver;
            else break;
        }
    }

    if(bi == 0) return 0;
    buf[bi] = '\0';
    long major = strtol(buf, NULL, 10);
    return major >= 2;
}
static bool interface_confirm_pixel_format(void * cnvs_ptr)
{
    lv_egl_adapter_interface_t * interface = CAST(cnvs_ptr);
    if(interface->format_color && interface->format_depth) return true;
    lv_egl_adapter_config_t * vc = lv_egl_adapter_config_create();
    lv_egl_adapter_config_make_default(vc);
    lv_egl_adapter_got_visual_config(interface->egl_adapter, vc);
    interface->format_color = 0;
    interface->format_depth = 0;
    bool supports_rgba8 = (false);
    bool supports_rgb8 = (false);
    bool supports_depth24 = (false);
    bool supports_depth32 = (false);
#if LV_EGL_ADAPTED_WITH_GLESV2
    if(determine_gl_extension_support("GL_ARM_rgba8")) supports_rgba8 = true;
    if(determine_gl_extension_support("GL_OES_depth24")) supports_depth24 = true;
    if(determine_gl_extension_support("GL_OES_depth32")) supports_depth32 = true;
    if(determine_gl_extension_support("GL_OES_rgb8_rgba8")) {
        supports_rgba8 = true;
        supports_rgb8 = true;
    }
#elif LV_EGL_ADAPTED_WITH_GL
    supports_rgba8 = true;
    supports_rgb8 = true;
    supports_depth24 = true;
    supports_depth32 = true;
#endif

    interface->format_color = determine_color_format(vc, supports_rgba8, supports_rgb8);
    interface->format_depth = determine_depth_format(vc, supports_depth32, supports_depth24);

    LV_LOG_INFO("Selected Renderbuffer ColorFormat: %s DepthFormat: %s",
                interface_gl_format_str(interface->format_color),
                interface_gl_format_str(interface->format_depth));

    free(vc);
    return (interface->format_color);// && interface->format_depth);
}
static bool interface_fbos_confirm(void * cnvs_ptr)
{
    lv_egl_adapter_interface_t * interface = CAST(cnvs_ptr);
    if(lv_array_size(interface->fbos) != interface->offscreen_fbo_count) {
        interface_fbos_reset(interface->fbos);
        if(!interface_confirm_pixel_format(interface)) return false;
        for(unsigned int i = 0; i < interface->offscreen_fbo_count; ++i) {
            LV_LOG_INFO("Creating FBO #%d", i);
            lv_array_push_back(interface->fbos, lv_egl_adapter_fbo_create(interface->width, interface->height,
                                                                          interface->format_color,
                                                                          interface->format_depth));
        }
        lv_array_resize(interface->fbos_syncs, interface->offscreen_fbo_count);
        interface->offscreen_fbo_index = 0;
    }
    return true;
}
static bool interface_fbos_apply(void * cnvs_ptr)
{
    lv_egl_adapter_interface_t * interface = CAST(cnvs_ptr);
    if(!interface->is_window_initialized) return
            FALSE_ERROR("glwindow has never been initialized, check native-state code\nit should not return a valid window until create_window() is called");
    lv_egl_adapter_init_surface(interface->egl_adapter, interface->output_internal_data);
    if(!lv_egl_adapter_is_valid(interface->egl_adapter))  return FALSE_ERROR("Canvas: Invalid EGL state\n");
    if(!lv_egl_adapter_init_extensions(interface->egl_adapter)) return false;
    if(interface->offscreen_fbo_count) {
        if(!glGenFramebuffers) return FALSE_ERROR("Off-screen rendering requires GL framebuffer support");
        if(!interface_fbos_confirm(interface)) return false;
        interface->is_sync_supported = lv_egl_adapter_supports_sync();
        if(!interface->is_sync_supported) {
            static bool warned = false;
            if(!warned) {
                LV_LOG_WARN("Sync objects not supported, falling back to glFinish");
                warned = true;
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, ((lv_egl_adapter_fbo_t *)lv_array_at(interface->fbos,
                                                                               interface->offscreen_fbo_index))->fbo);
    }
    return true;
}
static void interface_fbos_deinit(void * cnvs_ptr)
{
    lv_egl_adapter_interface_t * interface = CAST(cnvs_ptr);
    interface_fbos_reset(interface->fbos);
    lv_array_deinit(interface->fbos_syncs);
    interface->format_color = 0;
    interface->format_depth = 0;
    interface->offscreen_fbo_index = 0;
}
static bool interface_resize_output(void * cnvs_ptr, int width, int height)
{
    lv_egl_adapter_interface_t * interface = CAST(cnvs_ptr);
    lv_egl_adapter_output_core_t * core = CORE(interface);

    bool request_fullscreen = (width == -1 && height == -1);
    intptr_t vid;
    EGLint temp_vid = -1;
    lv_array_t new_mods_struct;
    lv_array_t * new_mods = &new_mods_struct;
    if(!lv_egl_adapter_got_native_config(interface->egl_adapter, &temp_vid,
                                         new_mods)) return FALSE_ERROR("Couldn't get GL visual config!");
    vid = temp_vid;

    struct native_window_properties properties;
    properties.width = width;
    properties.height = height;
    properties.fullscreen = request_fullscreen;
    properties.modifiers = *new_mods;
    properties.visual_id = vid;

    struct native_window_properties cur_properties;
    core->window(interface->output_module, &cur_properties);

    if(request_fullscreen) {
        width = -1;
        height = -1;
        cur_properties.fullscreen = false;
    }
    else {
        if((cur_properties.width == properties.width &&
            cur_properties.height == properties.height)) {
            lv_array_deinit(new_mods);
            return true;
        }
    }
    if(!core->create_window(interface->output_module, &properties))
        return FALSE_ERROR("Couldn't create native window!");
    interface->output_internal_data = core->window(interface->output_module, &cur_properties);
    interface->is_window_initialized = true;
    interface->width = cur_properties.width;
    interface->height = cur_properties.height;
    if(!lv_array_is_empty(interface->fbos)) {
        // Clear FBOs so that they are recreated (if needed) with the correct size
        interface_fbos_reset(interface->fbos);
        interface_fbos_confirm(interface);
    }
    lv_array_deinit(new_mods);
    return true;
}
static inline lv_egl_adapter_interface_t * CAST(void * _cnvs_ptr)
{
    return (lv_egl_adapter_interface_t *)_cnvs_ptr;
}
static inline lv_egl_adapter_output_core_t * CORE(void * cnvs_ptr)
{
    return lv_egl_adapter_get_output_core(CAST(cnvs_ptr)->egl_adapter);
}
static bool FALSE_ERROR(const char * desc)
{
    LV_LOG_ERROR(desc);
    return false;
}
/* Unused for now, this will take a mouse x/y, a scanned depth at that pixel
 * (normalized between near and far distances), and return the X/Y/Z co-ordinate
 * of that intersection.
static void unproject_pixel_world(float mouse_x, float mouse_y, float w, float h,
                                  const float invVP[16], float nd, float out[3])
{
    float nx = (2.f * mouse_x) / w - 1.f;
    float ny = 1.f - (2.f * mouse_y) / h;
    float cz = nd * 2.f - 1.f;
    float x = invVP[0] * nx + invVP[4] * ny + invVP[8] * cz + invVP[12];
    float y = invVP[1] * nx + invVP[5] * ny + invVP[9] * cz + invVP[13];
    float z = invVP[2] * nx + invVP[6] * ny + invVP[10] * cz + invVP[14];
    float wv = invVP[3] * nx + invVP[7] * ny + invVP[11] * cz + invVP[15];
    out[0] = x / wv;
    out[1] = y / wv;
    out[2] = z / wv;
}
*/