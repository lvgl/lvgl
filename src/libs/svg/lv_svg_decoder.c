/**
 * @file lv_svg_decoder.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../draw/lv_image_decoder_private.h"
#include "../../../lvgl.h"

#if LV_USE_SVG
#include "lv_svg_decoder.h"

#include "lv_svg.h"
#include "../../draw/lv_draw_buf_private.h"
#include "../../display/lv_display_private.h"

/*********************
 *      DEFINES
 *********************/

#define DECODER_NAME    "SVG"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_result_t svg_decoder_info(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * src,
                                    lv_image_header_t * header);
static lv_result_t svg_decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static void svg_decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static uint8_t * alloc_file(const char * filename, uint32_t * size);
static void svg_draw_buf_free(void * svg_buf);

static void svg_draw(lv_layer_t * layer, const lv_image_decoder_dsc_t * dsc, const lv_area_t * coords,
                     const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * clip_area);
/**********************
 *  STATIC VARIABLES
 **********************/
static struct _lv_draw_buf_handlers_t _svg_draw_buf_handler = {
    .buf_free_cb = svg_draw_buf_free,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Register the SVG decoder functions in LVGL
 */
void lv_svg_decoder_init(void)
{
    lv_image_decoder_t * dec = lv_image_decoder_create();
    lv_image_decoder_set_info_cb(dec, svg_decoder_info);
    lv_image_decoder_set_open_cb(dec, svg_decoder_open);
    lv_image_decoder_set_close_cb(dec, svg_decoder_close);

    dec->name = DECODER_NAME;
}

void lv_svg_decoder_deinit(void)
{
    lv_image_decoder_t * dec = NULL;
    while((dec = lv_image_decoder_get_next(dec)) != NULL) {
        if(dec->info_cb == svg_decoder_info) {
            lv_image_decoder_delete(dec);
            break;
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static bool valid_svg_data(const uint8_t * data, uint32_t data_size)
{
    return (data_size >= 4 && lv_memcmp(data, "<svg", 4) == 0)
           || (data_size >= 5 && lv_memcmp(data, "<?xml", 5) == 0);
}

static lv_result_t svg_decoder_info(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * src,
                                    lv_image_header_t * header)
{
    lv_image_src_t src_type = src->src_type;

    int width = 0;
    int height = 0;

    if(src_type == LV_IMAGE_SRC_FILE || src_type == LV_IMAGE_SRC_VARIABLE) {
        const void * src_data = src->src;
        uint8_t * buf = NULL;

        if(src_type == LV_IMAGE_SRC_FILE) {
            /*Support only "*.svg" files*/
            if(lv_strcmp(lv_fs_get_ext(src_data), "svg")) {
                return LV_RESULT_INVALID;
            }

            uint32_t rn;
            lv_fs_res_t res;
            uint32_t file_size = 0;
            res = lv_fs_seek(&src->file, 0, LV_FS_SEEK_END);
            if(res == LV_FS_RES_OK) {
                lv_fs_tell(&src->file, &file_size);
                lv_fs_seek(&src->file, 0, LV_FS_SEEK_SET); /* Reset position to start */
            }
#ifdef LV_USE_SVG_DEBUG
            LV_LOG_INFO("LVGL file_size = %d.", file_size);

#endif
            if(file_size > 512)
                file_size = 512;

            buf = (uint8_t *)lv_zalloc(file_size);
            LV_ASSERT_NULL(buf);
            /* read some bytes for searching svg header */
            res = lv_fs_read(&src->file, buf, file_size, &rn);
            if(res != LV_FS_RES_OK) {
                LV_LOG_WARN("can't open %s", (char *)src_data);
                lv_free(buf);
                return LV_RESULT_INVALID;
            }

            if(!valid_svg_data(buf, rn)) {
                lv_free(buf);
                return LV_RESULT_INVALID;
            }

            width = LV_DPI_DEF;
            height = LV_DPI_DEF;

            uint8_t * svg_start = NULL;
            uint8_t * svg_end = NULL;
            uint8_t * ptr = buf;
            uint8_t * ptr_end = buf + file_size - 1;
            while(ptr < ptr_end) {
                if(*ptr == '<') {
                    if(lv_strncmp((char *)(ptr + 1), "svg", 3) == 0) {
                        svg_start = ptr;
                    }
                }
                if(svg_start && (*ptr == '>')) {
                    svg_end = ptr;
                    break;
                }
                ptr++;
            }

            if(svg_start && svg_end) {
                lv_svg_node_t * svg_doc = lv_svg_load_data((char *)svg_start, svg_end - svg_start);
                lv_svg_render_obj_t * svg_header = lv_svg_render_create(svg_doc);
                if(svg_header->tag == LV_SVG_TAG_SVG) {
                    lv_area_t bounds;
                    svg_header->clz->get_bounds(svg_header, &bounds);
                    width = lv_area_get_width(&bounds) - 1;
                    height = lv_area_get_height(&bounds) - 1;
                }
                lv_svg_render_delete(svg_header);
                lv_svg_node_delete(svg_doc);
            }
            else
                LV_LOG_WARN("can't find svg viewport tag end");

            lv_free(buf);
        }
        else {
            const lv_image_dsc_t * img_dsc = src_data;
            uint32_t data_size = img_dsc->data_size;
            width = img_dsc->header.w;
            height = img_dsc->header.h;

            if(!valid_svg_data(img_dsc->data, data_size)) {
                return LV_RESULT_INVALID;
            }
        }

        header->cf = LV_COLOR_FORMAT_ARGB8888;
        header->w = width;
        header->h = height;
        header->flags |= LV_IMAGE_FLAGS_CUSTOM_DRAW;

        decoder->custom_draw_cb = svg_draw;

        return LV_RESULT_OK;
    }

    return LV_RESULT_INVALID;
}

static lv_result_t svg_decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);
    LV_PROFILER_DECODER_BEGIN_TAG("lv_svg_decoder_open");

    uint8_t * svg_data = NULL;
    uint32_t svg_data_size = 0;

    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        const char * fn = dsc->src;
        if(lv_strcmp(lv_fs_get_ext(fn), "svg") == 0) {              /*Check the extension*/

            svg_data = alloc_file(fn, &svg_data_size);
            if(svg_data == NULL) {
                LV_LOG_WARN("can't load file: %s", (const char *)dsc->src);
                LV_PROFILER_DECODER_END_TAG("lv_svg_decoder_open");
                return LV_RESULT_INVALID;
            }

        }
        else {
            LV_PROFILER_DECODER_END_TAG("lv_svg_decoder_open");
            return LV_RESULT_INVALID;
        }
    }
    else if(dsc->src_type == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t * img_dsc = dsc->src;
        svg_data = (uint8_t *)img_dsc->data;
        svg_data_size = (uint32_t)img_dsc->data_size;
    }
    else {
        LV_PROFILER_DECODER_END_TAG("lv_svg_decoder_open");
        return LV_RESULT_INVALID;
    }

    lv_svg_node_t * svg_doc = lv_svg_load_data((char *)svg_data, svg_data_size);
    lv_svg_render_obj_t * draw_list = lv_svg_render_create(svg_doc);

    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        lv_free(svg_data);
    }
    lv_svg_node_delete(svg_doc);

    /* create a fake draw_buf object */
    lv_draw_buf_t * draw_buf = lv_zalloc(sizeof(lv_draw_buf_t));

    draw_buf->header.w = 1;
    draw_buf->header.h = 1;
    draw_buf->header.cf = LV_COLOR_FORMAT_ARGB8888;
    draw_buf->header.flags = LV_IMAGE_FLAGS_ALLOCATED | LV_IMAGE_FLAGS_CUSTOM_DRAW;
    draw_buf->header.stride = 4;
    draw_buf->header.magic = LV_IMAGE_HEADER_MAGIC;
    draw_buf->data = NULL;
    draw_buf->unaligned_data = (void *)draw_list;
    draw_buf->data_size = lv_svg_render_get_size(draw_list);
    draw_buf->handlers = &_svg_draw_buf_handler;

    dsc->decoded = draw_buf;

    if(!dsc->args.no_cache && lv_image_cache_is_enabled()) {

        lv_image_cache_data_t search_key;
        search_key.src_type = dsc->src_type;
        search_key.src = dsc->src;
        search_key.slot.size = dsc->decoded->data_size;

        lv_cache_entry_t * entry = lv_image_decoder_add_to_cache(decoder, &search_key, draw_buf, NULL);

        if(entry == NULL) {
            lv_draw_buf_destroy(draw_buf);
            LV_PROFILER_DECODER_END_TAG("lv_svg_decoder_open");
            return LV_RESULT_INVALID;
        }
        dsc->cache_entry = entry;
    }

    LV_PROFILER_DECODER_END_TAG("lv_svg_decoder_open");
    return LV_RESULT_OK;
}

static void svg_decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);

    if(dsc->args.no_cache ||
       !lv_image_cache_is_enabled()) lv_draw_buf_destroy((lv_draw_buf_t *)dsc->decoded);
}

static uint8_t * alloc_file(const char * filename, uint32_t * size)
{
    uint8_t * data = NULL;
    lv_fs_file_t f;
    uint32_t data_size;
    uint32_t rn;
    lv_fs_res_t res;

    *size = 0;

    res = lv_fs_open(&f, filename, LV_FS_MODE_RD);
    if(res != LV_FS_RES_OK) {
        LV_LOG_WARN("can't open %s", filename);
        return NULL;
    }

    res = lv_fs_seek(&f, 0, LV_FS_SEEK_END);
    if(res != LV_FS_RES_OK) {
        goto failed;
    }

    res = lv_fs_tell(&f, &data_size);
    if(res != LV_FS_RES_OK) {
        goto failed;
    }

    res = lv_fs_seek(&f, 0, LV_FS_SEEK_SET);
    if(res != LV_FS_RES_OK) {
        goto failed;
    }

    /*Read file to buffer*/
    data = lv_malloc(data_size);
    if(data == NULL) {
        LV_LOG_WARN("malloc failed for data size %u", data_size);
        goto failed;
    }

    res = lv_fs_read(&f, data, data_size, &rn);

    if(res == LV_FS_RES_OK && rn == data_size) {
        *size = rn;
    }
    else {
        LV_LOG_WARN("read file failed");
        lv_free(data);
        data = NULL;
    }

failed:
    lv_fs_close(&f);
    return data;
}

static void svg_draw_buf_free(void * svg_buf)
{
    lv_svg_render_obj_t * draw_list = (lv_svg_render_obj_t *)svg_buf;
    lv_svg_render_delete(draw_list);
}

static void svg_draw(lv_layer_t * layer, const lv_image_decoder_dsc_t * dsc, const lv_area_t * coords,
                     const lv_draw_image_dsc_t * image_dsc, const lv_area_t * clip_area)
{
    const lv_draw_buf_t * draw_buf = dsc->decoded;
    const lv_svg_render_obj_t * list = draw_buf->unaligned_data;

    LV_PROFILER_DRAW_BEGIN;

    lv_vector_dsc_t * ctx = lv_vector_dsc_create(layer);
    lv_matrix_t matrix;
    lv_matrix_identity(&matrix);
    lv_matrix_translate(&matrix, coords->x1, coords->y1);
    ctx->current_dsc.scissor_area = *clip_area;
    if(image_dsc) {
        int32_t off_x = (lv_area_get_width(coords) - image_dsc->header.w - 1) / 2;
        int32_t off_y = (lv_area_get_height(coords) - image_dsc->header.h - 1) / 2;

        if(image_dsc->pivot.x != 0 || image_dsc->pivot.y != 0) {
            lv_matrix_translate(&matrix, off_x, off_y);
        }
        lv_matrix_translate(&matrix, image_dsc->pivot.x, image_dsc->pivot.y);
        lv_matrix_rotate(&matrix, image_dsc->rotation / 10.0f);
        lv_matrix_scale(&matrix, image_dsc->scale_x / 256.0f, image_dsc->scale_y / 256.0f);
        lv_matrix_translate(&matrix, -image_dsc->pivot.x, -image_dsc->pivot.y);
    }
    lv_vector_dsc_set_transform(ctx, &matrix);
    lv_draw_svg_render(ctx, list);
    lv_draw_vector(ctx);
    lv_vector_dsc_delete(ctx);

    LV_PROFILER_DRAW_END;
}

#endif /*LV_USE_SVG*/
