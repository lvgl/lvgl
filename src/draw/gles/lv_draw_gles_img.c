/**
 * @file lv_draw_gles_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_GLES

#include "lv_draw_gles_img.h"
#include "lv_draw_gles.h"
#include "lv_draw_gles_utils.h"
#include "lv_draw_gles_texture_cache.h"

#include LV_GPU_GLES_LOADER_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t opengl_draw_img(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * draw_dsc,
                               const lv_area_t * coords, const void * src);
void *bgra_to_rgba(void *data, int w, int h);
//draw_img_simple(ctx, texture, header, draw_dsc, &t_coords, &t_clip);

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
    return opengl_draw_img(draw_ctx, draw_dsc, coords, src);
#if LV_USE_GPU_GLES_SW_MIXED
    lv_draw_gles_utils_download_texture(draw_ctx);
#endif /* LV_USE_GPU_GLES_SW_MIXED */
}


void lv_draw_gles_img_decoded(struct _lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * dsc,
                              const lv_area_t * coords, const uint8_t * map_p, lv_img_cf_t color_format)
{
#if LV_USE_GPU_GLES_SW_MIXED
    lv_draw_gles_utils_upload_texture(draw_ctx);
#endif /* LV_USE_GPU_GLES_SW_MIXED */
}


void lv_draw_gles_img_load_texture(lv_draw_gles_ctx_t * ctx, lv_draw_gles_cache_key_head_img_t * key, size_t key_size,
                               const void * src, int32_t frame_id, GLuint *texture, uint32_t *width, uint32_t *height)
{

    _lv_img_cache_entry_t  *cdsc = _lv_img_cache_open(src, lv_color_white(), frame_id);
    lv_img_decoder_dsc_t * dsc = &cdsc->dec_dsc;

    int w = (int) dsc->header.w;
    int h = (int) dsc->header.h;
    void *data = bgra_to_rgba((void*)dsc->img_data, w, h);
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    lv_mem_free(data);

    lv_draw_gles_texture_cache_put(ctx, key, key_size, *texture, (uint32_t) w, (uint32_t) h);
    *width = (uint32_t)w;
    *height = (uint32_t)h;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_res_t opengl_draw_img(lv_draw_ctx_t * draw_ctx, const lv_draw_img_dsc_t * draw_dsc,
                                const lv_area_t * coords, const void * src)
{
    lv_draw_gles_ctx_t *draw_gles_ctx = (lv_draw_gles_ctx_t*) draw_ctx;
    lv_draw_gles_context_internals_t *internals = draw_gles_ctx->internals;
    const lv_area_t * clip = draw_ctx->clip_area;


    /* Find texture */
    size_t key_size;
    lv_draw_gles_cache_key_head_img_t * key = lv_draw_gles_texture_img_key_create(src, draw_dsc->frame_id, &key_size);
    bool texture_found = false;
    GLuint texture = 0;
    uint32_t texture_width = 0;
    uint32_t texture_height = 0;
    lv_draw_gles_texture_cache_get(draw_gles_ctx, key, key_size, &texture_found, &texture, &texture_width, &texture_height);

    if (!texture_found) {
        lv_draw_gles_img_load_texture(draw_gles_ctx, key, key_size, src, draw_dsc->frame_id, &texture, &texture_width, &texture_height);
    }

    lv_mem_free(key);




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


        lv_color_t re_color;
        if (draw_dsc->recolor_opa > LV_OPA_TRANSP) {
            re_color = lv_color_mix(draw_dsc->recolor, lv_color_white(), draw_dsc->recolor_opa);
        } else {
            re_color = lv_color_white();
        }

        vec4 color;
        lv_color_to_vec4_color_with_opacity(&re_color, draw_dsc->recolor_opa, color);

        vec2 uv;
        vec2 uv2;

        uv[0] = (float)(clipped_area.x1 - zoomed_cords.x1) / (float)(texture_width);
        uv[1] = (float)(clipped_area.y1 - zoomed_cords.y1) / (float)(texture_height);
        uv2[0] = 1.0f - (float)(zoomed_cords.x1 - clipped_area.x1) / (float)(texture_width);
        uv2[1] = 1.0f - (float)(zoomed_cords.y2 - clipped_area.y2) / (float)(texture_height);

        GLfloat vertices[] = {
            0.0f, 1.0f, uv[0], uv2[1],
            1.0f, 0.0f, uv2[0], uv[1],
            0.0f, 0.0f, uv[0], uv[1],

            0.0f, 1.0f, uv[0], uv2[1],
            1.0f, 1.0f, uv2[0], uv2[1],
            1.0f, 0.0f, uv2[0], uv[1]
        };

        mat4 model;
        lv_draw_gles_math_mat4_identity(model);
        lv_draw_gles_math_translate(model, (vec3) {clipped_area.x1, clipped_area.y1});
        lv_draw_gles_math_scale(model, (vec3) {clipped_area.x2 - clipped_area.x1, clipped_area.y2 - clipped_area.y1});
#if 1
        glBindFramebuffer(GL_FRAMEBUFFER, internals->framebuffer);
        glUseProgram(internals->simple_img_shader);
        glUniformMatrix4fv(internals->simple_img_shader_model_location, 1, GL_FALSE, &model[0][0]);
        glUniform1i(internals->simple_img_shader_texture_location, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform4f(internals->simple_img_shader_color_location, color[0], color[1], color[2], color[3]);
        glVertexAttribPointer(internals->simple_img_shader_pos_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), vertices);
        glEnableVertexAttribArray(internals->simple_img_shader_pos_location);
        glVertexAttribPointer(internals->simple_img_shader_uv_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), &vertices[2]);
        glEnableVertexAttribArray(internals->simple_img_shader_uv_location);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
        return LV_RES_OK;
    }

}


void *bgra_to_rgba(void *data, int w, int h)
{
    uint8_t *input = (uint8_t*) data;
    uint8_t *res = lv_mem_alloc(w * h * 4 * sizeof(uint8_t));

    uint32_t offset = 0;

    for(uint8_t y = 0; y < h; y++) {
        for(uint8_t x = 0; x < w; x++) {
            res[offset] = input[offset+ 2];
            res[offset + 1] = input[offset + 1];
            res[offset + 2] = input[offset];
            res[offset + 3] = input[offset + 3];

            offset += 4;
        }
    }

    return res;
}





#endif /*LV_USE_GPU_GLES*/
