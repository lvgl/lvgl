/**
 * @file lv_opengles_texture.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_opengles_texture.h"
#if LV_USE_OPENGLES

#include "lv_opengles_debug.h"
#include "../../display/lv_display_private.h"
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    unsigned int texture_id;
    uint8_t * fb1;
} lv_opengles_texture_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
static void release_disp_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_display_t * lv_opengles_texture_create(int32_t w, int32_t h)
{
    lv_display_t * disp = lv_display_create(w, h);
    if(disp == NULL) {
        return NULL;
    }
    lv_opengles_texture_t * dsc = lv_malloc_zeroed(sizeof(lv_opengles_texture_t));
    LV_ASSERT_MALLOC(dsc);
    if(dsc == NULL) {
        lv_display_delete(disp);
        return NULL;
    }
    uint32_t stride = lv_draw_buf_width_to_stride(w, lv_display_get_color_format(disp));
    uint32_t buf_size = stride * h;
    dsc->fb1 = malloc(buf_size);
    if(dsc->fb1 == NULL) {
        lv_free(dsc);
        lv_display_delete(disp);
        return NULL;
    }

    GL_CALL(glGenTextures(1, &dsc->texture_id));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, dsc->texture_id));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    /* set the dimensions and format to complete the texture */
    /* Color depth: 8 (L8), 16 (RGB565), 24 (RGB888), 32 (XRGB8888) */
#if LV_COLOR_DEPTH == 8
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, disp->hor_res, disp->ver_res, 0, GL_RED, GL_UNSIGNED_BYTE, NULL));
#elif LV_COLOR_DEPTH == 16
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, disp->hor_res, disp->ver_res, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5,
                         NULL));
#elif LV_COLOR_DEPTH == 24
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, disp->hor_res, disp->ver_res, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL));
#elif LV_COLOR_DEPTH == 32
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, disp->hor_res, disp->ver_res, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));
#else
#error("Unsupported color format")
#endif

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 20);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    lv_display_set_buffers(disp, dsc->fb1, NULL, buf_size, LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_driver_data(disp, dsc);
    lv_display_add_event_cb(disp, release_disp_cb, LV_EVENT_DELETE, disp);

    return disp;
}

unsigned int lv_opengles_texture_get_texture_id(lv_display_t * disp)
{
    if(disp->flush_cb != flush_cb) {
        return 0;
    }
    lv_opengles_texture_t * dsc = lv_display_get_driver_data(disp);
    return dsc->texture_id;
}

lv_display_t * lv_opengles_texture_get_from_texture_id(unsigned int texture_id)
{
    lv_display_t * disp = NULL;
    while(NULL != (disp = lv_display_get_next(disp))) {
        unsigned int disp_texture_id = lv_opengles_texture_get_texture_id(disp);
        if(disp_texture_id == texture_id) {
            return disp;
        }
    }
    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(area);
    LV_UNUSED(px_map);

#if !LV_USE_DRAW_OPENGLES
    if(lv_display_flush_is_last(disp)) {

        lv_opengles_texture_t * dsc = lv_display_get_driver_data(disp);
        lv_color_format_t cf = lv_display_get_color_format(disp);
        uint32_t stride = lv_draw_buf_width_to_stride(lv_display_get_horizontal_resolution(disp), cf);

        GL_CALL(glBindTexture(GL_TEXTURE_2D, dsc->texture_id));

        GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
        GL_CALL(glPixelStorei(GL_UNPACK_ROW_LENGTH, stride / lv_color_format_get_size(cf)));
        /*Color depth: 8 (L8), 16 (RGB565), 24 (RGB888), 32 (XRGB8888)*/
#if LV_COLOR_DEPTH == 8
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, disp->hor_res, disp->ver_res, 0, GL_RED, GL_UNSIGNED_BYTE, dsc->fb1));
#elif LV_COLOR_DEPTH == 16
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, disp->hor_res, disp->ver_res, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5,
                             dsc->fb1));
#elif LV_COLOR_DEPTH == 24
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, disp->hor_res, disp->ver_res, 0, GL_BGR, GL_UNSIGNED_BYTE, dsc->fb1));
#elif LV_COLOR_DEPTH == 32
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, disp->hor_res, disp->ver_res, 0, GL_BGRA, GL_UNSIGNED_BYTE, dsc->fb1));
#else
#error("Unsupported color format")
#endif
    }
#endif /* !LV_USE_DRAW_OPENGLES */

    lv_display_flush_ready(disp);
}

static void release_disp_cb(lv_event_t * e)
{
    lv_display_t * disp = lv_event_get_user_data(e);
    lv_opengles_texture_t * dsc = lv_display_get_driver_data(disp);
    free(dsc->fb1);
    GL_CALL(glDeleteTextures(1, &dsc->texture_id));
    lv_free(dsc);
}

#endif /*LV_USE_OPENGLES*/
