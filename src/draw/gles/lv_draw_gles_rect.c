/**
 * @file lv_draw_sdl_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL_GLES

#include "lv_draw_gles_priv.h"
#include "lv_draw_gles.h"
#include "../lv_draw_rect.h"
#include "../lv_draw_img.h"
#include "../lv_draw_label.h"
#include "../../core/lv_refr.h"


#include LV_GPU_SDL_GLES_GLAD_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void draw_bg(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_gles_draw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{
    draw_bg(draw_ctx, dsc, coords);


}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_bg(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->bg_opa <= LV_OPA_MIN) return;
    /* bg color part */

    lv_draw_gles_ctx_t *ctx = (lv_draw_gles_ctx_t*)draw_ctx;
    lv_draw_gles_context_internals_t * internals = (lv_draw_gles_context_internals_t*)ctx->internals;
    LV_LOG_INFO("x1:%d x2:%d y1:%d y2:%d", coords->x1, coords->x2, coords->y1, coords->y2);

    vec4 color;
    color[0] = (float)dsc->bg_color.ch.red/255.0f;
    color[1] = (float)dsc->bg_color.ch.green/255.0f;
    color[2] = (float)dsc->bg_color.ch.blue/255.0f;
    color[3] = (float)dsc->bg_color.ch.alpha/255.0f;


    if(coords->x2 - coords->x1 > 700)
    {
      //  return;
    }

    static GLfloat vertices[] = {
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };

    mat4 model;
    glm_mat4_identity(model);
    glm_translate(model, (vec3) {coords->x1, coords->y1});
    glm_scale(model, (vec3) {coords->x2 - coords->x1, coords->y2 - coords->y1});


    glBindFramebuffer(GL_FRAMEBUFFER, *ctx->framebuffer);
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(internals->rect_shader);

    glUniformMatrix4fv(internals->rect_shader_model_location, 1, GL_FALSE, &model[0][0]);
    glUniform4f(internals->rect_shader_color_location, color[0], color[1], color[2], color[3]);
    glVertexAttribPointer(internals->rect_shader_pos_location, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), vertices);
    glEnableVertexAttribArray(internals->rect_shader_pos_location);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

#endif /*LV_USE_GPU_SDL_GLES*/
