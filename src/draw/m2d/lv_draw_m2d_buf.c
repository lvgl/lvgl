/**
 * @file lv_draw_m2d_buf.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_DRAW_M2D
#include "lv_draw_m2d_private.h"
#include "../lv_draw_buf_private.h"
#include "../../misc/lv_map.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/

#define M2D_ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_draw_buf_t * m2d_create(const lv_draw_buf_handlers_t * handlers, uint32_t w, uint32_t h, lv_color_format_t cf,
                                  uint32_t stride);

static lv_draw_buf_t * m2d_drm_import(const lv_draw_buf_handlers_t * handlers, const lv_draw_buf_import_dsc_t * dsc);

static void m2d_destroy(lv_draw_buf_t * draw_buf);

static uint32_t m2d_width_to_stride(uint32_t w, lv_color_format_t cf);

static enum m2d_pixel_format to_m2d_pixel_format(lv_color_format_t cf);

static int m2d_address_comp(const lv_map_key_t * key1, const lv_map_key_t * key2);
static void m2d_address_copy(lv_map_key_t * dst_key, const lv_map_key_t * src_key);

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_map_t m2d_buf_map;

static const lv_map_class_t m2d_buf_map_class = {
    .key_size = sizeof(void *),
    .value_size = sizeof(lv_draw_m2d_buf_t *),
    .key_comp_cb = m2d_address_comp,
    .key_copy_cb = m2d_address_copy,
    .value_copy_cb = m2d_address_copy,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_m2d_init_buf_map(void)
{
    lv_map_init(&m2d_buf_map, &m2d_buf_map_class);
}

void lv_draw_m2d_cleanup_buf_map(void)
{
    lv_map_clear(&m2d_buf_map);
}

lv_draw_m2d_buf_t * lv_draw_m2d_search_buf_map(const lv_image_dsc_t * img_dsc)
{
    void * key = (void *)img_dsc->data;
    lv_map_entry_t * entry = lv_map_find(&m2d_buf_map, &key);
    if(entry != lv_map_end(&m2d_buf_map))
        return *(lv_draw_m2d_buf_t **)lv_map_entry_get_value(entry);

    const lv_image_header_t * h = &img_dsc->header;
    if(!h->w || !h->h)
        return NULL;

    const lv_draw_buf_handlers_t * handlers = lv_draw_buf_get_handlers();
    lv_draw_m2d_buf_t * draw_m2d_buf = (lv_draw_m2d_buf_t *)m2d_create(handlers,
                                                                       h->w,
                                                                       h->h,
                                                                       h->cf,
                                                                       h->stride);
    if(!draw_m2d_buf)
        return NULL;

    void * value = draw_m2d_buf;
    if(!lv_map_insert(&m2d_buf_map, &key, &value, NULL)) {
        LV_LOG_ERROR("failed to insert into m2d_buf_map the lv_draw_m2d_buf_t created from a lv_image_dsc_t");
        m2d_destroy(&draw_m2d_buf->base_buf);
        return NULL;
    }
    draw_m2d_buf->alias_key = key;

    struct m2d_buffer * m2d_buf = draw_m2d_buf->m2d_buf;

    uint32_t height = h->h;
    size_t src_stride = (h->stride != LV_STRIDE_AUTO) ? h->stride : (img_dsc->data_size / height);
    size_t dst_stride = m2d_get_stride(m2d_buf);
    const void * src = img_dsc->data;
    void * dst = m2d_get_data(m2d_buf);

    struct timespec timeout;
    clock_gettime(CLOCK_MONOTONIC, &timeout);
    timeout.tv_sec += 1;

    (void)m2d_sync_for_cpu(m2d_buf, &timeout);
    for(uint32_t i = 0; i < height; i++) {
        lv_memcpy(dst, src, src_stride);
        src = (uint8_t *)src + src_stride;
        dst = (uint8_t *)dst + dst_stride;
    }
    m2d_sync_for_gpu(m2d_buf);

    return draw_m2d_buf;
}

void lv_draw_buf_m2d_init_handlers(void)
{
    lv_draw_buf_handlers_t * handlers = lv_draw_buf_get_handlers();
    lv_draw_buf_handlers_t * image_handlers = lv_draw_buf_get_image_handlers();

    handlers->create_cb = m2d_create;
    handlers->import_cb = m2d_drm_import;
    handlers->destroy_cb = m2d_destroy;
    handlers->width_to_stride_cb = m2d_width_to_stride;

    image_handlers->create_cb = m2d_create;
    image_handlers->destroy_cb = m2d_destroy;
    image_handlers->width_to_stride_cb = m2d_width_to_stride;
}

int32_t lv_draw_m2d_buf_sync_for_cpu(lv_draw_m2d_buf_t * draw_m2d_buf)
{
    if(draw_m2d_buf->owned_by_gpu) {
        struct m2d_buffer * m2d_buf = draw_m2d_buf->m2d_buf;

        struct timespec timeout;
        clock_gettime(CLOCK_MONOTONIC, &timeout);
        timeout.tv_sec += 1;
        (void)m2d_sync_for_cpu(m2d_buf, &timeout);
        draw_m2d_buf->owned_by_gpu = false;
    }

    return 0;
}

int32_t lv_draw_m2d_buf_sync_for_gpu(lv_draw_m2d_buf_t * draw_m2d_buf)
{
    if(!draw_m2d_buf->owned_by_gpu) {
        m2d_sync_for_gpu(draw_m2d_buf->m2d_buf);
        draw_m2d_buf->owned_by_gpu = true;
    }

    return 0;
}

struct m2d_buffer * lv_draw_m2d_buf_get_tmp_buffer(lv_draw_m2d_buf_t * draw_m2d_buf)
{
    if(draw_m2d_buf->m2d_tmp_buf == NULL) {
        const lv_image_header_t * h = &draw_m2d_buf->base_buf.header;
        if(!h->w || !h->h)
            return NULL;

        size_t stride = h->w * sizeof(uint32_t);
        struct m2d_buffer * m2d_buf = m2d_alloc(h->w, h->h, M2D_PF_ARGB8888, stride);
        if(m2d_buf == NULL) {
            LV_LOG_ERROR("m2d_alloc() failed");
            return NULL;
        }
        draw_m2d_buf->m2d_tmp_buf = m2d_buf;
    }

    return draw_m2d_buf->m2d_tmp_buf;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_draw_buf_t * m2d_create(const lv_draw_buf_handlers_t * handlers,
                                  uint32_t w,
                                  uint32_t h,
                                  lv_color_format_t cf,
                                  uint32_t stride)
{
    lv_draw_m2d_buf_t * draw_m2d_buf = NULL;
    struct m2d_buffer * m2d_buf = NULL;

    if(stride == LV_STRIDE_AUTO)
        stride = lv_draw_buf_width_to_stride_ex(handlers, w, cf);

    draw_m2d_buf = lv_malloc_zeroed(sizeof(*draw_m2d_buf));
    if(draw_m2d_buf == NULL) {
        LV_LOG_ERROR("failed to allocate memory for lv_draw_m2d_buf_t");
        goto out;
    }

    m2d_buf = m2d_alloc(w, h, to_m2d_pixel_format(cf), stride);
    if(m2d_buf == NULL) {
        LV_LOG_ERROR("m2d_alloc() failed");
        goto out_lv_free;
    }
    draw_m2d_buf->m2d_buf = m2d_buf;

    uint32_t data_size = h * m2d_get_stride(m2d_buf);
    if(lv_draw_buf_init(&draw_m2d_buf->base_buf, w, h, cf,
                        m2d_get_stride(m2d_buf),
                        m2d_get_data(m2d_buf), data_size) != LV_RESULT_OK)
        goto out_m2d_free;

    draw_m2d_buf->base_buf.handlers = handlers;

    lv_image_header_t * header = &draw_m2d_buf->base_buf.header;
    header->flags = LV_IMAGE_FLAGS_MODIFIABLE | LV_IMAGE_FLAGS_ALLOCATED;

    draw_m2d_buf->owned_by_gpu = true;

    void * key = m2d_get_data(m2d_buf);
    void * value = draw_m2d_buf;
    if(!lv_map_insert(&m2d_buf_map, &key, &value, NULL)) {
        LV_LOG_ERROR("failed to insert into m2d_buf_map the created lv_draw_m2d_buf_t");
        goto out_m2d_free;
    }

    return &draw_m2d_buf->base_buf;

out_m2d_free:
    m2d_free(m2d_buf);
out_lv_free:
    lv_free(draw_m2d_buf);
out:
    return NULL;
}

static lv_draw_buf_t * m2d_drm_import(const lv_draw_buf_handlers_t * handlers,
                                      const lv_draw_buf_import_dsc_t * dsc)
{
    lv_draw_m2d_buf_t * draw_m2d_buf = NULL;
    struct m2d_buffer * m2d_buf = NULL;

    draw_m2d_buf = lv_malloc_zeroed(sizeof(*draw_m2d_buf));
    if(draw_m2d_buf == NULL) {
        LV_LOG_ERROR("failed to allocate memory for lv_draw_m2d_buf_t");
        goto out;
    }

    struct m2d_import_desc import_desc;
    lv_memzero(&import_desc, sizeof(import_desc));
    import_desc.width = dsc->w;
    import_desc.height = dsc->h;
    import_desc.format = to_m2d_pixel_format(dsc->cf);
    import_desc.stride = dsc->stride;
    import_desc.fd = dsc->fd;
    import_desc.cpu_addr = dsc->data;
    m2d_buf = m2d_import(&import_desc);
    if(m2d_buf == NULL) {
        LV_LOG_ERROR("m2d_import() failed");
        goto out_lv_free;
    }
    draw_m2d_buf->m2d_buf = m2d_buf;

    uint32_t data_size = dsc->h * m2d_get_stride(m2d_buf);
    if(lv_draw_buf_init(&draw_m2d_buf->base_buf, dsc->w, dsc->h, dsc->cf,
                        m2d_get_stride(m2d_buf),
                        m2d_get_data(m2d_buf), data_size) != LV_RESULT_OK)
        goto out_m2d_free;

    draw_m2d_buf->base_buf.handlers = handlers;

    lv_image_header_t * header = &draw_m2d_buf->base_buf.header;
    header->flags = LV_IMAGE_FLAGS_MODIFIABLE | LV_IMAGE_FLAGS_ALLOCATED;

    draw_m2d_buf->owned_by_gpu = true;

    void * key = m2d_get_data(m2d_buf);
    void * value = draw_m2d_buf;
    if(!lv_map_insert(&m2d_buf_map, &key, &value, NULL)) {
        LV_LOG_ERROR("failed to insert into m2d_buf_map the imported lv_draw_m2d_buf_t");
        goto out_m2d_free;
    }

    return &draw_m2d_buf->base_buf;

out_m2d_free:
    m2d_free(m2d_buf);
out_lv_free:
    lv_free(draw_m2d_buf);
out:
    return NULL;
}

static void m2d_destroy(lv_draw_buf_t * draw_buf)
{
    lv_draw_m2d_buf_t * draw_m2d_buf = (lv_draw_m2d_buf_t *)draw_buf;

    if(draw_m2d_buf->alias_key)
        lv_map_erase(&m2d_buf_map, &draw_m2d_buf->alias_key);
    void * key = m2d_get_data(draw_m2d_buf->m2d_buf);
    lv_map_erase(&m2d_buf_map, &key);
    m2d_free(draw_m2d_buf->m2d_tmp_buf);
    m2d_free(draw_m2d_buf->m2d_buf);
    lv_free(draw_m2d_buf);
}

static uint32_t m2d_width_to_stride(uint32_t w, lv_color_format_t cf)
{
    uint32_t bytes_per_pixel;

    switch(cf) {
        case LV_COLOR_FORMAT_A8:
            bytes_per_pixel = 1;
            break;
        case LV_COLOR_FORMAT_RGB565:
            bytes_per_pixel = 2;
            break;
        default:
            bytes_per_pixel = 4;
            break;
    }

    return M2D_ALIGN_UP(bytes_per_pixel * w, sizeof(uint32_t));
}

static enum m2d_pixel_format to_m2d_pixel_format(lv_color_format_t cf)
{
    switch(cf) {
        case LV_COLOR_FORMAT_A8:
            return M2D_PF_A8;
        case LV_COLOR_FORMAT_RGB565:
            return M2D_PF_RGB565;
        default:
            break;
    }

    return M2D_PF_ARGB8888;
}

static int m2d_address_comp(const lv_map_key_t * key1, const lv_map_key_t * key2)
{
    void * addr1 = *(void **)key1;
    void * addr2 = *(void **)key2;

    lv_intptr_t delta = (lv_intptr_t)addr1 - (lv_intptr_t)addr2;
    if(delta < 0)
        return -1;
    if(delta > 0)
        return 1;
    return 0;
}

static void m2d_address_copy(lv_map_key_t * dst_key, const lv_map_key_t * src_key)
{
    void ** dst = (void **)dst_key;
    void ** src = (void **)src_key;

    *dst = *src;
}

#endif /*LV_USE_DRAW_M2D*/
