/**
 * @file lv_freetype.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_freetype.h"
#if LV_USE_FREETYPE

#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_CACHE_H
#include FT_SIZES_H
#include FT_IMAGE_H
#include FT_OUTLINE_H

/*********************
 *      DEFINES
 *********************/
#ifdef FT_CONFIG_OPTION_ERROR_STRINGS
#define FT_ERROR_MSG(msg, error_code) \
    LV_LOG_ERROR(msg " error(%d): %s", (int)error_code, FT_Error_String(error_code))
#else
#define FT_ERROR_MSG(msg, error_code) \
    LV_LOG_ERROR(msg " error(%d)", (int)error_code)
#endif

#if LV_FREETYPE_CACHE_SIZE <= 0
    #error "LV_FREETYPE_CACHE_SIZE must > 0"
#endif

/**********************
 *      TYPEDEFS
 **********************/
typedef struct _lv_freetype_font_dsc_t {
    lv_font_t * font;
    char * pathname;
    uint16_t size;
    uint16_t style;
} lv_freetype_font_dsc_t;

typedef struct _lv_freetype_context_t {
    FT_Library library;
    FTC_Manager cache_manager;
    FTC_CMapCache cmap_cache;
    FT_Face current_face;
#if LV_FREETYPE_SBIT_CACHE
    FTC_SBitCache sbit_cache;
    FTC_SBit sbit;
#else
    FTC_ImageCache image_cache;
    FT_Glyph image_glyph;
#endif
} lv_freetype_context_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static FT_Error freetpye_face_requester(FTC_FaceID face_id,
                                        FT_Library library,
                                        FT_Pointer req_data,
                                        FT_Face * aface);
static FT_Error freetype_get_bold_glyph(const lv_font_t * font,
                                        FT_Face face,
                                        FT_UInt glyph_index,
                                        lv_font_glyph_dsc_t * dsc_out);
static bool freetype_get_glyph_dsc_cb(const lv_font_t * font,
                                      lv_font_glyph_dsc_t * dsc_out,
                                      uint32_t unicode_letter,
                                      uint32_t unicode_letter_next);
static const uint8_t * freetype_get_glyph_bitmap_cb(const lv_font_t * font,
                                                    uint32_t unicode_letter,
                                                    uint8_t * buf_out);

/**********************
*  STATIC VARIABLES
**********************/

static lv_freetype_context_t ft_ctx;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_res_t lv_freetype_init(uint16_t max_faces, uint16_t max_sizes, uint32_t max_bytes)
{
    FT_Error error;

    error = FT_Init_FreeType(&ft_ctx.library);
    if(error) {
        FT_ERROR_MSG("FT_Init_FreeType", error);
        return LV_RES_INV;
    }

    error = FTC_Manager_New(ft_ctx.library,
                            max_faces,
                            max_sizes,
                            max_bytes,
                            freetpye_face_requester,
                            NULL,
                            &ft_ctx.cache_manager);
    if(error) {
        FT_Done_FreeType(ft_ctx.library);
        FT_ERROR_MSG("FTC_Manager_New", error);
        return LV_RES_INV;
    }

    error = FTC_CMapCache_New(ft_ctx.cache_manager, &ft_ctx.cmap_cache);
    if(error) {
        FT_ERROR_MSG("FTC_CMapCache_New", error);
        goto failed;
    }

#if LV_FREETYPE_SBIT_CACHE
    error = FTC_SBitCache_New(ft_ctx.cache_manager, &ft_ctx.sbit_cache);
    if(error) {
        FT_ERROR_MSG("FTC_SBitCache_New", error);
        goto failed;
    }
#else
    error = FTC_ImageCache_New(ft_ctx.cache_manager, &ft_ctx.image_cache);
    if(error) {
        FT_ERROR_MSG("FTC_ImageCache_New", error);
        goto failed;
    }
#endif

    return LV_RES_OK;
failed:
    FTC_Manager_Done(ft_ctx.cache_manager);
    FT_Done_FreeType(ft_ctx.library);
    return LV_RES_INV;
}

void lv_freetype_uninit(void)
{
    FTC_Manager_Done(ft_ctx.cache_manager);
    FT_Done_FreeType(ft_ctx.library);
}

lv_font_t * lv_freetype_font_create(const char * pathname, uint16_t size, uint16_t style)
{
    LV_ASSERT_NULL(pathname);
    LV_ASSERT(size > 0);

    size_t pathname_len = lv_strlen(pathname);
    if(pathname_len == 0) {
        LV_LOG_ERROR("pathname is empty");
        return NULL;
    }

    size_t need_size = sizeof(lv_freetype_font_dsc_t) + sizeof(lv_font_t);
    lv_freetype_font_dsc_t * dsc = lv_malloc(need_size);
    LV_ASSERT_MALLOC(dsc);
    if(!dsc) {
        LV_LOG_ERROR("malloc failed for lv_freetype_font_dsc");
        return NULL;
    }
    lv_memzero(dsc, need_size);

    dsc->font = (lv_font_t *)(((uint8_t *)dsc) + sizeof(lv_freetype_font_dsc_t));

    dsc->pathname = lv_malloc(pathname_len + 1);
    LV_ASSERT_MALLOC(dsc->pathname);
    if(!dsc->pathname) {
        LV_LOG_ERROR("malloc failed for dsc->pathname");
        lv_free(dsc);
        return NULL;
    }
    lv_strcpy(dsc->pathname, pathname);

    dsc->size = size;
    dsc->style = style;

    /* use to get font info */
    FT_Size face_size;
    struct FTC_ScalerRec_ scaler;
    scaler.face_id = (FTC_FaceID)dsc;
    scaler.width = size;
    scaler.height = size;
    scaler.pixel = 1;
    FT_Error error = FTC_Manager_LookupSize(ft_ctx.cache_manager,
                                            &scaler,
                                            &face_size);
    if(error) {
        FT_ERROR_MSG("FTC_Manager_LookupSize", error);
        lv_free(dsc->pathname);
        lv_free(dsc);
        return NULL;
    }

    lv_font_t * font = dsc->font;
    font->dsc = dsc;
    font->get_glyph_dsc = freetype_get_glyph_dsc_cb;
    font->get_glyph_bitmap = freetype_get_glyph_bitmap_cb;
    font->subpx = LV_FONT_SUBPX_NONE;
    font->line_height = (face_size->face->size->metrics.height >> 6);
    font->base_line = -(face_size->face->size->metrics.descender >> 6);

    FT_Fixed scale = face_size->face->size->metrics.y_scale;
    int8_t thickness = FT_MulFix(scale, face_size->face->underline_thickness) >> 6;
    font->underline_position = FT_MulFix(scale, face_size->face->underline_position) >> 6;
    font->underline_thickness = thickness < 1 ? 1 : thickness;

    return font;
}

void lv_freetype_font_del(lv_font_t * font)
{
    LV_ASSERT_NULL(font);
    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)(font->dsc);
    LV_ASSERT_NULL(dsc);
    FTC_Manager_RemoveFaceID(ft_ctx.cache_manager, (FTC_FaceID)dsc);
    lv_free(dsc->pathname);
    lv_free(dsc);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static FT_Error freetpye_face_requester(FTC_FaceID face_id,
                                        FT_Library library,
                                        FT_Pointer req_data,
                                        FT_Face * aface)
{
    LV_UNUSED(library);
    LV_UNUSED(req_data);

    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)face_id;
    FT_Error error;

    error = FT_New_Face(ft_ctx.library, dsc->pathname, 0, aface);
    if(error) {
        FT_ERROR_MSG("FT_New_Face", error);
    }
    return error;
}

static FT_Error freetype_get_bold_glyph(const lv_font_t * font,
                                        FT_Face face,
                                        FT_UInt glyph_index,
                                        lv_font_glyph_dsc_t * dsc_out)
{
    FT_Error error;
    error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
    if(error) {
        FT_ERROR_MSG("FT_Load_Glyph", error);
        return error;
    }

    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)(font->dsc);
    if(face->glyph->format == FT_GLYPH_FORMAT_OUTLINE) {
        if(dsc->style & LV_FREETYPE_FONT_STYLE_BOLD) {
            int strength = 1 << 6;
            error = FT_Outline_Embolden(&face->glyph->outline, strength);
            if(error) {
                FT_ERROR_MSG("FT_Outline_Embolden", error);
            }
        }
    }

    error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
    if(error) {
        FT_ERROR_MSG("FT_Render_Glyph", error);
        return error;
    }

    dsc_out->adv_w = (face->glyph->metrics.horiAdvance >> 6);
    dsc_out->box_h = face->glyph->bitmap.rows;         /*Height of the bitmap in [px]*/
    dsc_out->box_w = face->glyph->bitmap.width;        /*Width of the bitmap in [px]*/
    dsc_out->ofs_x = face->glyph->bitmap_left;         /*X offset of the bitmap in [pf]*/
    dsc_out->ofs_y = face->glyph->bitmap_top -
                     face->glyph->bitmap.rows;         /*Y offset of the bitmap measured from the as line*/
    dsc_out->bpp = 8;                                  /*Bit per pixel: 1/2/4/8*/

    return FT_Err_Ok;
}

static bool freetype_get_glyph_dsc_cb(const lv_font_t * font,
                                      lv_font_glyph_dsc_t * dsc_out,
                                      uint32_t unicode_letter,
                                      uint32_t unicode_letter_next)
{
    LV_UNUSED(unicode_letter_next);
    if(unicode_letter < 0x20) {
        dsc_out->adv_w = 0;
        dsc_out->box_h = 0;
        dsc_out->box_w = 0;
        dsc_out->ofs_x = 0;
        dsc_out->ofs_y = 0;
        dsc_out->bpp = 0;
        return true;
    }

    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)(font->dsc);

    FTC_FaceID face_id = (FTC_FaceID)dsc;
    FT_Size face_size;
    FT_Error error;
    struct FTC_ScalerRec_ scaler;
    scaler.face_id = face_id;
    scaler.width = dsc->size;
    scaler.height = dsc->size;
    scaler.pixel = 1;
    error = FTC_Manager_LookupSize(ft_ctx.cache_manager, &scaler, &face_size);
    if(error) {
        FT_ERROR_MSG("FTC_Manager_LookupSize", error);
        return false;
    }

    FT_Face face = face_size->face;
    FT_UInt charmap_index = FT_Get_Charmap_Index(face->charmap);
    FT_UInt glyph_index = FTC_CMapCache_Lookup(ft_ctx.cmap_cache, face_id, charmap_index, unicode_letter);
    dsc_out->is_placeholder = glyph_index == 0;

    if(dsc->style & LV_FREETYPE_FONT_STYLE_ITALIC) {
        FT_Matrix italic_matrix;
        italic_matrix.xx = 1 << 16;
        italic_matrix.xy = 0x5800;
        italic_matrix.yx = 0;
        italic_matrix.yy = 1 << 16;
        FT_Set_Transform(face, &italic_matrix, NULL);
    }

    if(dsc->style & LV_FREETYPE_FONT_STYLE_BOLD) {
        ft_ctx.current_face = face;
        error = freetype_get_bold_glyph(font, face, glyph_index, dsc_out);
        if(error) {
            ft_ctx.current_face = NULL;
            return false;
        }
        goto end;
    }

    FTC_ImageTypeRec desc_type;
    desc_type.face_id = face_id;
    desc_type.flags = FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL;
    desc_type.height = dsc->size;
    desc_type.width = dsc->size;

#if LV_FREETYPE_SBIT_CACHE
    error = FTC_SBitCache_Lookup(ft_ctx.sbit_cache,
                                 &desc_type,
                                 glyph_index,
                                 &ft_ctx.sbit,
                                 NULL);
    if(error) {
        FT_ERROR_MSG("FTC_SBitCache_Lookup", error);
        return false;
    }

    FTC_SBit sbit = ft_ctx.sbit;
    dsc_out->adv_w = sbit->xadvance;
    dsc_out->box_h = sbit->height;  /*Height of the bitmap in [px]*/
    dsc_out->box_w = sbit->width;   /*Width of the bitmap in [px]*/
    dsc_out->ofs_x = sbit->left;    /*X offset of the bitmap in [pf]*/
    dsc_out->ofs_y = sbit->top - sbit->height; /*Y offset of the bitmap measured from the as line*/
    dsc_out->bpp = 8;               /*Bit per pixel: 1/2/4/8*/
#else
    error = FTC_ImageCache_Lookup(ft_ctx.image_cache,
                                  &desc_type,
                                  glyph_index,
                                  &ft_ctx.image_glyph,
                                  NULL);
    if(error) {
        FT_ERROR_MSG("ImageCache_Lookup", error);
        return false;
    }
    if(ft_ctx.image_glyph->format != FT_GLYPH_FORMAT_BITMAP) {
        LV_LOG_ERROR("image_glyph->format != FT_GLYPH_FORMAT_BITMAP");
        return false;
    }

    FT_BitmapGlyph glyph_bitmap = (FT_BitmapGlyph)ft_ctx.image_glyph;
    dsc_out->adv_w = (glyph_bitmap->root.advance.x >> 16);
    dsc_out->box_h = glyph_bitmap->bitmap.rows;         /*Height of the bitmap in [px]*/
    dsc_out->box_w = glyph_bitmap->bitmap.width;        /*Width of the bitmap in [px]*/
    dsc_out->ofs_x = glyph_bitmap->left;                /*X offset of the bitmap in [pf]*/
    dsc_out->ofs_y = glyph_bitmap->top -
                     glyph_bitmap->bitmap.rows;         /*Y offset of the bitmap measured from the as line*/
    dsc_out->bpp = 8;         /*Bit per pixel: 1/2/4/8*/
#endif

end:
    if((dsc->style & LV_FREETYPE_FONT_STYLE_ITALIC) && (unicode_letter_next == '\0')) {
        dsc_out->adv_w = dsc_out->box_w + dsc_out->ofs_x;
    }

    return true;
}

static const uint8_t * freetype_get_glyph_bitmap_cb(const lv_font_t * font, uint32_t unicode_letter, uint8_t * buf_out)
{
    LV_UNUSED(unicode_letter);
    LV_UNUSED(buf_out);

    lv_freetype_font_dsc_t * dsc = (lv_freetype_font_dsc_t *)font->dsc;
    if(dsc->style & LV_FREETYPE_FONT_STYLE_BOLD) {
        if(ft_ctx.current_face && ft_ctx.current_face->glyph->format == FT_GLYPH_FORMAT_BITMAP) {
            return (const uint8_t *)(ft_ctx.current_face->glyph->bitmap.buffer);
        }
        return NULL;
    }

#if LV_FREETYPE_SBIT_CACHE
    return (const uint8_t *)ft_ctx.sbit->buffer;
#else
    FT_BitmapGlyph glyph_bitmap = (FT_BitmapGlyph)ft_ctx.image_glyph;
    return (const uint8_t *)glyph_bitmap->bitmap.buffer;
#endif
}

#endif /*LV_USE_FREETYPE*/

