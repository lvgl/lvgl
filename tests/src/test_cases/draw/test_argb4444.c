#if LV_BUILD_TEST
#include "../lvgl.h"
#include "../../lvgl_private.h"
#include "unity/unity.h"

void setUp(void)
{
    lv_obj_clean(lv_screen_active());
}

void tearDown(void)
{
}

void test_argb4444_color_encoding(void)
{
    lv_color32_t color = lv_color_to_32(lv_color_make(0xa7, 0x5e, 0x26), 0xcd);
    TEST_ASSERT_EQUAL_HEX16(0xca52, lv_color_to_argb4444(color));

    color = lv_color_from_argb4444(0xca52);
    TEST_ASSERT_EQUAL_HEX8(0xcc, color.alpha);
    TEST_ASSERT_EQUAL_HEX8(0xaa, color.red);
    TEST_ASSERT_EQUAL_HEX8(0x55, color.green);
    TEST_ASSERT_EQUAL_HEX8(0x22, color.blue);

    /* Every packed color, including all transparent RGB values, is lossless on expansion. */
    for(uint32_t pixel = 0; pixel <= UINT16_MAX; pixel++) {
        TEST_ASSERT_EQUAL_HEX16(pixel, lv_color_to_argb4444(lv_color_from_argb4444((uint16_t)pixel)));
    }
}

#if LV_DRAW_SW_SUPPORT_ARGB4444 && LV_USE_CANVAS
/* Wider than two tiles, with different padding for the color and mask rows. */
#define WIDTH 73
#define HEIGHT 4
#define PACKED_STRIDE (2 * WIDTH + 3)
#define STRIDE32 (4 * WIDTH + 8)
#define MASK_STRIDE (WIDTH + 5)
#define PAD_BYTE 0xa5

static lv_draw_buf_t * make_buffer(lv_color_format_t format, uint32_t stride)
{
    lv_draw_buf_t * buf = lv_draw_buf_create(WIDTH, HEIGHT, format, stride);
    TEST_ASSERT_NOT_NULL(buf);
    lv_memset(buf->data, PAD_BYTE, stride * HEIGHT);
    return buf;
}

static void store_pixel(lv_draw_buf_t * buf, int32_t x, int32_t y, uint16_t pixel)
{
    uint8_t * row = buf->data + y * buf->header.stride;
    if(buf->header.cf == LV_COLOR_FORMAT_ARGB4444) {
        row[2 * x] = (uint8_t)pixel;
        row[2 * x + 1] = (uint8_t)(pixel >> 8);
    }
    else {
        ((lv_color32_t *)row)[x] = lv_color_from_argb4444(pixel);
    }
}

static void init_pixels(lv_draw_buf_t * buf, const uint16_t * samples, uint32_t sample_count)
{
    for(int32_t y = 0; y < HEIGHT; y++) {
        for(int32_t x = 0; x < WIDTH; x++) {
            store_pixel(buf, x, y, samples[(x + 2 * y) % sample_count]);
        }
    }
}

static lv_layer_t make_layer(lv_draw_buf_t * buf)
{
    lv_layer_t layer = {0};
    layer.draw_buf = buf;
    layer.color_format = buf->header.cf;
    lv_area_set(&layer.buf_area, 0, 0, WIDTH - 1, HEIGHT - 1);
    layer._clip_area = layer.buf_area;
    return layer;
}

static void assert_packed_matches(const lv_draw_buf_t * packed, const lv_draw_buf_t * reference)
{
    for(int32_t y = 0; y < HEIGHT; y++) {
        const uint8_t * row = packed->data + y * packed->header.stride;
        const lv_color32_t * ref = (const lv_color32_t *)(reference->data + y * reference->header.stride);
        for(int32_t x = 0; x < WIDTH; x++) {
            uint16_t pixel = (uint16_t)(row[2 * x] | (row[2 * x + 1] << 8));
            TEST_ASSERT_EQUAL_HEX16(lv_color_to_argb4444(ref[x]), pixel);
        }
        for(uint32_t x = 2 * WIDTH; x < packed->header.stride; x++) {
            TEST_ASSERT_EQUAL_HEX8(PAD_BYTE, row[x]);
        }
    }
}

static void check_masked_blending(bool image, bool packed_source, bool packed_destination,
                                  lv_blend_mode_t mode)
{
    const uint16_t src_colors[] = {0x0f00, 0xf042, 0x88be, 0x4135, 0xfed0};
    const uint16_t dest_colors[] = {0x0000, 0xf39d, 0x8731};
    lv_draw_buf_t * src = make_buffer(packed_source ? LV_COLOR_FORMAT_ARGB4444 : LV_COLOR_FORMAT_ARGB8888,
                                      packed_source ? PACKED_STRIDE : STRIDE32);
    lv_draw_buf_t * dest = make_buffer(packed_destination ? LV_COLOR_FORMAT_ARGB4444 : LV_COLOR_FORMAT_ARGB8888,
                                       packed_destination ? PACKED_STRIDE : STRIDE32);
    lv_draw_buf_t * src_ref = make_buffer(LV_COLOR_FORMAT_ARGB8888, STRIDE32);
    lv_draw_buf_t * dest_ref = make_buffer(LV_COLOR_FORMAT_ARGB8888, STRIDE32);
    init_pixels(src, src_colors, sizeof(src_colors) / sizeof(src_colors[0]));
    init_pixels(src_ref, src_colors, sizeof(src_colors) / sizeof(src_colors[0]));
    init_pixels(dest, dest_colors, sizeof(dest_colors) / sizeof(dest_colors[0]));
    init_pixels(dest_ref, dest_colors, sizeof(dest_colors) / sizeof(dest_colors[0]));

    const lv_opa_t opacities[] = {LV_OPA_TRANSP, LV_OPA_30, LV_OPA_COVER, LV_OPA_70};
    lv_opa_t mask[HEIGHT][MASK_STRIDE];
    for(int32_t y = 0; y < HEIGHT; y++) {
        for(int32_t x = 0; x < MASK_STRIDE; x++) {
            mask[y][x] = opacities[(x + y) % (sizeof(opacities) / sizeof(opacities[0]))];
        }
    }

    lv_layer_t layer = make_layer(dest);
    lv_layer_t reference = make_layer(dest_ref);
    const lv_area_t src_area = {1, 0, WIDTH, HEIGHT - 1};
    const lv_area_t clip = {3, 1, WIDTH - 2, HEIGHT - 1};
    const lv_area_t mask_area = {0, 0, WIDTH - 1, HEIGHT - 1};
    lv_draw_sw_blend_dsc_t blend = {
        .blend_area = &clip,
        .src_area = &src_area,
        .src_buf = image ? src->data : NULL,
        .src_stride = src->header.stride,
        .src_color_format = src->header.cf,
        .color = lv_color_make(0xdd, 0x88, 0x11),
        .opa = LV_OPA_70,
        .mask_buf = &mask[0][0],
        .mask_area = &mask_area,
        .mask_stride = MASK_STRIDE,
        .mask_res = LV_DRAW_SW_MASK_RES_CHANGED,
        .blend_mode = mode,
    };
    lv_draw_sw_blend(&layer, &clip, &blend);
    blend.src_buf = image ? src_ref->data : NULL;
    blend.src_stride = src_ref->header.stride;
    blend.src_color_format = src_ref->header.cf;
    lv_draw_sw_blend(&reference, &clip, &blend);

    if(packed_destination) {
        assert_packed_matches(dest, dest_ref);
    }
    else {
        TEST_ASSERT_EQUAL_MEMORY(dest_ref->data, dest->data, STRIDE32 * HEIGHT);
    }

    lv_draw_buf_destroy(src);
    lv_draw_buf_destroy(dest);
    lv_draw_buf_destroy(src_ref);
    lv_draw_buf_destroy(dest_ref);
}

static void check_transformed_image(bool antialias, bool recolor, bool colorkey)
{
    const uint16_t colors[] = {0x0f00, 0xf09e, 0x8137, 0xfed0};
    const uint16_t background[] = {0xf48d};
    lv_draw_buf_t * src = make_buffer(LV_COLOR_FORMAT_ARGB4444, PACKED_STRIDE);
    lv_draw_buf_t * src_ref = make_buffer(LV_COLOR_FORMAT_ARGB8888, STRIDE32);
    lv_draw_buf_t * dest = make_buffer(LV_COLOR_FORMAT_ARGB8888, STRIDE32);
    lv_draw_buf_t * dest_ref = make_buffer(LV_COLOR_FORMAT_ARGB8888, STRIDE32);
    init_pixels(src, colors, sizeof(colors) / sizeof(colors[0]));
    init_pixels(src_ref, colors, sizeof(colors) / sizeof(colors[0]));
    init_pixels(dest, background, 1);
    init_pixels(dest_ref, background, 1);

    lv_layer_t layer = make_layer(dest);
    lv_layer_t reference = make_layer(dest_ref);
    const lv_area_t coords = {3, 1, WIDTH + 2, HEIGHT};
    const lv_area_t clip = {4, 0, WIDTH - 1, HEIGHT - 1};
    const lv_image_colorkey_t key = {
        .low = lv_color_hex(0xeedd00),
        .high = lv_color_hex(0xeedd00),
    };
    lv_draw_image_dsc_t draw;
    lv_draw_image_dsc_init(&draw);
    draw.src = src;
    draw.antialias = antialias;
    draw.rotation = recolor ? 0 : 120;
    draw.scale_x = recolor ? LV_SCALE_NONE : LV_SCALE_NONE + 32;
    draw.pivot.x = WIDTH / 2;
    draw.pivot.y = HEIGHT / 2;
    draw.recolor = lv_color_hex(0x7744bb);
    draw.recolor_opa = recolor ? LV_OPA_40 : LV_OPA_TRANSP;
    draw.colorkey = colorkey ? &key : NULL;
    lv_draw_sw_image(&layer, &clip, &draw, &coords);
    draw.src = src_ref;
    lv_draw_sw_image(&reference, &clip, &draw, &coords);

    TEST_ASSERT_EQUAL_MEMORY(dest_ref->data, dest->data, STRIDE32 * HEIGHT);
    bool drew_pixels = false;
    for(int32_t y = 0; y < HEIGHT; y++) {
        const lv_color32_t * row = (const lv_color32_t *)(dest->data + y * dest->header.stride);
        for(int32_t x = 0; x < WIDTH; x++) {
            if(lv_color_to_argb4444(row[x]) != background[0]) drew_pixels = true;
        }
    }
    TEST_ASSERT_TRUE(drew_pixels);
    lv_draw_buf_destroy(src);
    lv_draw_buf_destroy(src_ref);
    lv_draw_buf_destroy(dest);
    lv_draw_buf_destroy(dest_ref);
}

#if LV_DRAW_SW_SUPPORT_RGB565 && LV_DRAW_SW_SUPPORT_XRGB8888
static void check_opaque_target(lv_color_format_t format, uint32_t stride)
{
    const uint16_t colors[] = {0x0000, 0xf0b2, 0x882e, 0x4eda, 0xffff};
    lv_draw_buf_t * src = make_buffer(LV_COLOR_FORMAT_ARGB4444, PACKED_STRIDE);
    lv_draw_buf_t * src_ref = make_buffer(LV_COLOR_FORMAT_ARGB8888, STRIDE32);
    lv_draw_buf_t * dest = make_buffer(format, stride);
    lv_draw_buf_t * dest_ref = make_buffer(format, stride);
    init_pixels(src, colors, sizeof(colors) / sizeof(colors[0]));
    init_pixels(src_ref, colors, sizeof(colors) / sizeof(colors[0]));

    lv_layer_t layer = make_layer(dest);
    lv_layer_t reference = make_layer(dest_ref);
    const lv_area_t coords = {0, 0, WIDTH - 1, HEIGHT - 1};
    const lv_area_t clip = {5, 0, WIDTH - 1, HEIGHT - 1};
    lv_draw_image_dsc_t draw;
    lv_draw_image_dsc_init(&draw);
    draw.opa = LV_OPA_80;
    draw.src = src;
    lv_draw_sw_image(&layer, &clip, &draw, &coords);
    draw.src = src_ref;
    lv_draw_sw_image(&reference, &clip, &draw, &coords);

    TEST_ASSERT_EQUAL_MEMORY(dest_ref->data, dest->data, stride * HEIGHT);
    uint32_t byte_offset = 6 * lv_color_format_get_size(format);
    TEST_ASSERT_NOT_EQUAL(PAD_BYTE, dest->data[byte_offset]);
    lv_draw_buf_destroy(src);
    lv_draw_buf_destroy(src_ref);
    lv_draw_buf_destroy(dest);
    lv_draw_buf_destroy(dest_ref);
}
#endif

static uint32_t display_flush_count;

static void argb4444_flush(lv_display_t * display, const lv_area_t * area, uint8_t * pixels)
{
    LV_UNUSED(area);
    LV_UNUSED(pixels);
    display_flush_count++;
    lv_display_flush_ready(display);
}
#endif

void test_argb4444_blending_with_clipping_and_padded_rows(void)
{
#if LV_DRAW_SW_SUPPORT_ARGB4444 && LV_USE_CANVAS
    check_masked_blending(false, false, true, LV_BLEND_MODE_NORMAL);
    check_masked_blending(true, true, false, LV_BLEND_MODE_NORMAL);
    check_masked_blending(true, false, true, LV_BLEND_MODE_NORMAL);
    check_masked_blending(true, true, true, LV_BLEND_MODE_NORMAL);
    check_masked_blending(true, true, true, LV_BLEND_MODE_ADDITIVE);
#else
    TEST_IGNORE_MESSAGE("ARGB4444 software rendering is disabled");
#endif
}

void test_argb4444_transform_recolor_and_colorkey(void)
{
#if LV_DRAW_SW_SUPPORT_ARGB4444 && LV_USE_CANVAS
    check_transformed_image(false, false, false);
    check_transformed_image(true, false, false);
    check_transformed_image(false, true, false);
    check_transformed_image(false, true, true);
#else
    TEST_IGNORE_MESSAGE("ARGB4444 software rendering is disabled");
#endif
}

void test_argb4444_canvas_pixel_and_draw_layer(void)
{
#if LV_DRAW_SW_SUPPORT_ARGB4444 && LV_USE_CANVAS
    uint32_t stride = PACKED_STRIDE;
#if LV_USE_DRAW_VG_LITE
    /* Canvas tasks can be taken by VG-Lite, which requires its calculated stride. */
    stride = lv_draw_buf_width_to_stride(WIDTH, LV_COLOR_FORMAT_ARGB4444);
#endif
    lv_draw_buf_t * buf = make_buffer(LV_COLOR_FORMAT_ARGB4444, stride);
    lv_obj_t * canvas = lv_canvas_create(lv_screen_active());
    lv_canvas_set_draw_buf(canvas, buf);
    lv_canvas_fill_bg(canvas, lv_color_make(0x12, 0x34, 0x56), 0x89);
    TEST_ASSERT_EQUAL_HEX8(0x35, buf->data[0]);
    TEST_ASSERT_EQUAL_HEX8(0x81, buf->data[1]);
    if(stride > 2 * WIDTH) TEST_ASSERT_EQUAL_HEX8(PAD_BYTE, buf->data[2 * WIDTH]);
    lv_canvas_set_px(canvas, 2, 1, lv_color_hex(0xa05020), 0xc0);
    lv_color32_t pixel = lv_canvas_get_px(canvas, 2, 1);
    TEST_ASSERT_EQUAL_HEX8(0xcc, pixel.alpha);
    TEST_ASSERT_EQUAL_HEX8(0xaa, pixel.red);
    TEST_ASSERT_EQUAL_HEX8(0x55, pixel.green);
    TEST_ASSERT_EQUAL_HEX8(0x22, pixel.blue);

    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);
    lv_draw_rect_dsc_t rect;
    lv_draw_rect_dsc_init(&rect);
    /* Exactly representable in four bits, independent of renderer quantization. */
    rect.bg_color = lv_color_hex(0x44ffbb);
    const lv_area_t rect_area = {2, 1, WIDTH - 1, 1};
    lv_draw_rect(&layer, &rect, &rect_area);
    lv_canvas_finish_layer(canvas, &layer);
    pixel = lv_canvas_get_px(canvas, WIDTH - 1, 1);
    TEST_ASSERT_EQUAL_HEX16(0xf4fb, lv_color_to_argb4444(pixel));
    TEST_ASSERT_EQUAL_HEX16(0x8135, lv_color_to_argb4444(lv_canvas_get_px(canvas, 1, 1)));
    lv_obj_delete(canvas);
    lv_draw_buf_destroy(buf);
#else
    TEST_IGNORE_MESSAGE("ARGB4444 software rendering is disabled");
#endif
}

void test_argb4444_source_on_opaque_displays(void)
{
#if LV_DRAW_SW_SUPPORT_ARGB4444 && LV_USE_CANVAS && LV_DRAW_SW_SUPPORT_RGB565 && LV_DRAW_SW_SUPPORT_XRGB8888
    check_opaque_target(LV_COLOR_FORMAT_RGB565, 2 * WIDTH + 4);
    check_opaque_target(LV_COLOR_FORMAT_XRGB8888, STRIDE32);
#else
    TEST_IGNORE_MESSAGE("Required software rendering formats are disabled");
#endif
}

void test_argb4444_layer_bitmap_mask_updates_only_alpha(void)
{
#if LV_DRAW_SW_SUPPORT_ARGB4444 && LV_USE_CANVAS && LV_DRAW_SW_SUPPORT_A8
    const uint16_t foreground[] = {0xf123};
    const uint16_t background[] = {0};
    const lv_opa_t mask_values[] = {LV_OPA_TRANSP, 128, LV_OPA_COVER};
    const uint8_t expected_alpha[] = {0, 119, 255};
    lv_draw_buf_t * src = make_buffer(LV_COLOR_FORMAT_ARGB4444, PACKED_STRIDE);
    lv_draw_buf_t * dest = make_buffer(LV_COLOR_FORMAT_ARGB8888, STRIDE32);
    lv_draw_buf_t * mask = make_buffer(LV_COLOR_FORMAT_A8, MASK_STRIDE);
    init_pixels(src, foreground, 1);
    init_pixels(dest, background, 1);
    for(int32_t y = 0; y < HEIGHT; y++) {
        for(int32_t x = 0; x < WIDTH; x++) {
            mask->data[y * MASK_STRIDE + x] = mask_values[(x + y) % 3];
        }
    }

    lv_layer_t source_layer = make_layer(src);
    lv_layer_t dest_layer = make_layer(dest);
    lv_image_dsc_t mask_image = {
        .header = mask->header,
        .data_size = mask->data_size,
        .data = mask->data,
    };
    mask_image.header.flags = 0;
    lv_draw_image_dsc_t draw;
    lv_draw_image_dsc_init(&draw);
    draw.src = &source_layer;
    draw.bitmap_mask_src = &mask_image;
    draw.image_area = source_layer.buf_area;
    lv_draw_sw_layer(&dest_layer, &dest_layer.buf_area, &draw, &source_layer.buf_area);

    for(int32_t y = 0; y < HEIGHT; y++) {
        const uint8_t * packed = src->data + y * src->header.stride;
        const lv_color32_t * row = (const lv_color32_t *)(dest->data + y * STRIDE32);
        for(int32_t x = 0; x < WIDTH; x++) {
            TEST_ASSERT_EQUAL_HEX8(0x23, packed[2 * x]);
            TEST_ASSERT_EQUAL_HEX8(1, packed[2 * x + 1] & 0xf);
            TEST_ASSERT_EQUAL_HEX8(expected_alpha[(x + y) % 3], row[x].alpha);
        }
        if(src->header.stride == PACKED_STRIDE) {
            TEST_ASSERT_EQUAL_HEX8(PAD_BYTE, packed[2 * WIDTH]);
        }
    }
    lv_draw_buf_destroy(mask);
    lv_draw_buf_destroy(src);
    lv_draw_buf_destroy(dest);
#else
    TEST_IGNORE_MESSAGE("Required software rendering formats are disabled");
#endif
}

void test_argb4444_display_clears_transparent_pixels_before_refresh(void)
{
#if LV_DRAW_SW_SUPPORT_ARGB4444 && LV_USE_CANVAS
    lv_display_t * display = lv_display_create(WIDTH, HEIGHT);
    TEST_ASSERT_NOT_NULL(display);
#if LV_USE_SYSMON && LV_USE_PERF_MONITOR
    lv_sysmon_hide_performance(display);
#endif
#if LV_USE_SYSMON && LV_USE_MEM_MONITOR
    lv_sysmon_hide_memory(display);
#endif
    lv_display_set_color_format(display, LV_COLOR_FORMAT_ARGB4444);
    lv_display_set_render_mode(display, LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(display, argb4444_flush);
    lv_draw_buf_t * buf = make_buffer(LV_COLOR_FORMAT_ARGB4444,
                                      lv_draw_buf_width_to_stride(WIDTH, LV_COLOR_FORMAT_ARGB4444));
    lv_display_set_draw_buffers(display, buf, NULL);
    lv_obj_t * screen = lv_display_get_screen_active(display);
    lv_obj_remove_style_all(screen);
    lv_obj_set_style_bg_opa(screen, LV_OPA_TRANSP, 0);
    lv_obj_t * rect = lv_obj_create(screen);
    lv_obj_remove_style_all(rect);
    lv_obj_set_size(rect, 35, 2);
    lv_obj_set_style_bg_opa(rect, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(rect, lv_color_hex(0x11aa77), 0);

    lv_obj_t * layered_rect = lv_obj_create(screen);
    lv_obj_remove_style_all(layered_rect);
    lv_obj_set_pos(layered_rect, 0, 2);
    lv_obj_set_size(layered_rect, 4, 2);
    lv_obj_set_style_bg_opa(layered_rect, LV_OPA_COVER, 0);
    /* Black has the same channels in straight and premultiplied output. */
    lv_obj_set_style_bg_color(layered_rect, lv_color_black(), 0);
    lv_obj_set_style_opa_layered(layered_rect, 0x9a, 0);

    display_flush_count = 0;
    lv_refr_now(display);

    const uint8_t * layered_pixel = buf->data + 2 * buf->header.stride;
    const uint8_t actual[] = {
        buf->data[34 * 2], buf->data[34 * 2 + 1],
        buf->data[35 * 2], buf->data[35 * 2 + 1],
        layered_pixel[0], layered_pixel[1]
    };
    lv_display_delete(display);
    lv_draw_buf_destroy(buf);

    const uint8_t expected[] = {0xa7, 0xf1, 0, 0, 0, 0x90};
    TEST_ASSERT_GREATER_THAN_UINT32(0, display_flush_count);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, actual, sizeof(expected));
#else
    TEST_IGNORE_MESSAGE("ARGB4444 software rendering is disabled");
#endif
}

void test_argb4444_bin_decoder_preserves_packed_pixels(void)
{
#if LV_USE_FS_MEMFS
    static const uint8_t bin[] = {
        LV_IMAGE_HEADER_MAGIC, LV_COLOR_FORMAT_ARGB4444, 0, 0,
        2, 0, 2, 0, 6, 0, 0, 0,
        0x23, 0xf1, 0x56, 0x84, 0xa5, 0xa5,
        0x89, 0x07, 0xbc, 0xda, 0xa5, 0xa5,
    };
    lv_fs_path_ex_t path;
    lv_fs_make_path_from_buffer(&path, LV_FS_MEMFS_LETTER, bin, sizeof(bin), "bin");
    lv_image_decoder_dsc_t decoder;
    const lv_image_decoder_args_t args = {.no_cache = true};
    TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_image_decoder_open(&decoder, &path, &args));
    const lv_area_t image_area = {0, 0, 1, 1};
    lv_area_t decoded_area = {.y1 = LV_COORD_MIN};
    for(int32_t y = 0; y < 2; y++) {
        if(!decoder.decoded || decoded_area.y1 != LV_COORD_MIN) {
            TEST_ASSERT_EQUAL(LV_RESULT_OK, lv_image_decoder_get_area(&decoder, &image_area, &decoded_area));
        }
        TEST_ASSERT_NOT_NULL(decoder.decoded);
        TEST_ASSERT_EQUAL(LV_COLOR_FORMAT_ARGB4444, decoder.decoded->header.cf);
        const uint8_t * row = decoder.decoded->data;
        if(decoded_area.y1 == LV_COORD_MIN) row += y * decoder.decoded->header.stride;
        TEST_ASSERT_EQUAL_MEMORY(bin + 12 + y * 6, row, 4);
    }
    lv_image_decoder_close(&decoder);
#else
    TEST_IGNORE_MESSAGE("Memory file system is disabled");
#endif
}
#endif
