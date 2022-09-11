// honey the codewitch (github codewitch-honey-crisis)
#ifndef LV_TINY_TTF_H
#define LV_TINY_TTF_H
#include "../../../lvgl.h"
// private structure
typedef struct lv_tiny_ttf_buf {
    lv_fs_file_t * stream;
    size_t offset;
    int cursor;
    int size;
} lv_tiny_ttf_buf_t;

typedef struct lv_tiny_ttf_font {
    void * userdata;
    //unsigned char *data; // pointer to .ttf file
    lv_fs_file_t stream; // stream of .ttf file
    int fontstart;      // offset of start of font

    int numGlyphs; // number of glyphs, needed for range checking

    int loca, head, glyf, hhea, hmtx, kern, gpos, svg; // table locations as offset from start of .ttf
    int index_map;                                     // a cmap mapping for our chosen character encoding
    int indexToLocFormat;                              // format needed to map from glyph index to glyph

    lv_tiny_ttf_buf_t cff;         // cff font data
    lv_tiny_ttf_buf_t charstrings; // the charstring index
    lv_tiny_ttf_buf_t gsubrs;      // global charstring subroutines index
    lv_tiny_ttf_buf_t subrs;       // private charstring subroutines index
    lv_tiny_ttf_buf_t fontdicts;   // array of font dicts
    lv_tiny_ttf_buf_t fdselect;    // map from glyph to fontdict

    int ascent, descent;
    float scale;
} lv_tiny_ttf_font_t;
struct stbtt_fontinfo;

#ifdef __cplusplus
extern "C" {
#endif
// create a font from the specified file or path with the specified line height, and fallback font
lv_font_t * lv_tiny_ttf_create(const char * path, lv_coord_t line_height, lv_font_t * fallback);
// destroy a font previously created with lv_tiny_ttf_create()
void lv_tiny_ttf_destroy(lv_font_t * font);
#ifdef __cplusplus
}
#endif
#endif // LV_TINY_TTF_H