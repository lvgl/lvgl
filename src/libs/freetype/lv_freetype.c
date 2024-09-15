/**
 * @file lv_freetype.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_fs_private.h"
#include "lv_freetype_private.h"

#if LV_USE_FREETYPE

#include "lv_freetype_private.h"
#include "../../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/

#define ft_ctx LV_GLOBAL_DEFAULT()->ft_context
#define LV_FREETYPE_OUTLINE_REF_SIZE_DEF 128

/**< This value is from the FreeType's function `FT_GlyphSlot_Oblique` in `ftsynth.c` */
#define LV_FREETYPE_OBLIQUE_SLANT_DEF 0x0366A

#if LV_FREETYPE_CACHE_FT_GLYPH_CNT <= 0
    #error "LV_FREETYPE_CACHE_FT_GLYPH_CNT must be greater than 0"
#endif

/**********************
 *      TYPEDEFS
 **********************/

/* Use the pointer storing pathname as the unique request ID of the face */
typedef struct {
    char * pathname;
    int ref_cnt;
} face_id_node_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_freetype_cleanup(lv_freetype_context_t * ctx);
static FTC_FaceID lv_freetype_req_face_id(lv_freetype_context_t * ctx, const char * pathname);
static void lv_freetype_drop_face_id(lv_freetype_context_t * ctx, FTC_FaceID face_id);
static bool freetype_on_font_create(lv_freetype_font_dsc_t * dsc, uint32_t max_glyph_cnt);
static void freetype_on_font_set_cbs(lv_freetype_font_dsc_t * dsc);

static bool cache_node_cache_create_cb(lv_freetype_cache_node_t * node, void * user_data);
static void cache_node_cache_free_cb(lv_freetype_cache_node_t * node, void * user_data);
static lv_cache_compare_res_t cache_node_cache_compare_cb(const lv_freetype_cache_node_t * lhs,
                                                          const lv_freetype_cache_node_t * rhs);
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_freetype_init(uint32_t max_glyph_cnt)
{
    if(ft_ctx) {
        LV_LOG_WARN("freetype already initialized");
        return LV_RESULT_INVALID;
    }

    ft_ctx = lv_malloc_zeroed(sizeof(lv_freetype_context_t));
    LV_ASSERT_MALLOC(ft_ctx);
    if(!ft_ctx) {
        LV_LOG_ERROR("malloc failed for lv_freetype_context_t");
        return LV_RESULT_INVALID;
    }

    lv_freetype_context_t * ctx = lv_freetype_get_context();

    ctx->max_glyph_cnt = max_glyph_cnt;

    FT_Error error;

    error = FT_Init_FreeType(&ctx->library);
    if(error) {
        FT_ERROR_MSG("FT_Init_FreeType", error);
        return LV_RESULT_INVALID;
    }

    lv_ll_init(&ctx->face_id_ll, sizeof(face_id_node_t));

    lv_cache_ops_t ops = {
        .compare_cb = (lv_cache_compare_cb_t)cache_node_cache_compare_cb,
        .create_cb = (lv_cache_create_cb_t)cache_node_cache_create_cb,
        .free_cb = (lv_cache_free_cb_t)cache_node_cache_free_cb,
    };
    ctx->cache_node_cache = lv_cache_create(&lv_cache_class_lru_rb_count, sizeof(lv_freetype_cache_node_t), INT32_MAX, ops);
    lv_cache_set_name(ctx->cache_node_cache, "FREETYPE_CACHE_NODE");

    return LV_RESULT_OK;
}

void lv_freetype_uninit(void)
{
    lv_freetype_context_t * ctx = lv_freetype_get_context();
    lv_freetype_cleanup(ctx);

    lv_free(ft_ctx);
    ft_ctx = NULL;
}

lv_font_t * lv_freetype_font_create(const char * pathname, lv_freetype_font_render_mode_t render_mode, uint32_t size,
                                    lv_freetype_font_style_t style)
{
    LV_ASSERT_NULL(pathname);
    LV_ASSERT(size > 0);

    size_t pathname_len = lv_strlen(pathname);
    LV_ASSERT(pathname_len > 0);

    lv_freetype_context_t * ctx = lv_freetype_get_context();

    lv_freetype_cache_node_t search_key = {
        .pathname = lv_freetype_req_face_id(ctx, pathname),
        .style = style,
        .render_mode = render_mode,
    };

    bool cache_hitting = true;
    lv_cache_entry_t * cache_node_entry = lv_cache_acquire(ctx->cache_node_cache, &search_key, NULL);
    if(cache_node_entry == NULL) {
        cache_hitting = false;
        cache_node_entry = lv_cache_acquire_or_create(ctx->cache_node_cache, &search_key, NULL);
        if(cache_node_entry == NULL) {
            lv_freetype_drop_face_id(ctx, (FTC_FaceID)search_key.pathname);
            LV_LOG_ERROR("cache node creating failed");
            return NULL;
        }
    }

    lv_freetype_font_dsc_t * dsc = lv_malloc_zeroed(sizeof(lv_freetype_font_dsc_t));
    LV_ASSERT_MALLOC(dsc);

    dsc->face_id = (FTC_FaceID)search_key.pathname;
    dsc->render_mode = render_mode;
    dsc->context = ctx;
    dsc->size = size;
    dsc->style = style;
    dsc->magic_num = LV_FREETYPE_FONT_DSC_MAGIC_NUM;
    dsc->cache_node = lv_cache_entry_get_data(cache_node_entry);
    dsc->cache_node_entry = cache_node_entry;

    if(cache_hitting == false && freetype_on_font_create(dsc, ctx->max_glyph_cnt) == false) {
        lv_cache_release(ctx->cache_node_cache, dsc->cache_node_entry, NULL);
        lv_freetype_drop_face_id(ctx, dsc->face_id);
        lv_free(dsc);
        return NULL;
    }
    freetype_on_font_set_cbs(dsc);

    FT_Face face = dsc->cache_node->face;
    FT_Error error;
    if(FT_IS_SCALABLE(face)) {
        error = FT_Set_Pixel_Sizes(face, 0, size);
    }
    else {
        LV_LOG_WARN("font is not scalable, selecting available size");
        error = FT_Select_Size(face, 0);
    }
    if(error) {
        FT_ERROR_MSG("FT_Set_Pixel_Sizes", error);
        return NULL;
    }

    lv_font_t * font = &dsc->font;
    font->dsc = dsc;
    font->subpx = LV_FONT_SUBPX_NONE;
    font->line_height = FT_F26DOT6_TO_INT(face->size->metrics.height);
    font->base_line = -FT_F26DOT6_TO_INT(face->size->metrics.descender);

    FT_Fixed scale = face->size->metrics.y_scale;
    int8_t thickness = FT_F26DOT6_TO_INT(FT_MulFix(scale, face->underline_thickness));
    font->underline_position = FT_F26DOT6_TO_INT(FT_MulFix(scale, face->underline_position));
    font->underline_thickness = thickness < 1 ? 1 : thickness;

    return font;
}

void lv_freetype_font_delete(lv_font_t * font)
{
    LV_ASSERT_NULL(font);
    lv_freetype_context_t * ctx = lv_freetype_get_context();
    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)(font->dsc);
    LV_ASSERT_FREETYPE_FONT_DSC(dsc);

    lv_cache_release(ctx->cache_node_cache, dsc->cache_node_entry, NULL);
    if(lv_cache_entry_get_ref(dsc->cache_node_entry) == 0) {
        lv_cache_drop(ctx->cache_node_cache, dsc->cache_node, NULL);
    }

    lv_freetype_drop_face_id(dsc->context, dsc->face_id);

    /* invalidate magic number */
    lv_memzero(dsc, sizeof(lv_freetype_font_dsc_t));
    lv_free(dsc);
}

lv_freetype_context_t * lv_freetype_get_context(void)
{
    return LV_GLOBAL_DEFAULT()->ft_context;
}

void lv_freetype_italic_transform(FT_Face face)
{
    LV_ASSERT_NULL(face);
    FT_Matrix matrix;
    matrix.xx = FT_INT_TO_F16DOT16(1);
    matrix.xy = LV_FREETYPE_OBLIQUE_SLANT_DEF;
    matrix.yx = 0;
    matrix.yy = FT_INT_TO_F16DOT16(1);
    FT_Set_Transform(face, &matrix, NULL);
}

int32_t lv_freetype_italic_transform_on_pos(lv_point_t point)
{
    return point.x + FT_F16DOT16_TO_INT(point.y * LV_FREETYPE_OBLIQUE_SLANT_DEF);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool freetype_on_font_create(lv_freetype_font_dsc_t * dsc, uint32_t max_glyph_cnt)
{
    /*
     * Glyph info uses a small amount of memory, and uses glyph info more frequently,
     * so it plans to use twice the maximum number of caches here to
     * get a better info acquisition performance.*/
    lv_cache_t * glyph_cache = lv_freetype_create_glyph_cache(max_glyph_cnt * 2);
    if(glyph_cache == NULL) {
        LV_LOG_ERROR("glyph cache creating failed");
        return false;
    }
    dsc->cache_node->glyph_cache = glyph_cache;

    lv_cache_t * draw_data_cache = NULL;
    if(dsc->render_mode == LV_FREETYPE_FONT_RENDER_MODE_BITMAP) {
        draw_data_cache = lv_freetype_create_draw_data_image(max_glyph_cnt);
    }
    else if(dsc->render_mode == LV_FREETYPE_FONT_RENDER_MODE_OUTLINE) {
        draw_data_cache = lv_freetype_create_draw_data_outline(max_glyph_cnt);
    }
    else {
        LV_LOG_ERROR("unknown render mode");
        return false;
    }

    if(draw_data_cache == NULL) {
        LV_LOG_ERROR("draw data cache creating failed");
        return false;
    }

    dsc->cache_node->draw_data_cache = draw_data_cache;

    return true;
}

static void freetype_on_font_set_cbs(lv_freetype_font_dsc_t * dsc)
{
    lv_freetype_set_cbs_glyph(dsc);
    if(dsc->render_mode == LV_FREETYPE_FONT_RENDER_MODE_BITMAP) {
        lv_freetype_set_cbs_image_font(dsc);
    }
    else if(dsc->render_mode == LV_FREETYPE_FONT_RENDER_MODE_OUTLINE) {
        lv_freetype_set_cbs_outline_font(dsc);
    }
}

static void lv_freetype_cleanup(lv_freetype_context_t * ctx)
{
    LV_ASSERT_NULL(ctx);
    if(ctx->cache_node_cache) {
        lv_cache_destroy(ctx->cache_node_cache, NULL);
        ctx->cache_node_cache = NULL;
    }

    if(ctx->library) {
        FT_Done_FreeType(ctx->library);
        ctx->library = NULL;
    }
}

static FTC_FaceID lv_freetype_req_face_id(lv_freetype_context_t * ctx, const char * pathname)
{
    size_t len = lv_strlen(pathname);
    LV_ASSERT(len > 0);

    lv_ll_t * ll_p = &ctx->face_id_ll;
    face_id_node_t * node;

    /* search cache */
    LV_LL_READ(ll_p, node) {
        if(strcmp(node->pathname, pathname) == 0) {
            node->ref_cnt++;
            LV_LOG_INFO("reuse face_id: %s, ref_cnt = %d", node->pathname, node->ref_cnt);
            return node->pathname;
        }
    }

    /* insert new cache */
    node = lv_ll_ins_tail(ll_p);
    LV_ASSERT_MALLOC(node);

#if LV_USE_FS_MEMFS
    if(pathname[0] == LV_FS_MEMFS_LETTER) {
#if !LV_FREETYPE_USE_LVGL_PORT
        LV_LOG_WARN("LV_FREETYPE_USE_LVGL_PORT is not enabled");
#endif
        node->pathname = lv_malloc(sizeof(lv_fs_path_ex_t));
        LV_ASSERT_MALLOC(node->pathname);
        lv_memcpy(node->pathname, pathname, sizeof(lv_fs_path_ex_t));
    }
    else
#endif
    {
        node->pathname = lv_strdup(pathname);
        LV_ASSERT_NULL(node->pathname);
    }

    LV_LOG_INFO("add face_id: %s", node->pathname);

    node->ref_cnt = 1;
    return node->pathname;
}

static void lv_freetype_drop_face_id(lv_freetype_context_t * ctx, FTC_FaceID face_id)
{
    lv_ll_t * ll_p = &ctx->face_id_ll;
    face_id_node_t * node;
    LV_LL_READ(ll_p, node) {
        if(face_id == node->pathname) {
            LV_LOG_INFO("found face_id: %s, ref_cnt = %d", node->pathname, node->ref_cnt);
            node->ref_cnt--;
            if(node->ref_cnt == 0) {
                LV_LOG_INFO("drop face_id: %s", node->pathname);
                lv_ll_remove(ll_p, node);
                lv_free(node->pathname);
                lv_free(node);
            }
            return;
        }
    }

    LV_ASSERT_MSG(false, "face_id not found");
}

/*-----------------
 * Cache Node Cache Callbacks
 *----------------*/

static bool cache_node_cache_create_cb(lv_freetype_cache_node_t * node, void * user_data)
{
    LV_UNUSED(user_data);
    lv_freetype_context_t * ctx = lv_freetype_get_context();

    /* Cache miss, load face */
    FT_Face face;
    FT_Error error = FT_New_Face(ctx->library, node->pathname, 0, &face);
    if(error) {
        FT_ERROR_MSG("FT_New_Face", error);
        return false;
    }

    node->ref_size = LV_FREETYPE_OUTLINE_REF_SIZE_DEF;

    if(node->style & LV_FREETYPE_FONT_STYLE_ITALIC) {
        lv_freetype_italic_transform(face);
    }

    node->face = face;
    lv_mutex_init(&node->face_lock);

    return true;
}
static void cache_node_cache_free_cb(lv_freetype_cache_node_t * node, void * user_data)
{
    FT_Done_Face(node->face);
    lv_mutex_delete(&node->face_lock);

    if(node->glyph_cache) {
        lv_cache_destroy(node->glyph_cache, user_data);
        node->glyph_cache = NULL;
    }
    if(node->draw_data_cache) {
        lv_cache_destroy(node->draw_data_cache, user_data);
        node->draw_data_cache = NULL;
    }
}
static lv_cache_compare_res_t cache_node_cache_compare_cb(const lv_freetype_cache_node_t * lhs,
                                                          const lv_freetype_cache_node_t * rhs)
{
    if(lhs->render_mode != rhs->render_mode) {
        return lhs->render_mode > rhs->render_mode ? 1 : -1;
    }
    if(lhs->style != rhs->style) {
        return lhs->style > rhs->style ? 1 : -1;
    }

    int32_t cmp_res = lv_strcmp(lhs->pathname, rhs->pathname);
    if(cmp_res != 0) {
        return cmp_res > 0 ? 1 : -1;
    }

    return 0;
}

#endif /*LV_USE_FREETYPE*/
