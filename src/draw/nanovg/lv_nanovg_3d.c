/**
 * @file lv_nanovg_3d.c
 * NanoVG 3D rendering extension implementation
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_nanovg_3d.h"

#if LV_USE_DRAW_NANOVG && LV_USE_3DTEXTURE

#include "lv_draw_nanovg_private.h"
#include "../../stdlib/lv_mem.h"
#include "../../stdlib/lv_string.h"
#include "../../misc/lv_log.h"
#include "../../misc/lv_math.h"

#if LV_USE_CGLTF
    #include "../../libs/cgltf/lv_cgltf.h"
#endif

/* Include OpenGL headers based on backend */
#if LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GLES2 || LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GLES3
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#else
    #include <GL/gl.h>
    #include <GL/glext.h>
#endif

/* Forward declarations for NanoVG GL functions (defined in nanovg_gl.h)
 * We cannot include nanovg_gl.h here as it contains implementation code
 * that would cause multiple definition errors */
int nvglCreateImageFromHandleGL2(NVGcontext * ctx, GLuint textureId, int w, int h, int flags);
int nvglCreateImageFromHandleGL3(NVGcontext * ctx, GLuint textureId, int w, int h, int flags);
int nvglCreateImageFromHandleGLES2(NVGcontext * ctx, GLuint textureId, int w, int h, int flags);
int nvglCreateImageFromHandleGLES3(NVGcontext * ctx, GLuint textureId, int w, int h, int flags);

/* Define NanoVG texture creation function based on backend */
#if LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GL2
    #define NVG_CREATE_IMAGE_FROM_HANDLE nvglCreateImageFromHandleGL2
#elif LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GL3
    #define NVG_CREATE_IMAGE_FROM_HANDLE nvglCreateImageFromHandleGL3
#elif LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GLES2
    #define NVG_CREATE_IMAGE_FROM_HANDLE nvglCreateImageFromHandleGLES2
#elif LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GLES3
    #define NVG_CREATE_IMAGE_FROM_HANDLE nvglCreateImageFromHandleGLES3
#endif

#include <math.h>

/*********************
 *      DEFINES
 *********************/

#define MAX_MESHES 64

/* GLES2 only supports GL_UNSIGNED_SHORT for indices */
#if LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GLES2 || LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GLES3
    #define GL_INDEX_TYPE GL_UNSIGNED_SHORT
    typedef uint16_t gl_index_t;
#else
    #define GL_INDEX_TYPE GL_UNSIGNED_INT
    typedef uint32_t gl_index_t;
#endif

/**********************
 *      TYPEDEFS
 **********************/

/** OpenGL state backup for state management */
typedef struct {
    GLint viewport[4];
    GLint scissor_box[4];
    GLboolean depth_test;
    GLboolean cull_face;
    GLboolean blend;
    GLboolean scissor_test;
    GLint blend_src_rgb;
    GLint blend_dst_rgb;
    GLint blend_src_alpha;
    GLint blend_dst_alpha;
    GLint current_program;
    GLint active_texture;
    GLint texture_2d;
    GLint array_buffer;
    GLint element_array_buffer;
    GLint framebuffer;
} lv_gl_state_t;

/** GPU mesh data */
typedef struct {
    GLuint vao;
    GLuint vbo_positions;
    GLuint vbo_normals;
    GLuint vbo_texcoords;
    GLuint ibo;
    uint32_t index_count;
    uint32_t vertex_count;
    float base_color[4];
} lv_nanovg_3d_mesh_t;

/** 3D model structure */
struct _lv_nanovg_3d_model_t {
    lv_nanovg_3d_mesh_t meshes[MAX_MESHES];
    uint32_t mesh_count;
    float min_bounds[3];
    float max_bounds[3];
    float rotation_y;
#if LV_USE_CGLTF
    cgltf_data * gltf_data;
#endif
};

/** 3D rendering context */
struct _lv_nanovg_3d_ctx_t {
    NVGcontext * vg;
    GLuint shader_program;
    GLuint fbo;
    GLuint fbo_texture;
    GLuint fbo_depth;
    int32_t fbo_width;
    int32_t fbo_height;
    lv_gl_state_t saved_state;

    /* Shader uniforms */
    GLint u_mvp;
    GLint u_model;
    GLint u_normal_matrix;
    GLint u_light_dir;
    GLint u_base_color;
    GLint u_view_pos;

    /* Shader attributes */
    GLint a_position;
    GLint a_normal;
    GLint a_texcoord;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool create_shader_program(lv_nanovg_3d_ctx_t * ctx);
static void destroy_shader_program(lv_nanovg_3d_ctx_t * ctx);
static bool create_fbo(lv_nanovg_3d_ctx_t * ctx, int32_t width, int32_t height);
static void destroy_fbo(lv_nanovg_3d_ctx_t * ctx);
static void save_gl_state(lv_gl_state_t * state);
static void restore_gl_state(const lv_gl_state_t * state);
static void mat4_identity(float * m);
static void mat4_perspective(float * m, float fov, float aspect, float near, float far);
static void mat4_look_at(float * m, const float * eye, const float * center, const float * up);
static void mat4_multiply(float * result, const float * a, const float * b);
static void mat4_rotate_y(float * m, float angle);
static void vec3_normalize(float * v);
static void vec3_cross(float * result, const float * a, const float * b);
static void vec3_sub(float * result, const float * a, const float * b);

#if LV_USE_CGLTF
static bool upload_mesh_to_gpu(lv_nanovg_3d_ctx_t * ctx, lv_nanovg_3d_mesh_t * gpu_mesh,
                               const lv_cgltf_mesh_data_t * mesh_data,
                               const lv_cgltf_material_t * material);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/* Simple vertex shader */
static const char * vertex_shader_src =
    "#version 100\n"
    "precision mediump float;\n"
    "attribute vec3 a_position;\n"
    "attribute vec3 a_normal;\n"
    "attribute vec2 a_texcoord;\n"
    "uniform mat4 u_mvp;\n"
    "uniform mat4 u_model;\n"
    "uniform mat3 u_normal_matrix;\n"
    "varying vec3 v_normal;\n"
    "varying vec3 v_position;\n"
    "varying vec2 v_texcoord;\n"
    "void main() {\n"
    "    gl_Position = u_mvp * vec4(a_position, 1.0);\n"
    "    v_normal = u_normal_matrix * a_normal;\n"
    "    v_position = (u_model * vec4(a_position, 1.0)).xyz;\n"
    "    v_texcoord = a_texcoord;\n"
    "}\n";

/* Simple fragment shader with basic lighting */
static const char * fragment_shader_src =
    "#version 100\n"
    "precision mediump float;\n"
    "varying vec3 v_normal;\n"
    "varying vec3 v_position;\n"
    "varying vec2 v_texcoord;\n"
    "uniform vec3 u_light_dir;\n"
    "uniform vec4 u_base_color;\n"
    "uniform vec3 u_view_pos;\n"
    "void main() {\n"
    "    vec3 normal = normalize(v_normal);\n"
    "    vec3 light_dir = normalize(u_light_dir);\n"
    "    float diff = max(dot(normal, light_dir), 0.0);\n"
    "    vec3 view_dir = normalize(u_view_pos - v_position);\n"
    "    vec3 reflect_dir = reflect(-light_dir, normal);\n"
    "    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32.0);\n"
    "    vec3 ambient = 0.3 * u_base_color.rgb;\n"
    "    vec3 diffuse = diff * u_base_color.rgb;\n"
    "    vec3 specular = 0.5 * spec * vec3(1.0);\n"
    "    vec3 result = ambient + diffuse + specular;\n"
    "    gl_FragColor = vec4(result, u_base_color.a);\n"
    "}\n";

/**********************
 *      MACROS
 **********************/

#define GL_CHECK_ERROR() do { \
        GLenum err = glGetError(); \
        if(err != GL_NO_ERROR) { \
            LV_LOG_WARN("GL error: 0x%x at %s:%d", err, __FILE__, __LINE__); \
        } \
    } while(0)

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_nanovg_3d_ctx_t * lv_nanovg_3d_create(NVGcontext * vg)
{
    if(vg == NULL) return NULL;

    lv_nanovg_3d_ctx_t * ctx = lv_malloc_zeroed(sizeof(lv_nanovg_3d_ctx_t));
    if(ctx == NULL) return NULL;

    ctx->vg = vg;

    if(!create_shader_program(ctx)) {
        LV_LOG_ERROR("Failed to create shader program");
        lv_free(ctx);
        return NULL;
    }

    LV_LOG_INFO("NanoVG 3D context created");
    return ctx;
}

void lv_nanovg_3d_destroy(lv_nanovg_3d_ctx_t * ctx)
{
    if(ctx == NULL) return;

    destroy_fbo(ctx);
    destroy_shader_program(ctx);
    lv_free(ctx);
}

lv_nanovg_3d_model_t * lv_nanovg_3d_load_model(lv_nanovg_3d_ctx_t * ctx, const char * path)
{
#if LV_USE_CGLTF
    if(ctx == NULL || path == NULL) return NULL;

    /* Parse glTF file */
    cgltf_data * gltf = lv_cgltf_parse_file(path);
    if(gltf == NULL) {
        LV_LOG_ERROR("Failed to parse glTF: %s", path);
        return NULL;
    }

    /* Load buffers */
    if(lv_cgltf_load_buffers(gltf, path) != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to load glTF buffers: %s", path);
        lv_cgltf_free(gltf);
        return NULL;
    }

    /* Create model */
    lv_nanovg_3d_model_t * model = lv_malloc_zeroed(sizeof(lv_nanovg_3d_model_t));
    if(model == NULL) {
        lv_cgltf_free(gltf);
        return NULL;
    }

    model->gltf_data = gltf;

    /* Initialize bounds */
    model->min_bounds[0] = model->min_bounds[1] = model->min_bounds[2] = 1e10f;
    model->max_bounds[0] = model->max_bounds[1] = model->max_bounds[2] = -1e10f;

    /* Process meshes */
    uint32_t mesh_count = lv_cgltf_get_mesh_count(gltf);
    for(uint32_t i = 0; i < mesh_count && model->mesh_count < MAX_MESHES; i++) {
        /* For simplicity, only process first primitive of each mesh */
        lv_cgltf_mesh_data_t mesh_data;
        lv_cgltf_material_t material;

        if(lv_cgltf_get_mesh_data(gltf, i, 0, &mesh_data) != LV_RESULT_OK) {
            continue;
        }

        lv_cgltf_get_material(gltf, i, 0, &material);

        /* Update bounds */
        if(mesh_data.positions) {
            for(uint32_t v = 0; v < mesh_data.vertex_count; v++) {
                float * pos = &mesh_data.positions[v * 3];
                for(int j = 0; j < 3; j++) {
                    if(pos[j] < model->min_bounds[j]) model->min_bounds[j] = pos[j];
                    if(pos[j] > model->max_bounds[j]) model->max_bounds[j] = pos[j];
                }
            }
        }

        /* Upload to GPU */
        if(upload_mesh_to_gpu(ctx, &model->meshes[model->mesh_count], &mesh_data, &material)) {
            model->mesh_count++;
        }

        lv_cgltf_free_mesh_data(&mesh_data);
    }

    LV_LOG_INFO("Loaded model: %s (meshes: %u)", path, model->mesh_count);
    return model;
#else
    LV_UNUSED(ctx);
    LV_UNUSED(path);
    LV_LOG_ERROR("LV_USE_CGLTF is not enabled");
    return NULL;
#endif
}

void lv_nanovg_3d_free_model(lv_nanovg_3d_ctx_t * ctx, lv_nanovg_3d_model_t * model)
{
    LV_UNUSED(ctx);
    if(model == NULL) return;

    /* Free GPU resources */
    for(uint32_t i = 0; i < model->mesh_count; i++) {
        lv_nanovg_3d_mesh_t * mesh = &model->meshes[i];
        if(mesh->vbo_positions) glDeleteBuffers(1, &mesh->vbo_positions);
        if(mesh->vbo_normals) glDeleteBuffers(1, &mesh->vbo_normals);
        if(mesh->vbo_texcoords) glDeleteBuffers(1, &mesh->vbo_texcoords);
        if(mesh->ibo) glDeleteBuffers(1, &mesh->ibo);
    }

#if LV_USE_CGLTF
    if(model->gltf_data) {
        lv_cgltf_free(model->gltf_data);
    }
#endif

    lv_free(model);
}

void lv_nanovg_3d_begin(lv_nanovg_3d_ctx_t * ctx)
{
    if(ctx == NULL) return;
    save_gl_state(&ctx->saved_state);
}

void lv_nanovg_3d_end(lv_nanovg_3d_ctx_t * ctx)
{
    if(ctx == NULL) return;
    restore_gl_state(&ctx->saved_state);
}

uint32_t lv_nanovg_3d_render_to_texture(lv_nanovg_3d_ctx_t * ctx, lv_nanovg_3d_model_t * model,
                                        int32_t width, int32_t height,
                                        const lv_nanovg_3d_camera_t * camera)
{
    if(ctx == NULL || model == NULL || camera == NULL) return 0;
    if(width <= 0 || height <= 0) return 0;

    /* Create or resize FBO if needed */
    if(ctx->fbo == 0 || ctx->fbo_width != width || ctx->fbo_height != height) {
        destroy_fbo(ctx);
        if(!create_fbo(ctx, width, height)) {
            LV_LOG_ERROR("Failed to create FBO");
            return 0;
        }
    }

    /* Bind FBO */
    glBindFramebuffer(GL_FRAMEBUFFER, ctx->fbo);
    glViewport(0, 0, width, height);

    /* Clear */
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Enable depth testing */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    /* Build matrices */
    float projection[16], view[16], model_mat[16], rotation[16], mvp[16], temp[16];
    mat4_perspective(projection, camera->fov, (float)width / (float)height,
                     camera->near_plane, camera->far_plane);
    mat4_look_at(view, camera->eye, camera->center, camera->up);
    mat4_identity(model_mat);
    mat4_rotate_y(rotation, model->rotation_y);
    mat4_multiply(model_mat, model_mat, rotation);

    mat4_multiply(temp, view, model_mat);
    mat4_multiply(mvp, projection, temp);

    /* Use shader */
    glUseProgram(ctx->shader_program);

    /* Set uniforms */
    glUniformMatrix4fv(ctx->u_mvp, 1, GL_FALSE, mvp);
    glUniformMatrix4fv(ctx->u_model, 1, GL_FALSE, model_mat);

    /* Normal matrix (inverse transpose of model matrix upper-left 3x3) */
    float normal_matrix[9] = {
        model_mat[0], model_mat[1], model_mat[2],
        model_mat[4], model_mat[5], model_mat[6],
        model_mat[8], model_mat[9], model_mat[10]
    };
    glUniformMatrix3fv(ctx->u_normal_matrix, 1, GL_FALSE, normal_matrix);

    /* Light direction (from top-right-front) */
    float light_dir[3] = {1.0f, 1.0f, 1.0f};
    vec3_normalize(light_dir);
    glUniform3fv(ctx->u_light_dir, 1, light_dir);
    glUniform3fv(ctx->u_view_pos, 1, camera->eye);

    /* Render meshes */
    for(uint32_t i = 0; i < model->mesh_count; i++) {
        lv_nanovg_3d_mesh_t * mesh = &model->meshes[i];

        glUniform4fv(ctx->u_base_color, 1, mesh->base_color);

        /* Bind buffers */
        if(mesh->vbo_positions) {
            glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_positions);
            glVertexAttribPointer(ctx->a_position, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(ctx->a_position);
        }

        if(mesh->vbo_normals) {
            glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_normals);
            glVertexAttribPointer(ctx->a_normal, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(ctx->a_normal);
        }
        else {
            glDisableVertexAttribArray(ctx->a_normal);
            glVertexAttrib3f(ctx->a_normal, 0.0f, 1.0f, 0.0f);
        }

        if(mesh->vbo_texcoords) {
            glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_texcoords);
            glVertexAttribPointer(ctx->a_texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(ctx->a_texcoord);
        }
        else {
            glDisableVertexAttribArray(ctx->a_texcoord);
        }

        /* Draw */
        if(mesh->ibo && mesh->index_count > 0) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
            glDrawElements(GL_TRIANGLES, mesh->index_count, GL_INDEX_TYPE, 0);
        }
        else if(mesh->vertex_count > 0) {
            glDrawArrays(GL_TRIANGLES, 0, mesh->vertex_count);
        }
    }

    glDisableVertexAttribArray(ctx->a_position);
    glDisableVertexAttribArray(ctx->a_normal);
    glDisableVertexAttribArray(ctx->a_texcoord);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return ctx->fbo_texture;
}

void lv_nanovg_3d_render_direct(lv_nanovg_3d_ctx_t * ctx, lv_nanovg_3d_model_t * model,
                                const lv_area_t * dest_area, int32_t layer_w, int32_t layer_h,
                                const lv_nanovg_3d_camera_t * camera)
{
    if(ctx == NULL || model == NULL || dest_area == NULL || camera == NULL) return;

    int32_t dest_w = lv_area_get_width(dest_area);
    int32_t dest_h = lv_area_get_height(dest_area);
    if(dest_w <= 0 || dest_h <= 0) return;

    /* Set viewport to the destination area
     * Note: OpenGL viewport Y is from bottom, but LVGL Y is from top
     * So we need to flip the Y coordinate */
    int32_t vp_x = dest_area->x1;
    int32_t vp_y = layer_h - dest_area->y2 - 1;  /* Flip Y */
    glViewport(vp_x, vp_y, dest_w, dest_h);

    /* Setup scissor test to clip to destination area */
    glEnable(GL_SCISSOR_TEST);
    glScissor(vp_x, vp_y, dest_w, dest_h);

    /* Clear only the destination area with transparent color */
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);  /* Dark gray background for visibility */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Enable depth testing */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    /* Build matrices */
    float projection[16], view[16], model_mat[16], rotation[16], mvp[16], temp[16];
    mat4_perspective(projection, camera->fov, (float)dest_w / (float)dest_h,
                     camera->near_plane, camera->far_plane);
    mat4_look_at(view, camera->eye, camera->center, camera->up);
    mat4_identity(model_mat);
    mat4_rotate_y(rotation, model->rotation_y);
    mat4_multiply(model_mat, model_mat, rotation);

    mat4_multiply(temp, view, model_mat);
    mat4_multiply(mvp, projection, temp);

    /* Use shader */
    glUseProgram(ctx->shader_program);

    /* Set uniforms */
    glUniformMatrix4fv(ctx->u_mvp, 1, GL_FALSE, mvp);
    glUniformMatrix4fv(ctx->u_model, 1, GL_FALSE, model_mat);

    /* Normal matrix (inverse transpose of model matrix upper-left 3x3) */
    float normal_matrix[9] = {
        model_mat[0], model_mat[1], model_mat[2],
        model_mat[4], model_mat[5], model_mat[6],
        model_mat[8], model_mat[9], model_mat[10]
    };
    glUniformMatrix3fv(ctx->u_normal_matrix, 1, GL_FALSE, normal_matrix);

    /* Light direction (from top-right-front) */
    float light_dir[3] = {1.0f, 1.0f, 1.0f};
    vec3_normalize(light_dir);
    glUniform3fv(ctx->u_light_dir, 1, light_dir);
    glUniform3fv(ctx->u_view_pos, 1, camera->eye);

    /* Render meshes */
    for(uint32_t i = 0; i < model->mesh_count; i++) {
        lv_nanovg_3d_mesh_t * mesh = &model->meshes[i];

        glUniform4fv(ctx->u_base_color, 1, mesh->base_color);

        /* Bind buffers */
        if(mesh->vbo_positions) {
            glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_positions);
            glVertexAttribPointer(ctx->a_position, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(ctx->a_position);
        }

        if(mesh->vbo_normals) {
            glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_normals);
            glVertexAttribPointer(ctx->a_normal, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(ctx->a_normal);
        }
        else {
            glDisableVertexAttribArray(ctx->a_normal);
            glVertexAttrib3f(ctx->a_normal, 0.0f, 1.0f, 0.0f);
        }

        if(mesh->vbo_texcoords) {
            glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_texcoords);
            glVertexAttribPointer(ctx->a_texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(ctx->a_texcoord);
        }
        else {
            glDisableVertexAttribArray(ctx->a_texcoord);
        }

        /* Draw */
        if(mesh->ibo && mesh->index_count > 0) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
            glDrawElements(GL_TRIANGLES, mesh->index_count, GL_INDEX_TYPE, 0);
        }
        else if(mesh->vertex_count > 0) {
            glDrawArrays(GL_TRIANGLES, 0, mesh->vertex_count);
        }
    }

    /* Clean up */
    glDisableVertexAttribArray(ctx->a_position);
    glDisableVertexAttribArray(ctx->a_normal);
    glDisableVertexAttribArray(ctx->a_texcoord);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* Disable scissor and depth testing */
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_DEPTH_TEST);

    /* Restore full viewport */
    glViewport(0, 0, layer_w, layer_h);
}

void lv_nanovg_3d_render_texture(lv_nanovg_3d_ctx_t * ctx, uint32_t texture_id,
                                 const lv_area_t * dest_area, lv_opa_t opa,
                                 int32_t layer_w, int32_t layer_h,
                                 const lv_area_t * clip_area,
                                 bool h_flip, bool v_flip)
{
    if(ctx == NULL || texture_id == 0 || dest_area == NULL) return;

    NVGcontext * vg = ctx->vg;
    int32_t w = lv_area_get_width(dest_area);
    int32_t h = lv_area_get_height(dest_area);

    /* Create NanoVG image from texture */
    int nvg_image = NVG_CREATE_IMAGE_FROM_HANDLE(vg, texture_id, w, h, NVG_IMAGE_FLIPY);
    if(nvg_image == 0) {
        LV_LOG_ERROR("Failed to create NanoVG image from texture");
        return;
    }

    /* Apply clipping */
    if(clip_area) {
        nvgScissor(vg, clip_area->x1, clip_area->y1,
                   lv_area_get_width(clip_area), lv_area_get_height(clip_area));
    }

    /* Calculate UV coordinates for flipping */
    float u0 = h_flip ? 1.0f : 0.0f;
    float v0 = v_flip ? 1.0f : 0.0f;
    float u1 = h_flip ? 0.0f : 1.0f;
    float v1 = v_flip ? 0.0f : 1.0f;

    LV_UNUSED(u0);
    LV_UNUSED(v0);
    LV_UNUSED(u1);
    LV_UNUSED(v1);
    LV_UNUSED(layer_w);
    LV_UNUSED(layer_h);

    /* Draw image */
    NVGpaint paint = nvgImagePattern(vg, dest_area->x1, dest_area->y1, w, h, 0, nvg_image,
                                     (float)opa / 255.0f);
    nvgBeginPath(vg);
    nvgRect(vg, dest_area->x1, dest_area->y1, w, h);
    nvgFillPaint(vg, paint);
    nvgFill(vg);

    /* Clean up */
    nvgDeleteImage(vg, nvg_image);

    if(clip_area) {
        nvgResetScissor(vg);
    }
}

void lv_nanovg_3d_model_set_rotation(lv_nanovg_3d_model_t * model, float angle_y)
{
    if(model == NULL) return;
    model->rotation_y = angle_y;
}

void lv_nanovg_3d_camera_init(lv_nanovg_3d_camera_t * camera)
{
    if(camera == NULL) return;

    camera->eye[0] = 0.0f;
    camera->eye[1] = 0.0f;
    camera->eye[2] = 5.0f;

    camera->center[0] = 0.0f;
    camera->center[1] = 0.0f;
    camera->center[2] = 0.0f;

    camera->up[0] = 0.0f;
    camera->up[1] = 1.0f;
    camera->up[2] = 0.0f;

    camera->fov = 45.0f;
    camera->near_plane = 0.1f;
    camera->far_plane = 100.0f;
}

void lv_nanovg_3d_model_get_bounds(lv_nanovg_3d_model_t * model, float * min_bounds, float * max_bounds)
{
    if(model == NULL) return;

    if(min_bounds) {
        min_bounds[0] = model->min_bounds[0];
        min_bounds[1] = model->min_bounds[1];
        min_bounds[2] = model->min_bounds[2];
    }

    if(max_bounds) {
        max_bounds[0] = model->max_bounds[0];
        max_bounds[1] = model->max_bounds[1];
        max_bounds[2] = model->max_bounds[2];
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static GLuint compile_shader(GLenum type, const char * source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE) {
        char log[512];
        glGetShaderInfoLog(shader, sizeof(log), NULL, log);
        LV_LOG_ERROR("Shader compile error: %s", log);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

static bool create_shader_program(lv_nanovg_3d_ctx_t * ctx)
{
    GLuint vs = compile_shader(GL_VERTEX_SHADER, vertex_shader_src);
    if(vs == 0) return false;

    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_src);
    if(fs == 0) {
        glDeleteShader(vs);
        return false;
    }

    ctx->shader_program = glCreateProgram();
    glAttachShader(ctx->shader_program, vs);
    glAttachShader(ctx->shader_program, fs);
    glLinkProgram(ctx->shader_program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint status;
    glGetProgramiv(ctx->shader_program, GL_LINK_STATUS, &status);
    if(status == GL_FALSE) {
        char log[512];
        glGetProgramInfoLog(ctx->shader_program, sizeof(log), NULL, log);
        LV_LOG_ERROR("Program link error: %s", log);
        glDeleteProgram(ctx->shader_program);
        ctx->shader_program = 0;
        return false;
    }

    /* Get uniform locations */
    ctx->u_mvp = glGetUniformLocation(ctx->shader_program, "u_mvp");
    ctx->u_model = glGetUniformLocation(ctx->shader_program, "u_model");
    ctx->u_normal_matrix = glGetUniformLocation(ctx->shader_program, "u_normal_matrix");
    ctx->u_light_dir = glGetUniformLocation(ctx->shader_program, "u_light_dir");
    ctx->u_base_color = glGetUniformLocation(ctx->shader_program, "u_base_color");
    ctx->u_view_pos = glGetUniformLocation(ctx->shader_program, "u_view_pos");

    /* Get attribute locations */
    ctx->a_position = glGetAttribLocation(ctx->shader_program, "a_position");
    ctx->a_normal = glGetAttribLocation(ctx->shader_program, "a_normal");
    ctx->a_texcoord = glGetAttribLocation(ctx->shader_program, "a_texcoord");

    LV_LOG_INFO("Shader program created: program=%u, a_position=%d, a_normal=%d",
                ctx->shader_program, ctx->a_position, ctx->a_normal);

    return true;
}

static void destroy_shader_program(lv_nanovg_3d_ctx_t * ctx)
{
    if(ctx->shader_program) {
        glDeleteProgram(ctx->shader_program);
        ctx->shader_program = 0;
    }
}

static bool create_fbo(lv_nanovg_3d_ctx_t * ctx, int32_t width, int32_t height)
{
    /* Create texture */
    glGenTextures(1, &ctx->fbo_texture);
    glBindTexture(GL_TEXTURE_2D, ctx->fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /* Create depth renderbuffer */
    glGenRenderbuffers(1, &ctx->fbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, ctx->fbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);

    /* Create framebuffer */
    glGenFramebuffers(1, &ctx->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, ctx->fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ctx->fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, ctx->fbo_depth);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if(status != GL_FRAMEBUFFER_COMPLETE) {
        LV_LOG_ERROR("Framebuffer incomplete: 0x%x", status);
        destroy_fbo(ctx);
        return false;
    }

    ctx->fbo_width = width;
    ctx->fbo_height = height;
    return true;
}

static void destroy_fbo(lv_nanovg_3d_ctx_t * ctx)
{
    if(ctx->fbo) {
        glDeleteFramebuffers(1, &ctx->fbo);
        ctx->fbo = 0;
    }
    if(ctx->fbo_texture) {
        glDeleteTextures(1, &ctx->fbo_texture);
        ctx->fbo_texture = 0;
    }
    if(ctx->fbo_depth) {
        glDeleteRenderbuffers(1, &ctx->fbo_depth);
        ctx->fbo_depth = 0;
    }
    ctx->fbo_width = 0;
    ctx->fbo_height = 0;
}

static void save_gl_state(lv_gl_state_t * state)
{
    glGetIntegerv(GL_VIEWPORT, state->viewport);
    glGetIntegerv(GL_SCISSOR_BOX, state->scissor_box);
    state->depth_test = glIsEnabled(GL_DEPTH_TEST);
    state->cull_face = glIsEnabled(GL_CULL_FACE);
    state->blend = glIsEnabled(GL_BLEND);
    state->scissor_test = glIsEnabled(GL_SCISSOR_TEST);
    glGetIntegerv(GL_BLEND_SRC_RGB, &state->blend_src_rgb);
    glGetIntegerv(GL_BLEND_DST_RGB, &state->blend_dst_rgb);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &state->blend_src_alpha);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &state->blend_dst_alpha);
    glGetIntegerv(GL_CURRENT_PROGRAM, &state->current_program);
    glGetIntegerv(GL_ACTIVE_TEXTURE, &state->active_texture);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &state->texture_2d);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &state->array_buffer);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &state->element_array_buffer);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &state->framebuffer);
}

static void restore_gl_state(const lv_gl_state_t * state)
{
    glViewport(state->viewport[0], state->viewport[1], state->viewport[2], state->viewport[3]);
    glScissor(state->scissor_box[0], state->scissor_box[1], state->scissor_box[2], state->scissor_box[3]);

    if(state->depth_test) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);

    if(state->cull_face) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);

    if(state->blend) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);

    if(state->scissor_test) glEnable(GL_SCISSOR_TEST);
    else glDisable(GL_SCISSOR_TEST);

    glBlendFuncSeparate(state->blend_src_rgb, state->blend_dst_rgb,
                        state->blend_src_alpha, state->blend_dst_alpha);
    glUseProgram(state->current_program);
    glActiveTexture(state->active_texture);
    glBindTexture(GL_TEXTURE_2D, state->texture_2d);
    glBindBuffer(GL_ARRAY_BUFFER, state->array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state->element_array_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, state->framebuffer);
}

/* Matrix math functions */
static void mat4_identity(float * m)
{
    lv_memzero(m, 16 * sizeof(float));
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

static void mat4_perspective(float * m, float fov, float aspect, float near, float far)
{
    float f = 1.0f / tanf(fov * 0.5f * 3.14159265358979f / 180.0f);
    lv_memzero(m, 16 * sizeof(float));
    m[0] = f / aspect;
    m[5] = f;
    m[10] = (far + near) / (near - far);
    m[11] = -1.0f;
    m[14] = (2.0f * far * near) / (near - far);
}

static void mat4_look_at(float * m, const float * eye, const float * center, const float * up)
{
    float f[3], s[3], u[3];

    vec3_sub(f, center, eye);
    vec3_normalize(f);

    vec3_cross(s, f, up);
    vec3_normalize(s);

    vec3_cross(u, s, f);

    mat4_identity(m);
    m[0] = s[0];
    m[4] = s[1];
    m[8] = s[2];
    m[1] = u[0];
    m[5] = u[1];
    m[9] = u[2];
    m[2] = -f[0];
    m[6] = -f[1];
    m[10] = -f[2];
    m[12] = -(s[0] * eye[0] + s[1] * eye[1] + s[2] * eye[2]);
    m[13] = -(u[0] * eye[0] + u[1] * eye[1] + u[2] * eye[2]);
    m[14] = f[0] * eye[0] + f[1] * eye[1] + f[2] * eye[2];
}

static void mat4_multiply(float * result, const float * a, const float * b)
{
    float temp[16];
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            temp[i * 4 + j] = 0;
            for(int k = 0; k < 4; k++) {
                temp[i * 4 + j] += a[k * 4 + j] * b[i * 4 + k];
            }
        }
    }
    lv_memcpy(result, temp, 16 * sizeof(float));
}

static void mat4_rotate_y(float * m, float angle)
{
    float rad = angle * 3.14159265358979f / 180.0f;
    float c = cosf(rad);
    float s = sinf(rad);

    mat4_identity(m);
    m[0] = c;
    m[2] = s;
    m[8] = -s;
    m[10] = c;
}

static void vec3_normalize(float * v)
{
    float len = sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    if(len > 0.0001f) {
        v[0] /= len;
        v[1] /= len;
        v[2] /= len;
    }
}

static void vec3_cross(float * result, const float * a, const float * b)
{
    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];
}

static void vec3_sub(float * result, const float * a, const float * b)
{
    result[0] = a[0] - b[0];
    result[1] = a[1] - b[1];
    result[2] = a[2] - b[2];
}

#if LV_USE_CGLTF
static bool upload_mesh_to_gpu(lv_nanovg_3d_ctx_t * ctx, lv_nanovg_3d_mesh_t * gpu_mesh,
                               const lv_cgltf_mesh_data_t * mesh_data,
                               const lv_cgltf_material_t * material)
{
    LV_UNUSED(ctx);

    lv_memzero(gpu_mesh, sizeof(lv_nanovg_3d_mesh_t));

    LV_LOG_INFO("Uploading mesh: vertices=%u, indices=%u",
                mesh_data->vertex_count, mesh_data->index_count);

    /* Upload positions */
    if(mesh_data->positions && mesh_data->vertex_count > 0) {
        glGenBuffers(1, &gpu_mesh->vbo_positions);
        glBindBuffer(GL_ARRAY_BUFFER, gpu_mesh->vbo_positions);
        glBufferData(GL_ARRAY_BUFFER, mesh_data->vertex_count * 3 * sizeof(float),
                     mesh_data->positions, GL_STATIC_DRAW);
        gpu_mesh->vertex_count = mesh_data->vertex_count;
        LV_LOG_INFO("  VBO positions: %u", gpu_mesh->vbo_positions);
    }

    /* Upload normals */
    if(mesh_data->normals) {
        glGenBuffers(1, &gpu_mesh->vbo_normals);
        glBindBuffer(GL_ARRAY_BUFFER, gpu_mesh->vbo_normals);
        glBufferData(GL_ARRAY_BUFFER, mesh_data->vertex_count * 3 * sizeof(float),
                     mesh_data->normals, GL_STATIC_DRAW);
        LV_LOG_INFO("  VBO normals: %u", gpu_mesh->vbo_normals);
    }

    /* Upload texcoords */
    if(mesh_data->texcoords) {
        glGenBuffers(1, &gpu_mesh->vbo_texcoords);
        glBindBuffer(GL_ARRAY_BUFFER, gpu_mesh->vbo_texcoords);
        glBufferData(GL_ARRAY_BUFFER, mesh_data->vertex_count * 2 * sizeof(float),
                     mesh_data->texcoords, GL_STATIC_DRAW);
    }

    /* Upload indices - convert to uint16 for GLES2 compatibility */
    if(mesh_data->indices && mesh_data->index_count > 0) {
#if LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GLES2 || LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GLES3
        /* GLES2 only supports GL_UNSIGNED_SHORT for indices */
        if(mesh_data->index_count > 65535) {
            LV_LOG_WARN("Index count %u exceeds GLES2 limit, truncating", mesh_data->index_count);
        }
        uint16_t * indices16 = lv_malloc(mesh_data->index_count * sizeof(uint16_t));
        if(indices16) {
            for(uint32_t i = 0; i < mesh_data->index_count; i++) {
                indices16[i] = (uint16_t)(mesh_data->indices[i] & 0xFFFF);
            }
            glGenBuffers(1, &gpu_mesh->ibo);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_mesh->ibo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_data->index_count * sizeof(uint16_t),
                         indices16, GL_STATIC_DRAW);
            lv_free(indices16);
            gpu_mesh->index_count = mesh_data->index_count;
            LV_LOG_INFO("  IBO (uint16): %u, count=%u", gpu_mesh->ibo, gpu_mesh->index_count);
        }
#else
        glGenBuffers(1, &gpu_mesh->ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gpu_mesh->ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_data->index_count * sizeof(uint32_t),
                     mesh_data->indices, GL_STATIC_DRAW);
        gpu_mesh->index_count = mesh_data->index_count;
        LV_LOG_INFO("  IBO (uint32): %u, count=%u", gpu_mesh->ibo, gpu_mesh->index_count);
#endif
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    /* Copy material */
    if(material) {
        gpu_mesh->base_color[0] = material->base_color[0];
        gpu_mesh->base_color[1] = material->base_color[1];
        gpu_mesh->base_color[2] = material->base_color[2];
        gpu_mesh->base_color[3] = material->base_color[3];
    }
    else {
        gpu_mesh->base_color[0] = gpu_mesh->base_color[1] = gpu_mesh->base_color[2] = 0.8f;
        gpu_mesh->base_color[3] = 1.0f;
    }

    return gpu_mesh->vertex_count > 0;
}
#endif /* LV_USE_CGLTF */

#endif /* LV_USE_DRAW_NANOVG && LV_USE_3DTEXTURE */
