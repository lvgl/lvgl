#ifndef LV_TTF_H
#define LV_TTF_H
#ifdef __cplusplus
extern "C" {
#endif
#include "../../../lvgl.h"
// private structure
typedef struct lv_ttf_buf {
    lv_fs_file_t *stream;
    size_t offset;
    int cursor;
    int size;
} lv_ttf_buf_t;

typedef struct lv_ttf {
    void *userdata;
    //unsigned char *data; // pointer to .ttf file
    lv_fs_file_t *stream; // stream of .ttf file
    int fontstart;      // offset of start of font

    int numGlyphs; // number of glyphs, needed for range checking

    int loca, head, glyf, hhea, hmtx, kern, gpos, svg; // table locations as offset from start of .ttf
    int index_map;                                     // a cmap mapping for our chosen character encoding
    int indexToLocFormat;                              // format needed to map from glyph index to glyph

    lv_ttf_buf_t cff;         // cff font data
    lv_ttf_buf_t charstrings; // the charstring index
    lv_ttf_buf_t gsubrs;      // global charstring subroutines index
    lv_ttf_buf_t subrs;       // private charstring subroutines index
    lv_ttf_buf_t fontdicts;   // array of font dicts
    lv_ttf_buf_t fdselect;    // map from glyph to fontdict
} lv_ttf_t;
struct stbtt_fontinfo;
enum {
    LV_TTF_RES_OK = 0,
    LV_TTF_RES_ARG = 1, /* Indicates an invalid argument*/
    LV_TTF_RES_FMT = 2, /*Typically indicates that the font format is invalid or unsupported*/
    LV_TTF_RES_MEM = 3, /* Out of memory*/
};
typedef uint8_t lv_ttf_res_t;
lv_ttf_res_t lv_ttf_open(lv_ttf_t* out_ttf, lv_fs_file_t* file);
lv_ttf_res_t lv_ttf_create_font(lv_font_t* out_font,const lv_ttf_t* ttf, lv_coord_t line_height, const lv_font_t* fallback);
void lv_ttf_destroy_font(lv_font_t* font);
#ifdef __cplusplus
}
#endif 
#endif // LV_TTF_H