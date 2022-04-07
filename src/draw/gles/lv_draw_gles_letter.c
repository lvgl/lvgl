/**
 * @file lv_draw_gles_letter.c
 *
 */


/*********************
 *      INCLUDES
 *********************/


#include "../../lv_conf_internal.h"


#if LV_USE_GPU_GLES

#include "../lv_draw.h"
#include "lv_draw_gles.h"
#include "lv_draw_gles_utils.h"

#include <stdint.h>


#include LV_GPU_GLES_GLAD_INCLUDE_PATH

/*********************
 *      DEFINES
 *********************/

void lv_draw_sw_letter(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,  const lv_point_t * pos_p,
                       uint32_t letter);

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

extern const uint8_t _lv_bpp1_opa_table[2];
extern const uint8_t _lv_bpp2_opa_table[4];
extern const uint8_t _lv_bpp4_opa_table[16];
extern const uint8_t _lv_bpp8_opa_table[256];
static void lv_sdl_to_8bpp(uint8_t * dest, const uint8_t * src, int width, int height, int stride, uint8_t bpp);
/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_gles_draw_letter(lv_draw_ctx_t * draw_ctx, const lv_draw_label_dsc_t * dsc,  const lv_point_t * pos_p,
                              uint32_t letter)
{
#if LV_USE_GPU_GLES_SW_MIXED
    lv_draw_sw_letter(draw_ctx, dsc, pos_p, letter);
    lv_draw_gles_utils_upload_texture(draw_ctx);
#else

    lv_draw_gles_ctx_t *draw_gles_ctx = (lv_draw_gles_ctx_t*) draw_ctx;
    lv_draw_gles_context_internals_t *internals = draw_gles_ctx->internals;
    const lv_area_t * clip_area = draw_ctx->clip_area;
    const lv_font_t * font_p = dsc->font;
    lv_opa_t opa = dsc->opa;
    lv_color_t color = dsc->color;
    if(opa < LV_OPA_MIN) return;
    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    if(font_p == NULL) {
        LV_LOG_WARN("lv_draw_letter: font is NULL");
        return;
    }

    lv_font_glyph_dsc_t g;
    bool g_ret = lv_font_get_glyph_dsc(font_p, &g, letter, '\0');
    if(g_ret == false) {
        /*Add warning if the dsc is not found
         *but do not print warning for non printable ASCII chars (e.g. '\n')*/
        if(letter >= 0x20 &&
            letter != 0xf8ff && /*LV_SYMBOL_DUMMY*/
            letter != 0x200c) { /*ZERO WIDTH NON-JOINER*/
            LV_LOG_WARN("lv_draw_letter: glyph dsc. not found for U+%X", letter);
        }
        return;
    }

    /*Don't draw anything if the character is empty. E.g. space*/
    if((g.box_h == 0) || (g.box_w == 0)) return;

    int32_t pos_x = pos_p->x + g.ofs_x;
    int32_t pos_y = pos_p->y + (font_p->line_height - font_p->base_line) - g.box_h - g.ofs_y;

    const lv_area_t letter_area = {pos_x, pos_y, pos_x + g.box_w - 1, pos_y + g.box_h - 1};
    lv_area_t draw_area;

    /*If the letter is completely out of mask don't draw it*/
    if(!_lv_area_intersect(&draw_area, &letter_area, clip_area)) {
        return;
    }

    GLuint texture;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    const uint8_t * bmp = lv_font_get_glyph_bitmap(font_p, letter);
    uint8_t * buf = lv_mem_alloc(g.box_w * g.box_h);
    lv_sdl_to_8bpp(buf, bmp, g.box_w, g.box_h, g.box_w, g.bpp);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED_EXT, g.box_w, g.box_h, 0, GL_RED_EXT, GL_UNSIGNED_BYTE, buf);

    lv_area_t t_letter = letter_area, t_clip = *clip_area, apply_area;

    /*If the letter is completely out of mask don't draw it*/
    if(!_lv_area_intersect(&draw_area, &t_letter, &t_clip)) {
        return;
    }
#if 0
    srcrect.x = draw_area.x1 - t_letter.x1;
    srcrect.y = draw_area.y1 - t_letter.y1;
    srcrect.w = dstrect.w;
    srcrect.h = dstrect.h;
#endif


    static GLfloat vertices[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    vec4 o_color;
    o_color[0] = (float)color.ch.red/255.0f;
    o_color[1] = (float)color.ch.green/255.0f;
    o_color[2] = (float)color.ch.blue/255.0f;
    o_color[3] = (float)opa/255.0f;

    mat4 model;
    lv_draw_gles_math_mat4_identity(model);
    lv_draw_gles_math_translate(model, (vec3) {draw_area.x1, draw_area.y1});
    lv_draw_gles_math_scale(model, (vec3) {draw_area.x2 - draw_area.x1, draw_area.y2 - draw_area.y1});

    glBindFramebuffer(GL_FRAMEBUFFER, internals->framebuffer);
    glUseProgram(internals->letter_shader);
    glUniformMatrix4fv(internals->letter_shader_model_location, 1, GL_FALSE, &model[0][0]);
    glUniform1i(internals->letter_shader_texture_location, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform4f(internals->letter_shader_color_location, o_color[0], o_color[1], o_color[2], o_color[3]);
    glVertexAttribPointer(internals->letter_shader_pos_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), vertices);
    glEnableVertexAttribArray(internals->letter_shader_pos_location);
    glVertexAttribPointer(internals->letter_shader_uv_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), &vertices[2]);
    glEnableVertexAttribArray(internals->letter_shader_uv_location);
    glDrawArrays(GL_TRIANGLES, 0, 6);


    glBindFramebuffer(GL_FRAMEBUFFER, 0);

#endif /* LV_USE_GPU_GLES_SW_MIXED */
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_sdl_to_8bpp(uint8_t * dest, const uint8_t * src, int width, int height, int stride, uint8_t bpp)
{
    int src_len = width * height;
    int cur = 0;
    int curbit;
    uint8_t opa_mask;
    const uint8_t *opa_table;
    switch (bpp) {
        case 1:
            opa_mask = 0x1;
            opa_table = _lv_bpp1_opa_table;
            break;
        case 2:
            opa_mask = 0x4;
            opa_table = _lv_bpp2_opa_table;
            break;
        case 4:
            opa_mask = 0xF;
            opa_table = _lv_bpp4_opa_table;
            break;
        case 8:
            opa_mask = 0xFF;
            opa_table = _lv_bpp8_opa_table;
            break;
        default:
            return;
    }
    /* Does this work well on big endian systems? */
    while (cur < src_len) {
        curbit = 8 - bpp;
        uint8_t src_byte = src[cur * bpp / 8];
        while (curbit >= 0 && cur < src_len) {
            uint8_t src_bits = opa_mask & (src_byte >> curbit);
            dest[(cur / width * stride) + (cur % width)] = opa_table[src_bits];
            curbit -= bpp;
            cur++;
        }
    }
}




#endif /*LV_USE_GPU_GLES*/