/**
 * @file lv_img_decoder.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_img_decoder.h"
#include "lv_ll.h"

#if defined(LV_GC_INCLUDE)
#include LV_GC_INCLUDE
#endif /* LV_ENABLE_GC */

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
/**
 * Initialize the image decoder module
 * */
void lv_img_decoder_init(void)
{
    lv_ll_init(&LV_GC_ROOT(_lv_img_defoder_ll), sizeof(lv_img_decoder_t));

    lv_img_decoder_t decoder;

    /*Create a decoder for true color images*/
    decoder = lv_img_decoder_craete();
    if(decoder) return;

    /*Create a decoder for Alpha indexed images*/
    decoder = lv_img_decoder_craete();
    if(decoder) return;

    /*Create a decoder for color indexed images*/
    decoder = lv_img_decoder_craete();
    if(decoder) return;

}

lv_img_decoder_t * lv_img_decoder_craete(void)
{
    lv_img_decoder_t * decoder;
    decoder = lv_ll_ins_head(&LV_GC_ROOT(_lv_img_defoder_ll));
    lv_mem_assert(decoder);

    return decoder;
}

void lv_img_decoder_delete(lv_img_decoder_t * decoder)
{
    lv_ll_rem(&LV_GC_ROOT(_lv_img_defoder_ll), decoder);
    lv_mem_free(decoder);
}

void lv_img_decoder_set_info_cb(lv_img_decoder_t * decoder, lv_img_decoder_info_f_t info_cb)
{
    decoder->info_cb = info_cb;
}

void lv_img_decoder_set_open_cb(lv_img_decoder_t * decoder, lv_img_decoder_open_f_t open_cb)
{
    decoder->open_cb = open_cb;
}

void lv_img_decoder_set_read_line_cb(lv_img_decoder_t * decoder, lv_img_decoder_read_line_f_t read_line_cb)
{
    decoder->read_line_cb = read_line_cb;
}

void lv_img_decoder_set_close_cb(lv_img_decoder_t * decoder, lv_img_decoder_close_f_t close_cb)
{
    decoder->read_line_cb = close_cb;
}

void lv_img_decoder_set_user_data(lv_img_decoder_t * decoder, lv_img_decoder_t user_data)
{
    memcpy(&decoder->user_data, &user_data, sizeof(user_data));
}

lv_img_decoder_t lv_img_decoder_get_user_data(lv_img_decoder_t * decoder)
{
    return decoder->user_data;
}

lv_img_decoder_t * lv_img_decoder_get_user_data_ptr(lv_img_decoder_t * decoder)
{
    return &decoder->user_data;
}


lv_res_t lv_img_decoder_get_info(const char * src, lv_img_header_t * header)
{
    header->always_zero = 0;

    lv_res_t res;
    lv_img_decoder_t * d;
    LV_LL_READ(LV_GC_ROOT(_lv_img_defoder_ll), d) {
        res = LV_RES_INV;
        if(d->info_cb) {
            res = d->info_cb(d, src, header);
            if(res == LV_RES_OK) break;
        }
    }

    return res;
}


const uint8_t * lv_img_decoder_open(lv_img_decoder_dsc_t * dsc, const void * src, const lv_style_t * style)
{
    dsc->style = style;
    dsc->src = src;
    dsc->src_type = lv_img_src_get_type(src);

    lv_res_t header_res;
    header_res = lv_img_decoder_get_info(src, &dsc->header);
    if(header_res != LV_RES_OK) return LV_IMG_DECODER_OPEN_FAIL;

    const uint8_t * res = NULL;
    lv_img_decoder_t * d;
    LV_LL_READ(LV_GC_ROOT(_lv_img_defoder_ll), d) {
        res = NULL;
        dsc->decoder = d;
        if(d->open_cb) res = d->open_cb(d, dsc);

        if(res != LV_IMG_DECODER_OPEN_FAIL) break;
    }
}

static lv_res_t lv_img_decoder_read_line(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc, lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf)
{
    lv_res_t res = LV_RES_INV;
    if(decoder->read_line_cb)res = decoder->read_line_cb(decoder, dsc, x, y, len, buf);

    return res;
}

static void lv_img_decoder_close(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc)
{
    if(decoder->close_cb) decoder->close_cb(decoder, dsc);
}


static lv_res_t img_decoder_built_in_info(lv_img_decoder_t * decoder, const void * src, lv_img_header_t * header)
{
    lv_img_src_t src_type = lv_img_src_get_type(src);
        if(src_type == LV_IMG_SRC_VARIABLE) {
            header->w  = ((lv_img_dsc_t *)src)->header.w;
            header->h  = ((lv_img_dsc_t *)src)->header.h;
            header->cf = ((lv_img_dsc_t *)src)->header.cf;
        }
    #if LV_USE_FILESYSTEM
        else if(src_type == LV_IMG_SRC_FILE) {
            lv_fs_file_t file;
            lv_fs_res_t res;
            uint32_t rn;
            res = lv_fs_open(&file, src, LV_FS_MODE_RD);
            if(res == LV_FS_RES_OK) {
                res = lv_fs_read(&file, header, sizeof(lv_img_header_t), &rn);
                lv_fs_close(&file);
            }

            /*Create a dummy header on fs error*/
            if(res != LV_FS_RES_OK || rn != sizeof(lv_img_header_t)) {
                header->w  = LV_DPI;
                header->h  = LV_DPI;
                header->cf = LV_IMG_CF_UNKNOWN;
                return LV_RES_INV;
            }
        }
    #endif
        else if(src_type == LV_IMG_SRC_SYMBOL) {
            /*The size depend on the font but it is unknown here. It should be handled outside of the
             * function*/
            header->w = 1;
            header->h = 1;
            /* Symbols always have transparent parts. Important because of cover check in the design
             * function. The actual value doesn't matter because lv_draw_label will draw it*/
            header->cf = LV_IMG_CF_ALPHA_1BIT;
        } else {
            LV_LOG_WARN("Image get info found unknown src type");
            return LV_RES_INV;
        }
        return LV_RES_OK;
}

static uint8_t * img_decoder_built_in_open(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc,)
{
    if(header_res == LV_RES_INV) {
        decoder_src      = NULL;
        decoder_src_type = LV_IMG_SRC_UNKNOWN;
        LV_LOG_WARN("Built-in image decoder can't get the header info");
        return LV_IMG_DECODER_OPEN_FAIL;
    }

    /*Open the file if it's a file*/
    if(decoder_src_type == LV_IMG_SRC_FILE) {
#if LV_USE_FILESYSTEM
        lv_fs_res_t res = lv_fs_open(&decoder_file, src, LV_FS_MODE_RD);
        if(res != LV_FS_RES_OK) {
            LV_LOG_WARN("Built-in image decoder can't open the file");
            return LV_IMG_DECODER_OPEN_FAIL;
        }
#else
        LV_LOG_WARN("Image built-in decoder can read file because LV_USE_FILESYSTEM = 0");
        return LV_IMG_DECODER_OPEN_FAIL;
#endif
    }

    /*Process the different color formats*/
    lv_img_cf_t cf = decoder_header.cf;
    if(cf == LV_IMG_CF_TRUE_COLOR || cf == LV_IMG_CF_TRUE_COLOR_ALPHA ||
       cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED) {
        if(decoder_src_type == LV_IMG_SRC_VARIABLE) {
            /*In case of uncompressed formats if the image stored in the ROM/RAM simply give it's
             * pointer*/
            return ((lv_img_dsc_t *)decoder_src)->data;
        } else {
            /*If it's file it need to be read line by line later*/
            return NULL;
        }
    } else if(cf == LV_IMG_CF_INDEXED_1BIT || cf == LV_IMG_CF_INDEXED_2BIT ||
              cf == LV_IMG_CF_INDEXED_4BIT || cf == LV_IMG_CF_INDEXED_8BIT) {

#if LV_IMG_CF_INDEXED
#if LV_USE_FILESYSTEM
        lv_color32_t palette_file[256];
#endif

        lv_color32_t * palette_p = NULL;
        uint8_t px_size          = lv_img_color_format_get_px_size(cf);
        uint32_t palette_size    = 1 << px_size;

        if(decoder_src_type == LV_IMG_SRC_FILE) {
            /*Read the palette from file*/
#if LV_USE_FILESYSTEM
            lv_fs_seek(&decoder_file, 4); /*Skip the header*/
            lv_fs_read(&decoder_file, palette_file, palette_size * sizeof(lv_color32_t), NULL);
            palette_p = palette_file;
#else
            LV_LOG_WARN(
                "Image built-in decoder can read the palette because LV_USE_FILESYSTEM = 0");
            return LV_IMG_DECODER_OPEN_FAIL;
#endif
        } else {
            /*The palette begins in the beginning of the image data. Just point to it.*/
            palette_p = (lv_color32_t *)((lv_img_dsc_t *)decoder_src)->data;
        }

        uint32_t i;
        for(i = 0; i < palette_size; i++) {
            decoder_index_map[i] =
                lv_color_make(palette_p[i].ch.red, palette_p[i].ch.green, palette_p[i].ch.blue);
        }
        return NULL;
#else
        LV_LOG_WARN("Indexed (palette) images are not enabled in lv_conf.h. See LV_IMG_CF_INDEXED");
        return LV_IMG_DECODER_OPEN_FAIL;
#endif
    } else if(cf == LV_IMG_CF_ALPHA_1BIT || cf == LV_IMG_CF_ALPHA_2BIT ||
              cf == LV_IMG_CF_ALPHA_4BIT || cf == LV_IMG_CF_ALPHA_8BIT) {
#if LV_IMG_CF_ALPHA
        return NULL; /*Nothing to process*/
#else
        LV_LOG_WARN("Alpha indexed images are not enabled in lv_conf.h. See LV_IMG_CF_ALPHA");
        return LV_IMG_DECODER_OPEN_FAIL;
#endif
    } else {
        LV_LOG_WARN("Image decoder open: unknown color format")
        return LV_IMG_DECODER_OPEN_FAIL;
    }
}


static lv_res_t lv_img_decoder_built_in_line_read(lv_img_decoder_t * decoder, lv_coord_t x, lv_coord_t y, lv_coord_t len,
                                                   uint8_t * buf)
{
    if(dsc->src_type == LV_IMG_SRC_FILE) {
  #if LV_USE_FILESYSTEM
          uint8_t px_size = lv_img_color_format_get_px_size(dsc->header.cf);

          lv_fs_res_t res;

          if(dsc->header.cf == LV_IMG_CF_TRUE_COLOR ||
             dsc->header.cf == LV_IMG_CF_TRUE_COLOR_ALPHA ||
             dsc->header.cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED)
          {
              uint32_t pos = ((y * dsc->header.w + x) * px_size) >> 3;
              pos += 4; /*Skip the header*/
              res = lv_fs_seek(&decoder_file, pos);
              if(res != LV_FS_RES_OK) {
                  LV_LOG_WARN("Built-in image decoder seek failed");
                  return false;
              }
              uint32_t btr = len * (px_size >> 3);
              uint32_t br  = 0;
              lv_fs_read(&decoder_file, buf, btr, &br);
              if(res != LV_FS_RES_OK || btr != br) {
                  LV_LOG_WARN("Built-in image decoder read failed");
                  return false;
              }
          } else if(decoder_header.cf == LV_IMG_CF_ALPHA_1BIT ||
                    decoder_header.cf == LV_IMG_CF_ALPHA_2BIT ||
                    decoder_header.cf == LV_IMG_CF_ALPHA_4BIT ||
                    decoder_header.cf == LV_IMG_CF_ALPHA_8BIT) {

              lv_img_built_in_decoder_line_alpha(x, y, len, buf);
          } else if(decoder_header.cf == LV_IMG_CF_INDEXED_1BIT ||
                    decoder_header.cf == LV_IMG_CF_INDEXED_2BIT ||
                    decoder_header.cf == LV_IMG_CF_INDEXED_4BIT ||
                    decoder_header.cf == LV_IMG_CF_INDEXED_8BIT) {
              lv_img_built_in_decoder_line_indexed(x, y, len, buf);
          } else {
              LV_LOG_WARN("Built-in image decoder read not supports the color format");
              return false;
          }
  #else
          LV_LOG_WARN("Image built-in decoder can't read file because LV_USE_FILESYSTEM = 0");
          return false;
  #endif
      } else if(decoder_src_type == LV_IMG_SRC_VARIABLE) {
          const lv_img_dsc_t * img_dsc = decoder_src;

          if(img_dsc->header.cf == LV_IMG_CF_ALPHA_1BIT ||
             img_dsc->header.cf == LV_IMG_CF_ALPHA_2BIT ||
             img_dsc->header.cf == LV_IMG_CF_ALPHA_4BIT ||
             img_dsc->header.cf == LV_IMG_CF_ALPHA_8BIT) {
              lv_img_built_in_decoder_line_alpha(x, y, len, buf);
          } else if(img_dsc->header.cf == LV_IMG_CF_INDEXED_1BIT ||
                    img_dsc->header.cf == LV_IMG_CF_INDEXED_2BIT ||
                    img_dsc->header.cf == LV_IMG_CF_INDEXED_4BIT ||
                    img_dsc->header.cf == LV_IMG_CF_INDEXED_8BIT) {
              lv_img_built_in_decoder_line_indexed(x, y, len, buf);
          } else {
              LV_LOG_WARN("Built-in image decoder not supports the color format");
              return false;
          }
      }

      return true;
}

static void lv_img_decoder_built_in_close(lv_img_decoder_t * decoder, lv_img_decoder_dsc_t * dsc)
{

    /*It was opened with built-in decoder*/
    if(decoder_src) {
#if LV_USE_FILESYSTEM
        if(decoder_src_type == LV_IMG_SRC_FILE) {
            lv_fs_close(&decoder_file);
        }
#endif
        decoder_src_type = LV_IMG_SRC_UNKNOWN;
        decoder_src      = NULL;
    }
}


static lv_res_t lv_img_built_in_decoder_line_alpha(lv_img_decoder_t * decoder, lv_coord_t x, lv_coord_t y, lv_coord_t len,
                                                   uint8_t * buf)
{

#if LV_IMG_CF_ALPHA
    const lv_opa_t alpha1_opa_table[2] = {
        0, 255}; /*Opacity mapping with bpp = 1 (Just for compatibility)*/
    const lv_opa_t alpha2_opa_table[4]  = {0, 85, 170, 255};  /*Opacity mapping with bpp = 2*/
    const lv_opa_t alpha4_opa_table[16] = {0,   17,  34,  51, /*Opacity mapping with bpp = 4*/
                                           68,  85,  102, 119, 136, 153,
                                           170, 187, 204, 221, 238, 255};

    /*Simply fill the buffer with the color. Later only the alpha value will be modified.*/
    lv_color_t bg_color = decoder_style->image.color;
    lv_coord_t i;
    for(i = 0; i < len; i++) {
#if LV_COLOR_DEPTH == 8 || LV_COLOR_DEPTH == 1
        buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE] = bg_color.full;
#elif LV_COLOR_DEPTH == 16
        /*Because of Alpha byte 16 bit color can start on odd address which can cause crash*/
        buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE] = bg_color.full & 0xFF;
        buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE + 1] = (bg_color.full >> 8) & 0xFF;
#elif LV_COLOR_DEPTH == 32
        *((uint32_t *)&buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE]) = bg_color.full;
#else
#error "Invalid LV_COLOR_DEPTH. Check it in lv_conf.h"
#endif
    }

    const lv_opa_t * opa_table = NULL;
    uint8_t px_size            = lv_img_color_format_get_px_size(decoder_header.cf);
    uint16_t mask              = (1 << px_size) - 1; /*E.g. px_size = 2; mask = 0x03*/

    lv_coord_t w = 0;
    uint32_t ofs = 0;
    int8_t pos   = 0;
    switch(decoder_header.cf) {
        case LV_IMG_CF_ALPHA_1BIT:
            w = (decoder_header.w >> 3); /*E.g. w = 20 -> w = 2 + 1*/
            if(decoder_header.w & 0x7) w++;
            ofs += w * y + (x >> 3); /*First pixel*/
            pos       = 7 - (x & 0x7);
            opa_table = alpha1_opa_table;
            break;
        case LV_IMG_CF_ALPHA_2BIT:
            w = (decoder_header.w >> 2); /*E.g. w = 13 -> w = 3 + 1 (bytes)*/
            if(decoder_header.w & 0x3) w++;
            ofs += w * y + (x >> 2); /*First pixel*/
            pos       = 6 - ((x & 0x3) * 2);
            opa_table = alpha2_opa_table;
            break;
        case LV_IMG_CF_ALPHA_4BIT:
            w = (decoder_header.w >> 1); /*E.g. w = 13 -> w = 6 + 1 (bytes)*/
            if(decoder_header.w & 0x1) w++;
            ofs += w * y + (x >> 1); /*First pixel*/
            pos       = 4 - ((x & 0x1) * 4);
            opa_table = alpha4_opa_table;
            break;
        case LV_IMG_CF_ALPHA_8BIT:
            w = decoder_header.w; /*E.g. x = 7 -> w = 7 (bytes)*/
            ofs += w * y + x;     /*First pixel*/
            pos = 0;
            break;
    }

#if LV_USE_FILESYSTEM
#if LV_COMPILER_VLA_SUPPORTED
    uint8_t fs_buf[w];
#else
    uint8_t fs_buf[LV_HOR_RES_MAX];
#endif
#endif
    const uint8_t * data_tmp = NULL;
    if(decoder_src_type == LV_IMG_SRC_VARIABLE) {
        const lv_img_dsc_t * img_dsc = decoder_src;
        data_tmp                     = img_dsc->data + ofs;
    } else {
#if LV_USE_FILESYSTEM
        lv_fs_seek(&decoder_file, ofs + 4); /*+4 to skip the header*/
        lv_fs_read(&decoder_file, fs_buf, w, NULL);
        data_tmp = fs_buf;
#else
        LV_LOG_WARN(
            "Image built-in alpha line reader can't read file because LV_USE_FILESYSTEM = 0");
        data_tmp = NULL; /*To avoid warnings*/
        return LV_RES_INV;
#endif
    }

    uint8_t byte_act = 0;
    uint8_t val_act;
    for(i = 0; i < len; i++) {
        val_act = (data_tmp[byte_act] & (mask << pos)) >> pos;

        buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE + LV_IMG_PX_SIZE_ALPHA_BYTE - 1] =
            decoder_header.cf == LV_IMG_CF_ALPHA_8BIT ? val_act : opa_table[val_act];

        pos -= px_size;
        if(pos < 0) {
            pos = 8 - px_size;
            data_tmp++;
        }
    }

    return LV_RES_OK;

#else
    LV_LOG_WARN(
        "Image built-in alpha line reader failed because LV_IMG_CF_ALPHA is 0 in lv_conf.h");
    return LV_RES_INV;
#endif
}

static lv_res_t lv_img_built_in_decoder_line_indexed(lv_img_decoder_t * decoder, lv_coord_t x, lv_coord_t y, lv_coord_t len,
                                                     uint8_t * buf)
{

#if LV_IMG_CF_INDEXED
    uint8_t px_size = lv_img_color_format_get_px_size(decoder_header.cf);
    uint16_t mask   = (1 << px_size) - 1; /*E.g. px_size = 2; mask = 0x03*/

    lv_coord_t w = 0;
    int8_t pos   = 0;
    uint32_t ofs = 0;
    switch(decoder_header.cf) {
        case LV_IMG_CF_INDEXED_1BIT:
            w = (decoder_header.w >> 3); /*E.g. w = 20 -> w = 2 + 1*/
            if(decoder_header.w & 0x7) w++;
            ofs += w * y + (x >> 3); /*First pixel*/
            ofs += 8;                /*Skip the palette*/
            pos = 7 - (x & 0x7);
            break;
        case LV_IMG_CF_INDEXED_2BIT:
            w = (decoder_header.w >> 2); /*E.g. w = 13 -> w = 3 + 1 (bytes)*/
            if(decoder_header.w & 0x3) w++;
            ofs += w * y + (x >> 2); /*First pixel*/
            ofs += 16;               /*Skip the palette*/
            pos = 6 - ((x & 0x3) * 2);
            break;
        case LV_IMG_CF_INDEXED_4BIT:
            w = (decoder_header.w >> 1); /*E.g. w = 13 -> w = 6 + 1 (bytes)*/
            if(decoder_header.w & 0x1) w++;
            ofs += w * y + (x >> 1); /*First pixel*/
            ofs += 64;               /*Skip the palette*/
            pos = 4 - ((x & 0x1) * 4);
            break;
        case LV_IMG_CF_INDEXED_8BIT:
            w = decoder_header.w; /*E.g. x = 7 -> w = 7 (bytes)*/
            ofs += w * y + x;     /*First pixel*/
            ofs += 1024;          /*Skip the palette*/
            pos = 0;
            break;
    }

#if LV_USE_FILESYSTEM
#if LV_COMPILER_VLA_SUPPORTED
    uint8_t fs_buf[w];
#else
    uint8_t fs_buf[LV_HOR_RES_MAX];
#endif
#endif
    const uint8_t * data_tmp = NULL;
    if(decoder_src_type == LV_IMG_SRC_VARIABLE) {
        const lv_img_dsc_t * img_dsc = decoder_src;
        data_tmp                     = img_dsc->data + ofs;
    } else {
#if LV_USE_FILESYSTEM
        lv_fs_seek(&decoder_file, ofs + 4); /*+4 to skip the header*/
        lv_fs_read(&decoder_file, fs_buf, w, NULL);
        data_tmp = fs_buf;
#else
        LV_LOG_WARN(
            "Image built-in indexed line reader can't read file because LV_USE_FILESYSTEM = 0");
        data_tmp = NULL; /*To avoid warnings*/
        return LV_RES_INV;
#endif
    }

    uint8_t byte_act = 0;
    uint8_t val_act;
    lv_coord_t i;
    lv_color_t * cbuf = (lv_color_t *)buf;
    for(i = 0; i < len; i++) {
        val_act = (data_tmp[byte_act] & (mask << pos)) >> pos;
        cbuf[i] = decoder_index_map[val_act];

        pos -= px_size;
        if(pos < 0) {
            pos = 8 - px_size;
            data_tmp++;
        }
    }

    return LV_RES_OK;
#else
    LV_LOG_WARN(
        "Image built-in indexed line reader failed because LV_IMG_CF_INDEXED is 0 in lv_conf.h");
    return LV_RES_INV;
#endif
}




/**********************
 *   STATIC FUNCTIONS
 **********************/
