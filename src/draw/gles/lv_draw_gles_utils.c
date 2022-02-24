/**
 * @file lv_draw_gles_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL_GLES

#include <stdio.h>
#include <stdlib.h>

#include <cglm/cglm.h>

#include "lv_draw_gles_utils.h"


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static GLuint shader_create(GLenum type, const char *src);

/**********************
 *  STATIC VARIABLES
 **********************/

static char rect_vertex_shader_str[] =
    "attribute vec2 a_position;   \n"
    "uniform mat4 projection;   \n"
    "uniform mat4 model;   \n"
    "void main()                  \n"
    "{                            \n"
    "   gl_Position = projection * model * vec4(a_position.x, a_position.y, 0.0, 1.0); \n"
    "}                            \n";

static char rect_fragment_shader_str[] =
    "precision mediump float;                            \n"
    "uniform vec4 color;   \n"
    "void main()                                         \n"
    "{                                                   \n"
    "  gl_FragColor = color;          \n"
    "}                                                   \n";

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void _lv_draw_gles_utils_init()
{
}

void lv_draw_gles_utils_internals_init(lv_draw_gles_context_internals_t * internals)
{
    mat4 tmp;
    glm_mat4_identity(tmp);
    glm_ortho(0.0f,
              (float)LV_GPU_SDL_GLES_HOR_RES,
              (float)LV_GPU_SDL_GLES_VER_RES,
              0.0f,
              -1.0f, 1.0f,
              tmp);
    /* unaligned? sigsev on glm_ortho */
    glm_mat4_ucopy(tmp, internals->projection);


    internals->rect_shader = lv_draw_gles_shader_program_create(rect_vertex_shader_str, rect_fragment_shader_str);
    glUseProgram(internals->rect_shader);
    internals->rect_shader_pos_location = glGetAttribLocation(internals->rect_shader, "a_position");
    internals->rect_shader_projection_location = glGetUniformLocation(internals->rect_shader, "projection");
    internals->rect_shader_model_location = glGetUniformLocation(internals->rect_shader, "model");
    internals->rect_shader_color_location = glGetUniformLocation(internals->rect_shader, "color");
    glUniformMatrix4fv(internals->rect_shader_projection_location, 1, GL_FALSE, &internals->projection[0][0]);
    internals->pixels = malloc( 800 * 600 * 3 * sizeof(GLubyte));
}

GLuint lv_draw_gles_shader_program_create(const char *vertex_src, const char *fragment_src)
{
    GLuint vertex = shader_create(GL_VERTEX_SHADER, vertex_src);
    GLuint fragment = shader_create(GL_FRAGMENT_SHADER, fragment_src);
    GLuint program = glCreateProgram();

    glAttachShader(program, vertex);
    glAttachShader(program, fragment);

    glLinkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

static GLuint shader_create(GLenum type, const char *src)
{
    GLint success = 0;

    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        GLint info_log_len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_len);

        char *info_log = malloc(info_log_len+1);
        info_log[info_log_len] = '\0';

        glGetShaderInfoLog(shader, info_log_len, NULL, info_log);
        fprintf(stderr, "Failed to compile shader : %s", info_log);
        free(info_log);
    }

    return shader;
}

#endif /*LV_USE_GPU_SDL_GLES*/
