#include "gifdec.h"
#include "../../../misc/lv_log.h"
#if LV_USE_GIF

#include "../../../misc/lv_mem.h"
#include "../../../misc/lv_color.h"
#include "../../../draw/lv_img_decoder.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



static int gif_init = 0;

/**********************
 *      MACROS
 **********************/
#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))
#define GIF_IMG_BEGIN	','
#define GIF_EXT_BEGIN	'!'
#define GIF_END			';'

/**********************
 *      TYPEDEFS
 **********************/
typedef struct gd_Palette {
    int size;
    uint8_t colors[0x100 * 3];
} gd_Palette;

typedef struct gd_GCE {
    uint16_t delay;
    uint8_t tindex;
    uint8_t disposal;
    int input;
    int transparency;
} gd_GCE;



typedef struct gd_GIF {
    lv_fs_file_t fd;
    const char * data;
    uint8_t is_file;
    uint32_t f_rw_p;
    int32_t anim_start;
    uint16_t width, height;
    uint16_t depth;
    uint16_t loop_count;
    gd_GCE gce;
    gd_Palette *palette;
    gd_Palette lct, gct;
    void (*plain_text)(
        struct gd_GIF *gif, uint16_t tx, uint16_t ty,
        uint16_t tw, uint16_t th, uint8_t cw, uint8_t ch,
        uint8_t fg, uint8_t bg
    );
    void (*comment)(struct gd_GIF *gif);
    void (*application)(struct gd_GIF *gif, char id[8], char auth[3]);
    uint16_t fx, fy, fw, fh;
    uint8_t bgindex;
    uint8_t *canvas, *frame;
} gd_GIF;

typedef struct Entry {
    uint16_t length;
    uint16_t prefix;
    uint8_t  suffix;
} Entry;

typedef struct Table {
    int bulk;
    int nentries;
    Entry *entries;
} Table;


/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t decoder_accept(const lv_img_src_t * src, uint8_t * caps);
static lv_res_t decoder_open(lv_img_decoder_dsc_t * dsc, const lv_img_dec_flags_t flags);


static lv_res_t decoder_read_line(lv_img_decoder_dsc_t * dsc,
                                  lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf);

static void decoder_close(lv_img_decoder_dsc_t * dsc);

static gd_GIF * gd_open_gif_file(const char *fname, const bool skip_alloc);

static gd_GIF * gd_open_gif_data(const void *data, const bool skip_alloc);

static void gd_render_frame(gd_GIF *gif, uint8_t *buffer);

static int gd_get_frame(gd_GIF *gif);
static void gd_rewind(gd_GIF *gif);
static void gd_close_gif(gd_GIF *gif);
static gd_GIF *  gif_open(gd_GIF * gif, const bool skip_alloc);
static bool f_gif_open(gd_GIF * gif, const void * path, bool is_file);
static void f_gif_read(gd_GIF * gif, void * buf, size_t len);
static int f_gif_seek(gd_GIF * gif, size_t pos, int k);
static void f_gif_close(gd_GIF * gif);

static uint16_t read_num(gd_GIF * gif);
static size_t count_frames(gd_GIF *gif);
static void discard_sub_blocks(gd_GIF *gif);
static void read_plain_text_ext(gd_GIF *gif);
static void read_graphic_control_ext(gd_GIF *gif);
static void read_comment_ext(gd_GIF *gif);
static void read_application_ext(gd_GIF *gif);
static void read_ext(gd_GIF *gif);

static Table * new_table(int key_size);
static int add_entry(Table **tablep, uint16_t length, uint16_t prefix, uint8_t suffix);
static uint16_t get_key(gd_GIF *gif, int key_size, uint8_t *sub_len, uint8_t *shift, uint8_t *byte);
static int interlaced_line_index(int h, int y);
static int read_image_data(gd_GIF *gif, int interlace);
static int read_image(gd_GIF *gif);
static void render_frame_rect(gd_GIF *gif, uint8_t *buffer);
static void dispose(gd_GIF *gif);
static void set_caps(uint8_t * caps);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_gif_init(void)
{
    if(gif_init)
        return;

    lv_img_decoder_t * dec = lv_img_decoder_create();
    lv_img_decoder_set_accept_cb(dec, decoder_accept);
    lv_img_decoder_set_open_cb(dec, decoder_open);
    lv_img_decoder_set_read_line_cb(dec, decoder_read_line);
    lv_img_decoder_set_close_cb(dec, decoder_close);
    gif_init = 1;
}

static uint16_t read_num(gd_GIF * gif)
{
    uint8_t bytes[2];

    f_gif_read(gif, bytes, 2);
    return bytes[0] + (((uint16_t) bytes[1]) << 8);
}

static gd_GIF * gd_open_gif_file(const char *fname, const bool skip_alloc)
{
    gd_GIF gif_base;
    memset(&gif_base, 0, sizeof(gif_base));

    bool res = f_gif_open(&gif_base, fname, true);
    if(!res) return NULL;

    return gif_open(&gif_base, skip_alloc);
}

static gd_GIF * gd_open_gif_data(const void *data, const bool skip_alloc)
{
    gd_GIF gif_base = {0};

    bool res = f_gif_open(&gif_base, data, false);
    if(!res) return NULL;

    return gif_open(&gif_base, skip_alloc);
}

static gd_GIF * gif_open(gd_GIF * gif_base, bool skip_alloc)
{
    uint8_t sigver[3];
    uint16_t width, height, depth;
    uint8_t fdsz, bgidx, aspect;
    int i;
    uint8_t *bgcolor;
    int gct_sz;
    gd_GIF *gif;

    /* Header */
    f_gif_read(gif_base, sigver, 3);
    if (memcmp(sigver, "GIF", 3) != 0) {
        LV_LOG_WARN("invalid signature\n");
        goto fail;
    }
    /* Version */
    f_gif_read(gif_base, sigver, 3);
    if (memcmp(sigver, "89a", 3) != 0) {
        LV_LOG_WARN("invalid version\n");
        goto fail;
    }
    /* Width x Height */
    width  = read_num(gif_base);
    height = read_num(gif_base);
    /* FDSZ */
    f_gif_read(gif_base, &fdsz, 1);
    /* Presence of GCT */
    if (!(fdsz & 0x80)) {
        LV_LOG_WARN("no global color table\n");
        goto fail;
    }
    /* Color Space's Depth */
    depth = ((fdsz >> 4) & 7) + 1;
    /* Ignore Sort Flag. */
    /* GCT Size */
    gct_sz = 1 << ((fdsz & 0x07) + 1);
    /* Background Color Index */
    f_gif_read(gif_base, &bgidx, 1);
    /* Aspect Ratio */
    f_gif_read(gif_base, &aspect, 1);
    /* Create gd_GIF Structure. */
    if(skip_alloc) {
        gif = lv_mem_alloc(sizeof(gd_GIF));
    } else {
#if LV_COLOR_DEPTH == 32
        gif = lv_mem_alloc(sizeof(gd_GIF) + 5 * width * height);
#elif LV_COLOR_DEPTH == 16
        gif = lv_mem_alloc(sizeof(gd_GIF) + 4 * width * height);
#elif LV_COLOR_DEPTH == 8 || LV_COLOR_DEPTH == 1
        gif = lv_mem_alloc(sizeof(gd_GIF) + 3 * width * height);
#endif
    }

    if (!gif) goto fail;
    memcpy(gif, gif_base, sizeof(gd_GIF));
    gif->width  = width;
    gif->height = height;
    gif->depth  = depth;
    /* Read GCT */
    gif->gct.size = gct_sz;
    if(skip_alloc)
        f_gif_seek(gif, 3 * gif->gct.size, LV_FS_SEEK_CUR);
    else
        f_gif_read(gif, gif->gct.colors, 3 * gif->gct.size);
    gif->palette = &gif->gct;
    gif->bgindex = bgidx;

    if(!skip_alloc) {
        gif->canvas = (uint8_t *) &gif[1];
        gif->frame = &gif->canvas[(LV_COLOR_SIZE / 8 + 1) * width * height];

        if(gif->bgindex && !skip_alloc)
            memset(gif->frame, gif->bgindex, gif->width * gif->height);
        bgcolor = &gif->palette->colors[gif->bgindex*3];

        if (bgcolor[0] || bgcolor[1] || bgcolor [2])
            for (i = 0; i < gif->width * gif->height; i++) {
    #if LV_COLOR_DEPTH == 32
                gif->canvas[i*4 + 0] = *(bgcolor + 2);
                gif->canvas[i*4 + 1] = *(bgcolor + 1);
                gif->canvas[i*4 + 2] = *(bgcolor + 0);
                gif->canvas[i*4 + 3] = 0xff;
    #elif LV_COLOR_DEPTH == 16
                lv_color_t c = lv_color_make(*(bgcolor + 0), *(bgcolor + 1), *(bgcolor + 2));
                gif->canvas[i*3 + 0] = c.full & 0xff;
                gif->canvas[i*3 + 1] = (c.full >> 8) & 0xff;
                gif->canvas[i*3 + 2] = 0xff;
    #elif LV_COLOR_DEPTH == 8
                lv_color_t c = lv_color_make(*(bgcolor + 0), *(bgcolor + 1), *(bgcolor + 2));
                gif->canvas[i*2 + 0] = c.full;
                gif->canvas[i*2 + 1] = 0xff;
    #endif
            }
    }
    gif->anim_start = f_gif_seek(gif, 0, LV_FS_SEEK_CUR);
    goto ok;
fail:
    f_gif_close(gif_base);
ok:
    return gif;
}

/* Must be called just after a seek to anim_start */
static size_t count_frames(gd_GIF *gif) {
    size_t img = 0;
    while(1) {
        uint8_t ch = 0;
        f_gif_read(gif, &ch, 1);
        switch(ch) {
            case GIF_IMG_BEGIN: {
                uint8_t fisrz;
                /* Image Descriptor. */
                f_gif_seek(gif, 8, LV_FS_SEEK_CUR);
                f_gif_read(gif, &fisrz, 1);

                /* Local Color Table? */
                if (fisrz & 0x80) {
                    f_gif_seek(gif, 3 * (1 << ((fisrz & 0x07) + 1)), LV_FS_SEEK_CUR);
                }
                f_gif_seek(gif, 1, LV_FS_SEEK_CUR);
                discard_sub_blocks(gif);
                img++;
                break;
            }
            case GIF_EXT_BEGIN:
                read_ext(gif);
                break;
            case GIF_END:
                f_gif_seek(gif, gif->anim_start, LV_FS_SEEK_SET);
                return img;
            default:
                f_gif_seek(gif, gif->anim_start, LV_FS_SEEK_SET);
                return 0; /* Error*/
        }
    }
    f_gif_seek(gif, gif->anim_start, LV_FS_SEEK_SET);
    return 0;
}

static void discard_sub_blocks(gd_GIF *gif)
{
    uint8_t size;

    do {
        f_gif_read(gif, &size, 1);
        f_gif_seek(gif, size, LV_FS_SEEK_CUR);
    } while (size);
}

static void read_plain_text_ext(gd_GIF *gif)
{
    if (gif->plain_text) {
        uint16_t tx, ty, tw, th;
        uint8_t cw, ch, fg, bg;
        size_t sub_block;
        f_gif_seek(gif, 1, LV_FS_SEEK_CUR); /* block size = 12 */
        tx = read_num(gif);
        ty = read_num(gif);
        tw = read_num(gif);
        th = read_num(gif);
        f_gif_read(gif, &cw, 1);
        f_gif_read(gif, &ch, 1);
        f_gif_read(gif, &fg, 1);
        f_gif_read(gif, &bg, 1);
        sub_block = f_gif_seek(gif, 0, LV_FS_SEEK_CUR);
        gif->plain_text(gif, tx, ty, tw, th, cw, ch, fg, bg);
        f_gif_seek(gif, sub_block, LV_FS_SEEK_SET);
    } else {
        /* Discard plain text metadata. */
        f_gif_seek(gif, 13, LV_FS_SEEK_CUR);
    }
    /* Discard plain text sub-blocks. */
    discard_sub_blocks(gif);
}

static void read_graphic_control_ext(gd_GIF *gif)
{
    uint8_t rdit;

    /* Discard block size (always 0x04). */
    f_gif_seek(gif, 1, LV_FS_SEEK_CUR);
    f_gif_read(gif, &rdit, 1);
    gif->gce.disposal = (rdit >> 2) & 3;
    gif->gce.input = rdit & 2;
    gif->gce.transparency = rdit & 1;
    gif->gce.delay = read_num(gif);
    f_gif_read(gif, &gif->gce.tindex, 1);
    /* Skip block terminator. */
    f_gif_seek(gif, 1, LV_FS_SEEK_CUR);
}

static void read_comment_ext(gd_GIF *gif)
{
    if (gif->comment) {
        size_t sub_block = f_gif_seek(gif, 0, LV_FS_SEEK_CUR);
        gif->comment(gif);
        f_gif_seek(gif, sub_block, LV_FS_SEEK_SET);
    }
    /* Discard comment sub-blocks. */
    discard_sub_blocks(gif);
}

static void read_application_ext(gd_GIF *gif)
{
    char app_id[8];
    char app_auth_code[3];

    /* Discard block size (always 0x0B). */
    f_gif_seek(gif, 1, LV_FS_SEEK_CUR);
    /* Application Identifier. */
    f_gif_read(gif, app_id, 8);
    /* Application Authentication Code. */
    f_gif_read(gif, app_auth_code, 3);
    if (!strncmp(app_id, "NETSCAPE", sizeof(app_id))) {
        /* Discard block size (0x03) and constant byte (0x01). */
        f_gif_seek(gif, 2, LV_FS_SEEK_CUR);
        gif->loop_count = read_num(gif);
        /* Skip block terminator. */
        f_gif_seek(gif, 1, LV_FS_SEEK_CUR);
    } else if (gif->application) {
        size_t sub_block = f_gif_seek(gif, 0, LV_FS_SEEK_CUR);
        gif->application(gif, app_id, app_auth_code);
        f_gif_seek(gif, sub_block, LV_FS_SEEK_SET);
        discard_sub_blocks(gif);
    } else {
        discard_sub_blocks(gif);
    }
}

static void read_ext(gd_GIF *gif)
{
    uint8_t label;

    f_gif_read(gif, &label, 1);
    switch (label) {
    case 0x01:
        read_plain_text_ext(gif);
        break;
    case 0xF9:
        read_graphic_control_ext(gif);
        break;
    case 0xFE:
        read_comment_ext(gif);
        break;
    case 0xFF:
        read_application_ext(gif);
        break;
    default:
        LV_LOG_WARN("unknown extension: %02X\n", label);
    }
}

static Table * new_table(int key_size)
{
    int key;
    int init_bulk = MAX(1 << (key_size + 1), 0x100);
    Table *table = lv_mem_alloc(sizeof(*table) + sizeof(Entry) * init_bulk);
    if (table) {
        table->bulk = init_bulk;
        table->nentries = (1 << key_size) + 2;
        table->entries = (Entry *) &table[1];
        for (key = 0; key < (1 << key_size); key++)
            table->entries[key] = (Entry) {1, 0xFFF, key};
    }
    return table;
}

/* Add table entry. Return value:
 *  0 on success
 *  +1 if key size must be incremented after this addition
 *  -1 if could not realloc table */
static int add_entry(Table **tablep, uint16_t length, uint16_t prefix, uint8_t suffix)
{
    Table *table = *tablep;
    if (table->nentries == table->bulk) {
        table->bulk *= 2;
        table = lv_mem_realloc(table, sizeof(*table) + sizeof(Entry) * table->bulk);
        if (!table) return -1;
        table->entries = (Entry *) &table[1];
        *tablep = table;
    }
    table->entries[table->nentries] = (Entry) {length, prefix, suffix};
    table->nentries++;
    if ((table->nentries & (table->nentries - 1)) == 0)
        return 1;
    return 0;
}

static uint16_t get_key(gd_GIF *gif, int key_size, uint8_t *sub_len, uint8_t *shift, uint8_t *byte)
{
    int bits_read;
    int rpad;
    int frag_size;
    uint16_t key;

    key = 0;
    for (bits_read = 0; bits_read < key_size; bits_read += frag_size) {
        rpad = (*shift + bits_read) % 8;
        if (rpad == 0) {
            /* Update byte. */
            if (*sub_len == 0) {
                f_gif_read(gif, sub_len, 1); /* Must be nonzero! */
                if (*sub_len == 0) return 0x1000;
            }
            f_gif_read(gif, byte, 1);
            (*sub_len)--;
        }
        frag_size = MIN(key_size - bits_read, 8 - rpad);
        key |= ((uint16_t) ((*byte) >> rpad)) << bits_read;
    }
    /* Clear extra bits to the left. */
    key &= (1 << key_size) - 1;
    *shift = (*shift + key_size) % 8;
    return key;
}

/* Compute output index of y-th input line, in frame of height h. */
static int interlaced_line_index(int h, int y)
{
    int p; /* number of lines in current pass */

    p = (h - 1) / 8 + 1;
    if (y < p) /* pass 1 */
        return y * 8;
    y -= p;
    p = (h - 5) / 8 + 1;
    if (y < p) /* pass 2 */
        return y * 8 + 4;
    y -= p;
    p = (h - 3) / 4 + 1;
    if (y < p) /* pass 3 */
        return y * 4 + 2;
    y -= p;
    /* pass 4 */
    return y * 2 + 1;
}

/* Decompress image pixels.
 * Return 0 on success or -1 on out-of-memory (w.r.t. LZW code table). */
static int read_image_data(gd_GIF *gif, int interlace)
{
    uint8_t sub_len, shift, byte;
    int init_key_size, key_size, table_is_full=0;
    int frm_off, frm_size, str_len=0, i, p, x, y;
    uint16_t key, clear, stop;
    int ret;
    Table *table;
    Entry entry = {0};
    size_t start, end;

    f_gif_read(gif, &byte, 1);
    key_size = (int) byte;
    start = f_gif_seek(gif, 0, LV_FS_SEEK_CUR);
    discard_sub_blocks(gif);
    end = f_gif_seek(gif, 0, LV_FS_SEEK_CUR);
    f_gif_seek(gif, start, LV_FS_SEEK_SET);
    clear = 1 << key_size;
    stop = clear + 1;
    table = new_table(key_size);
    key_size++;
    init_key_size = key_size;
    sub_len = shift = 0;
    key = get_key(gif, key_size, &sub_len, &shift, &byte); /* clear code */
    frm_off = 0;
    ret = 0;
    frm_size = gif->fw*gif->fh;
    while (frm_off < frm_size) {
        if (key == clear) {
            key_size = init_key_size;
            table->nentries = (1 << (key_size - 1)) + 2;
            table_is_full = 0;
        } else if (!table_is_full) {
            ret = add_entry(&table, str_len + 1, key, entry.suffix);
            if (ret == -1) {
                lv_mem_free(table);
                return -1;
            }
            if (table->nentries == 0x1000) {
                ret = 0;
                table_is_full = 1;
            }
        }
        key = get_key(gif, key_size, &sub_len, &shift, &byte);
        if (key == clear) continue;
        if (key == stop || key == 0x1000) break;
        if (ret == 1) key_size++;
        entry = table->entries[key];
        str_len = entry.length;
        for (i = 0; i < str_len; i++) {
            p = frm_off + entry.length - 1;
            x = p % gif->fw;
            y = p / gif->fw;
            if (interlace)
                y = interlaced_line_index((int) gif->fh, y);
            gif->frame[(gif->fy + y) * gif->width + gif->fx + x] = entry.suffix;
            if (entry.prefix == 0xFFF)
                break;
            else
                entry = table->entries[entry.prefix];
        }
        frm_off += str_len;
        if (key < table->nentries - 1 && !table_is_full)
            table->entries[table->nentries - 1].suffix = entry.suffix;
    }
    lv_mem_free(table);
    if (key == stop) f_gif_read(gif, &sub_len, 1); /* Must be zero! */
    f_gif_seek(gif, end, LV_FS_SEEK_SET);
    return 0;
}

/* Read image.
 * Return 0 on success or -1 on out-of-memory (w.r.t. LZW code table). */
static int read_image(gd_GIF *gif)
{
    uint8_t fisrz;
    int interlace;

    /* Image Descriptor. */
    gif->fx = read_num(gif);
    gif->fy = read_num(gif);
    gif->fw = read_num(gif);
    gif->fh = read_num(gif);
    f_gif_read(gif, &fisrz, 1);
    interlace = fisrz & 0x40;
    /* Ignore Sort Flag. */
    /* Local Color Table? */
    if (fisrz & 0x80) {
        /* Read LCT */
        gif->lct.size = 1 << ((fisrz & 0x07) + 1);
        f_gif_read(gif, gif->lct.colors, 3 * gif->lct.size);
        gif->palette = &gif->lct;
    } else
        gif->palette = &gif->gct;
    /* Image Data. */
    return read_image_data(gif, interlace);
}

static void render_frame_rect(gd_GIF *gif, uint8_t *buffer)
{
    int i, j, k;
    uint8_t index, *color;
    i = gif->fy * gif->width + gif->fx;
    for (j = 0; j < gif->fh; j++) {
        for (k = 0; k < gif->fw; k++) {
            index = gif->frame[(gif->fy + j) * gif->width + gif->fx + k];
            color = &gif->palette->colors[index*3];
            if (!gif->gce.transparency || index != gif->gce.tindex) {
#if LV_COLOR_DEPTH == 32
                buffer[(i+k)*4 + 0] = *(color + 2);
                buffer[(i+k)*4 + 1] = *(color + 1);
                buffer[(i+k)*4 + 2] = *(color + 0);
                buffer[(i+k)*4 + 3] = 0xFF;
#elif LV_COLOR_DEPTH == 16
                lv_color_t c = lv_color_make(*(color + 0), *(color + 1), *(color + 2));
                buffer[(i+k)*3 + 0] = c.full & 0xff;
                buffer[(i+k)*3 + 1] = (c.full >> 8) & 0xff;
                buffer[(i+k)*3 + 2] = 0xff;
#elif LV_COLOR_DEPTH == 8
                lv_color_t c = lv_color_make(*(color + 0), *(color + 1), *(color + 2));
                buffer[(i+k)*2 + 0] = c.full;
                buffer[(i+k)*2 + 1] = 0xff;
#elif LV_COLOR_DEPTH == 1
                uint8_t b = (*(color + 0)) | (*(color + 1)) | (*(color + 2));
                buffer[(i+k)*2 + 0] = b > 128 ? 1 : 0;
                buffer[(i+k)*2 + 1] = 0xff;
#endif
            }
        }
        i += gif->width;
    }
}

static void dispose(gd_GIF *gif)
{
    int i, j, k;
    uint8_t *bgcolor;
    switch (gif->gce.disposal) {
    case 2: /* Restore to background color. */
        bgcolor = &gif->palette->colors[gif->bgindex*3];

        uint8_t opa = 0xff;
        if(gif->gce.transparency) opa = 0x00;

        i = gif->fy * gif->width + gif->fx;
        for (j = 0; j < gif->fh; j++) {
            for (k = 0; k < gif->fw; k++) {
#if LV_COLOR_DEPTH == 32
                gif->canvas[(i+k)*4 + 0] = *(bgcolor + 2);
                gif->canvas[(i+k)*4 + 1] = *(bgcolor + 1);
                gif->canvas[(i+k)*4 + 2] = *(bgcolor + 0);
                gif->canvas[(i+k)*4 + 3] = opa;
#elif LV_COLOR_DEPTH == 16
                lv_color_t c = lv_color_make(*(bgcolor + 0), *(bgcolor + 1), *(bgcolor + 2));
                gif->canvas[(i+k)*3 + 0] = c.full & 0xff;
                gif->canvas[(i+k)*3 + 1] = (c.full >> 8) & 0xff;
                gif->canvas[(i+k)*3 + 2] = opa;
#elif LV_COLOR_DEPTH == 8
                lv_color_t c = lv_color_make(*(bgcolor + 0), *(bgcolor + 1), *(bgcolor + 2));
                gif->canvas[(i+k)*2 + 0] = c.full;
                gif->canvas[(i+k)*2 + 1] = opa;
#elif LV_COLOR_DEPTH == 1
                uint8_t b = (*(bgcolor + 0)) | (*(bgcolor + 1)) | (*(bgcolor + 2));
                gif->canvas[(i+k)*2 + 0] = b > 128 ? 1 : 0;
                gif->canvas[(i+k)*2 + 1] = opa;
#endif
            }
            i += gif->width;
        }
        break;
    case 3: /* Restore to previous, i.e., don't update canvas.*/
        break;
    default:
        /* Add frame non-transparent pixels to canvas. */
        render_frame_rect(gif, gif->canvas);
    }
}

/* Return 1 if got a frame; 0 if got GIF trailer; -1 if error. */
static int gd_get_frame(gd_GIF *gif)
{
    char sep;

    dispose(gif);
    f_gif_read(gif, &sep, 1);
    while (sep != ',') {
        if (sep == ';')
            return 0;
        if (sep == '!')
            read_ext(gif);
        else return -1;
        f_gif_read(gif, &sep, 1);
    }
    if (read_image(gif) == -1)
        return -1;
    return 1;
}

static void gd_render_frame(gd_GIF *gif, uint8_t *buffer)
{
    render_frame_rect(gif, buffer);
}

static void gd_rewind(gd_GIF *gif)
{
    f_gif_seek(gif, gif->anim_start, LV_FS_SEEK_SET);
}

static void gd_close_gif(gd_GIF *gif)
{
    f_gif_close(gif);
    lv_mem_free(gif);
}

static bool f_gif_open(gd_GIF * gif, const void * path, bool is_file)
{
    gif->f_rw_p = 0;
    gif->data = NULL;
    gif->is_file = is_file;

    if(is_file) {
        lv_fs_res_t res = lv_fs_open(&gif->fd, path, LV_FS_MODE_RD);
        if(res != LV_FS_RES_OK) return false;
        else return true;
    } else {
        gif->data = path;
        return true;
    }
}

static void f_gif_read(gd_GIF * gif, void * buf, size_t len)
{
    if(gif->is_file) {
        lv_fs_read(&gif->fd, buf, len, NULL);
    } else
    {
        memcpy(buf, &gif->data[gif->f_rw_p], len);
        gif->f_rw_p += len;
    }
}

static int f_gif_seek(gd_GIF * gif, size_t pos, int k)
{
    if(gif->is_file) {
        lv_fs_seek(&gif->fd, pos, k);
        uint32_t x;
        lv_fs_tell(&gif->fd, &x);
        return x;
    } else {
        if(k == LV_FS_SEEK_CUR) gif->f_rw_p += pos;
        else if(k == LV_FS_SEEK_SET) gif->f_rw_p = pos;
        return gif->f_rw_p;
    }
}

static void f_gif_close(gd_GIF * gif)
{
    if(gif->is_file) {
        lv_fs_close(&gif->fd);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void set_caps(uint8_t * caps)
{
    if(caps != NULL) *caps = LV_IMG_DEC_ANIMATED | LV_IMG_DEC_VFR | LV_IMG_DEC_CACHED;
}
/**
 * Get info about a rlottie image
 * @param src can be file name or pointer to a C array
 * @param header store the info here
 * @return LV_RES_OK: no error; LV_RES_INV: can't get the info
 */
static lv_res_t decoder_accept(const lv_img_src_t * src, uint8_t * caps)
{
    /*If it's a GIF file...*/
    if(src->type == LV_IMG_SRC_FILE) {
        if(!strncmp(src->ext, ".gif", 4)) {              /*Check the extension*/
            set_caps(caps);
            return LV_RES_OK;
        }
    }
    /* GIF as raw data */
    else if(src->type == LV_IMG_SRC_VARIABLE) {
        const char * str = (const char *)src->uri;
        if (memcmp(str, "GIF89a", 6) == 0) {
            set_caps(caps);
            return LV_RES_OK;
        }
    }
    return LV_RES_INV;
}

/**
 * Open a GIF animation image and return the decoded image
 * @param dsc Decoded descriptor for the animation
 * @return LV_RES_OK: no error; LV_RES_INV: can't decode the picture
 */
static lv_res_t decoder_open(lv_img_decoder_dsc_t * dsc, const lv_img_dec_flags_t flags)
{
    gd_GIF * gif = dsc->dec_ctx ? (gd_GIF *)dsc->dec_ctx->user_data : NULL;

    if(gif == NULL) {
        /* Unfortunately, creating a context here is absolutely required */
        if(dsc->in.src->type == LV_IMG_SRC_FILE) {
            gif = gd_open_gif_file(dsc->in.src->uri, flags == LV_IMG_DEC_ONLYMETA);
        } else {
            gif = gd_open_gif_data(dsc->in.src->uri, flags == LV_IMG_DEC_ONLYMETA);
        }
        if (!gif) return LV_RES_INV;

        dsc->header.w = gif->width;
        dsc->header.h = gif->height;
        dsc->header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
        set_caps(&dsc->caps);
        if (gif->loop_count) dsc->caps |= LV_IMG_DEC_LOOPING;
        if (dsc->dec_ctx == NULL && flags != LV_IMG_DEC_ONLYMETA) {
            LV_ZERO_ALLOC(dsc->dec_ctx);
            if (dsc->dec_ctx == NULL) {
                gd_close_gif(gif);
                return LV_RES_INV;
            }
            dsc->dec_ctx->user_data = gif;
            dsc->dec_ctx->auto_allocated = 1;
        }
        if (dsc->dec_ctx != NULL) {
            /* Count the frame number and set the frame delay here */
            dsc->dec_ctx->caps = dsc->caps;
            dsc->dec_ctx->frame_rate = 100; /* 10ms for the timer period */
            dsc->dec_ctx->total_frames = count_frames(gif); /* Order is important here, since it fills gce.delay */
            dsc->dec_ctx->frame_delay = gif->gce.delay * 10;
            dsc->img_data = gif->canvas;
        }

        if (flags == LV_IMG_DEC_ONLYMETA) {
            gd_close_gif(gif);
            return LV_RES_OK;
        }
    }
    /* Rewind is the only action we can do on a GIF file */
    if (dsc->dec_ctx->current_frame == 0) {
        gd_rewind(gif);
        if (gif->loop_count >= 1) dsc->dec_ctx->caps |= LV_IMG_DEC_LOOPING;
    }
    if(gd_get_frame(gif)) {
        gd_render_frame(gif, (uint8_t *)dsc->img_data);
        dsc->dec_ctx->frame_delay = gif->gce.delay * 10; /* VFR means delay changes between frames */
    } else {
        /* Ended the current loop, let's check what to do, if we remove the loop count */
        if(gif->loop_count == 1) dsc->dec_ctx->caps &= ~LV_IMG_DEC_LOOPING;
        else gif->loop_count--;
    }
    return LV_RES_OK;
}

static lv_res_t decoder_read_line(lv_img_decoder_dsc_t * dsc,
                                  lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf)
{
    LV_UNUSED(dsc);
    LV_UNUSED(x);
    LV_UNUSED(y);
    LV_UNUSED(len);
    LV_UNUSED(buf);
    return LV_RES_INV;
}

static void decoder_close(lv_img_decoder_dsc_t * dsc)
{
    lv_img_dec_ctx_t * dec_ctx = dsc->dec_ctx;
    if(dec_ctx && dec_ctx->auto_allocated) {
        /*Only free if allocated by ourselves.*/
        gd_GIF * gif = (gd_GIF *)dec_ctx->user_data;
        gd_close_gif(gif);
        gif = NULL;
        dec_ctx->user_data = 0;
        lv_mem_free(dec_ctx);
    }
    dsc->dec_ctx = 0;
}

#endif /*LV_USE_GIF*/
