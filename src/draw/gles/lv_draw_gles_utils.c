/**
 * @file lv_draw_gles_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL_GLES

#include "lv_draw_gles.h"
#include "lv_draw_gles_utils.h"
#include "../../misc/lv_log.h"
#include "../../core/lv_refr.h"


/*********************
 *      DEFINES
 *********************/
#define BYTES_PER_PIXEL 3
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lvgl_buf_to_opengl_buf(GLubyte *opengl_buf, const void *lvgl_buf);
static void opengl_buf_to_lvgl_buf(void *lvgl_buf, const GLubyte *opengl_buf);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_gles_utils_internals_init(lv_draw_gles_context_internals_t * internals)
{
    /* Generate buffer for temp gpu texture */
    internals->gpu_texture_pixels = malloc(LV_GPU_SDL_GLES_HOR_RES * LV_GPU_SDL_GLES_VER_RES * BYTES_PER_PIXEL * sizeof(GLubyte));
    /* Maybe initialize with all zeros? */

    /* Generate temp gpu texture */
    glGenTextures(1, &internals->gpu_texture);
    glBindTexture(GL_TEXTURE_2D, internals->gpu_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, LV_GPU_SDL_GLES_HOR_RES, LV_GPU_SDL_GLES_VER_RES, 0, GL_RGB, GL_UNSIGNED_BYTE, internals->gpu_texture_pixels);
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glBindTexture(GL_TEXTURE_2D, 0);

    LV_LOG_USER("GPU texture is created.");

}

void lv_draw_gles_utils_upload_texture(lv_draw_ctx_t * draw_ctx)
{
    lv_draw_gles_ctx_t *draw_gles_ctx = (lv_draw_gles_ctx_t*) draw_ctx;
    lv_draw_gles_context_internals_t *internals = draw_gles_ctx->internals;

    lvgl_buf_to_opengl_buf(internals->gpu_texture_pixels, draw_gles_ctx->base_draw.buf);

    glBindTexture(GL_TEXTURE_2D, internals->gpu_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, LV_GPU_SDL_GLES_HOR_RES, LV_GPU_SDL_GLES_VER_RES, GL_RGB, GL_UNSIGNED_BYTE, internals->gpu_texture_pixels);
    glBindTexture(GL_TEXTURE_2D,0);

}

void lv_draw_gles_utils_download_texture(lv_draw_ctx_t * draw_ctx)
{
    lv_draw_gles_ctx_t *draw_gles_ctx = (lv_draw_gles_ctx_t*) draw_ctx;
    lv_draw_gles_context_internals_t *internals = draw_gles_ctx->internals;

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, internals->gpu_texture, 0);
    glReadPixels(0, 0, LV_GPU_SDL_GLES_HOR_RES, LV_GPU_SDL_GLES_VER_RES, GL_RGB, GL_UNSIGNED_BYTE, internals->gpu_texture_pixels);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &fbo);


    lv_disp_t * disp = _lv_refr_get_disp_refreshing();

    opengl_buf_to_lvgl_buf(disp->driver->draw_buf->buf1,  internals->gpu_texture_pixels);


}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lvgl_buf_to_opengl_buf(GLubyte *opengl_buf, const void *lvgl_buf)
{
    lv_color_t *buf = (lv_color_t*)lvgl_buf;
    for (uint32_t y=0; y<LV_GPU_SDL_GLES_VER_RES; y++) {
        for (uint32_t x=0; x<LV_GPU_SDL_GLES_HOR_RES; x++) {
            uint32_t index = (y * LV_GPU_SDL_GLES_HOR_RES * BYTES_PER_PIXEL) + (x * BYTES_PER_PIXEL);
            opengl_buf[index++] = buf->ch.red;
            opengl_buf[index++] = buf->ch.green;
            opengl_buf[index++] = buf->ch.blue;
            buf++;
        }
    }
}

static void opengl_buf_to_lvgl_buf(void *lvgl_buf, const GLubyte *opengl_buf)
{
    lv_color_t *buf = (lv_color_t*)lvgl_buf;
    for (uint32_t y=0; y<LV_GPU_SDL_GLES_VER_RES; y++) {
        for (uint32_t x=0; x<LV_GPU_SDL_GLES_HOR_RES; x++) {
            uint32_t index = (y * LV_GPU_SDL_GLES_HOR_RES * BYTES_PER_PIXEL) + (x * BYTES_PER_PIXEL);
            buf[y*LV_GPU_SDL_GLES_HOR_RES + x].ch.red = opengl_buf[index++];
            buf[y*LV_GPU_SDL_GLES_HOR_RES + x].ch.green = opengl_buf[index++];
            buf[y*LV_GPU_SDL_GLES_HOR_RES + x].ch.blue = opengl_buf[index++];
            buf[y*LV_GPU_SDL_GLES_HOR_RES + x].ch.alpha = 0xFF;
        }
    }
}

#endif /*LV_USE_GPU_SDL_GLES*/
