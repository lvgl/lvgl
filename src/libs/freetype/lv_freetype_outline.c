/**
 * @file lv_freetype_outline.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_freetype_private.h"

#if LV_USE_FREETYPE && LV_FREETYPE_CACHE_TYPE == LV_FREETYPE_CACHE_TYPE_OUTLINE

/*********************
 *      DEFINES
 *********************/

#define LV_FREETYPE_OUTLINE_REF_SIZE_DEF 128

#define LV_FREETYPE_GLYPH_DSC_CACHE_SIZE (LV_FREETYPE_CACHE_FT_OUTLINES * 2)

#if LV_FREETYPE_CACHE_FT_OUTLINES <= 0
    #error "LV_FREETYPE_CACHE_FT_OUTLINES must be greater than 0"
#endif

/**********************
 *      TYPEDEFS
 **********************/

struct _lv_freetype_cache_context_t {
    uint32_t ref_size;
    lv_ll_t cache_ll;
    lv_event_cb_t event_cb;
    void * user_data;
};

typedef struct _lv_freetype_glyph_dsc_node_t {
    FT_UInt glyph_index;
    uint32_t size;
    lv_font_glyph_dsc_t glyph_dsc;
} lv_freetype_glyph_dsc_node_t;

struct _lv_freetype_cache_node_t {
    lv_freetype_cache_context_t * cache_context;

    int ref_cnt;
    const char * pathname;
    lv_freetype_font_style_t style;
    FT_Face face;

    /*glyph outline cache*/
    lv_cache_t * glyph_outline_cache;
    int outline_cnt;

    /*glyph size cache*/
    lv_cache_t * glyph_dsc_cache;
    int dsc_cnt;
};

typedef struct _lv_freetype_outline_node_t {
    FT_UInt glyph_index;
    lv_freetype_outline_t outline;
} lv_freetype_outline_node_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_freetype_outline_t outline_create(lv_freetype_context_t * ctx, FT_Face face, FT_UInt glyph_index,
                                            uint32_t size, uint32_t strength);
static lv_result_t outline_delete(lv_freetype_cache_context_t * cache_context, lv_freetype_outline_t outline);
static bool freetype_get_glyph_dsc_cb(const lv_font_t * font, lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter,
                                      uint32_t unicode_letter_next);
static const uint8_t * freetype_get_glyph_bitmap_cb(const lv_font_t * font, lv_font_glyph_dsc_t * g_dsc,
                                                    uint32_t unicode_letter,
                                                    uint8_t * bitmap_out);
static void freetype_release_glyph_cb(const lv_font_t * font, lv_font_glyph_dsc_t * g_dsc);

static lv_freetype_cache_node_t * lv_freetype_cache_node_lookup(lv_freetype_context_t * ctx, const char * pathname,
                                                                lv_freetype_font_style_t style);
static void lv_freetype_cache_node_drop(lv_freetype_font_dsc_t * dsc);
static lv_cache_entry_t * lv_freetype_outline_lookup(lv_freetype_font_dsc_t * dsc, uint32_t unicode_letter);

/*glyph dsc cache lru callbacks*/
static bool freetype_glyph_outline_create_cb(lv_freetype_outline_node_t * node, lv_freetype_font_dsc_t * dsc);
static void freetype_glyph_outline_free_cb(lv_freetype_outline_node_t * node, lv_freetype_font_dsc_t * dsc);
static lv_cache_compare_res_t freetype_glyph_outline_cmp_cb(const lv_freetype_outline_node_t * node_a,
                                                            const lv_freetype_outline_node_t * node_b);

/*glyph dsc cache lru callbacks*/
static bool freetype_glyph_dsc_create_cb(lv_freetype_glyph_dsc_node_t * glyph_dsc_node, lv_freetype_font_dsc_t * dsc);
static void freetype_glyph_dsc_free_cb(lv_freetype_glyph_dsc_node_t * glyph_dsc_node, lv_freetype_font_dsc_t * dsc);
static lv_cache_compare_res_t freetype_glyph_dsc_cmp_cb(const lv_freetype_glyph_dsc_node_t * node_a,
                                                        const lv_freetype_glyph_dsc_node_t * node_b);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_freetype_cache_context_t * lv_freetype_cache_context_create(lv_freetype_context_t * ctx)
{
    LV_UNUSED(ctx);
    lv_freetype_cache_context_t * cache_ctx;
    cache_ctx = lv_malloc(sizeof(lv_freetype_cache_context_t));
    LV_ASSERT_MALLOC(cache_ctx);
    lv_memzero(cache_ctx, sizeof(lv_freetype_cache_context_t));

    _lv_ll_init(&cache_ctx->cache_ll, sizeof(lv_freetype_cache_node_t));
    cache_ctx->ref_size = LV_FREETYPE_OUTLINE_REF_SIZE_DEF;

    LV_LOG_INFO("cache_context = %p", cache_ctx);
    return cache_ctx;
}

void lv_freetype_cache_context_delete(lv_freetype_cache_context_t * cache_context)
{
    LV_ASSERT_NULL(cache_context);

    lv_freetype_cache_node_t * cache;
    lv_ll_t * cache_ll = &cache_context->cache_ll;

    _LV_LL_READ(cache_ll, cache) {
        LV_LOG_ERROR("cache_node: ptr = %p, ref_cnt = %d",
                     cache, cache->ref_cnt);
    }

    /* Must ensure that cache_node is not used */
    LV_ASSERT(_lv_ll_get_len(cache_ll) == 0);

    LV_LOG_INFO("cache_context = %p", cache_context);
    lv_free(cache_context);
}

bool lv_freetype_on_font_create(lv_freetype_font_dsc_t * dsc)
{
    LV_ASSERT_FREETYPE_FONT_DSC(dsc);
    dsc->cache_node = lv_freetype_cache_node_lookup(dsc->context, lv_freetype_get_pathname(dsc->face_id), dsc->style);
    dsc->font.get_glyph_dsc = freetype_get_glyph_dsc_cb;
    dsc->font.get_glyph_bitmap = freetype_get_glyph_bitmap_cb;
    dsc->font.release_glyph = freetype_release_glyph_cb;
    return true;
}

void lv_freetype_on_font_delete(lv_freetype_font_dsc_t * dsc)
{
    LV_ASSERT_FREETYPE_FONT_DSC(dsc);
    LV_ASSERT_NULL(dsc->cache_node);
    lv_freetype_cache_node_drop(dsc);
}

void lv_freetype_outline_add_event(lv_event_cb_t event_cb, lv_event_code_t filter, void * user_data)
{
    lv_freetype_context_t * ctx = lv_freetype_get_context();
    lv_freetype_cache_context_t * cache_context = ctx->cache_context;
    LV_ASSERT_NULL(cache_context);

    LV_UNUSED(filter);
    cache_context->event_cb = event_cb;
    cache_context->user_data = user_data;
}

void lv_freetype_outline_set_ref_size(uint32_t size)
{
    LV_ASSERT(size > 0);
    lv_freetype_context_t * ctx = lv_freetype_get_context();
    lv_freetype_cache_context_t * cache_context = ctx->cache_context;
    LV_ASSERT_NULL(cache_context);

    cache_context->ref_size = size;
}

uint32_t lv_freetype_outline_get_ref_size(void)
{
    lv_freetype_context_t * ctx = lv_freetype_get_context();
    lv_freetype_cache_context_t * cache_context = ctx->cache_context;
    LV_ASSERT_NULL(cache_context);

    return cache_context->ref_size;
}

uint32_t lv_freetype_outline_get_scale(const lv_font_t * font)
{
    LV_ASSERT_NULL(font);
    const lv_freetype_font_dsc_t * dsc = font->dsc;
    LV_ASSERT_FREETYPE_FONT_DSC(dsc);

    return FT_INT_TO_F26DOT6(dsc->size) / dsc->context->cache_context->ref_size;
}

bool lv_freetype_is_outline_font(const lv_font_t * font)
{
    LV_ASSERT_NULL(font);
    const lv_freetype_font_dsc_t * dsc = font->dsc;
    if(!LV_FREETYPE_FONT_DSC_HAS_MAGIC_NUM(dsc)) {
        return false;
    }

    return true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*-------------------
 *     FACE CACHE
 *------------------*/

static lv_freetype_cache_node_t * lv_freetype_cache_node_lookup(lv_freetype_context_t * ctx,
                                                                const char * pathname, lv_freetype_font_style_t style)
{
    LV_ASSERT_NULL(ctx);
    LV_ASSERT_NULL(pathname);

    lv_freetype_cache_context_t * cache_context = ctx->cache_context;

    lv_freetype_cache_node_t * cache;
    lv_ll_t * cache_ll = &cache_context->cache_ll;

    /* Search cache */
    _LV_LL_READ(cache_ll, cache) {
        if(strcmp(pathname, cache->pathname) == 0 && style == cache->style) {
            lv_freetype_cache_node_t * head = _lv_ll_get_head(cache_ll);
            _lv_ll_move_before(cache_ll, cache, head);
            cache->ref_cnt++;
            LV_LOG_INFO("outline cache(name: %s, style: 0x%x) hit %p, ref_cnt = %d",
                        pathname, style, cache, cache->ref_cnt);
            return cache;
        }
    }

    /* Cache miss, load face */
    FT_Face face;
    FT_Error error = FT_New_Face(ctx->library, pathname, 0, &face);
    if(error) {
        FT_ERROR_MSG("FT_New_Face", error);
        return NULL;
    }

    FT_F26Dot6 pixel_size = FT_INT_TO_F26DOT6(cache_context->ref_size);
    error = FT_Set_Char_Size(face, 0, pixel_size, 0, 0);
    if(error) {
        FT_Done_Face(face);
        FT_ERROR_MSG("FT_Set_Char_Size", error);
        return NULL;
    }

    if(style & LV_FREETYPE_FONT_STYLE_ITALIC) {
        lv_freetype_italic_transform(face);
    }

    /* Init cache node */
    cache = _lv_ll_ins_head(cache_ll);
    LV_ASSERT_MALLOC(cache);
    lv_memzero(cache, sizeof(lv_freetype_cache_node_t));

    cache->face = face;
    cache->style = style;
    cache->pathname = pathname;

    cache->ref_cnt = 1;
    cache->cache_context = cache_context;

    lv_cache_ops_t glyph_dsc_cache_ops = {
        .create_cb = (lv_cache_create_cb_t)freetype_glyph_dsc_create_cb,
        .free_cb = (lv_cache_free_cb_t)freetype_glyph_dsc_free_cb,
        .compare_cb = (lv_cache_compare_cb_t)freetype_glyph_dsc_cmp_cb,
    };
    lv_cache_ops_t glyph_outline_cache_ops = {
        .create_cb = (lv_cache_create_cb_t)freetype_glyph_outline_create_cb,
        .free_cb = (lv_cache_free_cb_t)freetype_glyph_outline_free_cb,
        .compare_cb = (lv_cache_compare_cb_t)freetype_glyph_outline_cmp_cb,
    };

    cache->glyph_dsc_cache = lv_cache_create(&lv_cache_class_lru_rb_count, sizeof(lv_freetype_glyph_dsc_node_t),
                                             LV_FREETYPE_GLYPH_DSC_CACHE_SIZE,
                                             glyph_dsc_cache_ops);
    cache->glyph_outline_cache = lv_cache_create(&lv_cache_class_lru_rb_count, sizeof(lv_freetype_outline_node_t),
                                                 LV_FREETYPE_CACHE_FT_OUTLINES,
                                                 glyph_outline_cache_ops);

    LV_LOG_INFO("outline cache(name: %s, style: 0x%x) create %p, ref_cnt = %d",
                pathname, style, cache, cache->ref_cnt);

    return cache;
}

static void lv_freetype_cache_node_drop(lv_freetype_font_dsc_t * dsc)
{
    LV_ASSERT_NULL(dsc);
    LV_ASSERT_NULL(dsc->cache_node);
    lv_freetype_cache_node_t * cache_node = dsc->cache_node;

    cache_node->ref_cnt--;

    LV_LOG_INFO("outline cache(%s) drop %p, ref_cnt = %d",
                cache_node->pathname, cache_node, cache_node->ref_cnt);

    if(cache_node->ref_cnt > 0) {
        return;
    }

    LV_LOG_INFO("outline cache(%s) free %p", cache_node->pathname, cache_node);

    FT_Done_Face(cache_node->face);

    lv_ll_t * cache_ll = &cache_node->cache_context->cache_ll;
    _lv_ll_remove(cache_ll, cache_node);

    lv_cache_destroy(cache_node->glyph_dsc_cache, dsc);
    lv_cache_destroy(cache_node->glyph_outline_cache, dsc);
    lv_free(cache_node);
}

/*-------------------
 *  GLYPH DSC CACHE
 *------------------*/

static bool freetype_glyph_dsc_create_cb(lv_freetype_glyph_dsc_node_t * glyph_dsc_node, lv_freetype_font_dsc_t * dsc)
{
    lv_font_glyph_dsc_t * dsc_out = &glyph_dsc_node->glyph_dsc;

    FT_UInt glyph_index = glyph_dsc_node->glyph_index;
    /* cache miss, load dsc */
    FT_Size ft_size = lv_freetype_lookup_size(dsc);
    if(!ft_size) {
        return false;
    }

    FT_Error error = FT_Load_Glyph(ft_size->face, glyph_index, FT_LOAD_COMPUTE_METRICS | FT_LOAD_NO_BITMAP);
    if(error) {
        FT_ERROR_MSG("FT_Load_Glyph", error);
        return false;
    }

    LV_LOG_INFO("glyph_index = %u, cnt = %d", glyph_index, ++dsc->cache_node->dsc_cnt);

    FT_GlyphSlot glyph = ft_size->face->glyph;

    dsc_out->adv_w = FT_F26DOT6_TO_INT(glyph->metrics.horiAdvance);
    dsc_out->box_h = FT_F26DOT6_TO_INT(glyph->metrics.height);          /*Height of the bitmap in [px]*/
    dsc_out->box_w = FT_F26DOT6_TO_INT(glyph->metrics.width);           /*Width of the bitmap in [px]*/
    dsc_out->ofs_x = FT_F26DOT6_TO_INT(glyph->metrics.horiBearingX);    /*X offset of the bitmap in [pf]*/
    dsc_out->ofs_y = FT_F26DOT6_TO_INT(glyph->metrics.horiBearingY -
                                       glyph->metrics.height);          /*Y offset of the bitmap measured from the as line*/
    dsc_out->bpp = 8; /*Bit per pixel: 1/2/4/8*/
    dsc_out->is_placeholder = glyph_index == 0;

    return true;
}

static void freetype_glyph_dsc_free_cb(lv_freetype_glyph_dsc_node_t * glyph_dsc_node, lv_freetype_font_dsc_t * dsc)
{
    LV_UNUSED(glyph_dsc_node);
    LV_UNUSED(dsc);
    LV_LOG_INFO("cnt = %d", --dsc->cache_node->dsc_cnt);
}

static lv_cache_compare_res_t freetype_glyph_dsc_cmp_cb(const lv_freetype_glyph_dsc_node_t * node_a,
                                                        const lv_freetype_glyph_dsc_node_t * node_b)
{
    if(node_a->glyph_index != node_b->glyph_index)
        return (node_a->glyph_index > node_b->glyph_index) ? 1 : -1;

    if(node_a->size != node_b->size)
        return (node_a->size > node_b->size) ? 1 : -1;

    return 0;
}

static bool freetype_get_glyph_dsc_cb(const lv_font_t * font,
                                      lv_font_glyph_dsc_t * dsc_out,
                                      uint32_t unicode_letter,
                                      uint32_t unicode_letter_next)
{
    LV_ASSERT_NULL(font);
    LV_ASSERT_NULL(dsc_out);

    if(unicode_letter < 0x20) {
        dsc_out->adv_w = 0;
        dsc_out->box_h = 0;
        dsc_out->box_w = 0;
        dsc_out->ofs_x = 0;
        dsc_out->ofs_y = 0;
        dsc_out->bpp = 0;
        return true;
    }

    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)font->dsc;
    LV_ASSERT_FREETYPE_FONT_DSC(dsc);

    lv_freetype_cache_node_t * cache_node = dsc->cache_node;

    FT_UInt charmap_index = FT_Get_Charmap_Index(cache_node->face->charmap);
    FT_UInt glyph_index = FTC_CMapCache_Lookup(dsc->context->cmap_cache, dsc->face_id, charmap_index, unicode_letter);

    lv_freetype_glyph_dsc_node_t tmp_node;
    tmp_node.glyph_index = glyph_index;
    tmp_node.size = dsc->size;

    lv_cache_entry_t * entry = lv_cache_acquire_or_create(cache_node->glyph_dsc_cache, &tmp_node, dsc);
    if(entry == NULL) {
        return false;
    }
    lv_freetype_glyph_dsc_node_t * new_node = lv_cache_entry_get_data(entry);
    *dsc_out = new_node->glyph_dsc;

    if((dsc->style & LV_FREETYPE_FONT_STYLE_ITALIC) && (unicode_letter_next == '\0')) {
        dsc_out->adv_w = dsc_out->box_w + dsc_out->ofs_x;
    }
    lv_cache_release(cache_node->glyph_dsc_cache, entry, NULL);

    return true;
}

/*-------------------
 *   OUTLINE CACHE
 *------------------*/

static bool freetype_glyph_outline_create_cb(lv_freetype_outline_node_t * node, lv_freetype_font_dsc_t * dsc)
{
    lv_freetype_outline_t outline;
    outline = outline_create(dsc->context,
                             dsc->cache_node->face,
                             node->glyph_index,
                             dsc->cache_node->cache_context->ref_size,
                             dsc->style & LV_FREETYPE_FONT_STYLE_BOLD ? 1 : 0);

    if(!outline) {
        return false;
    }

    LV_LOG_INFO("glyph_index = %u, cnt = %d", node->glyph_index, ++dsc->cache_node->outline_cnt);

    node->outline = outline;
    return true;
}

static void freetype_glyph_outline_free_cb(lv_freetype_outline_node_t * node, lv_freetype_font_dsc_t * dsc)
{
    lv_freetype_outline_t outline = node->outline;
    outline_delete(dsc->context->cache_context, outline);
    LV_LOG_INFO("cnt = %d", --dsc->cache_node->outline_cnt);
}

static lv_cache_compare_res_t freetype_glyph_outline_cmp_cb(const lv_freetype_outline_node_t * node_a,
                                                            const lv_freetype_outline_node_t * node_b)
{
    if(node_a->glyph_index == node_b->glyph_index) {
        return 0;
    }
    return node_a->glyph_index > node_b->glyph_index ? 1 : -1;
}

static const uint8_t * freetype_get_glyph_bitmap_cb(const lv_font_t * font, lv_font_glyph_dsc_t * g_dsc,
                                                    uint32_t unicode_letter,
                                                    uint8_t * bitmap_out)
{
    LV_UNUSED(bitmap_out);
    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)font->dsc;
    LV_ASSERT_FREETYPE_FONT_DSC(dsc);
    lv_cache_entry_t * entry = lv_freetype_outline_lookup(dsc, unicode_letter);
    if(entry == NULL) {
        return NULL;
    }
    lv_freetype_outline_node_t * node = lv_cache_entry_get_data(entry);

    g_dsc->entry = entry;

    return node ? node->outline : NULL;
}

static void freetype_release_glyph_cb(const lv_font_t * font, lv_font_glyph_dsc_t * g_dsc)
{
    LV_ASSERT_NULL(font);
    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)font->dsc;

    if(g_dsc->entry == NULL) {
        return;
    }
    lv_cache_release(dsc->cache_node->glyph_outline_cache, g_dsc->entry, NULL);
    g_dsc->entry = NULL;
}

static lv_cache_entry_t * lv_freetype_outline_lookup(lv_freetype_font_dsc_t * dsc, uint32_t unicode_letter)
{
    lv_freetype_cache_node_t * cache_node = dsc->cache_node;

    FT_UInt charmap_index = FT_Get_Charmap_Index(cache_node->face->charmap);
    FT_UInt glyph_index = FTC_CMapCache_Lookup(dsc->context->cmap_cache, dsc->face_id, charmap_index, unicode_letter);

    lv_freetype_outline_node_t tmp_node;
    tmp_node.glyph_index = glyph_index;

    lv_cache_entry_t * entry = lv_cache_acquire_or_create(cache_node->glyph_outline_cache, &tmp_node, dsc);
    if(!entry) {
        LV_LOG_ERROR("glyph outline lookup failed for glyph_index = %u", glyph_index);
        return NULL;
    }
    return entry;
}

static void ft_vector_to_lv_vector(lv_freetype_outline_vector_t * dest, const FT_Vector * src)
{
    dest->x = src ? src->x : 0;
    dest->y = src ? src->y : 0;
}

static lv_result_t outline_send_event(lv_freetype_cache_context_t * ctx, lv_event_code_t code,
                                      lv_freetype_outline_event_param_t * param)
{
    if(!ctx->event_cb) {
        return LV_RESULT_INVALID;
    }

    lv_event_t e;
    lv_memzero(&e, sizeof(e));
    e.code = code;
    e.param = param;
    e.user_data = ctx->user_data;

    ctx->event_cb(&e);

    return LV_RESULT_OK;
}

static lv_result_t outline_push_point(
    lv_freetype_outline_t outline,
    lv_freetype_outline_type_t type,
    const FT_Vector * control1,
    const FT_Vector * control2,
    const FT_Vector * to)
{
    lv_freetype_context_t * ctx = lv_freetype_get_context();
    lv_freetype_cache_context_t * cache_context = ctx->cache_context;

    lv_freetype_outline_event_param_t param;
    lv_memzero(&param, sizeof(param));
    param.outline = outline;
    param.type = type;
    ft_vector_to_lv_vector(&param.control1, control1);
    ft_vector_to_lv_vector(&param.control2, control2);
    ft_vector_to_lv_vector(&param.to, to);

    return outline_send_event(cache_context, LV_EVENT_INSERT, &param);
}

static int outline_move_to_cb(
    const FT_Vector * to,
    void * user)
{
    lv_freetype_outline_t outline = user;
    outline_push_point(outline, LV_FREETYPE_OUTLINE_MOVE_TO, NULL, NULL, to);
    return FT_Err_Ok;
}

static int outline_line_to_cb(
    const FT_Vector * to,
    void * user)
{
    lv_freetype_outline_t outline = user;
    outline_push_point(outline, LV_FREETYPE_OUTLINE_LINE_TO, NULL, NULL, to);
    return FT_Err_Ok;
}

static int outline_conic_to_cb(
    const FT_Vector * control,
    const FT_Vector * to,
    void * user)
{
    lv_freetype_outline_t outline = user;
    outline_push_point(outline, LV_FREETYPE_OUTLINE_CONIC_TO, control, NULL, to);
    return FT_Err_Ok;
}

static int outline_cubic_to_cb(
    const FT_Vector * control1,
    const FT_Vector * control2,
    const FT_Vector * to,
    void * user)
{
    lv_freetype_outline_t outline = user;
    outline_push_point(outline, LV_FREETYPE_OUTLINE_CUBIC_TO, control1, control2, to);
    return FT_Err_Ok;
}

static lv_freetype_outline_t outline_create(
    lv_freetype_context_t * ctx,
    FT_Face face,
    FT_UInt glyph_index,
    uint32_t size,
    uint32_t strength)
{
    LV_UNUSED(size);

    LV_ASSERT_NULL(ctx);
    FT_Error error;

    /* Load glyph */
    error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT | FT_LOAD_NO_BITMAP);
    if(error) {
        FT_ERROR_MSG("FT_Load_Glyph", error);
        return NULL;
    }

    if(strength > 0) {
        error = FT_Outline_Embolden(&face->glyph->outline, FT_INT_TO_F26DOT6(strength));
        if(error) {
            FT_ERROR_MSG("FT_Outline_Embolden", error);
        }
    }

    lv_result_t res;
    lv_freetype_cache_context_t * cache_context = ctx->cache_context;
    lv_freetype_outline_event_param_t param;

    lv_memzero(&param, sizeof(param));
    res = outline_send_event(cache_context, LV_EVENT_CREATE, &param);

    lv_freetype_outline_t outline = param.outline;

    if(res != LV_RESULT_OK || !outline) {
        LV_LOG_ERROR("Outline object create failed");
        return NULL;
    }

    FT_Outline_Funcs outline_funcs = {
        .move_to = outline_move_to_cb,
        .line_to = outline_line_to_cb,
        .conic_to = outline_conic_to_cb,
        .cubic_to = outline_cubic_to_cb,
        .shift = 0,
        .delta = 0
    };

    /* Run outline decompose again to fill outline data */
    error = FT_Outline_Decompose(&face->glyph->outline, &outline_funcs, outline);
    if(error) {
        FT_ERROR_MSG("FT_Outline_Decompose", error);
        outline_delete(ctx->cache_context, outline);
        return NULL;
    }

    /* close outline */
    res = outline_push_point(outline, LV_FREETYPE_OUTLINE_END, NULL, NULL, NULL);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Outline object close failed");
        outline_delete(ctx->cache_context, outline);
        return NULL;
    }

    return outline;
}

static lv_result_t outline_delete(lv_freetype_cache_context_t * cache_context, lv_freetype_outline_t outline)
{
    lv_freetype_outline_event_param_t param;
    lv_memzero(&param, sizeof(param));
    param.outline = outline;

    return outline_send_event(cache_context, LV_EVENT_DELETE, &param);
}

#endif
