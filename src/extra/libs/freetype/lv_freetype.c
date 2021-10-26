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

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    char * name;
} lv_face_info_t;

typedef struct {
    lv_ll_t  face_ll;
} lv_faces_control_t;

typedef struct {
#if LV_FREETYPE_CACHE_SIZE >= 0
    void * face_id;
#else
    FT_Size     size;
#endif
    lv_font_t * font;
    uint16_t    style;
    uint16_t    height;
} lv_font_fmt_ft_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_FREETYPE_CACHE_SIZE >= 0
static FT_Error font_face_requester(FTC_FaceID face_id,
                                    FT_Library library_is, FT_Pointer req_data, FT_Face * aface);
static bool lv_ft_font_init_cache(lv_ft_info_t * info);
static void lv_ft_font_destroy_cache(lv_font_t * font);
static bool lv_ft_font_init_cache(lv_ft_info_t * info);
static void lv_ft_font_destroy_cache(lv_font_t * font);
#else
static FT_Face face_find_in_list(lv_ft_info_t * info);
static void face_add_to_list(FT_Face face);
static void face_remove_from_list(FT_Face face);
static void face_generic_finalizer(void * object);
static bool lv_ft_font_init_nocache(lv_ft_info_t * info);
static void lv_ft_font_destroy_nocache(lv_font_t * font);
#endif

/**********************
*  STATIC VARIABLES
**********************/
static FT_Library library;

#if LV_FREETYPE_CACHE_SIZE >= 0
    static FTC_Manager cache_manager;
    static FTC_CMapCache cmap_cache;
    static FTC_SBitCache sbit_cache;
    static FTC_SBit sbit;
#else
    static lv_faces_control_t face_control;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

bool lv_freetype_init(uint16_t max_faces, uint16_t max_sizes, uint32_t max_bytes)
{
    FT_Error error = FT_Init_FreeType(&library);
    if(error) {
        LV_LOG_ERROR("init freeType error(%d)", error);
        return false;
    }

#if LV_FREETYPE_CACHE_SIZE >= 0
    error = FTC_Manager_New(library, max_faces, max_sizes,
                            max_bytes, font_face_requester, NULL, &cache_manager);
    if(error) {
        FT_Done_FreeType(library);
        LV_LOG_ERROR("Failed to open cache manager");
        return false;
    }

    error = FTC_CMapCache_New(cache_manager, &cmap_cache);
    if(error) {
        LV_LOG_ERROR("Failed to open Cmap Cache");
        goto Fail;
    }

    error = FTC_SBitCache_New(cache_manager, &sbit_cache);
    if(error) {
        LV_LOG_ERROR("Failed to open sbit cache");
        goto Fail;
    }

    return true;
Fail:
    FTC_Manager_Done(cache_manager);
    FT_Done_FreeType(library);
    return false;
#else
    LV_UNUSED(max_faces);
    LV_UNUSED(max_sizes);
    LV_UNUSED(max_bytes);
    _lv_ll_init(&face_control.face_ll, sizeof(FT_Face *));
    return true;
#endif/* LV_FREETYPE_CACHE_SIZE */
}

void lv_freetype_destroy(void)
{
#if LV_FREETYPE_CACHE_SIZE >= 0
    FTC_Manager_Done(cache_manager);
#endif
    FT_Done_FreeType(library);
}

bool lv_ft_font_init(lv_ft_info_t * info)
{
#if LV_FREETYPE_CACHE_SIZE >= 0
    return lv_ft_font_init_cache(info);
#else
    return lv_ft_font_init_nocache(info);
#endif
}

void lv_ft_font_destroy(lv_font_t * font)
{
#if LV_FREETYPE_CACHE_SIZE >= 0
    lv_ft_font_destroy_cache(font);
#else
    lv_ft_font_destroy_nocache(font);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
#if LV_FREETYPE_CACHE_SIZE >= 0

static FT_Error font_face_requester(FTC_FaceID face_id,
                                    FT_Library library_is, FT_Pointer req_data, FT_Face * aface)
{
    LV_UNUSED(library_is);
    LV_UNUSED(req_data);

    lv_face_info_t * info = (lv_face_info_t *)face_id;
    FT_Error error = FT_New_Face(library, info->name, 0, aface);
    if(error) {
        LV_LOG_ERROR("FT_New_Face error:%d\n", error);
        return error;
    }
    return FT_Err_Ok;
}

static bool get_glyph_dsc_cb_cache(const lv_font_t * font,
                                   lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next)
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

    lv_font_fmt_ft_dsc_t * dsc = (lv_font_fmt_ft_dsc_t *)(font->dsc);

    FT_Face face;
    FTC_ImageTypeRec desc_sbit_type;
    FTC_FaceID face_id = (FTC_FaceID)dsc->face_id;
    FTC_Manager_LookupFace(cache_manager, face_id, &face);

    desc_sbit_type.face_id = face_id;
    desc_sbit_type.flags = FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL;
    desc_sbit_type.height = dsc->height;
    desc_sbit_type.width = dsc->height;
    FT_UInt charmap_index = FT_Get_Charmap_Index(face->charmap);
    FT_UInt glyph_index = FTC_CMapCache_Lookup(cmap_cache, face_id, charmap_index, unicode_letter);
    FT_Error error = FTC_SBitCache_Lookup(sbit_cache, &desc_sbit_type, glyph_index, &sbit, NULL);
    if(error) {
        LV_LOG_ERROR("SBitCache_Lookup error");
    }

    dsc_out->adv_w = sbit->xadvance;
    dsc_out->box_h = sbit->height;  /*Height of the bitmap in [px]*/
    dsc_out->box_w = sbit->width;   /*Width of the bitmap in [px]*/
    dsc_out->ofs_x = sbit->left;    /*X offset of the bitmap in [pf]*/
    dsc_out->ofs_y = sbit->top - sbit->height; /*Y offset of the bitmap measured from the as line*/
    dsc_out->bpp = 8;               /*Bit per pixel: 1/2/4/8*/

    return true;
}

static const uint8_t * get_glyph_bitmap_cb_cache(const lv_font_t * font, uint32_t unicode_letter)
{
    LV_UNUSED(font);
    LV_UNUSED(unicode_letter);
    return (const uint8_t *)sbit->buffer;
}

static bool lv_ft_font_init_cache(lv_ft_info_t * info)
{
    lv_font_fmt_ft_dsc_t * dsc = lv_mem_alloc(sizeof(lv_font_fmt_ft_dsc_t));
    if(dsc == NULL) return false;

    dsc->font = lv_mem_alloc(sizeof(lv_font_t));
    if(dsc->font == NULL) {
        lv_mem_free(dsc);
        return false;
    }

    lv_face_info_t * face_info = NULL;
    face_info = lv_mem_alloc(sizeof(lv_face_info_t) + strlen(info->name) + 1);
    if(face_info == NULL) {
        goto Fail;
    }
    face_info->name = ((char *)face_info) + sizeof(lv_face_info_t);
    strcpy(face_info->name, info->name);

    dsc->face_id = face_info;
    dsc->height = info->weight;
    dsc->style = info->style;

    /* use to get font info */
    FT_Size face_size;
    struct FTC_ScalerRec_ scaler;
    scaler.face_id = (FTC_FaceID)dsc->face_id;
    scaler.width = info->weight;
    scaler.height = info->weight;
    scaler.pixel = 1;
    FT_Error error = FTC_Manager_LookupSize(cache_manager, &scaler, &face_size);
    if(error) {
        lv_mem_free(face_info);
        LV_LOG_ERROR("Failed to LookupSize");
        goto Fail;
    }

    lv_font_t * font = dsc->font;
    font->dsc = dsc;
    font->get_glyph_dsc = get_glyph_dsc_cb_cache;
    font->get_glyph_bitmap = get_glyph_bitmap_cb_cache;
    font->subpx = LV_FONT_SUBPX_NONE;
    font->line_height = (face_size->face->size->metrics.height >> 6);
    font->base_line = -(face_size->face->size->metrics.descender >> 6);

    FT_Fixed scale = face_size->face->size->metrics.y_scale;
    int8_t thickness = FT_MulFix(scale, face_size->face->underline_thickness) >> 6;
    font->underline_position = FT_MulFix(scale, face_size->face->underline_position) >> 6;
    font->underline_thickness = thickness < 1 ? 1 : thickness;

    /* return to user */
    info->font = font;

    return true;

Fail:
    lv_mem_free(dsc->font);
    lv_mem_free(dsc);
    return false;
}

void lv_ft_font_destroy_cache(lv_font_t * font)
{
    if(font == NULL) {
        return;
    }

    lv_font_fmt_ft_dsc_t * dsc = (lv_font_fmt_ft_dsc_t *)(font->dsc);
    if(dsc) {
        lv_mem_free(dsc->face_id);
        lv_mem_free(dsc->font);
        lv_mem_free(dsc);
    }
}

#else/* LV_FREETYPE_CACHE_SIZE */

static FT_Face face_find_in_list(lv_ft_info_t * info)
{
    lv_face_info_t * face_info;
    FT_Face * pface = _lv_ll_get_head(&face_control.face_ll);
    while(pface) {
        face_info = (lv_face_info_t *)(*pface)->generic.data;
        if(strcmp(face_info->name, info->name) == 0) {
            return *pface;
        }
        pface = _lv_ll_get_next(&face_control.face_ll, pface);
    }

    return NULL;
}

static void face_add_to_list(FT_Face face)
{
    FT_Face * pface;
    pface = (FT_Face *)_lv_ll_ins_tail(&face_control.face_ll);
    *pface = face;
}

static void face_remove_from_list(FT_Face face)
{
    FT_Face * pface = _lv_ll_get_head(&face_control.face_ll);
    while(pface) {
        if(*pface == face) {
            _lv_ll_remove(&face_control.face_ll, pface);
            lv_mem_free(pface);
            break;
        }
        pface = _lv_ll_get_next(&face_control.face_ll, pface);
    }
}

static void face_generic_finalizer(void * object)
{
    FT_Face face = (FT_Face)object;
    face_remove_from_list(face);
    if(face->generic.data) {
        lv_face_info_t * face_info = (lv_face_info_t *)face->generic.data;
        lv_mem_free(face_info);
    }
    LV_LOG_INFO("face finalizer(%p)\n", face);
}

static bool get_glyph_dsc_cb_nocache(const lv_font_t * font,
                                     lv_font_glyph_dsc_t * dsc_out, uint32_t unicode_letter, uint32_t unicode_letter_next)
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

    FT_Error error;
    lv_font_fmt_ft_dsc_t * dsc = (lv_font_fmt_ft_dsc_t *)(font->dsc);
    FT_Face face = dsc->size->face;

    FT_UInt glyph_index = FT_Get_Char_Index(face, unicode_letter);

    if(face->size != dsc->size) {
        FT_Activate_Size(dsc->size);
    }

    error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
    if(error) {
        return false;
    }

    error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
    if(error) {
        return false;
    }

    dsc_out->adv_w = (face->glyph->metrics.horiAdvance >> 6);
    dsc_out->box_h = face->glyph->bitmap.rows;         /*Height of the bitmap in [px]*/
    dsc_out->box_w = face->glyph->bitmap.width;         /*Width of the bitmap in [px]*/
    dsc_out->ofs_x = face->glyph->bitmap_left;         /*X offset of the bitmap in [pf]*/
    dsc_out->ofs_y = face->glyph->bitmap_top -
                     face->glyph->bitmap.rows;         /*Y offset of the bitmap measured from the as line*/
    dsc_out->bpp = 8;         /*Bit per pixel: 1/2/4/8*/

    return true;
}

static const uint8_t * get_glyph_bitmap_cb_nocache(const lv_font_t * font, uint32_t unicode_letter)
{
    LV_UNUSED(unicode_letter);
    lv_font_fmt_ft_dsc_t * dsc = (lv_font_fmt_ft_dsc_t *)(font->dsc);
    FT_Face face = dsc->size->face;
    return (const uint8_t *)(face->glyph->bitmap.buffer);
}

static bool lv_ft_font_init_nocache(lv_ft_info_t * info)
{
    lv_font_fmt_ft_dsc_t * dsc = lv_mem_alloc(sizeof(lv_font_fmt_ft_dsc_t));
    if(dsc == NULL) return false;

    dsc->font = lv_mem_alloc(sizeof(lv_font_t));
    if(dsc->font == NULL) {
        lv_mem_free(dsc);
        return false;
    }

    lv_face_info_t * face_info = NULL;
    FT_Face face = face_find_in_list(info);
    if(face == NULL) {
        face_info = lv_mem_alloc(sizeof(lv_face_info_t) + strlen(info->name) + 1);
        if(face_info == NULL) {
            goto Fail;
        }
        FT_Error error = FT_New_Face(library, info->name, 0, &face);
        if(error) {
            lv_mem_free(face_info);
            LV_LOG_WARN("create face error(%d)", error);
            goto Fail;
        }

        /* link face and face info */
        face_info->name = ((char *)face_info) + sizeof(lv_face_info_t);
        strcpy(face_info->name, info->name);
        face->generic.data = face_info;
        face->generic.finalizer = face_generic_finalizer;
        face_add_to_list(face);
    }
    else {
        FT_Size size;
        FT_Error error = FT_New_Size(face, &size);
        if(error) {
            goto Fail;
        }
        FT_Activate_Size(size);
        FT_Reference_Face(face);
    }

    FT_Set_Pixel_Sizes(face, 0, info->weight);
    dsc->size = face->size;
    dsc->height = info->weight;
    dsc->style = info->style;

    lv_font_t * font = dsc->font;
    font->dsc = dsc;
    font->get_glyph_dsc = get_glyph_dsc_cb_nocache;
    font->get_glyph_bitmap = get_glyph_bitmap_cb_nocache;
    font->line_height = (face->size->metrics.height >> 6);
    font->base_line = -(face->size->metrics.descender >> 6);
    font->subpx = LV_FONT_SUBPX_NONE;

    FT_Fixed scale = face->size->metrics.y_scale;
    int8_t thickness = FT_MulFix(scale, face->underline_thickness) >> 6;
    font->underline_position = FT_MulFix(scale, face->underline_position) >> 6;
    font->underline_thickness = thickness < 1 ? 1 : thickness;

    info->font = font;
    return true;

Fail:
    lv_mem_free(dsc->font);
    lv_mem_free(dsc);
    return false;
}

static void lv_ft_font_destroy_nocache(lv_font_t * font)
{
    if(font == NULL) {
        return;
    }

    lv_font_fmt_ft_dsc_t * dsc = (lv_font_fmt_ft_dsc_t *)(font->dsc);
    if(dsc) {
        FT_Face face = dsc->size->face;
        FT_Done_Size(dsc->size);
        FT_Done_Face(face);
        lv_mem_free(dsc->font);
        lv_mem_free(dsc);
    }
}

#endif/* LV_FREETYPE_CACHE_SIZE */

#endif /*LV_USE_FREETYPE*/
