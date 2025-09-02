#include "lv_draw_dave2d.h"
#if LV_USE_DRAW_DAVE2D

#include "../../lv_draw_label_private.h"
#include "../../../misc/lv_area_private.h"

static void lv_draw_dave2d_draw_letter_cb(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                                          lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area);

void lv_draw_dave2d_label(lv_draw_task_t * t, const lv_draw_label_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;

    lv_draw_label_iterate_characters(t, dsc, coords, lv_draw_dave2d_draw_letter_cb);

}

static void lv_draw_dave2d_draw_letter_cb(lv_draw_task_t * t, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                                          lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area)
{
    if(glyph_draw_dsc) {
        d2_u8 current_fillmode;
        lv_area_t clip_area;
        lv_area_t letter_coords;

        int32_t x;
        int32_t y;

        letter_coords = *glyph_draw_dsc->letter_coords;
        lv_draw_dave2d_unit_t * unit = (lv_draw_dave2d_unit_t *)t->draw_unit;

        bool is_common;
        is_common = lv_area_intersect(&clip_area, glyph_draw_dsc->letter_coords, &t->clip_area);
        if(!is_common) return;

        x = 0 - t->target_layer->buf_area.x1;
        y = 0 - t->target_layer->buf_area.y1;

        lv_area_move(&clip_area, x, y);
        lv_area_move(&letter_coords, x, y);

#if LV_USE_OS
        lv_result_t  status;
        status = lv_mutex_lock(unit->pd2Mutex);
        LV_ASSERT(LV_RESULT_OK == status);
#endif
        /* Labels use their own render buffer, select it first. */
        d2_selectrenderbuffer(unit->d2_handle, unit->label_renderbuffer);

        //
        // Generate render operations
        //

        d2_framebuffer_from_layer(unit->d2_handle, t->target_layer);

        current_fillmode = d2_getfillmode(unit->d2_handle);

        d2_cliprect(unit->d2_handle, (d2_border)clip_area.x1, (d2_border)clip_area.y1, (d2_border)clip_area.x2,
                    (d2_border)clip_area.y2);

        switch(glyph_draw_dsc->format) {
            case LV_FONT_GLYPH_FORMAT_NONE: {
#if LV_USE_FONT_PLACEHOLDER
                    /* Draw a placeholder rectangle*/
                    lv_draw_border_dsc_t border_draw_dsc;
                    lv_draw_border_dsc_init(&border_draw_dsc);
                    border_draw_dsc.opa = glyph_draw_dsc->opa;
                    border_draw_dsc.color = glyph_draw_dsc->color;
                    border_draw_dsc.width = 1;
                    //lv_draw_sw_border(u, &border_draw_dsc, glyph_draw_dsc->bg_coords);
                    lv_draw_dave2d_border(t, &border_draw_dsc, glyph_draw_dsc->bg_coords);
#endif
                }
                break;
            case LV_FONT_GLYPH_FORMAT_A1 ... LV_FONT_GLYPH_FORMAT_A8: {
                    glyph_draw_dsc->glyph_data = lv_font_get_glyph_bitmap(glyph_draw_dsc->g, glyph_draw_dsc->_draw_buf);
                    lv_area_t mask_area = letter_coords;
                    mask_area.x2 = mask_area.x1 + lv_draw_buf_width_to_stride(lv_area_get_width(&mask_area), LV_COLOR_FORMAT_A8) - 1;
                    //            lv_draw_sw_blend_dsc_t blend_dsc;
                    //            lv_memzero(&blend_dsc, sizeof(blend_dsc));
                    //            blend_dsc.color = glyph_draw_dsc->color;
                    //            blend_dsc.opa = glyph_draw_dsc->opa;
                    //            blend_dsc.mask_buf = glyph_draw_dsc->glyph_data;
                    //            blend_dsc.mask_area = &mask_area;
                    //            blend_dsc.blend_area = glyph_draw_dsc->letter_coords;
                    //            blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
                    //lv_draw_sw_blend(u, &blend_dsc);

                    const lv_draw_buf_t * draw_buf = glyph_draw_dsc->glyph_data;

#if defined(RENESAS_CORTEX_M85)
#if (BSP_CFG_DCACHE_ENABLED)
                    d1_cacheblockflush(unit->d2_handle, 0, draw_buf->data, draw_buf->data_size);
#endif
#endif
                    d2_settexture(unit->d2_handle, (void *)draw_buf->data,
                                  (d2_s32)lv_draw_buf_width_to_stride((uint32_t)lv_area_get_width(&letter_coords), LV_COLOR_FORMAT_A8),
                                  lv_area_get_width(&letter_coords),  lv_area_get_height(&letter_coords), d2_mode_alpha8);
                    d2_settexopparam(unit->d2_handle, d2_cc_red, glyph_draw_dsc->color.red, 0);
                    d2_settexopparam(unit->d2_handle, d2_cc_green, glyph_draw_dsc->color.green, 0);
                    d2_settexopparam(unit->d2_handle, d2_cc_blue, glyph_draw_dsc->color.blue, 0);
                    d2_settexopparam(unit->d2_handle, d2_cc_alpha, glyph_draw_dsc->opa, 0);

                    d2_settextureoperation(unit->d2_handle, d2_to_multiply, d2_to_multiply, d2_to_multiply, d2_to_multiply);

                    d2_settexturemapping(unit->d2_handle, D2_FIX4(letter_coords.x1), D2_FIX4(letter_coords.y1), D2_FIX16(0), D2_FIX16(0),
                                         D2_FIX16(1), D2_FIX16(0), D2_FIX16(0), D2_FIX16(1));

                    d2_settexturemode(unit->d2_handle, d2_tm_filter);

                    d2_setfillmode(unit->d2_handle, d2_fm_texture);

                    d2_renderbox(unit->d2_handle, (d2_point)D2_FIX4(letter_coords.x1),
                                 (d2_point)D2_FIX4(letter_coords.y1),
                                 (d2_point)D2_FIX4(lv_area_get_width(&letter_coords)),
                                 (d2_point)D2_FIX4(lv_area_get_height(&letter_coords)));

                    d2_setfillmode(unit->d2_handle, current_fillmode);
                }
                break;
            case LV_FONT_GLYPH_FORMAT_IMAGE: {
#if LV_USE_IMGFONT
                    glyph_draw_dsc->glyph_data = lv_font_get_glyph_bitmap(glyph_draw_dsc->g, glyph_draw_dsc->_draw_buf);
                    lv_draw_image_dsc_t img_dsc;
                    lv_draw_image_dsc_init(&img_dsc);
                    img_dsc.rotation = 0;
                    img_dsc.scale_x = LV_SCALE_NONE;
                    img_dsc.scale_y = LV_SCALE_NONE;
                    img_dsc.opa = glyph_draw_dsc->opa;
                    img_dsc.src = glyph_draw_dsc->glyph_data;
                    //lv_draw_sw_image(t, &img_dsc, glyph_draw_dsc->letter_coords);
#endif
                }
                break;
            default:
                break;
        }

        /* Drawing labels is a special case, because its draw buffer is shared
         * between all the glyphs, then using the global render bufeer to defer
         * its drawing later with other shapes, will corrupt the text drawn,
         * instead, pushes all the glyph commands and its dedicated draw buffer
         * to a specific render buffer, and draw it immediately while the contents
         * are valid.
         */
        d2_executerenderbuffer(unit->d2_handle, unit->label_renderbuffer, 0);
        d2_flushframe(unit->d2_handle);
        d2_selectrenderbuffer(unit->d2_handle, unit->renderbuffer);

#if LV_USE_OS
        status = lv_mutex_unlock(unit->pd2Mutex);
        LV_ASSERT(LV_RESULT_OK == status);
#endif
    }

    if(fill_draw_dsc && fill_area) {
        lv_draw_dave2d_fill_single(t, fill_draw_dsc, fill_area);
    }
}

#endif /*LV_USE_DRAW_DAVE2D*/
