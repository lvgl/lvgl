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
static void lv_opengles_render_internal(unsigned int texture, const lv_area_t * texture_area, lv_opa_t opa,
                                        int32_t disp_w, int32_t disp_h, const lv_area_t * texture_clip_area,
                                        bool h_flip, bool v_flip, lv_color_t fill_color);
static void lv_opengles_enable_blending(void);
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
static unsigned int lv_opengles_shader_compile(unsigned int type, const char * source);
static unsigned int lv_opengles_shader_create(const char * vertexShader, const char * fragmentShader);
static void lv_opengles_shader_init(void);
static void lv_opengles_shader_deinit(void);
static void lv_opengles_shader_bind(void);
static void lv_opengles_shader_unbind(void);
static int lv_opengles_shader_get_uniform_location(const char * name);
static void lv_opengles_shader_set_uniform1i(const char * name, int value);
static void lv_opengles_shader_set_uniformmatrix3fv(const char * name, int count, bool transpose, const float * values);
static void lv_opengles_shader_set_uniform1f(const char * name, float value);
static void lv_opengles_shader_set_uniform3f(const char * name, float value_0, float value_1, float value_2);
static void lv_opengles_render_draw(void);
static float lv_opengles_map_float(float x, float min_in, float max_in, float min_out, float max_out);

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static bool is_init;

static unsigned int vertex_buffer_id = 0;

static unsigned int vertex_array_id = 0;

static unsigned int index_buffer_id = 0;
static unsigned int index_buffer_count = 0;

static unsigned int shader_id;

static const char * shader_names[] = { "u_Texture", "u_ColorDepth", "u_VertexTransform", "u_Opa", "u_IsFill", "u_FillColor" };
static int shader_location[] = { 0, 0, 0, 0, 0, 0 };

static const char * vertex_shader =
    "#version 300 es\n"
    "\n"
    "precision mediump float;\n"
    "\n"
    "in vec4 position;\n"
    "in vec2 texCoord;\n"
    "\n"
    "out vec2 v_TexCoord;\n"
    "\n"
    "uniform mat3 u_VertexTransform;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = vec4((u_VertexTransform * vec3(position.xy, 1)).xy, position.zw);\n"
    "    v_TexCoord = texCoord;\n"
    "}\n";

static const char * fragment_shader =
    "#version 300 es\n"
    "\n"
    "precision lowp float;\n"
    "\n"
    "out vec4 color;\n"
    "\n"
    "in vec2 v_TexCoord;\n"
    "\n"
    "uniform sampler2D u_Texture;\n"
    "uniform float u_ColorDepth;\n"
    "uniform float u_Opa;\n"
    "uniform bool u_IsFill;\n"
    "uniform vec3 u_FillColor;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec4 texColor;\n"
    "    if (u_IsFill) {\n"
    "        texColor = vec4(u_FillColor, 1.0);\n"
    "    } else {\n"
    "        texColor = texture(u_Texture, v_TexCoord);\n"
    "    }\n"
    "    if (abs(u_ColorDepth - 8.0) < 0.1) {\n"
    "        float gray = texColor.r;\n"
    "        color = vec4(gray, gray, gray, u_Opa);\n"
    "    } else {\n"
    "        float combinedAlpha = texColor.a * u_Opa;\n"
    "        color = vec4(texColor.rgb * combinedAlpha, combinedAlpha);\n"
    "    }\n"
    "}\n";

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_opengles_init(void)
{
    if(is_init) return;

    lv_opengles_enable_blending();

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    lv_opengles_vertex_buffer_init(NULL, sizeof(float) * LV_OPENGLES_VERTEX_BUFFER_LEN);

    lv_opengles_vertex_array_init();
    lv_opengles_vertex_array_add_buffer();

    lv_opengles_index_buffer_init(indices, 6);

    lv_opengles_shader_init();
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
    lv_opengles_render_internal(texture, texture_area, opa, disp_w, disp_h, texture_clip_area, h_flip, v_flip,
                                lv_color_black());
    LV_PROFILER_DRAW_END;
}

void lv_opengles_render_fill(lv_color_t color, const lv_area_t * area, lv_opa_t opa, int32_t disp_w, int32_t disp_h)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_opengles_render_internal(0, area, opa, disp_w, disp_h, area, false, false, color);
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

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_opengles_render_internal(unsigned int texture, const lv_area_t * texture_area, lv_opa_t opa,
                                        int32_t disp_w, int32_t disp_h, const lv_area_t * texture_clip_area, bool h_flip, bool v_flip, lv_color_t fill_color)
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
    float matrix[9] = {
        hor_scale, 0.0f,      hor_translate,
        0.0f,      ver_scale, ver_translate,
        0.0f,      0.0f,      1.0f
    };

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

    lv_opengles_shader_bind();
    lv_opengles_shader_set_uniform1f("u_ColorDepth", LV_COLOR_DEPTH);
    lv_opengles_shader_set_uniform1i("u_Texture", 0);
    lv_opengles_shader_set_uniformmatrix3fv("u_VertexTransform", 1, true, matrix);
    lv_opengles_shader_set_uniform1f("u_Opa", (float)opa / (float)LV_OPA_100);
    lv_opengles_shader_set_uniform1i("u_IsFill", texture == 0);
    lv_opengles_shader_set_uniform3f("u_FillColor", (float)fill_color.red / 255.0f, (float)fill_color.green / 255.0f,
                                     (float)fill_color.blue / 255.0f);
    lv_opengles_render_draw();
    LV_PROFILER_DRAW_END;
}

static void lv_opengles_enable_blending(void)
{
    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA));
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

static unsigned int lv_opengles_shader_compile(unsigned int type, const char * source)
{
    unsigned int id;
    GL_CALL(id = glCreateShader(type));
    const char * src = source;
    GL_CALL(glShaderSource(id, 1, &src, NULL));
    GL_CALL(glCompileShader(id));

    int result;
    GL_CALL(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if(result == GL_FALSE) {
        int length;
        GL_CALL(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char * message = lv_malloc_zeroed(length * sizeof(char));
        GL_CALL(glGetShaderInfoLog(id, length, &length, message));
        LV_LOG_ERROR("Failed to compile %s shader!", type == GL_VERTEX_SHADER ? "vertex" : "fragment");
        LV_LOG_ERROR("%s", message);
        GL_CALL(glDeleteShader(id));
        return 0;
    }

    return id;
}

static unsigned int lv_opengles_shader_create(const char * vertexShader, const char * fragmentShader)
{
    unsigned int program;
    GL_CALL(program = glCreateProgram());
    unsigned int vs = lv_opengles_shader_compile(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = lv_opengles_shader_compile(GL_FRAGMENT_SHADER, fragmentShader);

    GL_CALL(glAttachShader(program, vs));
    GL_CALL(glAttachShader(program, fs));
    GL_CALL(glLinkProgram(program));
    GL_CALL(glValidateProgram(program));

    GL_CALL(glDeleteShader(vs));
    GL_CALL(glDeleteShader(fs));

    return program;
}

static void lv_opengles_shader_init(void)
{
    if(shader_id == 0) shader_id = lv_opengles_shader_create(vertex_shader, fragment_shader);
}

static void lv_opengles_shader_deinit(void)
{
    if(shader_id == 0) return;
    GL_CALL(glDeleteProgram(shader_id));
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
    if(id == -1) {
        return -1;
    }

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

static void lv_opengles_shader_set_uniformmatrix3fv(const char * name, int count, bool transpose, const float * values)
{
    LV_PROFILER_DRAW_BEGIN;
    GL_CALL(glUniformMatrix3fv(lv_opengles_shader_get_uniform_location(name), count, transpose, values));
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

#endif /* LV_USE_OPENGLES */
