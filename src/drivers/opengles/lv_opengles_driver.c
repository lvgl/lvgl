/**
 * @file lv_opengles_driver.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_opengles_driver.h"
#if LV_USE_OPENGLES

#include "../../misc/lv_types.h"
#include "../../misc/lv_profiler.h"
#include "../../misc/lv_matrix.h"
#include "lv_opengles_debug.h"
#include "lv_opengles_private.h"

#include "../../display/lv_display_private.h"
#include "../../misc/lv_area_private.h"
#include "opengl_shader/lv_opengl_shader_internal.h"
#include "assets/lv_opengles_shader.h"

#include "../../draw/nanovg/lv_draw_nanovg.h"
#include "../../draw/opengles/lv_draw_opengles.h"

/*********************
 *      DEFINES
 *********************/

#define LV_OPENGLES_VERTEX_BUFFER_LEN 16

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_opengles_enable_blending(bool blend_opt);
static void lv_opengles_disable_blending(void);
static void lv_opengles_vertex_buffer_init(lv_opengles_gl_t * ctx, const void * data, unsigned int size);
static void lv_opengles_vertex_buffer_deinit(lv_opengles_gl_t * ctx);
static void lv_opengles_vertex_buffer_bind(lv_opengles_gl_t * ctx);
static void lv_opengles_vertex_buffer_unbind(void);
static void lv_opengles_vertex_array_init(lv_opengles_gl_t * ctx);
static void lv_opengles_vertex_array_deinit(lv_opengles_gl_t * ctx);
static void lv_opengles_vertex_array_bind(lv_opengles_gl_t * ctx);
static void lv_opengles_vertex_array_unbind(void);
static void lv_opengles_vertex_array_add_buffer(lv_opengles_gl_t * ctx);
static void lv_opengles_index_buffer_init(lv_opengles_gl_t * ctx, const unsigned int * data,
                                          unsigned int count);
static void lv_opengles_index_buffer_deinit(lv_opengles_gl_t * ctx);
static unsigned int lv_opengles_index_buffer_get_count(lv_opengles_gl_t * ctx);
static void lv_opengles_index_buffer_bind(lv_opengles_gl_t * ctx);
static void lv_opengles_index_buffer_unbind(void);
static unsigned int lv_opengles_shader_manager_init(void);
static lv_result_t lv_opengles_shader_init(lv_opengles_gl_t * ctx);
static void lv_opengles_shader_deinit(lv_opengles_gl_t * ctx);
static void lv_opengles_shader_bind(lv_opengles_gl_t * ctx);
static void lv_opengles_shader_unbind(void);
static int lv_opengles_shader_get_uniform_location(lv_opengles_gl_t * ctx, const char * name);
static void lv_opengles_shader_set_uniform1i(lv_opengles_gl_t * ctx, const char * name, int value);
static void lv_opengles_shader_set_uniformmatrix3fv(lv_opengles_gl_t * ctx, const char * name, int count,
                                                    const float * values);
static void lv_opengles_shader_set_uniform1f(lv_opengles_gl_t * ctx, const char * name, float value);
static void lv_opengles_shader_set_uniform3f(lv_opengles_gl_t * ctx, const char * name, float value_0,
                                             float value_1, float value_2);
static void lv_opengles_render_draw(lv_opengles_gl_t * ctx);
static float lv_opengles_map_float(float x, float min_in, float max_in, float min_out, float max_out);
static void populate_vertex_buffer(float vertex_buffer[LV_OPENGLES_VERTEX_BUFFER_LEN],
                                   lv_display_rotation_t rotation, bool * h_flip, bool * v_flip,
                                   float clip_x1, float clip_y1, float clip_x2, float clip_y2);

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static const char * shader_names[LV_OPENGLES_SHADER_NAMES_COUNT] = {
    "u_Texture",
    "u_ColorDepth",
    "u_VertexTransform",
    "u_Opa",
    "u_IsFill",
    "u_FillColor",
    "u_SwapRB",
    "u_Hue",
    "u_Saturation",
    "u_Value"
};

static bool global_init = false;
static lv_opengl_shader_manager_t shader_manager;
static lv_opengles_gl_t * bound_ctx; /* Used for backwards compatibility*/


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_opengles_init(void)
{
    bound_ctx = lv_opengles_context_create();
}

void lv_opengles_deinit(void)
{
    lv_opengles_context_delete(bound_ctx);
    bound_ctx = NULL;
}

void lv_opengles_context_bind(lv_opengles_gl_t * ctx)
{
    if(!ctx) {
        return;
    }
    bound_ctx = ctx;
}

lv_opengles_gl_t * lv_opengles_context_create(void)
{
    lv_opengles_gl_t * ctx = lv_zalloc(sizeof(*ctx));
    LV_ASSERT_MALLOC(ctx);
    if(!ctx) {
        return NULL;
    }

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    lv_opengles_vertex_buffer_init(ctx, NULL, sizeof(float) * LV_OPENGLES_VERTEX_BUFFER_LEN);

    lv_opengles_vertex_array_init(ctx);
    lv_opengles_vertex_array_add_buffer(ctx);

    lv_opengles_index_buffer_init(ctx, indices, 6);

    lv_result_t res = lv_opengles_shader_init(ctx);
    LV_ASSERT_MSG(res == LV_RESULT_OK, "Failed to initialize shaders");

    lv_opengles_shader_bind(ctx);

    /* unbind everything */
    lv_opengles_vertex_array_unbind();
    lv_opengles_vertex_buffer_unbind();
    lv_opengles_index_buffer_unbind();
    lv_opengles_shader_unbind();

    if(!global_init) {
        lv_opengles_enable_blending(false);
#if LV_USE_DRAW_NANOVG
        lv_draw_nanovg_init();
#elif LV_USE_DRAW_OPENGLES
        lv_draw_opengles_init();
#endif /*LV_USE_DRAW_NANOVG*/
        global_init = true;
    }

    return ctx;
}

void lv_opengles_context_delete(lv_opengles_gl_t * ctx)
{
    if(!ctx) {
        return;
    }
    lv_opengles_shader_deinit(ctx);
    lv_opengles_index_buffer_deinit(ctx);
    lv_opengles_vertex_buffer_deinit(ctx);
    lv_opengles_vertex_array_deinit(ctx);
    lv_free(ctx);
}

void lv_opengles_render_params_init(lv_opengles_render_params_t * params)
{
    LV_ASSERT_NULL(params);
    lv_memzero(params, sizeof(lv_opengles_render_params_t));
}

void lv_opengles_render_texture(unsigned int texture, const lv_area_t * texture_area,
                                lv_opa_t opa, int32_t disp_w,
                                int32_t disp_h, const lv_area_t * texture_clip_area, bool h_flip, bool v_flip)
{
    if(!bound_ctx) {
        LV_LOG_WARN("Can't render texture without a bound OpenGL context");
        return;
    }
    LV_PROFILER_DRAW_BEGIN;

    lv_opengles_render_params_t params;
    lv_opengles_render_params_init(&params);
    params.texture = texture;
    params.texture_area = texture_area;
    params.opa = opa;
    params.disp_w = disp_w;
    params.disp_h = disp_h;
    params.texture_clip_area = texture_clip_area;
    params.h_flip = h_flip;
    params.v_flip = v_flip;
    lv_opengles_render(&params);
    LV_PROFILER_DRAW_END;
}

void lv_opengles_render_texture_rbswap(unsigned int texture,
                                       const lv_area_t * texture_area, lv_opa_t opa,
                                       int32_t disp_w,
                                       int32_t disp_h, const lv_area_t * texture_clip_area, bool h_flip, bool v_flip)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_opengles_render_params_t params;
    lv_opengles_render_params_init(&params);
    params.texture = texture;
    params.texture_area = texture_area;
    params.opa = opa;
    params.disp_w = disp_w;
    params.disp_h = disp_h;
    params.texture_clip_area = texture_clip_area;
    params.h_flip = h_flip;
    params.v_flip = v_flip;
    params.rb_swap = true;
    lv_opengles_render(&params);
    LV_PROFILER_DRAW_END;
}

void lv_opengles_render_fill(lv_color_t color, const lv_area_t * area, lv_opa_t opa,
                             int32_t disp_w, int32_t disp_h)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_opengles_render_params_t params;
    lv_opengles_render_params_init(&params);
    params.texture_area = area;
    params.opa = opa;
    params.disp_w = disp_w;
    params.disp_h = disp_h;
    params.texture_clip_area = area;
    params.fill_color = color;
    params.rb_swap = true;
    lv_opengles_render(&params);
    LV_PROFILER_DRAW_END;
}

void lv_opengles_render_display(lv_display_t * display, const lv_opengles_render_params_t * params)
{
    LV_ASSERT_NULL(display);
    LV_ASSERT_NULL(params);
    LV_PROFILER_DRAW_BEGIN;
    unsigned int texture = (lv_uintptr_t)display->layer_head->user_data;
    GL_CALL(glActiveTexture(GL_TEXTURE0));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));

    lv_display_rotation_t rotation = lv_display_get_rotation(display);
    bool h_flip = params->h_flip;
    bool v_flip = params->v_flip;

    float vert_buffer[LV_OPENGLES_VERTEX_BUFFER_LEN];
    populate_vertex_buffer(vert_buffer, rotation, &h_flip, &v_flip, 0.f, 0.f, 1.f, 1.f);
    lv_opengles_vertex_buffer_init(bound_ctx, vert_buffer, sizeof(vert_buffer));

    float hor_scale = 1.0f;
    float ver_scale = 1.0f;
    float hor_translate = 0.0f;
    float ver_translate = 0.0f;
    hor_scale = h_flip ? -hor_scale : hor_scale;
    ver_scale = v_flip ? ver_scale : -ver_scale;

    const float transposed_matrix[9] = {
        hor_scale,  0.0f,        0.0f,
        0.0f,       ver_scale,   0.0f,
        hor_translate, ver_translate, 1.0f
    };

    lv_opengles_shader_bind(bound_ctx);
    lv_opengles_shader_set_uniform1f(bound_ctx, "u_ColorDepth", LV_COLOR_DEPTH);
    lv_opengles_shader_set_uniform1i(bound_ctx, "u_Texture", 0);
    lv_opengles_shader_set_uniformmatrix3fv(bound_ctx, "u_VertexTransform", 1, transposed_matrix);
    lv_opengles_shader_set_uniform1f(bound_ctx, "u_Opa", 1);
    lv_opengles_shader_set_uniform1i(bound_ctx, "u_IsFill", 0);
    lv_opengles_shader_set_uniform3f(bound_ctx, "u_FillColor", 1.0f, 1.0f, 1.0f);
    lv_opengles_shader_set_uniform1i(bound_ctx, "u_SwapRB", params->rb_swap);

    lv_opengles_render_draw(bound_ctx);
    LV_PROFILER_DRAW_END;
}

void lv_opengles_render_display_texture(lv_display_t * display, bool h_flip,
                                        bool v_flip)
{
    /*TODO: Deprecate this function and make lv_opengles_render_display public instead*/

    lv_opengles_render_params_t params = {
        .v_flip = v_flip,
        .h_flip = h_flip,
        .rb_swap = true
    };
    lv_opengles_render_display(display, &params);
}

void lv_opengles_render_clear(void)
{
    LV_PROFILER_DRAW_BEGIN;
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
    LV_PROFILER_DRAW_END;
}

void lv_opengles_viewport(int32_t x, int32_t y, int32_t w, int32_t h)
{
    LV_PROFILER_DRAW_BEGIN;
    GL_CALL(glViewport(x, y, w, h));
    LV_PROFILER_DRAW_END;
}

void lv_opengles_reinit_state(void)
{
    LV_PROFILER_DRAW_BEGIN;

    /* Rebind VAO, VBO, IBO to restore state after NanoVG or other external GL operations */
    lv_opengles_vertex_array_bind(bound_ctx);
    lv_opengles_vertex_buffer_bind(bound_ctx);
    lv_opengles_index_buffer_bind(bound_ctx);

    /* Re-setup vertex attributes since NanoVG may have modified them */
    for(unsigned int i = 0; i < 2; i++) {
        GL_CALL(glEnableVertexAttribArray(i));
        GL_CALL(glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, 16, (const void *)(intptr_t)(i * 2 * 4)));
    }

    LV_PROFILER_DRAW_END;
}

void lv_opengles_render(const lv_opengles_render_params_t * params)
{
    LV_ASSERT_NULL(params);
    if(!bound_ctx) {
        LV_LOG_WARN("Can't render texture without a bound OpenGL context");
        return;
    }
    LV_PROFILER_DRAW_BEGIN;
    lv_area_t intersection;
    if(!lv_area_intersect(&intersection, params->texture_area, params->texture_clip_area)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    GL_CALL(glActiveTexture(GL_TEXTURE0));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, params->texture));

    bool is_turned = false;
    if(params->matrix) {
        is_turned = params->matrix->m[0][0] == 0.f;
    }
    float tex_w, tex_h, full_w, full_h, inter_w, inter_h;
    full_w = (float)params->disp_w;
    full_h = (float)params->disp_h;
    if(is_turned) {
        tex_w = (float)lv_area_get_height(&intersection);
        tex_h = (float)lv_area_get_width(&intersection);
        inter_w = (float)intersection.y1;
        inter_h = (float)intersection.x1;
    }
    else {
        tex_w = (float)lv_area_get_width(&intersection);
        tex_h = (float)lv_area_get_height(&intersection);
        inter_w = (float)intersection.x1;
        inter_h = (float)intersection.y1;
    }

    float hor_scale = tex_w / full_w;
    float ver_scale = tex_h / full_h;
    float hor_translate = inter_w / full_w * 2.0f - (1.0f - hor_scale);
    float ver_translate = -(inter_h / full_h * 2.0f - (1.0f - ver_scale));
    hor_scale = params->h_flip ? -hor_scale : hor_scale;
    ver_scale = params->v_flip ? ver_scale : -ver_scale;

    if(params->texture != 0) {
        float clip_x1 = params->h_flip ? lv_opengles_map_float(params->texture_clip_area->x2, params->texture_area->x2,
                                                               params->texture_area->x1, 0.f, 1.f)
                        : lv_opengles_map_float(params->texture_clip_area->x1, params->texture_area->x1, params->texture_area->x2, 0.f, 1.f);
        float clip_x2 = params->h_flip ? lv_opengles_map_float(params->texture_clip_area->x1, params->texture_area->x2,
                                                               params->texture_area->x1, 0.f, 1.f)
                        : lv_opengles_map_float(params->texture_clip_area->x2, params->texture_area->x1, params->texture_area->x2, 0.f, 1.f);
        float clip_y1 = params->v_flip ? lv_opengles_map_float(params->texture_clip_area->y2, params->texture_area->y2,
                                                               params->texture_area->y1, 0.f, 1.f)
                        : lv_opengles_map_float(params->texture_clip_area->y1, params->texture_area->y1, params->texture_area->y2, 0.f, 1.f);
        float clip_y2 = params->v_flip ? lv_opengles_map_float(params->texture_clip_area->y1, params->texture_area->y2,
                                                               params->texture_area->y1, 0.f, 1.f)
                        : lv_opengles_map_float(params->texture_clip_area->y2, params->texture_area->y1, params->texture_area->y2, 0.f, 1.f);

        const float positions[LV_OPENGLES_VERTEX_BUFFER_LEN] = {
            -1.f,  1.0f, clip_x1, clip_y2,
            1.0f,  1.0f, clip_x2, clip_y2,
            1.0f, -1.0f, clip_x2, clip_y1,
            -1.f, -1.0f, clip_x1, clip_y1
        };
        lv_opengles_vertex_buffer_init(bound_ctx, positions, sizeof(positions));
    }

    lv_matrix_t matrix;
    lv_matrix_identity(&matrix);

    if(params->matrix) {
        if(is_turned) {
            /* Display turned 90 or 270 */
            if(params->matrix->m[0][1] < 0.f) hor_translate = -hor_translate;
            if(params->matrix->m[1][0] < 0.f) ver_translate = -ver_translate;
            hor_scale = -hor_scale;
            ver_scale = -ver_scale;
            lv_matrix_translate(&matrix, hor_translate, ver_translate);
            lv_matrix_scale(&matrix, hor_scale, ver_scale);

            lv_matrix_t adj_matrix;
            lv_memcpy(&adj_matrix, params->matrix, sizeof(lv_matrix_t));
            adj_matrix.m[0][2] = 0.f;
            adj_matrix.m[1][2] = 0.f;
            lv_matrix_multiply(&matrix, &adj_matrix);

        }
        else {
            /* Display turned 0 or 180 */
            if(params->matrix->m[0][0] < -0.0001f) {
                ver_scale = -ver_scale;
                ver_translate = -ver_translate;
            }

            if(params->matrix->m[1][1] < -0.0001f) {
                hor_scale = -hor_scale;
                hor_translate = -hor_translate;
            }

            lv_matrix_translate(&matrix, hor_translate, ver_translate);
            lv_matrix_scale(&matrix, hor_scale, ver_scale);
        }
    }
    else {
        lv_matrix_translate(&matrix, hor_translate, ver_translate);
        lv_matrix_scale(&matrix, hor_scale, ver_scale);
    }

    lv_matrix_t gl_matrix;
    lv_matrix_transpose(&matrix, &gl_matrix);

    lv_opengles_shader_bind(bound_ctx);
    lv_opengles_enable_blending(params->blend_opt);
    lv_opengles_shader_set_uniform1f(bound_ctx, "u_ColorDepth", LV_COLOR_DEPTH);
    lv_opengles_shader_set_uniform1i(bound_ctx, "u_Texture", 0);
    lv_opengles_shader_set_uniformmatrix3fv(bound_ctx, "u_VertexTransform", 1, (float *)&gl_matrix);
    lv_opengles_shader_set_uniform1f(bound_ctx, "u_Opa", (float)params->opa / (float)LV_OPA_100);
    lv_opengles_shader_set_uniform1i(bound_ctx, "u_IsFill", params->texture == 0);
    lv_opengles_shader_set_uniform3f(bound_ctx, "u_FillColor", (float)params->fill_color.red / 255.0f,
                                     (float)params->fill_color.green / 255.0f,
                                     (float)params->fill_color.blue / 255.0f);
    lv_opengles_shader_set_uniform1i(bound_ctx, "u_SwapRB", params->rb_swap ? 1 : 0);

    lv_opengles_render_draw(bound_ctx);
    lv_opengles_disable_blending();
    LV_PROFILER_DRAW_END;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_opengles_enable_blending(bool blend_opt)
{
    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glBlendFunc(blend_opt ? GL_SRC_ALPHA : GL_ONE, GL_ONE_MINUS_SRC_ALPHA));
}

static void lv_opengles_disable_blending(void)
{
    GL_CALL(glDisable(GL_BLEND));
}

static void lv_opengles_vertex_buffer_init(lv_opengles_gl_t * ctx, const void * data, unsigned int size)
{
    if(ctx->vertex_buffer_id == 0) GL_CALL(glGenBuffers(1, &ctx->vertex_buffer_id));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, ctx->vertex_buffer_id));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW));
}

static void lv_opengles_vertex_buffer_deinit(lv_opengles_gl_t * ctx)
{
    if(ctx->vertex_buffer_id == 0) return;
    GL_CALL(glDeleteBuffers(1, &ctx->vertex_buffer_id));
    ctx->vertex_buffer_id = 0;
}

static void lv_opengles_vertex_buffer_bind(lv_opengles_gl_t * ctx)
{
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, ctx->vertex_buffer_id));
}

static void lv_opengles_vertex_buffer_unbind(void)
{
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

static void lv_opengles_vertex_array_init(lv_opengles_gl_t * ctx)
{
    if(ctx->vertex_array_id == 0) GL_CALL(glGenVertexArrays(1, &ctx->vertex_array_id));
}

static void lv_opengles_vertex_array_deinit(lv_opengles_gl_t * ctx)
{
    if(ctx->vertex_array_id == 0) return;
    GL_CALL(glDeleteVertexArrays(1, &ctx->vertex_array_id));
    ctx->vertex_array_id = 0;
}

static void lv_opengles_vertex_array_bind(lv_opengles_gl_t * ctx)
{
    GL_CALL(glBindVertexArray(ctx->vertex_array_id));
}

static void lv_opengles_vertex_array_unbind(void)
{
    GL_CALL(glBindVertexArray(0));
}

static void lv_opengles_vertex_array_add_buffer(lv_opengles_gl_t * ctx)
{
    lv_opengles_vertex_buffer_bind(ctx);
    intptr_t offset = 0;

    for(unsigned int i = 0; i < 2; i++) {
        lv_opengles_vertex_array_bind(ctx);
        GL_CALL(glEnableVertexAttribArray(i));
        GL_CALL(glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, 16, (const void *)offset));
        offset += 2 * 4;
    }
}

static void lv_opengles_index_buffer_init(lv_opengles_gl_t * ctx, const unsigned int * data,
                                          unsigned int count)
{
    LV_ASSERT_NULL(ctx);
    ctx->index_buffer_count = count;
    if(ctx->index_buffer_id == 0) GL_CALL(glGenBuffers(1, &ctx->index_buffer_id));

    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx->index_buffer_id));

    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, GL_STATIC_DRAW));
}

static void lv_opengles_index_buffer_deinit(lv_opengles_gl_t * ctx)
{
    LV_ASSERT_NULL(ctx);
    if(ctx->index_buffer_id == 0) return;
    GL_CALL(glDeleteBuffers(1, &ctx->index_buffer_id));
    ctx->index_buffer_id = 0;
}

static unsigned int lv_opengles_index_buffer_get_count(lv_opengles_gl_t * ctx)
{
    return ctx->index_buffer_count;
}

static void lv_opengles_index_buffer_bind(lv_opengles_gl_t * ctx)
{
    LV_ASSERT_NULL(ctx);
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx->index_buffer_id));
}

static void lv_opengles_index_buffer_unbind(void)
{
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

static unsigned int lv_opengles_shader_manager_init(void)
{
    for(lv_opengl_glsl_version_t version = LV_OPENGL_GLSL_VERSION_300ES; version < LV_OPENGL_GLSL_VERSION_LAST; ++version) {
        LV_LOG_INFO("Trying GLSL version %s", lv_opengles_glsl_version_to_string(version));
        {
            /* Initialize the shader manager*/
            lv_opengl_shader_portions_t portions;
            lv_opengles_shader_get_source(&portions, version);
            char * vertex_shader = lv_opengles_shader_get_vertex(version);
            char * frag_shader = lv_opengles_shader_get_fragment(version);
            lv_opengl_shader_manager_init(&shader_manager, portions.all, portions.count, vertex_shader, frag_shader);
            lv_free(vertex_shader);
            lv_free(frag_shader);
        }

        lv_opengl_shader_params_t frag_shader = {.name = "__MAIN__.frag"};
        lv_opengl_shader_params_t vert_shader = {.name = "__MAIN__.vert"};
        lv_opengl_shader_program_t * program = lv_opengl_shader_manager_compile_program(&shader_manager, &frag_shader,
                                                                                        &vert_shader, version);
        if(program) {
            LV_LOG_INFO("Compiled shaders with version %s", lv_opengles_glsl_version_to_string(version));
            return lv_opengl_shader_program_get_id(program);
        }
        lv_opengl_shader_manager_deinit(&shader_manager);
    }
    LV_LOG_ERROR("Failed to initialize shaders");
    return 0;
}

static lv_result_t lv_opengles_shader_init(lv_opengles_gl_t * ctx)
{
    LV_ASSERT_NULL(ctx);
    if(ctx->shader_id != 0) {
        return LV_RESULT_OK;
    }
    ctx->shader_id = lv_opengles_shader_manager_init();
    return ctx->shader_id != 0 ? LV_RESULT_OK : LV_RESULT_INVALID;
}

static void lv_opengles_shader_deinit(lv_opengles_gl_t * ctx)
{
    LV_ASSERT_NULL(ctx);
    if(ctx->shader_id == 0) return;
    /* The program is part of the manager and as such will be destroyed inside */
    lv_opengl_shader_manager_deinit(&shader_manager);
    ctx->shader_id = 0;
}

static void lv_opengles_shader_bind(lv_opengles_gl_t * ctx)
{
    LV_ASSERT_NULL(ctx);
    GL_CALL(glUseProgram(ctx->shader_id));
}

static void lv_opengles_shader_unbind(void)
{
    GL_CALL(glUseProgram(0));
}

static int lv_opengles_shader_get_uniform_location(lv_opengles_gl_t * ctx, const char * name)
{
    LV_ASSERT_NULL(ctx);
    int id = -1;
    const size_t shader_name_count = sizeof(shader_names) / sizeof(shader_names[0]);
    for(size_t i = 0; i < shader_name_count; i++) {
        if(lv_strcmp(shader_names[i], name) == 0) {
            id = i;
        }
    }

    LV_ASSERT_FORMAT_MSG(id > -1, "Uniform location doesn't exist for '%s'. Check `shader_location` array", name);

    if(ctx->shader_location[id] != 0) {
        return ctx->shader_location[id];
    }

    int location;
    GL_CALL(location = glGetUniformLocation(ctx->shader_id, name));
    if(location == -1)
        LV_LOG_WARN("Warning: uniform '%s' doesn't exist!", name);

    ctx->shader_location[id] = location;
    return location;
}

static void lv_opengles_shader_set_uniform1i(lv_opengles_gl_t * ctx, const char * name, int value)
{
    LV_PROFILER_DRAW_BEGIN;
    GL_CALL(glUniform1i(lv_opengles_shader_get_uniform_location(ctx, name), value));
    LV_PROFILER_DRAW_END;
}

static void lv_opengles_shader_set_uniformmatrix3fv(lv_opengles_gl_t * ctx, const char * name, int count,
                                                    const float * values)
{
    LV_PROFILER_DRAW_BEGIN;
    /*
     * GLES2.0 doesn't support transposing the matrix via glUniformMatrix3fv so this is the transposed matrix
     * https://registry.khronos.org/OpenGL/specs/es/2.0/es_full_spec_2.0.pdf page 47
     */
    GL_CALL(glUniformMatrix3fv(lv_opengles_shader_get_uniform_location(ctx, name), count, GL_FALSE, values));
    LV_PROFILER_DRAW_END;
}

static void lv_opengles_shader_set_uniform1f(lv_opengles_gl_t * ctx, const char * name, float value)
{
    LV_PROFILER_DRAW_BEGIN;
    GL_CALL(glUniform1f(lv_opengles_shader_get_uniform_location(ctx, name), value));
    LV_PROFILER_DRAW_END;
}

static void lv_opengles_shader_set_uniform3f(lv_opengles_gl_t * ctx, const char * name, float value_0,
                                             float value_1, float value_2)
{
    LV_PROFILER_DRAW_BEGIN;
    GL_CALL(glUniform3f(lv_opengles_shader_get_uniform_location(ctx, name), value_0, value_1, value_2));
    LV_PROFILER_DRAW_END;
}

static void lv_opengles_render_draw(lv_opengles_gl_t * ctx)
{
    LV_PROFILER_DRAW_BEGIN;
    LV_ASSERT_NULL(ctx);
    lv_opengles_shader_bind(ctx);
    lv_opengles_vertex_array_bind(ctx);
    lv_opengles_index_buffer_bind(ctx);
    unsigned int count = lv_opengles_index_buffer_get_count(ctx);
    GL_CALL(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, NULL));
    LV_PROFILER_DRAW_END;
}

/**
 * Copied from `lv_map` in lv_math.h to operate on floats
 */
static float lv_opengles_map_float(float x, float min_in, float max_in, float min_out, float max_out)
{
    if(max_in >= min_in && x >= max_in) return max_out;
    if(max_in >= min_in && x <= min_in) return min_out;

    if(max_in <= min_in && x <= max_in) return max_out;
    if(max_in <= min_in && x >= min_in) return min_out;

    /**
     * The equation should be:
     *   ((x - min_in) * delta_out) / delta in) + min_out
     * To avoid rounding error reorder the operations:
     *   (x - min_in) * (delta_out / delta_min) + min_out
     */

    float delta_in = max_in - min_in;
    float delta_out = max_out - min_out;

    return ((x - min_in) * delta_out) / delta_in + min_out;
}

static void populate_vertex_buffer(float vertex_buffer[LV_OPENGLES_VERTEX_BUFFER_LEN],
                                   lv_display_rotation_t rotation, bool * h_flip, bool * v_flip, float clip_x1, float clip_y1, float clip_x2,
                                   float clip_y2)
{
#if !LV_USE_DRAW_OPENGLES
    LV_UNUSED(h_flip);
    LV_UNUSED(v_flip);
#endif
    switch(rotation) {
        case LV_DISPLAY_ROTATION_0:
            vertex_buffer[0] = -1.f;
            vertex_buffer[1] = 1.0f;
            vertex_buffer[2] = clip_x1;
            vertex_buffer[3] = clip_y2;
            vertex_buffer[4] = 1.0f;
            vertex_buffer[5] = 1.0f;
            vertex_buffer[6] = clip_x2;
            vertex_buffer[7] = clip_y2;
            vertex_buffer[8] = 1.0f;
            vertex_buffer[9] = -1.0f;
            vertex_buffer[10] = clip_x2;
            vertex_buffer[11] = clip_y1;
            vertex_buffer[12] = -1.f;
            vertex_buffer[13] = -1.0f;
            vertex_buffer[14] = clip_x1;
            vertex_buffer[15] = clip_y1;
            break;

        case LV_DISPLAY_ROTATION_270:
#if LV_USE_DRAW_OPENGLES
            *h_flip = !*h_flip;
            *v_flip = !*v_flip;
#endif
            vertex_buffer[0] = 1.0f;
            vertex_buffer[1] = 1.0f;
            vertex_buffer[2] = clip_x1;
            vertex_buffer[3] = clip_y2;
            vertex_buffer[4] = 1.0f;
            vertex_buffer[5] = -1.0f;
            vertex_buffer[6] = clip_x2;
            vertex_buffer[7] = clip_y2;
            vertex_buffer[8] = -1.f;
            vertex_buffer[9] = -1.0f;
            vertex_buffer[10] = clip_x2;
            vertex_buffer[11] = clip_y1;
            vertex_buffer[12] = -1.f;
            vertex_buffer[13] = 1.0f;
            vertex_buffer[14] = clip_x1;
            vertex_buffer[15] = clip_y1;
            break;

        case LV_DISPLAY_ROTATION_180:
            vertex_buffer[0] = 1.0f;
            vertex_buffer[1] = -1.0f;
            vertex_buffer[2] = clip_x1;
            vertex_buffer[3] = clip_y2;
            vertex_buffer[4] = -1.f;
            vertex_buffer[5] = -1.0f;
            vertex_buffer[6] = clip_x2;
            vertex_buffer[7] = clip_y2;
            vertex_buffer[8] = -1.f;
            vertex_buffer[9] = 1.0f;
            vertex_buffer[10] = clip_x2;
            vertex_buffer[11] = clip_y1;
            vertex_buffer[12] = 1.0f;
            vertex_buffer[13] = 1.0f;
            vertex_buffer[14] = clip_x1;
            vertex_buffer[15] = clip_y1;
            break;
        case LV_DISPLAY_ROTATION_90:
#if LV_USE_DRAW_OPENGLES
            *h_flip = !*h_flip;
            *v_flip = !*v_flip;
#endif
            vertex_buffer[0] = -1.f;
            vertex_buffer[1] = -1.0f;
            vertex_buffer[2] = clip_x1;
            vertex_buffer[3] = clip_y2;
            vertex_buffer[4] = -1.f;
            vertex_buffer[5] = 1.0f;
            vertex_buffer[6] = clip_x2;
            vertex_buffer[7] = clip_y2;
            vertex_buffer[8] = 1.0f;
            vertex_buffer[9] = 1.0f;
            vertex_buffer[10] = clip_x2;
            vertex_buffer[11] = clip_y1;
            vertex_buffer[12] = 1.0f;
            vertex_buffer[13] = -1.0f;
            vertex_buffer[14] = clip_x1;
            vertex_buffer[15] = clip_y1;
            break;
    }
}
#endif /* LV_USE_OPENGLES */
