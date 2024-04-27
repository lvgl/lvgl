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
#include "lv_opengles_common.h"

void GLClearError()
{
    while(glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char * function, const char * file, int line)
{
    GLenum error;
    while((error = glGetError()) != GL_NO_ERROR) {
        LV_LOG_ERROR("[OpenGL Error] (%d) %s %s:%d\n", error, function, file, line);
        return false;
    }
    return true;
}

static unsigned int m_vertex_buffer_id = 0;

void lv_opengles_vertex_buffer_init(const void * data, unsigned int size)
{
    GLCall(glGenBuffers(1, &m_vertex_buffer_id));//create an ID :buffer
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_id));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

void lv_opengles_vertex_buffer_deinit()
{
    GLCall(glDeleteBuffers(1, &m_vertex_buffer_id));
}

void lv_opengles_vertex_buffer_bind()
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer_id));
}

void lv_opengles_vertex_buffer_unbind()
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

static unsigned int m_vertex_array_id = 0;

void lv_opengles_vertex_array_init()
{
    GLCall(glGenVertexArrays(1, &m_vertex_array_id));
}

void lv_opengles_vertex_array_deinit()
{
    GLCall(glDeleteVertexArrays(1, &m_vertex_array_id));
}

void lv_opengles_vertex_array_bind()
{
    GLCall(glBindVertexArray(m_vertex_array_id));
}

void lv_opengles_vertex_array_unbind()
{
    GLCall(glBindVertexArray(0));
}

void lv_opengles_vertex_array_add_buffer()
{
    lv_opengles_vertex_buffer_bind();
    intptr_t offset = 0;

    for(unsigned int i = 0; i < 2; i++) {
        lv_opengles_vertex_array_bind();
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(i, 2, GL_FLOAT, GL_FALSE, 16, (const void *)offset));
        offset += 2 * 4;
    }
}

static unsigned int m_index_buffer_id = 0;
static unsigned int m_index_buffer_count = 0;

void lv_opengles_index_buffer_init(const unsigned int * data, unsigned int count)
{
    m_index_buffer_count = count;
    GLCall(glGenBuffers(1, &m_index_buffer_id));//create an ID :buffer

    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer_id));

    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, GL_STATIC_DRAW));
}

void lv_opengles_index_buffer_deinit()
{
    GLCall(glDeleteBuffers(1, &m_index_buffer_id));
}

unsigned int lv_opengles_index_buffer_get_count()
{
    return m_index_buffer_count;
}

void lv_opengles_index_buffer_bind()
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer_id));
}

void lv_opengles_index_buffer_unbind()
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void lv_opengles_render_clear()
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

const char * m_vertex_shader =
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

const char * m_fragment_shader =
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
    "\n"
    "void main()\n"
    "{\n"
    "    vec4 texColor = texture(u_Texture, v_TexCoord);\n"
    "    color = texColor;\n"
    "};\n";

static unsigned int m_shader_id;

unsigned int lv_opengles_shader_compile(unsigned int type, const char * source)
{
    GLCall(unsigned int id = glCreateShader(type));
    const char * src = source;
    GLCall(glShaderSource(id, 1, &src, NULL));
    GLCall(glCompileShader(id));

    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if(result == GL_FALSE) {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char * message = (char *)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        LV_LOG_ERROR("Failed to compile %s shader!\n", type == GL_VERTEX_SHADER ? "vertex" : "fragment");
        LV_LOG_ERROR("%s\n", message);
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

unsigned int lv_opengles_shader_create(const char * vertexShader, const char * fragmentShader)
{
    GLCall(unsigned int program = glCreateProgram());
    unsigned int vs = lv_opengles_shader_compile(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = lv_opengles_shader_compile(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

void lv_opengles_shader_init()
{
    m_shader_id = lv_opengles_shader_create(m_vertex_shader, m_fragment_shader);
}

void lv_opengles_shader_deinit()
{
    GLCall(glDeleteProgram(m_shader_id));
}

void lv_opengles_shader_bind()
{
    GLCall(glUseProgram(m_shader_id));
}

void lv_opengles_shader_unbind()
{
    GLCall(glUseProgram(0));
}

static const char * m_shader_names[] = { "u_Color", "u_Texture" };
static int m_shader_location[2] = { 0, 0 };

int lv_opengles_shader_get_uniform_location(const char * name)
{
    int id = -1;
    if(strcmp(m_shader_names[0], name) == 0) {
        id = 0;
    }
    else if(strcmp(m_shader_names[1], name) == 0) {
        id = 1;
    }
    else {
        return -1;
    }

    if(m_shader_location[id] != 0) {
        return m_shader_location[id];
    }

    GLCall(int location = glGetUniformLocation(m_shader_id, name));
    if(location == -1)
        LV_LOG_WARN("Warning: uniform '%s' doesn't exist!\n", name);

    m_shader_location[id] = location;
    return location;
}

void lv_opengles_shader_set_uniform1i(const char * name, int value)
{
    GLCall(glUniform1i(lv_opengles_shader_get_uniform_location(name), value));
}

void lv_opengles_shader_set_uniform1f(const char * name, float value)
{
    GLCall(glUniform1f(lv_opengles_shader_get_uniform_location(name), value));
}

void lv_opengles_shader_set_uniform4f(const char * name, float v0, float v1, float v2, float v3)
{
    GLCall(glUniform4f(lv_opengles_shader_get_uniform_location(name), v0, v1, v2, v3));
}

void lv_opengles_render_draw()
{
    lv_opengles_shader_bind();
    lv_opengles_vertex_array_bind();
    lv_opengles_index_buffer_bind();
    unsigned int count = lv_opengles_index_buffer_get_count();
    GLCall(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, NULL));
}

static unsigned int m_texture_id = 0;
static unsigned char * m_texture_buffer = NULL;

void lv_opengles_texture_init(void * buffer, int width, int height)
{
    if(buffer == NULL) {
        return;
    }

    GLCall(glGenTextures(1, &m_texture_id));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_texture_id));

    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    //GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, buffer));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void lv_opengles_texture_deinit()
{
    GLCall(glDeleteTextures(1, &m_texture_id));
    m_texture_id = 0;
}

void lv_opengles_texture_bind(unsigned int slot)
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, m_texture_id));
}

void lv_opengles_texture_unbind()
{
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void lv_opengles_texture_update(void * buffer, int width, int height)
{
    GLCall(glBindTexture(GL_TEXTURE_2D, m_texture_id));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, buffer));
}

#endif /* LV_USE_OPENGLES */
