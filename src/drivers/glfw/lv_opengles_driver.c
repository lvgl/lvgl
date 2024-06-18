/**
 * @file lv_opengles_driver.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../display/lv_display.h"

#if LV_USE_OPENGLES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
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
static void lv_opengles_render_clear(void);
static unsigned int lv_opengles_shader_compile(unsigned int type, const char * source);
static unsigned int lv_opengles_shader_create(const char * vertexShader, const char * fragmentShader);
static void lv_opengles_shader_init(void);
static void lv_opengles_shader_deinit(void);
static void lv_opengles_shader_bind(void);
static void lv_opengles_shader_unbind(void);
static int lv_opengles_shader_get_uniform_location(const char * name);
static void lv_opengles_shader_set_uniform1i(const char * name, int value);
static void lv_opengles_shader_set_uniform4f(const char * name, float v0, float v1, float v2, float v3);
static void lv_opengles_render_draw(void);
static void lv_opengles_texture_init(void * buffer, int width, int height);
static void lv_opengles_texture_deinit(void);
static void lv_opengles_texture_bind(unsigned int slot);
static void lv_opengles_texture_update(void * buffer, int width, int height);

/***********************
 *   GLOBAL PROTOTYPES
 ***********************/

/**********************
 *  STATIC VARIABLES
 **********************/
static unsigned int vertex_buffer_id = 0;

static unsigned int vertex_array_id = 0;

static unsigned int index_buffer_id = 0;
static unsigned int index_buffer_count = 0;

static unsigned int texture_id = 0;

static unsigned int shader_id;

static const char * shader_names[] = { "u_Color", "u_Texture", "u_ColorDepth" };
static int shader_location[] = { 0, 0, 0 };

static const char * vertex_shader =
    "#version 300 es\n"
    "\n"
    "in vec4 position;\n"
    "in vec2 texCoord;\n"
    "\n"
    "out vec2 v_TexCoord;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = position;\n"
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
    "uniform vec4 u_Color;\n"
    "uniform sampler2D u_Texture;\n"
    "uniform int u_ColorDepth;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    vec4 texColor = texture(u_Texture, v_TexCoord);\n"
    "    if (u_ColorDepth == 8) {\n"
    "        float gray = texColor.r;\n"
    "        color = vec4(gray, gray, gray, 1.0);\n"
    "    } else {\n"
    "        color = texColor;\n"
    "    }\n"
    "};\n";

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_opengles_init(uint8_t * frame_buffer, int32_t hor, int32_t ver)
{
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

    lv_opengles_vertex_buffer_init(positions, 4 * 4 * sizeof(float));

    lv_opengles_vertex_array_init();
    lv_opengles_vertex_array_add_buffer();

    lv_opengles_index_buffer_init(indices, 6);

    lv_opengles_shader_init();
    lv_opengles_shader_bind();
    lv_opengles_shader_set_uniform1i("u_ColorDepth", LV_COLOR_DEPTH);
    lv_opengles_shader_set_uniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

    int slot = 0;
    lv_opengles_texture_init(frame_buffer, hor, ver);
    lv_opengles_texture_bind(slot);

    lv_opengles_shader_set_uniform1i("u_Texture", slot);

    /* unbound everything */
    lv_opengles_vertex_array_unbind();
    lv_opengles_vertex_buffer_unbind();
    lv_opengles_index_buffer_unbind();
    lv_opengles_shader_unbind();
}

void lv_opengles_deinit(void)
{
    lv_opengles_texture_deinit();
    lv_opengles_shader_deinit();
    lv_opengles_index_buffer_deinit();
    lv_opengles_vertex_buffer_deinit();
    lv_opengles_vertex_array_deinit();
}

void lv_opengles_update(uint8_t * frame_buffer, int32_t hor, int32_t ver)
{
    lv_opengles_render_clear();
    lv_opengles_texture_update(frame_buffer, hor, ver);

    lv_opengles_shader_bind();
    lv_opengles_shader_set_uniform1i("u_ColorDepth", LV_COLOR_DEPTH);
    lv_opengles_shader_set_uniform4f("u_Color", 0.0f, 0.3f, 0.8f, 1.0f);
    lv_opengles_render_draw();
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_opengles_vertex_buffer_init(const void * data, unsigned int size)
{
    GL_CALL(glGenBuffers(1, &vertex_buffer_id));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

static void lv_opengles_vertex_buffer_deinit()
{
    GL_CALL(glDeleteBuffers(1, &vertex_buffer_id));
}

static void lv_opengles_vertex_buffer_bind()
{
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id));
}

static void lv_opengles_vertex_buffer_unbind()
{
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

static void lv_opengles_vertex_array_init()
{
    GL_CALL(glGenVertexArrays(1, &vertex_array_id));
}

static void lv_opengles_vertex_array_deinit()
{
    GL_CALL(glDeleteVertexArrays(1, &vertex_array_id));
}

static void lv_opengles_vertex_array_bind()
{
    GL_CALL(glBindVertexArray(vertex_array_id));
}

static void lv_opengles_vertex_array_unbind()
{
    GL_CALL(glBindVertexArray(0));
}

static void lv_opengles_vertex_array_add_buffer()
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

static void lv_opengles_index_buffer_deinit()
{
    GL_CALL(glDeleteBuffers(1, &index_buffer_id));
}

static unsigned int lv_opengles_index_buffer_get_count()
{
    return index_buffer_count;
}

static void lv_opengles_index_buffer_bind()
{
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_id));
}

static void lv_opengles_index_buffer_unbind()
{
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

static void lv_opengles_render_clear()
{
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
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
        LV_LOG_ERROR("Failed to compile %s shader!\n", type == GL_VERTEX_SHADER ? "vertex" : "fragment");
        LV_LOG_ERROR("%s\n", message);
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

static void lv_opengles_shader_init()
{
    shader_id = lv_opengles_shader_create(vertex_shader, fragment_shader);
}

static void lv_opengles_shader_deinit()
{
    GL_CALL(glDeleteProgram(shader_id));
}

static void lv_opengles_shader_bind()
{
    GL_CALL(glUseProgram(shader_id));
}

static void lv_opengles_shader_unbind()
{
    GL_CALL(glUseProgram(0));
}

static int lv_opengles_shader_get_uniform_location(const char * name)
{
    int id = -1;
    for(size_t i = 0; i < sizeof(shader_location) / sizeof(int); i++) {
        if(strcmp(shader_names[i], name) == 0) {
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
        LV_LOG_WARN("Warning: uniform '%s' doesn't exist!\n", name);

    shader_location[id] = location;
    return location;
}

static void lv_opengles_shader_set_uniform1i(const char * name, int value)
{
    GL_CALL(glUniform1i(lv_opengles_shader_get_uniform_location(name), value));
}

static void lv_opengles_shader_set_uniform4f(const char * name, float v0, float v1, float v2, float v3)
{
    GL_CALL(glUniform4f(lv_opengles_shader_get_uniform_location(name), v0, v1, v2, v3));
}

static void lv_opengles_render_draw()
{
    lv_opengles_shader_bind();
    lv_opengles_vertex_array_bind();
    lv_opengles_index_buffer_bind();
    unsigned int count = lv_opengles_index_buffer_get_count();
    GL_CALL(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, NULL));
}

static void lv_opengles_texture_init(void * buffer, int width, int height)
{
    if(buffer == NULL) {
        return;
    }

    GL_CALL(glGenTextures(1, &texture_id));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture_id));

    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    /*Color depth: 8 (A8), 16 (RGB565), 24 (RGB888), 32 (XRGB8888)*/
#if LV_COLOR_DEPTH == 8
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, buffer));
#elif LV_COLOR_DEPTH == 16
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, buffer));
#elif LV_COLOR_DEPTH == 24
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, buffer));
#elif LV_COLOR_DEPTH == 32
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, buffer));
#else
#error("Unsupported color format")
#endif

    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

static void lv_opengles_texture_deinit()
{
    GL_CALL(glDeleteTextures(1, &texture_id));
    texture_id = 0;
}

static void lv_opengles_texture_bind(unsigned int slot)
{
    GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture_id));
}

static void lv_opengles_texture_update(void * buffer, int width, int height)
{
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture_id));

    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    /*Color depth: 8 (A8), 16 (RGB565), 24 (RGB888), 32 (XRGB8888)*/
#if LV_COLOR_DEPTH == 8
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, buffer));
#elif LV_COLOR_DEPTH == 16
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, buffer));
#elif LV_COLOR_DEPTH == 24
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, buffer));
#elif LV_COLOR_DEPTH == 32
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, buffer));
#else
#error("Unsupported color format")
#endif
}

#endif /* LV_USE_OPENGLES */
