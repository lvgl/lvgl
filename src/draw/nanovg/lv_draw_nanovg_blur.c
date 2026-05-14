/**
 * @file lv_draw_nanovg_blur.c
 *
 * Separable gaussian blur for the NanoVG draw unit, implemented with a
 * fragment shader. Two passes: horizontal then vertical, ping-ponging
 * between the layer's FBO and a scratch FBO.
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nanovg_private.h"

#if LV_USE_DRAW_NANOVG

#include "lv_nanovg_utils.h"
#include "lv_nanovg_fbo_cache.h"

#if LV_USE_OPENGLES && LV_USE_EGL
    #include "../../drivers/opengles/lv_opengles_private.h"
#else
    #define NANOVG_GL_STATIC_LINK
#endif

#if defined(NANOVG_GL2_IMPLEMENTATION) || defined(NANOVG_GL3_IMPLEMENTATION)
    #ifdef NANOVG_GL_STATIC_LINK
        #include <GL/glew.h>
    #endif
#elif defined(NANOVG_GLES2_IMPLEMENTATION)
    #ifdef NANOVG_GL_STATIC_LINK
        #include <GLES2/gl2.h>
    #endif
#elif defined(NANOVG_GLES3_IMPLEMENTATION)
    #ifdef NANOVG_GL_STATIC_LINK
        #include <GLES3/gl3.h>
    #endif
#endif

#include "../../libs/nanovg/nanovg_gl_utils.h"

#include <math.h>

/*********************
 *      DEFINES
 *********************/

#define BLUR_MAX_TAPS      16
#define BLUR_MAX_RADIUS    256

/* GL3 / GLES3 core profile require a Vertex Array Object to be bound
 * before any vertex-attribute setup. GL2 and GLES2 don't have VAOs. */
#if LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GL3 || LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GLES3
    #define BLUR_NEEDS_VAO 1
#else
    #define BLUR_NEEDS_VAO 0
#endif

/* Per-backend GLSL prologue. The shader body uses legacy GLES2/GL2 syntax
 * (attribute / varying / texture2D / gl_FragColor) and is remapped to GL3
 * / GLES3 modern equivalents via #define when needed. */
#if LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GL3
#define BLUR_VS_HEADER \
    "#version 130\n" \
    "#define attribute in\n" \
    "#define varying out\n"
#define BLUR_FS_HEADER \
    "#version 130\n" \
    "#define varying in\n" \
    "#define texture2D texture\n" \
    "out vec4 blur_frag_out;\n" \
    "#define gl_FragColor blur_frag_out\n"
#elif LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GLES3
#define BLUR_VS_HEADER \
    "#version 300 es\n" \
    "#define attribute in\n" \
    "#define varying out\n"
#define BLUR_FS_HEADER \
    "#version 300 es\n" \
    "precision mediump float;\n" \
    "#define varying in\n" \
    "#define texture2D texture\n" \
    "out vec4 blur_frag_out;\n" \
    "#define gl_FragColor blur_frag_out\n"
#elif LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GLES2
#define BLUR_VS_HEADER ""
#define BLUR_FS_HEADER "precision mediump float;\n"
#else /* GL2 */
#define BLUR_VS_HEADER ""
#define BLUR_FS_HEADER ""
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    GLuint program;
    GLuint vbo;
#if BLUR_NEEDS_VAO
    GLuint vao;
#endif
    GLint  loc_tex;
    GLint  loc_step;
    GLint  loc_tap_count;
    GLint  loc_weights;
    GLint  loc_src_uv;
    GLint  loc_recolor;
    GLint  attr_pos;
    GLint  attr_uv;
    bool   program_ok;

    /* Scratch FBO sized to the current blur area; ping-pong target. */
    NVGLUframebuffer * scratch_fb;
    int                scratch_w;
    int                scratch_h;

    /* Capture texture used when blurring the screen layer (no FBO).
     * Holds a copy of the screen region we're about to blur. */
    GLuint             capture_tex;
    int                capture_w;
    int                capture_h;
} blur_state_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool ensure_program(blur_state_t * s);
static void ensure_scratch_fb(blur_state_t * s, NVGcontext * vg, int w, int h);
static void ensure_capture_tex(blur_state_t * s, int w, int h);
static GLuint compile_shader(GLenum type, const char * src);
static void compute_weights(int radius, int tap_count, int step_px, float * weights);
static void set_tex_params(void);

/**********************
 *  STATIC VARIABLES
 **********************/

/* Shader body uses legacy GLES2/GL2 syntax (attribute/varying/texture2D/
 * gl_FragColor). The backend-specific BLUR_*_HEADER macro prepends a
 * #version directive and #defines that remap those keywords for GL3 and
 * GLES3 core profiles. */
static const char * VS_SRC =
    BLUR_VS_HEADER
    "attribute vec2 a_pos;\n"
    "attribute vec2 a_uv;\n"
    "varying vec2 v_uv;\n"
    "void main() {\n"
    "    v_uv = a_uv;\n"
    "    gl_Position = vec4(a_pos, 0.0, 1.0);\n"
    "}\n";

static const char * FS_SRC =
    BLUR_FS_HEADER
    "uniform sampler2D u_tex;\n"
    "uniform vec2 u_step;\n"
    "uniform vec4 u_src_uv;\n"   /* xy = sub-region UV offset, zw = sub-region UV scale */
    "uniform vec4 u_recolor;\n"  /* rgb = recolor, a > 0.5 enables alpha-masked recolor */
    "uniform int u_tap_count;\n"
    "uniform float u_weights[17];\n"
    "varying vec2 v_uv;\n"
    "void main() {\n"
    "    vec2 base = u_src_uv.xy + v_uv * u_src_uv.zw;\n"
    "    vec4 sum = texture2D(u_tex, base) * u_weights[0];\n"
    "    for(int i = 1; i <= 16; i++) {\n"
    "        if(i > u_tap_count) break;\n"
    "        vec2 off = u_step * float(i);\n"
    "        sum += texture2D(u_tex, base + off) * u_weights[i];\n"
    "        sum += texture2D(u_tex, base - off) * u_weights[i];\n"
    "    }\n"
    /* When recolor is active, treat sum's alpha as the shape mask and
     * replace its RGB with the recolor tinted by that alpha (premultiplied
     * output). Used for drop shadows so the subsequent layer composite
     * doesn't need to apply recolor — important because nanovg's image
     * draw path doesn't recolor pre-existing FBO image handles. */
    "    if(u_recolor.a > 0.5) {\n"
    "        sum = vec4(u_recolor.rgb * sum.a, sum.a);\n"
    "    }\n"
    "    gl_FragColor = sum;\n"
    "}\n";

/* Fullscreen triangle strip: x, y, u, v */
static const float QUAD_VERTS[] = {
    -1.0f, -1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 1.0f,
    1.0f,  1.0f, 1.0f, 1.0f,
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_nanovg_blur_init(lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);
    blur_state_t * s = lv_malloc_zeroed(sizeof(blur_state_t));
    LV_ASSERT_MALLOC(s);
    u->blur_state = s;
}

void lv_draw_nanovg_blur_deinit(lv_draw_nanovg_unit_t * u)
{
    LV_ASSERT_NULL(u);
    blur_state_t * s = u->blur_state;
    if(s == NULL) return;

    if(s->scratch_fb) {
        nvgluDeleteFramebuffer(s->scratch_fb);
        s->scratch_fb = NULL;
    }
    if(s->capture_tex) {
        glDeleteTextures(1, &s->capture_tex);
        s->capture_tex = 0;
    }
    if(s->program) {
        glDeleteProgram(s->program);
        s->program = 0;
    }
    if(s->vbo) {
        glDeleteBuffers(1, &s->vbo);
        s->vbo = 0;
    }
#if BLUR_NEEDS_VAO
    if(s->vao) {
        glDeleteVertexArrays(1, &s->vao);
        s->vao = 0;
    }
#endif

    lv_free(s);
    u->blur_state = NULL;
}

void lv_draw_nanovg_blur(lv_draw_task_t * t, const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords)
{
    int radius = dsc->blur_radius;
    if(radius <= 0) return;
    if(radius > BLUR_MAX_RADIUS) radius = BLUR_MAX_RADIUS;

    LV_PROFILER_DRAW_BEGIN;

    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)t->draw_unit;
    lv_layer_t * layer = t->target_layer;
    blur_state_t * s = u->blur_state;

    if(s == NULL) {
        LV_LOG_ERROR("nanovg blur: state not initialized");
        LV_PROFILER_DRAW_END;
        return;
    }

    /* Determine the blur area: requested coords clipped to clip_area and
     * the layer's own extent, expressed in layer-relative coordinates with
     * the layer's top-left at (0, 0). LVGL coordinates are top-down. */
    lv_area_t clip;
    if(!lv_area_intersect(&clip, coords, &t->clip_area)) {
        LV_PROFILER_DRAW_END;
        return;
    }
    if(!lv_area_intersect(&clip, &clip, &layer->buf_area)) {
        LV_PROFILER_DRAW_END;
        return;
    }
    const int blur_w = lv_area_get_width(&clip);
    const int blur_h = lv_area_get_height(&clip);
    if(blur_w <= 0 || blur_h <= 0) {
        LV_PROFILER_DRAW_END;
        return;
    }

    const int layer_h    = lv_area_get_height(&layer->buf_area);
    const int rel_x      = clip.x1 - layer->buf_area.x1;
    const int rel_y_top  = clip.y1 - layer->buf_area.y1;
    /* GL viewport / glCopyTex* take Y measured from the bottom of the
     * framebuffer; LVGL gives Y from the top. */
    const int gl_y       = layer_h - rel_y_top - blur_h;

    const bool is_root = (layer->user_data == NULL);

    NVGLUframebuffer * src_fb = NULL;
    if(!is_root) {
        src_fb = lv_nanovg_fbo_cache_entry_to_fb(layer->user_data);
        if(src_fb == NULL) {
            LV_PROFILER_DRAW_END;
            return;
        }
    }

    /* --- Allocate resources BEFORE mutating any GL state, so an early
     * failure simply returns without leaving the driver in a partially
     * configured state for the next nanovg draws. */
    if(!ensure_program(s)) {
        LV_PROFILER_DRAW_END;
        return;
    }
    ensure_scratch_fb(s, u->vg, blur_w, blur_h);
    if(s->scratch_fb == NULL) {
        LV_PROFILER_DRAW_END;
        return;
    }
    if(is_root) {
        ensure_capture_tex(s, blur_w, blur_h);
        if(s->capture_tex == 0) {
            LV_PROFILER_DRAW_END;
            return;
        }
    }

    /* Flush nanovg's pending batched draws before we touch raw GL state.
     * The next dispatch will call nvgBeginFrame again. */
    lv_nanovg_end_frame(u);

    /* Map (radius, quality) → (tap_count, step_px). For BLUR_QUALITY_SPEED
     * we use half the taps; the per-tap step grows so the same radius is
     * still covered, at the cost of coarser sampling. */
    const int max_taps  = (dsc->quality == LV_BLUR_QUALITY_SPEED) ? 8 : BLUR_MAX_TAPS;
    int       tap_count = radius < max_taps ? radius : max_taps;
    if(tap_count < 1) tap_count = 1;
    int step_px = (radius + tap_count - 1) / tap_count; /* ceil(radius / taps) */
    if(step_px < 1) step_px = 1;

    float weights[BLUR_MAX_TAPS + 1];
    compute_weights(radius, tap_count, step_px, weights);

    /* --- Set up shared GL state for both passes --- */
#if BLUR_NEEDS_VAO
    glBindVertexArray(s->vao);
#endif
    glUseProgram(s->program);
    glBindBuffer(GL_ARRAY_BUFFER, s->vbo);
    glEnableVertexAttribArray((GLuint)s->attr_pos);
    glVertexAttribPointer((GLuint)s->attr_pos, 2, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 4, (const void *)0);
    glEnableVertexAttribArray((GLuint)s->attr_uv);
    glVertexAttribPointer((GLuint)s->attr_uv, 2, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 4, (const void *)(sizeof(float) * 2));

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(s->loc_tex, 0);
    glUniform1i(s->loc_tap_count, tap_count);
    glUniform1fv(s->loc_weights, BLUR_MAX_TAPS + 1, weights);

    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_SCISSOR_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    /* --- Pick the source texture and compute its sampling sub-region.
     * Source textures are grow-only, so the blur area may map to less
     * than the full UV [0,1] range; the shader scales via u_src_uv. */
    GLuint src_tex;
    int    src_tex_w, src_tex_h;
    float  src_uv_off_x, src_uv_off_y, src_uv_scl_x, src_uv_scl_y;
    if(is_root) {
        /* Snapshot the screen region into the capture texture's lower-left
         * corner (capture_w × capture_h may be larger than blur_w × blur_h). */
        glBindTexture(GL_TEXTURE_2D, s->capture_tex);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rel_x, gl_y, blur_w, blur_h);
        src_tex      = s->capture_tex;
        src_tex_w    = s->capture_w;
        src_tex_h    = s->capture_h;
        src_uv_off_x = 0.0f;
        src_uv_off_y = 0.0f;
        src_uv_scl_x = (float)blur_w / (float)src_tex_w;
        src_uv_scl_y = (float)blur_h / (float)src_tex_h;
    }
    else {
        /* Sub-layer FBO is exactly the layer's size; sample just the clip
         * sub-region by computing UV offset/scale against the full FBO. */
        src_tex      = (GLuint)lv_nanovg_fb_get_texture_id(src_fb);
        src_tex_w    = lv_area_get_width(&layer->buf_area);
        src_tex_h    = layer_h;
        src_uv_off_x = (float)rel_x / (float)src_tex_w;
        src_uv_off_y = (float)gl_y  / (float)src_tex_h;
        src_uv_scl_x = (float)blur_w / (float)src_tex_w;
        src_uv_scl_y = (float)blur_h / (float)src_tex_h;
    }
    const GLuint scratch_tex = (GLuint)lv_nanovg_fb_get_texture_id(s->scratch_fb);

    /* --- Pass 1: source sub-region → scratch lower-left, horizontal.
     * No recolor in pass 1; it's only applied at the final pass output. */
    LV_PROFILER_DRAW_BEGIN_TAG("blur_h");
    nvgluBindFramebuffer(s->scratch_fb);
    glViewport(0, 0, blur_w, blur_h);
    glBindTexture(GL_TEXTURE_2D, src_tex);
    set_tex_params();
    glUniform4f(s->loc_src_uv, src_uv_off_x, src_uv_off_y, src_uv_scl_x, src_uv_scl_y);
    glUniform4f(s->loc_recolor, 0.0f, 0.0f, 0.0f, 0.0f);
    /* Step is expressed in the source texture's UV space. */
    glUniform2f(s->loc_step, (float)step_px / (float)src_tex_w, 0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    LV_PROFILER_DRAW_END_TAG("blur_h");

    /* --- Pass 2: scratch lower-left → destination sub-region, vertical.
     * Scratch is grow-only too, so we sample its (blur_w × blur_h)
     * lower-left region via UV scale. If this blur task is part of a
     * drop-shadow flow (drop_shadow_opa > 0 in the base dsc), apply the
     * shadow recolor here so the FBO content ends up tinted before the
     * layer composite draws it. */
    LV_PROFILER_DRAW_BEGIN_TAG("blur_v");
    if(is_root) {
        nvgluBindFramebuffer(NULL);
    }
    else {
        nvgluBindFramebuffer(src_fb);
    }
    glViewport(rel_x, gl_y, blur_w, blur_h);
    glBindTexture(GL_TEXTURE_2D, scratch_tex);
    set_tex_params();
    glUniform4f(s->loc_src_uv,
                0.0f, 0.0f,
                (float)blur_w / (float)s->scratch_w,
                (float)blur_h / (float)s->scratch_h);
    if(dsc->base.drop_shadow_opa > 0) {
        glUniform4f(s->loc_recolor,
                    (float)dsc->base.drop_shadow_color.red   / 255.0f,
                    (float)dsc->base.drop_shadow_color.green / 255.0f,
                    (float)dsc->base.drop_shadow_color.blue  / 255.0f,
                    1.0f);
    }
    else {
        glUniform4f(s->loc_recolor, 0.0f, 0.0f, 0.0f, 0.0f);
    }
    glUniform2f(s->loc_step, 0.0f, (float)step_px / (float)s->scratch_h);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    LV_PROFILER_DRAW_END_TAG("blur_v");

    /* --- Restore minimal state. NanoVG re-binds its own state on next
     * frame begin. We leave the destination framebuffer bound, matching
     * what on_layer_changed would do for this layer. */
    glDisableVertexAttribArray((GLuint)s->attr_pos);
    glDisableVertexAttribArray((GLuint)s->attr_uv);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
#if BLUR_NEEDS_VAO
    glBindVertexArray(0);
#endif

    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool ensure_program(blur_state_t * s)
{
    if(s->program_ok) return true;
    if(s->program != 0) return false; /* previously failed */

    GLuint vs = compile_shader(GL_VERTEX_SHADER, VS_SRC);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, FS_SRC);
    if(vs == 0 || fs == 0) {
        if(vs) glDeleteShader(vs);
        if(fs) glDeleteShader(fs);
        return false;
    }

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint linked = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &linked);
    if(!linked) {
        char log[512];
        GLsizei len = 0;
        glGetProgramInfoLog(prog, sizeof(log), &len, log);
        LV_LOG_ERROR("nanovg blur: program link failed: %.*s", (int)len, log);
        glDeleteProgram(prog);
        return false;
    }

    s->program       = prog;
    s->loc_tex       = glGetUniformLocation(prog, "u_tex");
    s->loc_step      = glGetUniformLocation(prog, "u_step");
    s->loc_src_uv    = glGetUniformLocation(prog, "u_src_uv");
    s->loc_recolor   = glGetUniformLocation(prog, "u_recolor");
    s->loc_tap_count = glGetUniformLocation(prog, "u_tap_count");
    s->loc_weights   = glGetUniformLocation(prog, "u_weights[0]");
    if(s->loc_weights < 0) s->loc_weights = glGetUniformLocation(prog, "u_weights");
    s->attr_pos      = glGetAttribLocation(prog, "a_pos");
    s->attr_uv       = glGetAttribLocation(prog, "a_uv");

    glGenBuffers(1, &s->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, s->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD_VERTS), QUAD_VERTS, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

#if BLUR_NEEDS_VAO
    glGenVertexArrays(1, &s->vao);
#endif

    s->program_ok = true;
    return true;
}

static void ensure_scratch_fb(blur_state_t * s, NVGcontext * vg, int w, int h)
{
    /* Must be an exact match: the shader samples scratch with UV [0,1], so
     * any larger FBO would address unwritten pixels in the second pass. */
    if(s->scratch_fb != NULL && s->scratch_w == w && s->scratch_h == h) return;

    if(s->scratch_fb) {
        nvgluDeleteFramebuffer(s->scratch_fb);
        s->scratch_fb = NULL;
        s->scratch_w = 0;
        s->scratch_h = 0;
    }
    s->scratch_fb = nvgluCreateFramebuffer(vg, w, h, 0, NVG_TEXTURE_RGBA);
    if(s->scratch_fb == NULL) {
        LV_LOG_ERROR("nanovg blur: failed to allocate %dx%d scratch FBO", w, h);
        return;
    }
    s->scratch_w = w;
    s->scratch_h = h;
}

static void ensure_capture_tex(blur_state_t * s, int w, int h)
{
    /* Capture must be an exact match for the same reason as scratch. */
    if(s->capture_tex != 0 && s->capture_w == w && s->capture_h == h) return;

    if(s->capture_tex == 0) {
        glGenTextures(1, &s->capture_tex);
    }
    glBindTexture(GL_TEXTURE_2D, s->capture_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    s->capture_w = w;
    s->capture_h = h;
}

static GLuint compile_shader(GLenum type, const char * src)
{
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, NULL);
    glCompileShader(sh);

    GLint compiled = 0;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &compiled);
    if(!compiled) {
        char log[512];
        GLsizei len = 0;
        glGetShaderInfoLog(sh, sizeof(log), &len, log);
        LV_LOG_ERROR("nanovg blur: %s shader compile failed: %.*s",
                     type == GL_VERTEX_SHADER ? "vertex" : "fragment",
                     (int)len, log);
        glDeleteShader(sh);
        return 0;
    }
    return sh;
}

static void compute_weights(int radius, int tap_count, int step_px, float * weights)
{
    /* Gaussian with sigma = radius / 3 — the usual "3-sigma covers radius"
     * convention. Sample positions: 0, step_px, 2*step_px, ... */
    float sigma = (float)radius / 3.0f;
    if(sigma < 1.0f) sigma = 1.0f;
    const float inv_two_sigma_sq = 1.0f / (2.0f * sigma * sigma);

    float total = 0.0f;
    for(int i = 0; i <= BLUR_MAX_TAPS; i++) {
        if(i > tap_count) {
            weights[i] = 0.0f;
            continue;
        }
        float x = (float)(i * step_px);
        float w = expf(-x * x * inv_two_sigma_sq);
        weights[i] = w;
        total += (i == 0) ? w : (2.0f * w);
    }
    if(total > 0.0f) {
        float inv = 1.0f / total;
        for(int i = 0; i <= BLUR_MAX_TAPS; i++) weights[i] *= inv;
    }
}

static void set_tex_params(void)
{
    /* Wrap must be CLAMP_TO_EDGE so taps near the edge don't read from the
     * opposite side. Filter stays at the FBO default (LINEAR); sample
     * positions land on integer texel offsets either way. */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

#endif /* LV_USE_DRAW_NANOVG */
