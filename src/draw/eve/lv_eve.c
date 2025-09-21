/**
 * @file lv_eve.c
 *
 */

/*  Created on: 8 jun 2023
 *      Author: juanj
 *
 *  Modified by LVGL
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_eve.h"
#if LV_USE_DRAW_EVE
#include "lv_eve.h"
#include "../../libs/FT800-FT813/EVE_commands.h"


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
 *  STATIC VARIABLES
 **********************/

static uint16_t scissor_x1 = 0;
static uint16_t scissor_y1 = 0;
static uint16_t scissor_x2 = 0;
static uint16_t scissor_y2 = 0;

static lv_eve_drawing_context_t ct = {
    .primitive = LV_EVE_PRIMITIVE_ZERO_VALUE,
    .color = {0xff, 0xff, 0xff},
    .opa = 255,
    .line_width = 1,  /* for format(0) */
    .point_size = 1,
    .color_mask = {1, 1, 1, 1},
    .stencil_func = {EVE_ALWAYS, 0, 255},
    .stencil_op = {EVE_KEEP, EVE_KEEP},
    .blend_func = {EVE_SRC_ALPHA, EVE_ONE_MINUS_SRC_ALPHA},
    .scx = 0,
    .scy = 0,
};

static lv_eve_drawing_context_t ct_temp;

static lv_eve_drawing_state_t st;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_eve_save_context(void)
{
    EVE_cmd_dl_burst(DL_SAVE_CONTEXT);
    ct_temp = ct;
}

void lv_eve_restore_context(void)
{
    EVE_cmd_dl_burst(DL_RESTORE_CONTEXT);
    ct = ct_temp;
}


void lv_eve_primitive(uint8_t context)
{
    if(context != ct.primitive && context != LV_EVE_PRIMITIVE_ZERO_VALUE) {
        EVE_cmd_dl_burst(DL_BEGIN | context);
        ct.primitive = context;
    }
}

void lv_eve_scissor(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    if(x1 != scissor_x1 || y1 != scissor_y1) {
        int16_t adjusted_x1 = x1 > 0 ? x1 - 1 : 0;
        int16_t adjusted_y1 = y1 > 0 ? y1 - 1 : 0;
        EVE_cmd_dl_burst(SCISSOR_XY(adjusted_x1, adjusted_y1));
        scissor_x1 = x1;
        scissor_y1 = y1;
    }

    if(x2 != scissor_x2 || y2 != scissor_y2) {
        uint16_t w = x2 - x1 + 3;
        uint16_t h = y2 - y1 + 3;
        EVE_cmd_dl_burst(SCISSOR_SIZE(w, h));
        scissor_x2 = x2;
        scissor_y2 = y2;
    }
}

void lv_eve_color(lv_color_t color)
{
    if((ct.color.red != color.red) || (ct.color.green != color.green) || (ct.color.blue != color.blue)) {
        EVE_cmd_dl_burst(COLOR_RGB(color.red, color.green, color.blue));
        ct.color = color;
    }
}

void lv_eve_color_mask(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    if((ct.color_mask[0] != r) ||
       (ct.color_mask[1] != g) ||
       (ct.color_mask[2] != b) ||
       (ct.color_mask[3] != a)) {

        EVE_cmd_dl_burst(COLOR_MASK(r, g, b, a));
        ct.color_mask[0] = r;
        ct.color_mask[1] = g;
        ct.color_mask[2] = b;
        ct.color_mask[3] = a;
    }
}

void lv_eve_stencil_func(uint8_t func, uint8_t ref, uint8_t mask)
{
    if(func != ct.stencil_func[0] || ref != ct.stencil_func[1] || mask != ct.stencil_func[2]) {

        EVE_cmd_dl_burst(STENCIL_FUNC(func, ref, mask));
        ct.stencil_func[0] = func;
        ct.stencil_func[1] = ref;
        ct.stencil_func[2] = mask;
    }
}

void lv_eve_stencil_op(uint8_t sfail, uint8_t spass)
{
    if(sfail != ct.stencil_op[0] || spass != ct.stencil_op[1]) {
        EVE_cmd_dl_burst(STENCIL_OP(sfail, spass));
        ct.stencil_op[0] = sfail;
        ct.stencil_op[1] = spass;

    }
}

void lv_eve_blend_func(uint8_t src, uint8_t dst)
{
    if(src != ct.blend_func[0] || dst != ct.blend_func[1]) {
        EVE_cmd_dl_burst(BLEND_FUNC(src, dst));
        ct.blend_func[0] = src;
        ct.blend_func[1] = dst;
    }
}

void lv_eve_color_opa(lv_opa_t opa)
{
    if(opa != ct.opa) {
        EVE_cmd_dl_burst(COLOR_A(opa));
        ct.opa = opa;
    }
}

void lv_eve_line_width(int32_t width)
{
    if(width != ct.line_width) {
        EVE_cmd_dl_burst(LINE_WIDTH(width));
        ct.line_width = width;
    }
}

void lv_eve_point_size(uint16_t radius)
{
    if(radius != ct.point_size) {
        EVE_cmd_dl_burst(POINT_SIZE(radius * 16));
        ct.point_size = radius;
    }
}

void lv_eve_vertex_2f(int16_t x, int16_t y)
{
    EVE_cmd_dl_burst(VERTEX2F(x, y));
}

void lv_eve_draw_circle_simple(int16_t coord_x1, int16_t coord_y1, uint16_t radius_t)
{
    lv_eve_primitive(LV_EVE_PRIMITIVE_POINTS);
    lv_eve_point_size(radius_t);
    lv_eve_vertex_2f(coord_x1, coord_y1);
}


void lv_eve_draw_rect_simple(int16_t coord_x1, int16_t coord_y1, int16_t coord_x2, int16_t coord_y2, uint16_t radius)
{
    lv_eve_primitive(LV_EVE_PRIMITIVE_RECTS);
    if(radius > 1) {
        lv_eve_line_width(radius * 16);
    }

    lv_eve_vertex_2f(coord_x1 + radius, coord_y1 + radius);
    lv_eve_vertex_2f(coord_x2 - radius, coord_y2 - radius);
}

void lv_eve_mask_round(int16_t coord_x1, int16_t coord_y1, int16_t coord_x2, int16_t coord_y2, int16_t radius)
{
    lv_eve_color_mask(0, 0, 0, 1);
    EVE_cmd_dl_burst(CLEAR(1, 1, 1));


    lv_eve_draw_rect_simple(coord_x1, coord_y1, coord_x2, coord_y2, radius);
    lv_eve_color_mask(1, 1, 1, 0);
    lv_eve_blend_func(EVE_DST_ALPHA, EVE_ONE_MINUS_DST_ALPHA);
}

void lv_eve_bitmap_source(uint32_t addr)
{
    uint32_t bitmap_source = BITMAP_SOURCE(addr);
    if(st.bitmap_source != bitmap_source) {
        EVE_cmd_dl_burst(bitmap_source);
        st.bitmap_source = bitmap_source;
    }
}

void lv_eve_bitmap_size(uint8_t filter, uint8_t wrapx, uint8_t wrapy, uint16_t width, uint16_t height)
{
    uint32_t bitmap_size = BITMAP_SIZE(filter, wrapx, wrapy, width, height);
    if(st.bitmap_size != bitmap_size) {
        EVE_cmd_dl_burst(bitmap_size);
        st.bitmap_size = bitmap_size;
    }
    /* set the high bits too, of the width and height */
    uint32_t bitmap_size_h = BITMAP_SIZE_H(width, height);
    if(st.bitmap_size_h != bitmap_size_h) {
        EVE_cmd_dl_burst(bitmap_size_h);
        st.bitmap_size_h = bitmap_size_h;
    }
}

void lv_eve_bitmap_layout(uint8_t format, uint16_t linestride, uint16_t height)
{
    uint32_t bitmap_layout = BITMAP_LAYOUT(format, linestride, height);
    if(st.bitmap_layout != bitmap_layout) {
        EVE_cmd_dl_burst(bitmap_layout);
        st.bitmap_layout = bitmap_layout;
    }
    /* set the high bits too, of the linestride and height */
    uint32_t bitmap_layout_h = BITMAP_LAYOUT_H(linestride, height);
    if(st.bitmap_layout_h != bitmap_layout_h) {
        EVE_cmd_dl_burst(bitmap_layout_h);
        st.bitmap_layout_h = bitmap_layout_h;
    }
}



/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_EVE*/
