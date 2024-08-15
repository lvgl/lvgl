/**
 * @file lv_opengles_driver.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../display/lv_display.h"

#if LV_USE_OPENGLES

#include "lv_opengles_debug.h"
#include "lv_opengles_driver.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
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
static void lv_opengles_render_draw(void);

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

static const char * shader_names[] = { "u_Texture", "u_ColorDepth", "u_VertexTransform", "u_Opa" };
static int shader_location[] = { 0, 0, 0, 0 };

static const char * vertex_shader =
    "#version 300 es\n"
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
    "};\n";

static const char * fragment_shader =
    "#version 300 es\n"
    "\n"
    "precision mediump float;\n"
    "\n"
    "layout(location = 0) out vec4 color;\n"
    "\n"
    "in vec2 v_TexCoord;\n"
    "\n"
    "uniform sampler2D u_Texture;\n"
    "uniform int u_ColorDepth;\n"
    "uniform float u_Opa;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec4 texColor = texture(u_Texture, v_TexCoord);\n"
    "    if (u_ColorDepth == 8) {\n"
    "        float gray = texColor.r;\n"
    "        color = vec4(gray, gray, gray, u_Opa);\n"
    "    } else {\n"
    "        color = vec4(texColor.rgb, texColor.a * u_Opa);\n"
    "    }\n"
    "};\n";

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

    float positions[] = {
        -1.0f,  1.0f,  0.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 1.0f
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    lv_opengles_vertex_buffer_init(positions, sizeof(positions));

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
                                int32_t disp_h)
{
    GL_CALL(glActiveTexture(GL_TEXTURE0));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));

    float hor_scale = (float)lv_area_get_width(texture_area) / (float)disp_w;
    float ver_scale = (float)lv_area_get_height(texture_area) / (float)disp_h;
    float hor_translate = (float)texture_area->x1 / (float)disp_w * 2.0f - (1.0f - hor_scale);
    float ver_translate = -((float)texture_area->y1 / (float)disp_h * 2.0f - (1.0f - ver_scale));
    float matrix[9] = {
        hor_scale, 0.0f,      hor_translate,
        0.0f,      ver_scale, ver_translate,
        0.0f,      0.0f,      1.0f
    };

    lv_opengles_shader_bind();
    lv_opengles_shader_set_uniform1i("u_ColorDepth", LV_COLOR_DEPTH);
    lv_opengles_shader_set_uniform1i("u_Texture", 0);
    lv_opengles_shader_set_uniformmatrix3fv("u_VertexTransform", 1, true, matrix);
    lv_opengles_shader_set_uniform1f("u_Opa", (float)opa / (float)LV_OPA_100);
    lv_opengles_render_draw();
}

void lv_opengles_render_clear(void)
{
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
}

void lv_opengles_viewport(int32_t x, int32_t y, int32_t w, int32_t h)
{
    glViewport(x, y, w, h);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_opengles_enable_blending(void)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void lv_opengles_vertex_buffer_init(const void * data, unsigned int size)
{
    GL_CALL(glGenBuffers(1, &vertex_buffer_id));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

static void lv_opengles_vertex_buffer_deinit(void)
{
    GL_CALL(glDeleteBuffers(1, &vertex_buffer_id));
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
    GL_CALL(glGenVertexArrays(1, &vertex_array_id));
}

static void lv_opengles_vertex_array_deinit(void)
{
    GL_CALL(glDeleteVertexArrays(1, &vertex_array_id));
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
    GL_CALL(glGenBuffers(1, &index_buffer_id));

    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id));

    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, GL_STATIC_DRAW));
}

static void lv_opengles_index_buffer_deinit(void)
{
    GL_CALL(glDeleteBuffers(1, &index_buffer_id));
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
    GL_CALL(unsigned int id = glCreateShader(type));
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
    GL_CALL(unsigned int program = glCreateProgram());
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
    shader_id = lv_opengles_shader_create(vertex_shader, fragment_shader);
}

static void lv_opengles_shader_deinit(void)
{
    GL_CALL(glDeleteProgram(shader_id));
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

    GL_CALL(int location = glGetUniformLocation(shader_id, name));
    if(location == -1)
        LV_LOG_WARN("Warning: uniform '%s' doesn't exist!", name);

    shader_location[id] = location;
    return location;
}

static void lv_opengles_shader_set_uniform1i(const char * name, int value)
{
    GL_CALL(glUniform1i(lv_opengles_shader_get_uniform_location(name), value));
}

static void lv_opengles_shader_set_uniformmatrix3fv(const char * name, int count, bool transpose, const float * values)
{
    GL_CALL(glUniformMatrix3fv(lv_opengles_shader_get_uniform_location(name), count, transpose, values));
}

static void lv_opengles_shader_set_uniform1f(const char * name, float value)
{
    GL_CALL(glUniform1f(lv_opengles_shader_get_uniform_location(name), value));
}

static void lv_opengles_render_draw(void)
{
    lv_opengles_shader_bind();
    lv_opengles_vertex_array_bind();
    lv_opengles_index_buffer_bind();
    unsigned int count = lv_opengles_index_buffer_get_count();
    GL_CALL(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, NULL));
}

#endif /* LV_USE_OPENGLES */
