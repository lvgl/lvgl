/**
 * @file lv_freetype.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_freetype.h"

#if LV_USE_FREETYPE

#include "lv_freetype_private.h"
#include "../../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/

#define ft_ctx LV_GLOBAL_DEFAULT()->ft_context

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

static FT_Error lv_freetype_face_requester(FTC_FaceID face_id,
                                           FT_Library library,
                                           FT_Pointer req_data,
                                           FT_Face * aface);
static void lv_freetype_cleanup(lv_freetype_context_t * ctx);
static FTC_FaceID lv_freetype_req_face_id(lv_freetype_context_t * ctx, const char * pathname);
static void lv_freetype_drop_face_id(lv_freetype_context_t * ctx, FTC_FaceID face_id);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_freetype_init(uint32_t max_faces, uint32_t max_sizes, uint32_t max_kilobytes)
{
    if(ft_ctx) {
        LV_LOG_WARN("freetype already initialized");
        return LV_RESULT_INVALID;
    }

    ft_ctx = lv_malloc(sizeof(lv_freetype_context_t));
    LV_ASSERT_MALLOC(ft_ctx);
    if(!ft_ctx) {
        LV_LOG_ERROR("malloc failed for lv_freetype_context_t");
        return LV_RESULT_INVALID;
    }

    lv_freetype_context_t * ctx = lv_freetype_get_context();
    FT_Error error;

    error = FT_Init_FreeType(&ctx->library);
    if(error) {
        FT_ERROR_MSG("FT_Init_FreeType", error);
        return LV_RESULT_INVALID;
    }

    error = FTC_Manager_New(ctx->library,
                            max_faces,
                            max_sizes,
                            max_kilobytes * 1024,
                            lv_freetype_face_requester,
                            NULL,
                            &ctx->cache_manager);
    if(error) {
        FT_ERROR_MSG("FTC_Manager_New", error);
        lv_freetype_cleanup(ctx);
        return LV_RESULT_INVALID;
    }

    error = FTC_CMapCache_New(ctx->cache_manager, &ctx->cmap_cache);
    if(error) {
        FT_ERROR_MSG("FTC_CMapCache_New", error);
        lv_freetype_cleanup(ctx);
        return LV_RESULT_INVALID;
    }

    ctx->cache_context = lv_freetype_cache_context_create(ctx);

    if(!ctx->cache_context) {
        LV_LOG_ERROR("cache context create failed");
        lv_freetype_cleanup(ctx);
        return LV_RESULT_INVALID;
    }

    _lv_ll_init(&ctx->face_id_ll, sizeof(face_id_node_t));

    return LV_RESULT_OK;
}

void lv_freetype_uninit(void)
{
    lv_freetype_context_t * ctx = lv_freetype_get_context();
    lv_freetype_cleanup(ctx);

    lv_free(ft_ctx);
    ft_ctx = NULL;
}

lv_font_t * lv_freetype_font_create(const char * pathname, uint32_t size, lv_freetype_font_style_t style)
{
    LV_ASSERT_NULL(pathname);
    LV_ASSERT(size > 0);

    lv_freetype_context_t * ctx = lv_freetype_get_context();

    size_t pathname_len = lv_strlen(pathname);
    LV_ASSERT(pathname_len > 0);

    lv_freetype_font_dsc_t * dsc = lv_malloc_zeroed(sizeof(lv_freetype_font_dsc_t));
    LV_ASSERT_MALLOC(dsc);

    dsc->face_id = lv_freetype_req_face_id(ctx, pathname);
    dsc->context = ctx;
    dsc->size = size;
    dsc->style = style;
    dsc->magic_num = LV_FREETYPE_FONT_DSC_MAGIC_NUM;

    /* get font info */
    FT_Size ft_size = lv_freetype_lookup_size(dsc);

    if(!ft_size || !lv_freetype_on_font_create(dsc)) {
        lv_freetype_drop_face_id(ctx, dsc->face_id);
        lv_free(dsc);
        return NULL;
    }

    lv_font_t * font = &dsc->font;
    font->dsc = dsc;
    font->subpx = LV_FONT_SUBPX_NONE;
    font->line_height = FT_F26DOT6_TO_INT(ft_size->face->size->metrics.height);
    font->base_line = -FT_F26DOT6_TO_INT(ft_size->face->size->metrics.descender);

    FT_Fixed scale = ft_size->face->size->metrics.y_scale;
    int8_t thickness = FT_F26DOT6_TO_INT(FT_MulFix(scale, ft_size->face->underline_thickness));
    font->underline_position = FT_F26DOT6_TO_INT(FT_MulFix(scale, ft_size->face->underline_position));
    font->underline_thickness = thickness < 1 ? 1 : thickness;

    return font;
}

void lv_freetype_font_delete(lv_font_t * font)
{
    LV_ASSERT_NULL(font);
    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)(font->dsc);
    LV_ASSERT_NULL(dsc);
    LV_ASSERT_FREETYPE_FONT_DSC(dsc);

    lv_freetype_on_font_delete(dsc);
    lv_freetype_drop_face_id(dsc->context, dsc->face_id);

    /* invalidate magic number */
    lv_memzero(dsc, sizeof(lv_freetype_font_dsc_t));
    lv_free(dsc);
}

lv_freetype_context_t * lv_freetype_get_context(void)
{
    return LV_GLOBAL_DEFAULT()->ft_context;
}

FT_Size lv_freetype_lookup_size(const lv_freetype_font_dsc_t * dsc)
{
    FT_Error error;
    lv_freetype_context_t * ctx = dsc->context;

    FT_Size ft_size;
    struct FTC_ScalerRec_ scaler;
    scaler.face_id = dsc->face_id;
    scaler.width = dsc->size;
    scaler.height = dsc->size;
    scaler.pixel = 1;
    error = FTC_Manager_LookupSize(ctx->cache_manager, &scaler, &ft_size);

    if(error) {
        FT_ERROR_MSG("FTC_Manager_LookupSize", error);
        return NULL;
    }

    return ft_size;
}

void lv_freetype_italic_transform(FT_Face face)
{
    LV_ASSERT_NULL(face);
    FT_Matrix matrix;
    matrix.xx = FT_INT_TO_F16DOT16(1);
    matrix.xy = 0x5800;
    matrix.yx = 0;
    matrix.yy = FT_INT_TO_F16DOT16(1);
    FT_Set_Transform(face, &matrix, NULL);
}

const char * lv_freetype_get_pathname(FTC_FaceID face_id)
{
    LV_ASSERT_NULL(face_id);
    return (const char *)face_id;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static FT_Error lv_freetype_face_requester(FTC_FaceID face_id,
                                           FT_Library library,
                                           FT_Pointer req_data,
                                           FT_Face * aface)
{
    LV_UNUSED(library);
    LV_UNUSED(req_data);

    const char * pathname = lv_freetype_get_pathname(face_id);

    FT_Error error = FT_New_Face(library, pathname, 0, aface);
    if(error) {
        FT_ERROR_MSG("FT_New_Face", error);
        LV_LOG_ERROR("error pathname = %s", pathname);
    }
    return error;
}

static void lv_freetype_cleanup(lv_freetype_context_t * ctx)
{
    LV_ASSERT_NULL(ctx);

    if(ctx->cache_context) {
        lv_freetype_cache_context_delete(ctx->cache_context);
        ctx->cache_context = NULL;
    }

    if(ctx->cache_manager) {
        FTC_Manager_Done(ctx->cache_manager);
        ctx->cache_manager = NULL;
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
    _LV_LL_READ(ll_p, node) {
        if(strcmp(node->pathname, pathname) == 0) {
            node->ref_cnt++;
            LV_LOG_INFO("reuse face_id: %s, ref_cnt = %d", node->pathname, node->ref_cnt);
            return node->pathname;
        }
    }

    /* insert new cache */
    node = _lv_ll_ins_tail(ll_p);
    LV_ASSERT_MALLOC(node);

    node->pathname = lv_malloc(len + 1);
    LV_ASSERT_MALLOC(node->pathname);
    strcpy(node->pathname, pathname);

    LV_LOG_INFO("add face_id: %s", node->pathname);

    node->ref_cnt = 1;
    return node->pathname;
}

static void lv_freetype_drop_face_id(lv_freetype_context_t * ctx, FTC_FaceID face_id)
{
    lv_ll_t * ll_p = &ctx->face_id_ll;
    face_id_node_t * node;
    _LV_LL_READ(ll_p, node) {
        if(face_id == node->pathname) {
            LV_LOG_INFO("found face_id: %s, ref_cnt = %d", node->pathname, node->ref_cnt);
            node->ref_cnt--;
            if(node->ref_cnt == 0) {
                LV_LOG_INFO("drop face_id: %s", node->pathname);
                _lv_ll_remove(ll_p, node);
                FTC_Manager_RemoveFaceID(ctx->cache_manager, face_id);
                lv_free(node->pathname);
                lv_free(node);
            }
            return;
        }
    }

    LV_ASSERT_MSG(false, "face_id not found");
}

#endif /*LV_USE_FREETYPE*/
