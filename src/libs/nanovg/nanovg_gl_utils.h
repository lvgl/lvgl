//
// Copyright (c) 2009-2013 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
#ifndef NANOVG_GL_UTILS_H
#define NANOVG_GL_UTILS_H

#include "../../lvgl_public.h"

#if LV_USE_NANOVG

#ifdef NANOVG_GL_IMPLEMENTATION
struct NVGLUframebuffer {
    NVGcontext * ctx;
    GLuint fbo;
    GLuint rbo;
    GLuint texture;
    int image;
};
#endif

typedef struct NVGLUframebuffer NVGLUframebuffer;

// Helper function to create GL frame buffer to render to.
// format: see NVGtexture
void nvgluBindFramebuffer(NVGLUframebuffer * fb);
NVGLUframebuffer * nvgluCreateFramebuffer(NVGcontext * ctx, int w, int h, int imageFlags, int format);
void nvgluDeleteFramebuffer(NVGLUframebuffer * fb);

// ---- Blur utilities ----

// Quality hint for blur operations
enum NVGLUblurQuality {
    NVGLU_BLUR_QUALITY_SPEED  = 0,  // Fewer taps, faster
    NVGLU_BLUR_QUALITY_NORMAL = 1,  // Full taps, better quality
};

// Parameters for nvgluBlurRegion
typedef struct NVGLUblurParams {
    int radius;                     // Blur radius in pixels (clamped to internal max)
    int quality;                    // NVGLUblurQuality
    NVGcolor recolor;               // If recolor.a > 0, apply as tint (for drop shadows)
} NVGLUblurParams;

// Opaque blur state handle (created/destroyed per draw unit lifetime)
typedef struct NVGLUblurState NVGLUblurState;

// Create/destroy blur state (shader program, VBO, scratch resources)
NVGLUblurState * nvgluCreateBlurState(void);
void nvgluDeleteBlurState(NVGLUblurState * state, NVGcontext * ctx);

// Blur a rectangular region of the given framebuffer in-place.
// If fb is NULL, operates on the default (screen) framebuffer via texture copy.
// The region (x, y, w, h) is in GL coordinates (origin at bottom-left).
// Returns 0 on success, -1 on failure.
int nvgluBlurRegion(NVGLUblurState * state, NVGcontext * ctx, NVGLUframebuffer * fb,
                    int x, int y, int w, int h,
                    const NVGLUblurParams * params);

#endif // NANOVG_GL_UTILS_H

#ifdef NANOVG_GL_IMPLEMENTATION

#if defined(NANOVG_GL3) || defined(NANOVG_GLES2) || defined(NANOVG_GLES3)
    // FBO is core in OpenGL 3>.
    #define NANOVG_FBO_VALID 1
#elif defined(NANOVG_GL2)
    // On OS X including glext defines FBO on GL2 too.
    #ifdef __APPLE__
        #include <OpenGL/glext.h>
        #define NANOVG_FBO_VALID 1
    #endif
#endif

static GLint defaultFBO = -1;

NVGLUframebuffer * nvgluCreateFramebuffer(NVGcontext * ctx, int w, int h, int imageFlags, int format)
{
#ifdef NANOVG_FBO_VALID
    GLint defFBO;
    GLint defaultRBO;
    NVGLUframebuffer * fb = NULL;

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defFBO);
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &defaultRBO);

    fb = (NVGLUframebuffer *)lv_malloc(sizeof(NVGLUframebuffer));
    if(fb == NULL) goto error;
    lv_memzero(fb, sizeof(NVGLUframebuffer));

    fb->image = nvgCreateImage(ctx, w, h, imageFlags | NVG_IMAGE_FLIPY | NVG_IMAGE_PREMULTIPLIED, format, NULL);

#if defined NANOVG_GL2
    fb->texture = nvglImageHandleGL2(ctx, fb->image);
#elif defined NANOVG_GL3
    fb->texture = nvglImageHandleGL3(ctx, fb->image);
#elif defined NANOVG_GLES2
    fb->texture = nvglImageHandleGLES2(ctx, fb->image);
#elif defined NANOVG_GLES3
    fb->texture = nvglImageHandleGLES3(ctx, fb->image);
#endif

    fb->ctx = ctx;

    // frame buffer object
    glGenFramebuffers(1, &fb->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);

    // render buffer object
    glGenRenderbuffers(1, &fb->rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, fb->rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, w, h);

    // combine all
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb->rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
#ifdef GL_DEPTH24_STENCIL8
        // If GL_STENCIL_INDEX8 is not supported, try GL_DEPTH24_STENCIL8 as a fallback.
        // Some graphics cards require a depth buffer along with a stencil.
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->texture, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, fb->rbo);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
#endif // GL_DEPTH24_STENCIL8
            goto error;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, defFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, defaultRBO);
    return fb;
error:
    glBindFramebuffer(GL_FRAMEBUFFER, defFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, defaultRBO);
    nvgluDeleteFramebuffer(fb);
    return NULL;
#else
    NVG_NOTUSED(ctx);
    NVG_NOTUSED(w);
    NVG_NOTUSED(h);
    NVG_NOTUSED(imageFlags);
    NVG_NOTUSED(format);
    return NULL;
#endif
}

void nvgluBindFramebuffer(NVGLUframebuffer * fb)
{
#ifdef NANOVG_FBO_VALID
    if(defaultFBO == -1) glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, fb != NULL ? fb->fbo : (GLuint)defaultFBO);
#else
    NVG_NOTUSED(fb);
#endif
}

void nvgluDeleteFramebuffer(NVGLUframebuffer * fb)
{
#ifdef NANOVG_FBO_VALID
    if(fb == NULL) return;
    if(fb->fbo != 0)
        glDeleteFramebuffers(1, &fb->fbo);
    if(fb->rbo != 0)
        glDeleteRenderbuffers(1, &fb->rbo);
    if(fb->image >= 0)
        nvgDeleteImage(fb->ctx, fb->image);
    fb->ctx = NULL;
    fb->fbo = 0;
    fb->rbo = 0;
    fb->texture = 0;
    fb->image = -1;
    lv_free(fb);
#else
    NVG_NOTUSED(fb);
#endif
}

// ---- Blur implementation ----

#include <math.h>

#define NVGLU_BLUR_MAX_TAPS   16
#define NVGLU_BLUR_MAX_RADIUS 256

// GL3/GLES3 need VAO
#if defined(NANOVG_GL3) || defined(NANOVG_GLES3)
    #define NVGLU_BLUR_NEEDS_VAO 1
#else
    #define NVGLU_BLUR_NEEDS_VAO 0
#endif

// GLSL version prologues
#if defined(NANOVG_GL3)
#define NVGLU_BLUR_VS_HEADER \
    "#version 130\n" \
    "#define attribute in\n" \
    "#define varying out\n"
#define NVGLU_BLUR_FS_HEADER \
    "#version 130\n" \
    "#define varying in\n" \
    "#define texture2D texture\n" \
    "out vec4 nvglu_frag_out;\n" \
    "#define gl_FragColor nvglu_frag_out\n"
#elif defined(NANOVG_GLES3)
#define NVGLU_BLUR_VS_HEADER \
    "#version 300 es\n" \
    "#define attribute in\n" \
    "#define varying out\n"
#define NVGLU_BLUR_FS_HEADER \
    "#version 300 es\n" \
    "precision mediump float;\n" \
    "#define varying in\n" \
    "#define texture2D texture\n" \
    "out vec4 nvglu_frag_out;\n" \
    "#define gl_FragColor nvglu_frag_out\n"
#elif defined(NANOVG_GLES2)
#define NVGLU_BLUR_VS_HEADER ""
#define NVGLU_BLUR_FS_HEADER "precision mediump float;\n"
#else // GL2
#define NVGLU_BLUR_VS_HEADER ""
#define NVGLU_BLUR_FS_HEADER ""
#endif

struct NVGLUblurState {
    GLuint program;
    GLuint vbo;
#if NVGLU_BLUR_NEEDS_VAO
    GLuint vao;
#endif
    GLint loc_tex;
    GLint loc_step;
    GLint loc_tap_count;
    GLint loc_weights;
    GLint loc_offsets;
    GLint loc_src_uv;
    GLint loc_recolor;
    GLint attr_pos;
    GLint attr_uv;
    int   program_ok;

    NVGLUframebuffer * scratch_fb;
    int scratch_w;
    int scratch_h;

    GLuint capture_tex;
    int capture_w;
    int capture_h;
};

static const char * nvglu__blur_vs_src =
    NVGLU_BLUR_VS_HEADER
    "attribute vec2 a_pos;\n"
    "attribute vec2 a_uv;\n"
    "varying vec2 v_uv;\n"
    "void main() {\n"
    "    v_uv = a_uv;\n"
    "    gl_Position = vec4(a_pos, 0.0, 1.0);\n"
    "}\n";

/* Fragment shader with per-tap offsets.
 * u_offsets[i] = sample offset in texels (multiplied by u_step on GPU)
 * u_weights[i] = weight for that sample
 * u_weights[0] / u_offsets[0] = center tap */
static const char * nvglu__blur_fs_src =
    NVGLU_BLUR_FS_HEADER
    "uniform sampler2D u_tex;\n"
    "uniform vec2 u_step;\n"
    "uniform vec4 u_src_uv;\n"
    "uniform vec4 u_recolor;\n"
    "uniform int u_tap_count;\n"
    "uniform float u_weights[17];\n"
    "uniform float u_offsets[17];\n"
    "varying vec2 v_uv;\n"
    "void main() {\n"
    "    vec2 base = u_src_uv.xy + v_uv * u_src_uv.zw;\n"
    "    vec4 sum = texture2D(u_tex, base) * u_weights[0];\n"
    "    for(int i = 1; i <= 16; i++) {\n"
    "        if(i > u_tap_count) break;\n"
    "        vec2 off = u_step * u_offsets[i];\n"
    "        sum += texture2D(u_tex, base + off) * u_weights[i];\n"
    "        sum += texture2D(u_tex, base - off) * u_weights[i];\n"
    "    }\n"
    "    if(u_recolor.a > 0.5) {\n"
    "        sum = vec4(u_recolor.rgb * sum.a, sum.a);\n"
    "    }\n"
    "    gl_FragColor = sum;\n"
    "}\n";

static const float nvglu__blur_quad[] = {
    -1.0f, -1.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 1.0f,
    1.0f,  1.0f, 1.0f, 1.0f,
};

static GLuint nvglu__compile_shader(GLenum type, const char * src)
{
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, NULL);
    glCompileShader(sh);
    GLint compiled = 0;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &compiled);
    if(!compiled) {
        glDeleteShader(sh);
        return 0;
    }
    return sh;
}

static int nvglu__ensure_program(NVGLUblurState * s)
{
    if(s->program_ok) return 1;
    if(s->program != 0) return 0;

    GLuint vs = nvglu__compile_shader(GL_VERTEX_SHADER, nvglu__blur_vs_src);
    GLuint fs = nvglu__compile_shader(GL_FRAGMENT_SHADER, nvglu__blur_fs_src);
    if(vs == 0 || fs == 0) {
        if(vs) glDeleteShader(vs);
        if(fs) glDeleteShader(fs);
        return 0;
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
        glDeleteProgram(prog);
        return 0;
    }

    s->program       = prog;
    s->loc_tex       = glGetUniformLocation(prog, "u_tex");
    s->loc_step      = glGetUniformLocation(prog, "u_step");
    s->loc_src_uv    = glGetUniformLocation(prog, "u_src_uv");
    s->loc_recolor   = glGetUniformLocation(prog, "u_recolor");
    s->loc_tap_count = glGetUniformLocation(prog, "u_tap_count");
    s->loc_weights   = glGetUniformLocation(prog, "u_weights[0]");
    if(s->loc_weights < 0) s->loc_weights = glGetUniformLocation(prog, "u_weights");
    s->loc_offsets   = glGetUniformLocation(prog, "u_offsets[0]");
    if(s->loc_offsets < 0) s->loc_offsets = glGetUniformLocation(prog, "u_offsets");
    s->attr_pos      = glGetAttribLocation(prog, "a_pos");
    s->attr_uv       = glGetAttribLocation(prog, "a_uv");

    glGenBuffers(1, &s->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, s->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(nvglu__blur_quad), nvglu__blur_quad, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

#if NVGLU_BLUR_NEEDS_VAO
    glGenVertexArrays(1, &s->vao);
#endif

    s->program_ok = 1;
    return 1;
}

/* Grow-only scratch FBO: only reallocate when a larger size is needed.
 * The shader uses u_src_uv to address the sub-region, so a larger FBO is fine. */
static void nvglu__ensure_scratch(NVGLUblurState * s, NVGcontext * ctx, int w, int h)
{
    if(s->scratch_fb && s->scratch_w >= w && s->scratch_h >= h) return;
    if(s->scratch_fb) {
        nvgluDeleteFramebuffer(s->scratch_fb);
        s->scratch_fb = NULL;
    }
    /* Allocate with some headroom to reduce future reallocations */
    int alloc_w = w > s->scratch_w ? w : s->scratch_w;
    int alloc_h = h > s->scratch_h ? h : s->scratch_h;
    s->scratch_fb = nvgluCreateFramebuffer(ctx, alloc_w, alloc_h, 0, NVG_TEXTURE_RGBA);
    s->scratch_w = s->scratch_fb ? alloc_w : 0;
    s->scratch_h = s->scratch_fb ? alloc_h : 0;
}

static void nvglu__ensure_capture(NVGLUblurState * s, int w, int h)
{
    if(s->capture_tex && s->capture_w == w && s->capture_h == h) return;
    if(!s->capture_tex) glGenTextures(1, &s->capture_tex);
    glBindTexture(GL_TEXTURE_2D, s->capture_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    s->capture_w = w;
    s->capture_h = h;
}

/* Compute discrete Gaussian kernel weights and offsets.
 * Output: weights[0] = center, weights[1..tap_count] = symmetric taps
 *         offsets[i] = sample position in texels (i * step_px)
 * Returns tap_count. */
static int nvglu__compute_kernel(int radius, int max_taps, int step_px,
                                 float * weights, float * offsets)
{
    float sigma = (float)radius / 3.0f;
    if(sigma < 1.0f) sigma = 1.0f;
    float inv2s2 = 1.0f / (2.0f * sigma * sigma);

    int tap_count = radius < max_taps ? radius : max_taps;
    if(tap_count < 1) tap_count = 1;

    float total = 0.0f;
    int i;
    for(i = 0; i <= tap_count; i++) {
        float x = (float)(i * step_px);
        float w = expf(-x * x * inv2s2);
        weights[i] = w;
        offsets[i] = x;
        total += (i == 0) ? w : (2.0f * w);
    }
    /* Normalize */
    if(total > 0.0f) {
        float inv = 1.0f / total;
        for(i = 0; i <= tap_count; i++) weights[i] *= inv;
    }
    /* Zero remaining */
    for(i = tap_count + 1; i <= NVGLU_BLUR_MAX_TAPS; i++) {
        weights[i] = 0.0f;
        offsets[i] = 0.0f;
    }
    return tap_count;
}

/* Bilinear-optimized kernel: merge adjacent discrete tap pairs into single
 * HW-filtered fetches. Halves texture bandwidth at slight quality cost.
 * Returns bilinear tap count (excluding center). */
static int nvglu__compute_kernel_bilinear(int radius, int step_px,
                                          float * weights, float * offsets)
{
    float sigma = (float)radius / 3.0f;
    if(sigma < 1.0f) sigma = 1.0f;
    float inv2s2 = 1.0f / (2.0f * sigma * sigma);

    /* Compute discrete weights for up to 16 taps */
    int n = radius < NVGLU_BLUR_MAX_TAPS ? radius : NVGLU_BLUR_MAX_TAPS;
    float dw[NVGLU_BLUR_MAX_TAPS + 1];
    float total = 0.0f;
    int i;
    for(i = 0; i <= n; i++) {
        float x = (float)(i * step_px);
        dw[i] = expf(-x * x * inv2s2);
        total += (i == 0) ? dw[i] : (2.0f * dw[i]);
    }
    if(total > 0.0f) {
        float inv = 1.0f / total;
        for(i = 0; i <= n; i++) dw[i] *= inv;
    }

    /* Center tap */
    weights[0] = dw[0];
    offsets[0] = 0.0f;

    /* Merge pairs: (1,2), (3,4), ... */
    int bi = 0;
    for(i = 1; i <= n; i += 2) {
        float w1 = dw[i];
        float w2 = (i + 1 <= n) ? dw[i + 1] : 0.0f;
        float wsum = w1 + w2;
        if(wsum < 1e-10f) break;
        bi++;
        weights[bi] = wsum;
        float p1 = (float)(i * step_px);
        float p2 = (float)((i + 1) * step_px);
        offsets[bi] = (p1 * w1 + p2 * w2) / wsum;
    }

    /* Zero remaining */
    for(i = bi + 1; i <= NVGLU_BLUR_MAX_TAPS; i++) {
        weights[i] = 0.0f;
        offsets[i] = 0.0f;
    }
    return bi;
}

NVGLUblurState * nvgluCreateBlurState(void)
{
#ifdef NANOVG_FBO_VALID
    NVGLUblurState * s = (NVGLUblurState *)lv_malloc_zeroed(sizeof(NVGLUblurState));
    return s;
#else
    return NULL;
#endif
}

void nvgluDeleteBlurState(NVGLUblurState * state, NVGcontext * ctx)
{
#ifdef NANOVG_FBO_VALID
    NVG_NOTUSED(ctx);
    if(!state) return;
    if(state->scratch_fb) nvgluDeleteFramebuffer(state->scratch_fb);
    if(state->capture_tex) glDeleteTextures(1, &state->capture_tex);
    if(state->program) glDeleteProgram(state->program);
    if(state->vbo) glDeleteBuffers(1, &state->vbo);
#if NVGLU_BLUR_NEEDS_VAO
    if(state->vao) glDeleteVertexArrays(1, &state->vao);
#endif
    lv_free(state);
#else
    NVG_NOTUSED(state);
    NVG_NOTUSED(ctx);
#endif
}

int nvgluBlurRegion(NVGLUblurState * state, NVGcontext * ctx, NVGLUframebuffer * fb,
                    int x, int y, int w, int h,
                    const NVGLUblurParams * params)
{
#ifdef NANOVG_FBO_VALID
    if(!state || !ctx || !params || w <= 0 || h <= 0) return -1;

    int radius = params->radius;
    if(radius <= 0) return 0;
    if(radius > NVGLU_BLUR_MAX_RADIUS) radius = NVGLU_BLUR_MAX_RADIUS;

    if(!nvglu__ensure_program(state)) return -1;
    nvglu__ensure_scratch(state, ctx, w, h);
    if(!state->scratch_fb) return -1;

    int is_root = (fb == NULL);
    if(is_root) {
        nvglu__ensure_capture(state, w, h);
        if(!state->capture_tex) return -1;
    }

    // Compute kernel — speed mode uses bilinear merging (half the fetches)
    float weights[NVGLU_BLUR_MAX_TAPS + 1];
    float offsets[NVGLU_BLUR_MAX_TAPS + 1];
    int tap_count;

    if(params->quality == NVGLU_BLUR_QUALITY_SPEED) {
        int step_px = (radius + 8 - 1) / 8;
        if(step_px < 1) step_px = 1;
        tap_count = nvglu__compute_kernel_bilinear(radius, step_px, weights, offsets);
    }
    else {
        int max_taps = NVGLU_BLUR_MAX_TAPS;
        int step_px = (radius + max_taps - 1) / max_taps;
        if(step_px < 1) step_px = 1;
        tap_count = nvglu__compute_kernel(radius, max_taps, step_px, weights, offsets);
    }

    // Save GL state that we modify
    GLint prev_program = 0, prev_fbo = 0, prev_vbo = 0, prev_tex = 0;
    GLint prev_active_tex = 0;
    GLint prev_viewport[4] = {0};
    GLboolean prev_blend, prev_scissor, prev_depth, prev_stencil, prev_cull;
    GLboolean prev_colormask[4];
#if NVGLU_BLUR_NEEDS_VAO
    GLint prev_vao = 0;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prev_vao);
#endif
    glGetIntegerv(GL_CURRENT_PROGRAM, &prev_program);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_fbo);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prev_vbo);
    glGetIntegerv(GL_ACTIVE_TEXTURE, &prev_active_tex);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prev_tex);
    glGetIntegerv(GL_VIEWPORT, prev_viewport);
    prev_blend = glIsEnabled(GL_BLEND);
    prev_scissor = glIsEnabled(GL_SCISSOR_TEST);
    prev_depth = glIsEnabled(GL_DEPTH_TEST);
    prev_stencil = glIsEnabled(GL_STENCIL_TEST);
    prev_cull = glIsEnabled(GL_CULL_FACE);
    glGetBooleanv(GL_COLOR_WRITEMASK, prev_colormask);

    // Setup GL state
#if NVGLU_BLUR_NEEDS_VAO
    glBindVertexArray(state->vao);
#endif
    glUseProgram(state->program);
    glBindBuffer(GL_ARRAY_BUFFER, state->vbo);
    glEnableVertexAttribArray((GLuint)state->attr_pos);
    glVertexAttribPointer((GLuint)state->attr_pos, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (const void *)0);
    glEnableVertexAttribArray((GLuint)state->attr_uv);
    glVertexAttribPointer((GLuint)state->attr_uv, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4,
                          (const void *)(sizeof(float) * 2));

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(state->loc_tex, 0);
    glUniform1i(state->loc_tap_count, tap_count);
    glUniform1fv(state->loc_weights, NVGLU_BLUR_MAX_TAPS + 1, weights);
    glUniform1fv(state->loc_offsets, NVGLU_BLUR_MAX_TAPS + 1, offsets);

    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_SCISSOR_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    // Determine source texture
    GLuint src_tex;
    int src_tex_w, src_tex_h;
    float uv_ox, uv_oy, uv_sx, uv_sy;

    if(is_root) {
        glBindTexture(GL_TEXTURE_2D, state->capture_tex);
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, x, y, w, h);
        src_tex = state->capture_tex;
        src_tex_w = state->capture_w;
        src_tex_h = state->capture_h;
        uv_ox = 0.0f;
        uv_oy = 0.0f;
        uv_sx = (float)w / (float)src_tex_w;
        uv_sy = (float)h / (float)src_tex_h;
    }
    else {
        src_tex = fb->texture;
        src_tex_w = w; // caller provides region size
        src_tex_h = h;
        // fb may be larger; compute UV from full FBO size via image query
        int iw = 0, ih = 0;
        nvgImageSize(ctx, fb->image, &iw, &ih);
        if(iw <= 0) iw = w;
        if(ih <= 0) ih = h;
        src_tex_w = iw;
        src_tex_h = ih;
        uv_ox = (float)x / (float)src_tex_w;
        uv_oy = (float)y / (float)src_tex_h;
        uv_sx = (float)w / (float)src_tex_w;
        uv_sy = (float)h / (float)src_tex_h;
    }

    GLuint scratch_tex = state->scratch_fb->texture;

    // Pass 1: src -> scratch (horizontal)
    nvgluBindFramebuffer(state->scratch_fb);
    glViewport(0, 0, w, h);
    glBindTexture(GL_TEXTURE_2D, src_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glUniform4f(state->loc_src_uv, uv_ox, uv_oy, uv_sx, uv_sy);
    glUniform4f(state->loc_recolor, 0.0f, 0.0f, 0.0f, 0.0f);
    glUniform2f(state->loc_step, 1.0f / (float)src_tex_w, 0.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Pass 2: scratch -> dst (vertical)
    if(is_root) {
        nvgluBindFramebuffer(NULL);
    }
    else {
        nvgluBindFramebuffer(fb);
    }
    glViewport(x, y, w, h);
    glBindTexture(GL_TEXTURE_2D, scratch_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glUniform4f(state->loc_src_uv, 0.0f, 0.0f,
                (float)w / (float)state->scratch_w,
                (float)h / (float)state->scratch_h);
    if(params->recolor.ch.a > 0.0f) {
        glUniform4f(state->loc_recolor, params->recolor.ch.r, params->recolor.ch.g, params->recolor.ch.b, 1.0f);
    }
    else {
        glUniform4f(state->loc_recolor, 0.0f, 0.0f, 0.0f, 0.0f);
    }
    glUniform2f(state->loc_step, 0.0f, 1.0f / (float)state->scratch_h);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Restore GL state
    glDisableVertexAttribArray((GLuint)state->attr_pos);
    glDisableVertexAttribArray((GLuint)state->attr_uv);
#if NVGLU_BLUR_NEEDS_VAO
    glBindVertexArray((GLuint)prev_vao);
#endif
    glUseProgram((GLuint)prev_program);
    glBindBuffer(GL_ARRAY_BUFFER, (GLuint)prev_vbo);
    glActiveTexture((GLenum)prev_active_tex);
    glBindTexture(GL_TEXTURE_2D, (GLuint)prev_tex);
    glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)prev_fbo);
    glViewport(prev_viewport[0], prev_viewport[1], prev_viewport[2], prev_viewport[3]);
    if(prev_blend) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
    if(prev_scissor) glEnable(GL_SCISSOR_TEST);
    else glDisable(GL_SCISSOR_TEST);
    if(prev_depth) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
    if(prev_stencil) glEnable(GL_STENCIL_TEST);
    else glDisable(GL_STENCIL_TEST);
    if(prev_cull) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);
    glColorMask(prev_colormask[0], prev_colormask[1], prev_colormask[2], prev_colormask[3]);

    return 0;
#else
    NVG_NOTUSED(state);
    NVG_NOTUSED(ctx);
    NVG_NOTUSED(fb);
    NVG_NOTUSED(x);
    NVG_NOTUSED(y);
    NVG_NOTUSED(w);
    NVG_NOTUSED(h);
    NVG_NOTUSED(params);
    return -1;
#endif
}

#endif // LV_USE_NANOVG

#endif // NANOVG_GL_IMPLEMENTATION
