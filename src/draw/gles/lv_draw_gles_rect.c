/**
 * @file lv_draw_gles_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_GLES

#include "../lv_draw.h"
#include "lv_draw_gles_utils.h"
#include "lv_draw_gles.h"
#include "lv_draw_gles_math.h"

#include LV_GPU_GLES_GLAD_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
void opengl_draw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);
void opengl_draw_plain_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);
void opengl_draw_corner_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords, int x, int y, int radius);

/**********************
 *      MACROS
 **********************/
#define SKIP_BORDER(dsc) ((dsc)->border_opa <= LV_OPA_MIN || (dsc)->border_width == 0 || (dsc)->border_side == LV_BORDER_SIDE_NONE || (dsc)->border_post)
#define SKIP_SHADOW(dsc) ((dsc)->shadow_width == 0 || (dsc)->shadow_opa <= LV_OPA_MIN || ((dsc)->shadow_width == 1 && (dsc)->shadow_spread <= 0 && (dsc)->shadow_ofs_x == 0 && (dsc)->shadow_ofs_y == 0))
#define SKIP_IMAGE(dsc) ((dsc)->bg_img_src == NULL || (dsc)->bg_img_opa <= LV_OPA_MIN)
#define SKIP_OUTLINE(dsc) ((dsc)->outline_opa <= LV_OPA_MIN || (dsc)->outline_width == 0)

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_sw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_gles_draw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{
    /* Do software drawing */
    //lv_draw_sw_rect(draw_ctx, dsc, coords);

    //lv_draw_gles_utils_upload_texture(draw_ctx);
    /* Do opengl drawing */
#if LV_USE_GPU_GLES_SW_MIXED
    opengl_draw_rect(draw_ctx, dsc, coords);
    lv_draw_gles_utils_download_texture(draw_ctx);
#else
    opengl_draw_rect(draw_ctx, dsc, coords);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

void opengl_draw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{
    //if(coords->x2 - coords->x1 <799) return;
    lv_draw_gles_ctx_t *draw_gles_ctx = (lv_draw_gles_ctx_t*) draw_ctx;
    lv_draw_gles_context_internals_t *internals = draw_gles_ctx->internals;

    if(dsc->bg_opa <= LV_OPA_MIN) return;

    lv_area_t bg_coords;
    lv_area_copy(&bg_coords, coords);

    /*If the border fully covers make the bg area 1px smaller to avoid artifacts on the corners*/
    if(dsc->border_width > 1 && dsc->border_opa >= LV_OPA_MAX && dsc->radius != 0) {
        bg_coords.x1 += (dsc->border_side & LV_BORDER_SIDE_LEFT) ? 1 : 0;
        bg_coords.y1 += (dsc->border_side & LV_BORDER_SIDE_TOP) ? 1 : 0;
        bg_coords.x2 -= (dsc->border_side & LV_BORDER_SIDE_RIGHT) ? 1 : 0;
        bg_coords.y2 -= (dsc->border_side & LV_BORDER_SIDE_BOTTOM) ? 1 : 0;
    }

    lv_area_t clipped_coords;
    if(!_lv_area_intersect(&clipped_coords, &bg_coords, draw_ctx->clip_area)) return;

    lv_grad_dir_t grad_dir = dsc->bg_grad.dir;
    lv_color_t bg_color    = grad_dir == LV_GRAD_DIR_NONE ? dsc->bg_color : dsc->bg_grad.stops[0].color;
    if(bg_color.full == dsc->bg_grad.stops[1].color.full) grad_dir = LV_GRAD_DIR_NONE;

    bool mask_any = lv_draw_mask_is_any(&bg_coords);
    lv_draw_sw_blend_dsc_t blend_dsc = {0};
    blend_dsc.blend_mode = dsc->blend_mode;
    blend_dsc.color = bg_color;

    /*Most simple case: just a plain rectangle*/
    if(!mask_any && dsc->radius == 0 && (grad_dir == LV_GRAD_DIR_NONE)) {
        opengl_draw_plain_rect(draw_ctx, dsc, &clipped_coords);
        return;
    }

    lv_opa_t opa = dsc->bg_opa >= LV_OPA_MAX ? LV_OPA_COVER : dsc->bg_opa;

    /*Get the real radius. Can't be larger than the half of the shortest side */
    lv_coord_t coords_bg_w = lv_area_get_width(&bg_coords);
    lv_coord_t coords_bg_h = lv_area_get_height(&bg_coords);
    int32_t short_side = LV_MIN(coords_bg_w, coords_bg_h);
    int32_t rout = LV_MIN(dsc->radius, short_side >> 1);


    /*Add a radius mask if there is radius*/
    int32_t clipped_w = lv_area_get_width(&clipped_coords);
    int16_t mask_rout_id = LV_MASK_ID_INV;
    lv_opa_t * mask_buf = NULL;
    lv_draw_mask_radius_param_t mask_rout_param;
    if(rout > 0 || mask_any) {
        /*********** Radius mask drawing ***********/

        /*** top-mid part ***/
        lv_area_t top_mid_coords;
        top_mid_coords.x1 = clipped_coords.x1 + rout;
        top_mid_coords.x2 = clipped_coords.x2 - rout;
        top_mid_coords.y1 = clipped_coords.y1;
        top_mid_coords.y2 = clipped_coords.y1 + rout;
        opengl_draw_plain_rect(draw_ctx, dsc, &top_mid_coords);

        /*** center part ***/
        lv_area_t center_coords;
        center_coords.x1 = clipped_coords.x1 + rout;
        center_coords.x2 = clipped_coords.x2 - rout;
        center_coords.y1 = clipped_coords.y1 + rout;
        center_coords.y2 = clipped_coords.y2 - rout;
        opengl_draw_plain_rect(draw_ctx, dsc, &center_coords);

        /*** bottom-mid part ***/
        lv_area_t bottom_mid_coords;
        bottom_mid_coords.x1 = clipped_coords.x1 + rout;
        bottom_mid_coords.x2 = clipped_coords.x2 - rout;
        bottom_mid_coords.y1 = clipped_coords.y2 - rout;
        bottom_mid_coords.y2 = clipped_coords.y2;
        opengl_draw_plain_rect(draw_ctx, dsc, &bottom_mid_coords);

        /*** left-mid part ***/
        lv_area_t left_mid_coords;
        left_mid_coords.x1 = clipped_coords.x1;
        left_mid_coords.x2 = clipped_coords.x1 + rout;
        left_mid_coords.y1 = clipped_coords.y1 + rout;
        left_mid_coords.y2 = clipped_coords.y2 - rout;
        opengl_draw_plain_rect(draw_ctx, dsc, &left_mid_coords);

        /*** right-mid part ***/
        lv_area_t right_mid_coords;
        right_mid_coords.x1 = clipped_coords.x2-rout;
        right_mid_coords.x2 = clipped_coords.x2;
        right_mid_coords.y1 = clipped_coords.y1 + rout;
        right_mid_coords.y2 = clipped_coords.y2 - rout;
        opengl_draw_plain_rect(draw_ctx, dsc, &right_mid_coords);

        /*** top-left part ***/
        lv_area_t top_left_coords;
        top_left_coords.x1 = clipped_coords.x1;
        top_left_coords.x2 = clipped_coords.x1 + rout;
        top_left_coords.y1 = clipped_coords.y1;
        top_left_coords.y2 = clipped_coords.y1  + rout;
        opengl_draw_corner_rect(draw_ctx, dsc, &top_left_coords,
                                top_left_coords.x2, top_left_coords.y2, rout);

        /*** bottom-left part ***/
        lv_area_t bottom_left_coords;
        bottom_left_coords.x1 = clipped_coords.x1;
        bottom_left_coords.x2 = clipped_coords.x1 + rout;
        bottom_left_coords.y1 = clipped_coords.y2 - rout;
        bottom_left_coords.y2 = clipped_coords.y2;
        opengl_draw_corner_rect(draw_ctx, dsc, &bottom_left_coords,
                                top_left_coords.x2, bottom_left_coords.y1, rout);
        /*** top-right part ***/
        lv_area_t top_right_coords;
        top_right_coords.x1 = clipped_coords.x2 - rout;
        top_right_coords.x2 = clipped_coords.x2;
        top_right_coords.y1 = clipped_coords.y1;
        top_right_coords.y2 = clipped_coords.y1  + rout;
        opengl_draw_corner_rect(draw_ctx, dsc, &top_right_coords,
                                top_right_coords.x1, top_left_coords.y2, rout);
        /*** bottom-right part ***/
        lv_area_t bottom_right_coords;
        bottom_right_coords.x1 = clipped_coords.x2 - rout;
        bottom_right_coords.x2 = clipped_coords.x2;
        bottom_right_coords.y1 = clipped_coords.y2 - rout;
        bottom_right_coords.y2 = clipped_coords.y2;
        opengl_draw_corner_rect(draw_ctx, dsc, &bottom_right_coords,
                                bottom_right_coords.x1, bottom_right_coords.y1, rout);


        LV_LOG_USER("rout : %d coords: x1 :%d x2: %d y1 : %d y2 : %d", dsc->radius, bg_coords.x1, bg_coords.x2, bg_coords.y1, bg_coords.y2);
      //  mask_buf = lv_mem_buf_get(clipped_w);
        //lv_draw_mask_radius_init(&mask_rout_param, &bg_coords, rout, false);
       // mask_rout_id = lv_draw_mask_add(&mask_rout_param, NULL);

    } else {
        /* TODO(tan): handle it */
    }
}

void opengl_draw_plain_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_gles_ctx_t *draw_gles_ctx = (lv_draw_gles_ctx_t*) draw_ctx;
    lv_draw_gles_context_internals_t *internals = draw_gles_ctx->internals;

    lv_grad_dir_t grad_dir = dsc->bg_grad.dir;
    lv_color_t bg_color    = grad_dir == LV_GRAD_DIR_NONE ? dsc->bg_color : dsc->bg_grad.stops[0].color;
    vec4 color;
    color[0] = (float)bg_color.ch.red/255.0f;
    color[1] = (float)bg_color.ch.green/255.0f;
    color[2] = (float)bg_color.ch.blue/255.0f;
    color[3] = (float)dsc->bg_opa/255.0f;
    static GLfloat vertices[] = {
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };

    mat4 model;
    lv_draw_gles_math_mat4_identity(model);
    lv_draw_gles_math_translate(model, (vec3) {coords->x1, coords->y1});
    lv_draw_gles_math_scale(model, (vec3) {coords->x2 - coords->x1, coords->y2 - coords->y1});
    LV_LOG_USER("coords: x1 :%d x2: %d y1 : %d y2 : %d", coords->x1,
                coords->x2,
                coords->y1,
                coords->y2);

    glBindFramebuffer(GL_FRAMEBUFFER, internals->framebuffer);

    glUseProgram(internals->plain_rect_shader);

    glUniform4f(internals->plain_rect_shader_color_location, color[0], color[1], color[2], color[3]);
    glUniformMatrix4fv(internals->plain_rect_shader_model_location, 1, GL_FALSE, &model[0][0]);
    glVertexAttribPointer(internals->plain_rect_shader_pos_location, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), vertices);
    glEnableVertexAttribArray(internals->plain_rect_shader_pos_location);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void opengl_draw_corner_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords, int x, int y, int radius)
{
    lv_draw_gles_ctx_t *draw_gles_ctx = (lv_draw_gles_ctx_t*) draw_ctx;
    lv_draw_gles_context_internals_t *internals = draw_gles_ctx->internals;

    lv_grad_dir_t grad_dir = dsc->bg_grad.dir;
    lv_color_t bg_color    = grad_dir == LV_GRAD_DIR_NONE ? dsc->bg_color : dsc->bg_grad.stops[0].color;
    vec4 color;
    color[0] = (float)bg_color.ch.red/255.0f;
    color[1] = (float)bg_color.ch.green/255.0f;
    color[2] = (float)bg_color.ch.blue/255.0f;
    color[3] = (float)dsc->bg_opa/255.0f;
    static GLfloat vertices[] = {
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
    };

    mat4 model;
    lv_draw_gles_math_mat4_identity(model);
    lv_draw_gles_math_translate(model, (vec3) {coords->x1, coords->y1});
    lv_draw_gles_math_scale(model, (vec3) {coords->x2 - coords->x1, coords->y2 - coords->y1});

    vec2 corner;
    corner[0] = (float)x;
    corner[1] = LV_VER_RES - (float)y;

    float r = (float) radius;

    LV_LOG_USER("coords: x1 :%d x2: %d y1 : %d y2 : %d", coords->x1,
                coords->x2,
                coords->y1,
                coords->y2);

    glBindFramebuffer(GL_FRAMEBUFFER, internals->framebuffer);

    glUseProgram(internals->corner_rect_shader);

    glUniform4f(internals->corner_rect_shader_color_location, color[0], color[1], color[2], color[3]);
    glUniform2f(internals->corner_rect_shader_corner_location, corner[0], corner[1]);
    glUniform1f(internals->corner_rect_shader_radius_location, r);
    glUniformMatrix4fv(internals->corner_rect_shader_model_location, 1, GL_FALSE, &model[0][0]);
    glVertexAttribPointer(internals->corner_rect_shader_pos_location, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), vertices);
    glEnableVertexAttribArray(internals->corner_rect_shader_pos_location);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
#endif /*LV_USE_GPU_GLES*/
