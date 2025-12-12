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
#include "lv_opengles_debug.h"
#include "lv_opengles_private.h"

#include "../../display/lv_display.h"
#include "../../misc/lv_area_private.h"
#include "opengl_shader/lv_opengl_shader_internal.h"
#include "assets/lv_opengles_shader.h"

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
static void lv_opengles_vertex_buffer_init(const void * data, unsigned int size);
static void lv_opengles_vertex_buffer_deinit(void);
static void lv_opengles_vertex_buffer_bind(void);
static void lv_opengles_vertex_buffer_unbind(void);
static void lv_opengles_vertex_array_init(void);
static void lv_opengles_vertex_array_deinit(void);
static void lv_opengles_vertex_array_bind(void);
static void lv_opengles_vertex_array_unbind(void);
static void lv_opengles_vertex_array_add_buffer(void);
static void lv_opengles_index_buffer_init(const unsigned int * data, unsigned int count);
static void lv_opengles_index_buffer_deinit(void);
static unsigned int lv_opengles_index_buffer_get_count(void);
static void lv_opengles_index_buffer_bind(void);
static void lv_opengles_index_buffer_unbind(void);
static unsigned int lv_opengles_shader_manager_init(void);
static lv_result_t lv_opengles_shader_init(void);
static void lv_opengles_shader_deinit(void);
static void lv_opengles_shader_bind(void);
static void lv_opengles_shader_unbind(void);
static int lv_opengles_shader_get_uniform_location(const char * name);
static void lv_opengles_shader_set_uniform1i(const char * name, int value);
static void lv_opengles_shader_set_uniformmatrix3fv(const char * name, int count, const float * values);
static void lv_opengles_shader_set_uniform1f(const char * name, float value);
static void lv_opengles_shader_set_uniform3f(const char * name, float value_0, float value_1, float value_2);
static void lv_opengles_render_draw(void);
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
static bool is_init;

static lv_opengl_shader_manager_t shader_manager;

static unsigned int vertex_buffer_id = 0;

static unsigned int vertex_array_id = 0;

static unsigned int index_buffer_id = 0;
static unsigned int index_buffer_count = 0;

static unsigned int shader_id;

static const char * shader_names[] = { "u_Texture", "u_ColorDepth", "u_VertexTransform", "u_Opa", "u_IsFill", "u_FillColor", "u_SwapRB", "u_Hue", "u_Saturation", "u_Value" };
static int shader_location[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_opengles_init(void)
{
    if(is_init) return;

    lv_opengles_enable_blending(false);

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    lv_opengles_vertex_buffer_init(NULL, sizeof(float) * LV_OPENGLES_VERTEX_BUFFER_LEN);

    lv_opengles_vertex_array_init();
    lv_opengles_vertex_array_add_buffer();

    lv_opengles_index_buffer_init(indices, 6);

    lv_result_t res = lv_opengles_shader_init();
    LV_ASSERT_MSG(res == LV_RESULT_OK, "Failed to initialize shaders");

    lv_opengles_shader_bind();

    /* unbind everything */
    lv_opengles_vertex_array_unbind();
    lv_opengles_vertex_buffer_unbind();
    lv_opengles_index_buffer_unbind();
    lv_opengles_shader_unbind();

    is_init = true;
}

void lv_opengles_deinit(void)
{
    if(!is_init) return;

    lv_opengles_shader_deinit();
    lv_opengles_index_buffer_deinit();
    lv_opengles_vertex_buffer_deinit();
    lv_opengles_vertex_array_deinit();

    is_init = false;
}

void lv_opengles_render_texture(unsigned int texture, const lv_area_t * texture_area, lv_opa_t opa, int32_t disp_w,
                                int32_t disp_h, const lv_area_t * texture_clip_area, bool h_flip, bool v_flip)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_opengles_render(texture, texture_area, opa, disp_w, disp_h, texture_clip_area, h_flip, v_flip,
                       lv_color_black(), false, false);
    LV_PROFILER_DRAW_END;
}

void lv_opengles_render_texture_rbswap(unsigned int texture, const lv_area_t * texture_area, lv_opa_t opa,
                                       int32_t disp_w,
                                       int32_t disp_h, const lv_area_t * texture_clip_area, bool h_flip, bool v_flip)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_opengles_render(texture, texture_area, opa, disp_w, disp_h, texture_clip_area, h_flip, v_flip,
                       lv_color_black(), false, true);
    LV_PROFILER_DRAW_END;
}

void lv_opengles_render_fill(lv_color_t color, const lv_area_t * area, lv_opa_t opa, int32_t disp_w, int32_t disp_h)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_opengles_render(0, area, opa, disp_w, disp_h, area, false, false, color, false, true);
    LV_PROFILER_DRAW_END;
}
void lv_opengles_render_display_texture(lv_display_t * display, bool h_flip, bool v_flip)
{
    LV_PROFILER_DRAW_BEGIN;
    unsigned int texture = *(unsigned int *)lv_display_get_driver_data(display);
    GL_CALL(glActiveTexture(GL_TEXTURE0));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));

    lv_display_rotation_t rotation = lv_display_get_rotation(display);

    float vert_buffer[LV_OPENGLES_VERTEX_BUFFER_LEN];
    populate_vertex_buffer(vert_buffer, rotation, &h_flip, &v_flip, 0.f, 0.f, 1.f, 1.f);
    lv_opengles_vertex_buffer_init(vert_buffer, sizeof(vert_buffer));

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

    lv_opengles_shader_bind();
    lv_opengles_shader_set_uniform1f("u_ColorDepth", LV_COLOR_DEPTH);
    lv_opengles_shader_set_uniform1i("u_Texture", 0);
    lv_opengles_shader_set_uniformmatrix3fv("u_VertexTransform", 1, transposed_matrix);
    lv_opengles_shader_set_uniform1f("u_Opa", 1);
    lv_opengles_shader_set_uniform1i("u_IsFill", 0);
    lv_opengles_shader_set_uniform3f("u_FillColor", 1.0f, 1.0f, 1.0f);
    lv_opengles_shader_set_uniform1i("u_SwapRB", 1);

    lv_opengles_render_draw();
    LV_PROFILER_DRAW_END;
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

void lv_opengles_render(unsigned int texture, const lv_area_t * texture_area, lv_opa_t opa,
                        int32_t disp_w, int32_t disp_h, const lv_area_t * texture_clip_area,
                        bool h_flip, bool v_flip, lv_color_t fill_color, bool blend_opt, bool swap_red_blue)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_area_t intersection;
    if(!lv_area_intersect(&intersection, texture_area, texture_clip_area)) {
        LV_PROFILER_DRAW_END;
        return;
    }

    GL_CALL(glActiveTexture(GL_TEXTURE0));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));

    float tex_w = (float)lv_area_get_width(&intersection);
    float tex_h = (float)lv_area_get_height(&intersection);

    float hor_scale = tex_w / (float)disp_w;
    float ver_scale = tex_h / (float)disp_h;
    float hor_translate = (float)intersection.x1 / (float)disp_w * 2.0f - (1.0f - hor_scale);
    float ver_translate = -((float)intersection.y1 / (float)disp_h * 2.0f - (1.0f - ver_scale));
    hor_scale = h_flip ? -hor_scale : hor_scale;
    ver_scale = v_flip ? ver_scale : -ver_scale;

    if(texture != 0) {
        float clip_x1 = h_flip ? lv_opengles_map_float(texture_clip_area->x2, texture_area->x2, texture_area->x1, 0.f, 1.f)
                        : lv_opengles_map_float(texture_clip_area->x1, texture_area->x1, texture_area->x2, 0.f, 1.f);
        float clip_x2 = h_flip ? lv_opengles_map_float(texture_clip_area->x1, texture_area->x2, texture_area->x1, 0.f, 1.f)
                        : lv_opengles_map_float(texture_clip_area->x2, texture_area->x1, texture_area->x2, 0.f, 1.f);
        float clip_y1 = v_flip ? lv_opengles_map_float(texture_clip_area->y2, texture_area->y2, texture_area->y1, 0.f, 1.f)
                        : lv_opengles_map_float(texture_clip_area->y1, texture_area->y1, texture_area->y2, 0.f, 1.f);
        float clip_y2 = v_flip ? lv_opengles_map_float(texture_clip_area->y1, texture_area->y2, texture_area->y1, 0.f, 1.f)
                        : lv_opengles_map_float(texture_clip_area->y2, texture_area->y1, texture_area->y2, 0.f, 1.f);

        float positions[LV_OPENGLES_VERTEX_BUFFER_LEN] = {
            -1.f,  1.0f, clip_x1, clip_y2,
            1.0f,  1.0f, clip_x2, clip_y2,
            1.0f, -1.0f, clip_x2, clip_y1,
            -1.f, -1.0f, clip_x1, clip_y1
        };
        lv_opengles_vertex_buffer_init(positions, sizeof(positions));
    }

    const float transposed_matrix[9] = {
        hor_scale,  0.0f,        0.0f,
        0.0f,       ver_scale,   0.0f,
        hor_translate, ver_translate, 1.0f
    };


    lv_opengles_shader_bind();
    lv_opengles_enable_blending(blend_opt);
    lv_opengles_shader_set_uniform1f("u_ColorDepth", LV_COLOR_DEPTH);
    lv_opengles_shader_set_uniform1i("u_Texture", 0);
    lv_opengles_shader_set_uniformmatrix3fv("u_VertexTransform", 1, transposed_matrix);
    lv_opengles_shader_set_uniform1f("u_Opa", (float)opa / (float)LV_OPA_100);
    lv_opengles_shader_set_uniform1i("u_IsFill", texture == 0);
    lv_opengles_shader_set_uniform3f("u_FillColor", (float)fill_color.red / 255.0f, (float)fill_color.green / 255.0f,
                                     (float)fill_color.blue / 255.0f);
    lv_opengles_shader_set_uniform1i("u_SwapRB", swap_red_blue ? 1 : 0);

    lv_opengles_render_draw();
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

static void lv_opengles_vertex_buffer_init(const void * data, unsigned int size)
{
    if(vertex_buffer_id == 0) GL_CALL(glGenBuffers(1, &vertex_buffer_id));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW));
}

static void lv_opengles_vertex_buffer_deinit(void)
{
    if(vertex_buffer_id == 0) return;
    GL_CALL(glDeleteBuffers(1, &vertex_buffer_id));
    vertex_buffer_id = 0;
}

static void lv_opengles_vertex_buffer_bind(void)
{
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id));
}

static void lv_opengles_vertex_buffer_unbind(void)
{
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

static void lv_opengles_vertex_array_init(void)
{
    if(vertex_array_id == 0) GL_CALL(glGenVertexArrays(1, &vertex_array_id));
}

static void lv_opengles_vertex_array_deinit(void)
{
    if(vertex_array_id == 0) return;
    GL_CALL(glDeleteVertexArrays(1, &vertex_array_id));
    vertex_array_id = 0;
}

static void lv_opengles_vertex_array_bind(void)
{
    GL_CALL(glBindVertexArray(vertex_array_id));
}

static void lv_opengles_vertex_array_unbind(void)
{
    GL_CALL(glBindVertexArray(0));
}

static void lv_opengles_vertex_array_add_buffer(void)
{
    lv_opengles_vertex_buffer_bind();
    intptr_t offset = 0;

    for(unsigned int i = 0; i < 2; i++) {
        lv_opengles_vertex_array_bind();
        GL_CALL(glEnableVertexAttribArray(i));
        GL_CALL(glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, 16, (const void *)offset));
        offset += 2 * 4;
    }
}

static void lv_opengles_index_buffer_init(const unsigned int * data, unsigned int count)
{
    index_buffer_count = count;
    if(index_buffer_id == 0) GL_CALL(glGenBuffers(1, &index_buffer_id));

    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id));

    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, GL_STATIC_DRAW));
}

static void lv_opengles_index_buffer_deinit(void)
{
    if(index_buffer_id == 0) return;
    GL_CALL(glDeleteBuffers(1, &index_buffer_id));
    index_buffer_id = 0;
}

static unsigned int lv_opengles_index_buffer_get_count(void)
{
    return index_buffer_count;
}

static void lv_opengles_index_buffer_bind(void)
{
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id));
}

static void lv_opengles_index_buffer_unbind(void)
{
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

static unsigned int lv_opengles_shader_manager_init(void)
{
    lv_opengl_shader_program_t * program  = NULL;
    for(lv_opengl_glsl_version version = LV_OPENGL_GLSL_VERSION_300ES; version < LV_OPENGL_GLSL_VERSION_LAST; ++version) {
        LV_LOG_INFO("Trying GLSL version %s", lv_opengles_glsl_version_to_string(version));
        lv_opengl_shader_portions_t portions;
        lv_opengles_shader_get_source(&portions, version);
        char * vertex_shader = lv_opengles_shader_get_vertex(version);
        char * frag_shader = lv_opengles_shader_get_fragment(version);
        lv_opengl_shader_manager_init(&shader_manager, portions.all, portions.count, vertex_shader, frag_shader);
        lv_free(vertex_shader);
        lv_free(frag_shader);

        uint32_t frag_shader_hash;
        uint32_t vert_shader_hash;

        lv_result_t res = lv_opengl_shader_manager_select_shader(&shader_manager, "__MAIN__.frag", NULL, 0, version,
                                                                 &frag_shader_hash);
        if(res != LV_RESULT_OK) {
            lv_opengl_shader_manager_deinit(&shader_manager);
            continue;
        }
        res = lv_opengl_shader_manager_select_shader(&shader_manager, "__MAIN__.vert", NULL, 0, version, &vert_shader_hash);
        if(res != LV_RESULT_OK) {
            lv_opengl_shader_manager_deinit(&shader_manager);
            continue;
        }
        program = lv_opengl_shader_manager_get_program(&shader_manager, frag_shader_hash, vert_shader_hash);
        if(!program) {
            lv_opengl_shader_manager_deinit(&shader_manager);
            continue;
        }
        LV_LOG_INFO("Compiled shaders with version %s", lv_opengles_glsl_version_to_string(version));
        break;
    }

    if(!program) {
        LV_LOG_ERROR("Failed to initialize shaders");
        return 0;
    }
    return lv_opengl_shader_program_get_id(program);
}

static lv_result_t lv_opengles_shader_init(void)
{
    if(shader_id != 0) {
        return LV_RESULT_OK;
    }
    shader_id = lv_opengles_shader_manager_init();
    return shader_id != 0 ? LV_RESULT_OK : LV_RESULT_INVALID;
}

static void lv_opengles_shader_deinit(void)
{
    if(shader_id == 0) return;
    /* The program is part of the manager and as such will be destroyed inside */
    lv_opengl_shader_manager_deinit(&shader_manager);
    shader_id = 0;
}

static void lv_opengles_shader_bind(void)
{
    GL_CALL(glUseProgram(shader_id));
}

static void lv_opengles_shader_unbind(void)
{
    GL_CALL(glUseProgram(0));
}

static int lv_opengles_shader_get_uniform_location(const char * name)
{
    int id = -1;
    for(size_t i = 0; i < sizeof(shader_location) / sizeof(int); i++) {
        if(lv_strcmp(shader_names[i], name) == 0) {
            id = i;
        }
    }

    LV_ASSERT_FORMAT_MSG(id > -1, "Uniform location doesn't exist for '%s'. Check `shader_location` array", name);

    if(shader_location[id] != 0) {
        return shader_location[id];
    }

    int location;
    GL_CALL(location = glGetUniformLocation(shader_id, name));
    if(location == -1)
        LV_LOG_WARN("Warning: uniform '%s' doesn't exist!", name);

    shader_location[id] = location;
    return location;
}

static void lv_opengles_shader_set_uniform1i(const char * name, int value)
{
    LV_PROFILER_DRAW_BEGIN;
    GL_CALL(glUniform1i(lv_opengles_shader_get_uniform_location(name), value));
    LV_PROFILER_DRAW_END;
}

static void lv_opengles_shader_set_uniformmatrix3fv(const char * name, int count, const float * values)
{
    LV_PROFILER_DRAW_BEGIN;
    /*
     * GLES2.0 doesn't support transposing the matrix via glUniformMatrix3fv so this is the transposed matrix
     * https://registry.khronos.org/OpenGL/specs/es/2.0/es_full_spec_2.0.pdf page 47
     */
    GL_CALL(glUniformMatrix3fv(lv_opengles_shader_get_uniform_location(name), count, GL_FALSE, values));
    LV_PROFILER_DRAW_END;
}

static void lv_opengles_shader_set_uniform1f(const char * name, float value)
{
    LV_PROFILER_DRAW_BEGIN;
    GL_CALL(glUniform1f(lv_opengles_shader_get_uniform_location(name), value));
    LV_PROFILER_DRAW_END;
}

static void lv_opengles_shader_set_uniform3f(const char * name, float value_0, float value_1, float value_2)
{
    LV_PROFILER_DRAW_BEGIN;
    GL_CALL(glUniform3f(lv_opengles_shader_get_uniform_location(name), value_0, value_1, value_2));
    LV_PROFILER_DRAW_END;
}

static void lv_opengles_render_draw(void)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_opengles_shader_bind();
    lv_opengles_vertex_array_bind();
    lv_opengles_index_buffer_bind();
    unsigned int count = lv_opengles_index_buffer_get_count();
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
