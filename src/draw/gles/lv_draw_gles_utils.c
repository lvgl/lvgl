/**
 * @file lv_draw_gles_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_GPU_GLES

#include "lv_draw_gles.h"
#include "lv_draw_gles_utils.h"
#include "lv_draw_gles_math.h"
#include "lv_draw_gles_shader.h"
#include "../../misc/lv_log.h"
#include "../../core/lv_refr.h"


/*********************
 *      DEFINES
 *********************/
#define BYTES_PER_PIXEL 4
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lvgl_buf_to_opengl_buf(GLubyte *opengl_buf, const void *lvgl_buf);
static void opengl_buf_to_lvgl_buf(void *lvgl_buf, const GLubyte *opengl_buf);
static GLuint shader_create(GLenum type, const char *src);
static GLuint shader_program_create(const char *vertex_src, const char *fragment_src);




/**********************
 *  STATIC VARIABLES
 **********************/

static lv_coord_t hor;
static lv_coord_t ver;
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_gles_utils_internals_init(lv_draw_gles_context_internals_t * internals)
{
    /* Generate buffer for temp gpu texture */
    internals->gpu_texture_pixels = lv_mem_alloc(internals->hor *  internals->ver * BYTES_PER_PIXEL * sizeof(GLubyte));
    /* Maybe initialize with all zeros? */

    /* Generate temp gpu texture */
    glGenTextures(1, &internals->gpu_texture);
    glBindTexture(GL_TEXTURE_2D, internals->gpu_texture);

    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    LV_LOG_USER("GPU texture is created.");

    glGenFramebuffers(1, &internals->framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, internals->framebuffer);
    glBindTexture(GL_TEXTURE_2D, internals->gpu_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, internals->hor, internals->ver, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);


    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, internals->gpu_texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindTexture(GL_TEXTURE_2D, 0);



    mat4 projection;
    lv_draw_gles_math_mat4_identity(projection);
    lv_draw_gles_math_ortho(0.0f,
              (float)internals->hor,
              (float)internals->ver,
              0.0f,
              -1.0f, 1.0f,
              projection);
    lv_draw_gles_math_mat4_copy(projection, internals->projection);

    internals->rect_shader = shader_program_create( RECT_VERTEX_SHADER_SRC, PLAIN_RECT_FRAGMENT_SHADER_SRC);
    glUseProgram(internals->rect_shader);
    internals->rect_shader_pos_location = glGetAttribLocation(internals->rect_shader, "a_position");
    internals->rect_shader_projection_location = glGetUniformLocation(internals->rect_shader, "u_projection");
    internals->rect_shader_model_location = glGetUniformLocation(internals->rect_shader, "u_model");
    internals->rect_shader_color_location = glGetUniformLocation(internals->rect_shader, "u_color");
    internals->rect_shader_resolution_location = glGetUniformLocation(internals->rect_shader, "u_resolution");
    internals->rect_shader_radius_location = glGetUniformLocation(internals->rect_shader, "u_radius");
    internals->rect_shader_coords_location = glGetUniformLocation(internals->rect_shader, "u_coords");
    glUniformMatrix4fv(internals->rect_shader_projection_location, 1, GL_FALSE, &internals->projection[0][0]);
    glUseProgram(0);

    internals->plain_rect_shader = shader_program_create(PLAIN_RECT_VERTEX_SHADER_SRC, PLAIN_RECT_FRAGMENT_SHADER_SRC);
    glUseProgram(internals->plain_rect_shader);
    internals->plain_rect_shader_pos_location = glGetAttribLocation(internals->plain_rect_shader, "a_position");
    internals->plain_rect_shader_projection_location = glGetUniformLocation(internals->plain_rect_shader, "u_projection");
    internals->plain_rect_shader_model_location = glGetUniformLocation(internals->plain_rect_shader, "u_model");
    internals->plain_rect_shader_color_location = glGetUniformLocation(internals->plain_rect_shader, "u_color");
    glUniformMatrix4fv(internals->rect_shader_projection_location, 1, GL_FALSE, &internals->projection[0][0]);
    glUseProgram(0);

    internals->corner_rect_shader = shader_program_create(CORNER_RECT_VERTEX_SHADER_SRC, CORNER_RECT_FRAGMENT_SHADER_SRC);
    glUseProgram(internals->corner_rect_shader);
    internals->corner_rect_shader_pos_location = glGetAttribLocation(internals->corner_rect_shader, "a_position");
    internals->corner_rect_shader_projection_location = glGetUniformLocation(internals->corner_rect_shader, "u_projection");
    internals->corner_rect_shader_model_location = glGetUniformLocation(internals->corner_rect_shader, "u_model");
    internals->corner_rect_shader_color_location = glGetUniformLocation(internals->corner_rect_shader, "u_color");
    internals->corner_rect_shader_corner_location = glGetUniformLocation(internals->corner_rect_shader, "u_corner");
    internals->corner_rect_shader_radius_location= glGetUniformLocation(internals->corner_rect_shader, "u_radius");
    glUniformMatrix4fv(internals->corner_rect_shader_projection_location, 1, GL_FALSE, &internals->projection[0][0]);
    glUseProgram(0);
    LV_LOG_USER("%d", glGetError());

    internals->simple_img_shader = shader_program_create(SIMPLE_IMG_VERTEX_SHADER_SRC, SIMPLE_IMG_FRAGMENT_SHADER_SRC);
    glUseProgram(internals->simple_img_shader);
    internals->simple_img_shader_pos_location = glGetAttribLocation(internals->simple_img_shader, "a_position");
    internals->simple_img_shader_uv_location = glGetAttribLocation(internals->simple_img_shader, "a_uv");
    internals->simple_img_shader_projection_location = glGetUniformLocation(internals->simple_img_shader, "u_projection");
    internals->simple_img_shader_model_location = glGetUniformLocation(internals->simple_img_shader, "u_model");
    internals->simple_img_shader_color_location = glGetUniformLocation(internals->simple_img_shader, "u_color");
    internals->simple_img_shader_texture_location = glGetUniformLocation(internals->simple_img_shader, "s_texture");
    glUniformMatrix4fv(internals->simple_img_shader_projection_location, 1, GL_FALSE, &internals->projection[0][0]);
    glUseProgram(0);

    /* TODO(tan): It's dumb change later. */
    hor = internals->hor;
    ver = internals->ver;

}

void lv_draw_gles_utils_upload_texture(lv_draw_ctx_t * draw_ctx)
{
    lv_draw_gles_ctx_t *draw_gles_ctx = (lv_draw_gles_ctx_t*) draw_ctx;
    lv_draw_gles_context_internals_t *internals = draw_gles_ctx->internals;

    lvgl_buf_to_opengl_buf(internals->gpu_texture_pixels, draw_gles_ctx->base_draw.buf);

    glBindTexture(GL_TEXTURE_2D, internals->gpu_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, internals->hor, ver, GL_RGBA, GL_UNSIGNED_BYTE, internals->gpu_texture_pixels);
    glBindTexture(GL_TEXTURE_2D,0);

}

void lv_draw_gles_utils_download_texture(lv_draw_ctx_t * draw_ctx)
{
    lv_draw_gles_ctx_t *draw_gles_ctx = (lv_draw_gles_ctx_t*) draw_ctx;
    lv_draw_gles_context_internals_t *internals = draw_gles_ctx->internals;

    glBindFramebuffer(GL_FRAMEBUFFER, internals->framebuffer);

    glReadPixels(0, 0, internals->hor, ver,
                 GL_RGBA, GL_UNSIGNED_BYTE,
                 internals->gpu_texture_pixels);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    opengl_buf_to_lvgl_buf(draw_ctx->buf,  internals->gpu_texture_pixels);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lvgl_buf_to_opengl_buf(GLubyte *opengl_buf, const void *lvgl_buf)
{
    lv_color_t *buf = (lv_color_t*)lvgl_buf;
    for (uint32_t y=0; y<ver; y++) {
        for (uint32_t x=0; x<hor; x++) {
            uint32_t index = ((ver - y - 1) * hor * BYTES_PER_PIXEL) + (x * BYTES_PER_PIXEL);
            opengl_buf[index++] = buf->ch.red;
            opengl_buf[index++] = buf->ch.green;
            opengl_buf[index++] = buf->ch.blue;
            opengl_buf[index++] = buf->ch.alpha;
            buf++;
        }
    }
}

static void opengl_buf_to_lvgl_buf(void *lvgl_buf, const GLubyte *opengl_buf)
{
    lv_color_t *buf = (lv_color_t*)lvgl_buf;
    for (uint32_t y=0; y<ver; y++) {
        for (uint32_t x=0; x<hor; x++) {
            //uint32_t index = (y * internals->hor * BYTES_PER_PIXEL) + (x * BYTES_PER_PIXEL);
            uint32_t index = ((ver - y - 1) * hor * BYTES_PER_PIXEL) + (x * BYTES_PER_PIXEL);
            buf[y*hor + x].ch.red = opengl_buf[index++];
            buf[y*hor + x].ch.green = opengl_buf[index++];
            buf[y*hor + x].ch.blue = opengl_buf[index++];
            buf[y*hor + x].ch.alpha = opengl_buf[index++];
        }
    }
}

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

        char *info_log = lv_mem_alloc(info_log_len+1);
        info_log[info_log_len] = '\0';

        glGetShaderInfoLog(shader, info_log_len, NULL, info_log);
        LV_LOG_ERROR("Failed to compile shader : %s", info_log);
        lv_mem_free(info_log);
    }

    return shader;
}

static GLuint shader_program_create(const char *vertex_src, const char *fragment_src)
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

#endif /*LV_USE_GPU_GLES*/
