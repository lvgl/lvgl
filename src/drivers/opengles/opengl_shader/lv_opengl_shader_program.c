/**
 * @file lv_opengl_shader_program.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_opengl_shader_internal.h"

#if LV_USE_OPENGLES

#include "../lv_opengles_private.h"
#include "../lv_opengles_debug.h"
#include "../../../misc/lv_assert.h"
#include "../../../stdlib/lv_mem.h"
/*********************
 *      DEFINES
 *********************/

#define INVALID_LOCATION 0xFFFFFFFFu

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void update_uniform_1i(lv_opengl_shader_program_t * program, const char * prop,
                              int value);

static void update_uniform_1f(lv_opengl_shader_program_t * program, const char * prop,
                              float value);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_opengl_shader_program_t * lv_opengl_shader_program_create(unsigned int _program)
{
    lv_opengl_shader_program_t * program = lv_malloc(sizeof(*program));
    LV_ASSERT_MALLOC(program);
    if(!program) {
        return NULL;
    }
    program->update_uniform_1i = &update_uniform_1i;
    program->update_uniform_1f = &update_uniform_1f;
    program->id = _program;

    return program;
}

void lv_opengl_shader_program_destroy(lv_opengl_shader_program_t * program)
{
#ifndef __EMSCRIPTEN__
    GLuint shader_names[10];
    GLsizei shader_count;
    GL_CALL(glGetAttachedShaders(program->id, 10, &shader_count,
                                 shader_names));

    // Detach and delete each shader
    for(GLsizei i = 0; i < shader_count; ++i) {
        if(shader_names[i] != 0)
            GL_CALL(glDetachShader(program->id, shader_names[i]));
    }
#endif

    GL_CALL(glDeleteProgram(program->id));
}

GLuint lv_opengl_shader_program_get_id(lv_opengl_shader_program_t * program)
{
    LV_ASSERT_NULL(program);
    return program->id;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


static void update_uniform_1i(lv_opengl_shader_program_t * program, const char * prop,
                              int value)
{
    GLuint location = glGetUniformLocation(program->id, prop);
    LV_ASSERT_FORMAT_MSG(location != INVALID_LOCATION,
                         "Uniform '%s' not found in program %d",
                         prop, program->id);
    if(location == INVALID_LOCATION) {
        LV_LOG_ERROR("Uniform '%s' not found in program %d", prop,
                     program->id);
        return;
    }
    GL_CALL(glUniform1i(location, value));
}

static void update_uniform_1f(lv_opengl_shader_program_t * program, const char * prop,
                              float value)
{
    GLuint location = glGetUniformLocation(program->id, prop);

    LV_ASSERT_FORMAT_MSG(location != INVALID_LOCATION,
                         "Uniform '%s' not found in program %d",
                         prop, program->id);
    if(location == INVALID_LOCATION) {
        LV_LOG_ERROR("Uniform '%s' not found in program %d", prop,
                     program->id);
        return;
    }
    GL_CALL(glUniform1f(location, value));
}

#endif /*LV_USE_OPENGLES*/
