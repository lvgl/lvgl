/**
 * @file lv_draw_gles_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL_GLES

#include "lv_draw_gles_img.h"
#include "lv_draw_gles.h"
#include "lv_draw_gles_utils.h"

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

/**********************
 *      MACROS
 **********************/


/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#include "../sw/lv_draw_sw.h"
lv_res_t lv_draw_gles_draw_img(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * draw_dsc,
                               const lv_area_t * coords, const void * src)
{
    lv_draw_gles_ctx_t *draw_gles_ctx = (lv_draw_gles_ctx_t*) draw_ctx;
    lv_draw_gles_context_internals_t *internals = draw_gles_ctx->internals;
    const lv_area_t * clip = draw_ctx->clip_area;


    /* TODO : Find texture */
    _lv_img_cache_entry_t  *cdsc = _lv_img_cache_open(src, lv_color_white(), draw_dsc->frame_id);
    lv_img_decoder_dsc_t * dsc = &cdsc->dec_dsc;

    int h = (int) dsc->header.h;
    int w = (int) dsc->header.w;
    void *data = (void*) dsc->img_data;
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data );



    lv_area_t zoomed_cords;
    _lv_img_buf_get_transformed_area(&zoomed_cords, lv_area_get_width(coords), lv_area_get_height(coords), 0,
                                     draw_dsc->zoom, &draw_dsc->pivot);
    lv_area_move(&zoomed_cords, coords->x1, coords->y1);

    /* When in > 0, draw simple radius */
    lv_coord_t radius = 0;
    /* Coords will be translated so coords will start at (0,0) */
    lv_area_t t_coords = zoomed_cords, t_clip = *clip, apply_area;

    lv_area_t clipped_area;
    _lv_area_intersect(&clipped_area, coords, clip);

    if(radius > 0) {
        //draw_img_rounded(ctx, texture, header, draw_dsc, &t_coords, &t_clip, radius);


    }
    else {

        //draw_img_simple(ctx, texture, header, draw_dsc, &t_coords, &t_clip);

        static GLfloat vertices[] = {
            -1.0f,  1.0f,  0.0f, 0.0f,
            -1.0f, -1.0f,  0.0f, 1.0f,
            1.0f, -1.0f,  1.0f, 1.0f,

            -1.0f,  1.0f,  0.0f, 0.0f,
            1.0f, -1.0f,  1.0f, 1.0f,
            1.0f,  1.0f,  1.0f, 0.0f
        };

        mat4 model;
        glm_mat4_identity(model);
        glm_translate(model, (vec3) {t_clip.x1, t_clip.y2});
        glm_scale(model, (vec3) {t_clip.x2 - t_clip.x1, t_clip.y1 - t_clip.y2});
#if 1
        glBindFramebuffer(GL_FRAMEBUFFER, internals->framebuffer);
        glUseProgram(internals->simple_img_shader);
        glUniformMatrix4fv(internals->simple_img_shader_model_location, 1, GL_FALSE, &model[0][0]);
        glUniform1i(internals->simple_img_shader_texture_location, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glVertexAttribPointer(internals->simple_img_shader_pos_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), vertices);
        glEnableVertexAttribArray(internals->simple_img_shader);
        glVertexAttribPointer(internals->simple_img_shader_uv_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), &vertices[2]);
        glEnableVertexAttribArray(internals->simple_img_shader_uv_location);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
        LV_LOG_USER("%d", glGetError());
        lv_draw_gles_utils_download_texture(draw_ctx);
    }
}
/**********************
 *   STATIC FUNCTIONS
 **********************/
#endif /*LV_USE_GPU_SDL_GLES*/
