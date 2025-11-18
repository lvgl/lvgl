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
#include "lv_opengles_driver.h"

#include "../../misc/lv_types.h"
#include "../../stdlib/lv_mem.h"
#include "lv_opengles_private.h"
#include "lv_opengles_texture_private.h"
#include "../../display/lv_display_private.h"

#include <stdlib.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_display_t * lv_opengles_texture_create_common(int32_t w, int32_t h);
static unsigned int create_texture(int32_t w, int32_t h);
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
    lv_display_t * disp = lv_opengles_texture_create_common(w, h);
    if(!disp) {
        LV_LOG_ERROR("Failed to create display");
        return NULL;
    }
    lv_opengles_texture_t * dsc = lv_display_get_driver_data(disp);
    unsigned int texture_id = create_texture(w, h);
    dsc->texture_id = texture_id;
    dsc->is_texture_owner = true;
    return disp;
}

lv_display_t * lv_opengles_texture_create_from_texture_id(int32_t w, int32_t h, unsigned int texture_id)
{
    lv_display_t * disp = lv_opengles_texture_create_common(w, h);
    if(!disp) {
        LV_LOG_ERROR("Failed to create display");
        return NULL;
    }
    lv_opengles_texture_t * dsc = lv_display_get_driver_data(disp);
    dsc->texture_id = texture_id;
    dsc->is_texture_owner = false;
    return disp;
}

void lv_opengles_texture_reshape(lv_display_t * disp, int32_t width, int32_t height)
{
    lv_opengles_texture_t * dsc = lv_display_get_driver_data(disp);
    unsigned int new_texture = create_texture(width, height);
    if(new_texture == GL_NONE) {
        LV_LOG_ERROR("Failed to reshape texture. Couldn't acquire new texture from GPU");
        return;
    }
#if !LV_USE_DRAW_OPENGLES
    uint32_t stride = lv_draw_buf_width_to_stride(width, lv_display_get_color_format(disp));
    uint32_t buf_size = stride * height;
    uint8_t * buffer = lv_realloc(dsc->fb1, buf_size);
    LV_ASSERT_MALLOC(buffer);
    if(!buffer) {
        GL_CALL(glDeleteTextures(1, &new_texture));
        LV_LOG_ERROR("Failed to reshape texture. Couldn't resize buffer");
        return;
    }
    dsc->fb1 = buffer;
#endif /*!LV_USE_DRAW_OPENGLES*/

    if(dsc->is_texture_owner && dsc->texture_id != 0) {
        GL_CALL(glDeleteTextures(1, &dsc->texture_id));
    }
    dsc->texture_id = new_texture;
}

lv_result_t lv_opengles_texture_create_draw_buffers(lv_opengles_texture_t * texture, lv_display_t * display)
{
    int32_t w = lv_display_get_horizontal_resolution(display);
    int32_t h = lv_display_get_vertical_resolution(display);

#if LV_USE_DRAW_OPENGLES
    LV_UNUSED(texture);
    static size_t LV_ATTRIBUTE_MEM_ALIGN dummy_buf;
    lv_display_set_buffers(display, &dummy_buf, NULL, w * h * 4, LV_DISPLAY_RENDER_MODE_DIRECT);
#else
    uint32_t stride = lv_draw_buf_width_to_stride(w, lv_display_get_color_format(display));
    uint32_t buf_size = stride * h;
    texture->fb1 = lv_malloc(buf_size);
    LV_ASSERT_MALLOC(texture->fb1);
    if(!texture->fb1) {
        return LV_RESULT_INVALID;
    }
    lv_display_set_buffers(display, texture->fb1, NULL, buf_size, LV_DISPLAY_RENDER_MODE_DIRECT);
#endif
    return LV_RESULT_OK;
}

void lv_opengles_texture_deinit(lv_opengles_texture_t * texture)
{
#if !LV_USE_DRAW_OPENGLES
    lv_free(texture->fb1);
#endif /*!LV_USE_DRAW_OPENGLES*/

    if(texture->is_texture_owner && texture->texture_id != 0) {
        GL_CALL(glDeleteTextures(1, &texture->texture_id));
    }
}

unsigned int lv_opengles_texture_get_texture_id(lv_display_t * disp)
{
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

static lv_display_t * lv_opengles_texture_create_common(int32_t w, int32_t h)
{
    lv_display_t * disp = lv_display_create(w, h);
    if(!disp) {
        LV_LOG_ERROR("Failed to create display");
        return NULL;
    }
    lv_opengles_texture_t * texture = lv_malloc_zeroed(sizeof(lv_opengles_texture_t));
    LV_ASSERT_MALLOC(texture);
    if(!texture) {
        LV_LOG_ERROR("Failed to create texture");
        lv_display_delete(disp);
        return NULL;
    }
    lv_result_t res = lv_opengles_texture_create_draw_buffers(texture, disp);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to create draw buffers");
        lv_free(texture);
        lv_display_delete(disp);
        return NULL;
    }
    lv_display_set_resolution(disp, w, h);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_driver_data(disp, texture);
    lv_display_add_event_cb(disp, release_disp_cb, LV_EVENT_DELETE, disp);

    lv_opengles_init();
    return disp;
}

static unsigned int create_texture(int32_t w, int32_t h)
{
    unsigned int texture;
    GL_CALL(glGenTextures(1, &texture));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    /* set the dimensions and format to complete the texture */
    /* Color depth: 16 (RGB565), 32 (XRGB8888) */
#if LV_COLOR_DEPTH == 16
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, w, h, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5,
                         NULL));
#elif LV_COLOR_DEPTH == 32
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));
#else
#error("Unsupported color format")
#endif

#if 0
    GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 20));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
#endif

    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));
    return texture;
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
        /*Color depth: 16 (RGB565), 32 (XRGB8888)*/
#if LV_COLOR_DEPTH == 16
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, disp->hor_res, disp->ver_res, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5,
                             dsc->fb1));
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
    lv_opengles_texture_t * texture = lv_display_get_driver_data(disp);
    lv_opengles_texture_deinit(texture);
    lv_free(texture);
}

#endif /*LV_USE_OPENGLES*/
