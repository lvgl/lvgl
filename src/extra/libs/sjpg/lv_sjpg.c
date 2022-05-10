/**
 * @file lv_sjpg.c
 *
 */

/*----------------------------------------------------------------------------------------------------------------------------------
/    Added normal JPG support [7/10/2020]
/    ----------
/    SJPEG is a custom created modified JPEG file format for small embedded platforms.
/    It will contain multiple JPEG fragments all embedded into a single file with a custom header.
/    This makes JPEG decoding easier using any JPEG library. Overall file size will be almost
/    similar to the parent jpeg file. We can generate sjpeg from any jpeg using a python script
/    provided along with this project.
/                                                                                     (by vinodstanur | 2020 )
/    SJPEG FILE STRUCTURE
/    --------------------------------------------------------------------------------------------------------------------------------
/    Bytes                       |   Value                                                                                           |
/    --------------------------------------------------------------------------------------------------------------------------------
/
/    0 - 7                       |   "_SJPG__" followed by '\0'
/
/    8 - 13                      |   "V1.00" followed by '\0'       [VERSION OF SJPG FILE for future compatibiliby]
/
/    14 - 15                     |   X_RESOLUTION (width)            [little endian]
/
/    16 - 17                     |   Y_RESOLUTION (height)           [little endian]
/
/    18 - 19                     |   TOTAL_FRAMES inside sjpeg       [little endian]
/
/    20 - 21                     |   JPEG BLOCK WIDTH (16 normally)  [little endian]
/
/    22 - [(TOTAL_FRAMES*2 )]    |   SIZE OF EACH JPEG SPLIT FRAGMENTS   (FRAME_INFO_ARRAY)
/
/   SJPEG data                   |   Each JPEG frame can be extracted from SJPEG data by parsing the FRAME_INFO_ARRAY one time.
/
/----------------------------------------------------------------------------------------------------------------------------------
/                   JPEG DECODER
/                   ------------
/   We are using TJpgDec - Tiny JPEG Decompressor library from ELM-CHAN for decoding each split-jpeg fragments.
/   The tjpgd.c and tjpgd.h is not modified and those are used as it is. So if any update comes for the tiny-jpeg,
/   just replace those files with updated files.
/---------------------------------------------------------------------------------------------------------------------------------*/

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl.h"
#if LV_USE_SJPG

#include "tjpgd.h"
#include "lv_sjpg.h"
#include "../../../misc/lv_fs.h"

/*********************
 *      DEFINES
 *********************/
#define TJPGD_WORKBUFF_SIZE             4096    //Recommended by TJPGD libray

//NEVER EDIT THESE OFFSET VALUES
#define SJPEG_VERSION_OFFSET            8
#define SJPEG_X_RES_OFFSET              14
#define SJPEG_y_RES_OFFSET              16
#define SJPEG_TOTAL_FRAMES_OFFSET       18
#define SJPEG_BLOCK_WIDTH_OFFSET        20
#define SJPEG_FRAME_INFO_ARRAY_OFFSET   22

/**********************
 *      TYPEDEFS
 **********************/

enum io_source_type {
    SJPEG_IO_SOURCE_C_ARRAY,
    SJPEG_IO_SOURCE_DISK,
};

typedef struct {
    enum io_source_type type;
    lv_fs_file_t lv_file;
    uint8_t * img_cache_buff;
    int img_cache_x_res;
    int img_cache_y_res;
    uint8_t * raw_sjpg_data;              //Used when type==SJPEG_IO_SOURCE_C_ARRAY.
    uint32_t raw_sjpg_data_size;          //Num bytes pointed to by raw_sjpg_data.
    uint32_t raw_sjpg_data_next_read_pos; //Used for all types.
} io_source_t;


typedef struct {
    uint8_t * sjpeg_data;
    uint32_t sjpeg_data_size;
    int sjpeg_x_res;
    int sjpeg_y_res;
    int sjpeg_total_frames;
    int sjpeg_single_frame_height;
    int sjpeg_cache_frame_index;
    uint8_t ** frame_base_array;        //to save base address of each split frames upto sjpeg_total_frames.
    int * frame_base_offset;            //to save base offset for fseek
    uint8_t * frame_cache;
    uint8_t * workb;                    //JPG work buffer for jpeg library
    JDEC * tjpeg_jd;
    io_source_t io;
} SJPEG;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t decoder_accept(const lv_img_src_t * src, uint8_t * caps);
static lv_res_t decoder_open(lv_img_dec_dsc_t * dsc, const lv_img_dec_flags_t flags);


static lv_res_t decoder_read_line(lv_img_dec_dsc_t * dsc,
                                  lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf);

static void decoder_close(lv_img_dec_dsc_t * dsc);
static size_t input_func(JDEC * jd, uint8_t * buff, size_t ndata);
static int is_jpg(const uint8_t * raw_data, size_t len);
static void lv_sjpg_cleanup(SJPEG * sjpeg);
static void lv_sjpg_free(SJPEG * sjpeg);
static void set_caps(uint8_t * caps);

static lv_res_t decode_jpg_header(JDEC * jd_tmp, SJPEG * sjpeg, bool is_file);
static lv_res_t extract_sjpeg_meta(lv_img_header_t * header, uint8_t * raw_sjpeg_data);
static lv_res_t decode_sjpeg_header(SJPEG * sjpeg, lv_fs_file_t * fd);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_split_jpeg_init(void)
{
    lv_img_dec_t * dec = lv_img_decoder_create();
    lv_img_decoder_set_accept_cb(dec, decoder_accept);
    lv_img_decoder_set_open_cb(dec, decoder_open);
    lv_img_decoder_set_close_cb(dec, decoder_close);
    lv_img_decoder_set_read_line_cb(dec, decoder_read_line);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void set_caps(uint8_t * caps)
{
    *caps = LV_IMG_DEC_DEFAULT;
}


static lv_res_t decoder_accept(const lv_img_src_t * src, uint8_t * caps)
{
    /* Read the SJPG/JPG header to assert it's the right format */
    if(src->type == LV_IMG_SRC_VARIABLE) {
        uint8_t * raw_sjpeg_data = (uint8_t *)src->data;
        if(!strncmp((char *)raw_sjpeg_data, "_SJPG__", strlen("_SJPG__"))) {
            set_caps(caps);
            return LV_RES_OK;
        }
    }
    else if(src->type == LV_IMG_SRC_FILE) {
        /*Support only "*.jpg" & ".sjpg" files*/
        if(!src->ext || (strcmp(src->ext, ".jpg") && strcmp(src->ext, ".sjpg"))) return LV_RES_INV;
        *caps = LV_IMG_DEC_CACHED;

        /*Check file exists*/
        lv_fs_file_t f;
        lv_fs_res_t res = lv_fs_open(&f, src->data, LV_FS_MODE_RD);
        if(res != LV_FS_RES_OK) return LV_RES_INV;
        lv_fs_close(&f);

        return LV_RES_OK;
    }
    return LV_RES_INV;
}

static lv_res_t extract_sjpeg_meta(lv_img_header_t * header, uint8_t * raw_sjpeg_data)
{
    if(!strncmp((char *)raw_sjpeg_data, "_SJPG__", strlen("_SJPG__"))) {
        raw_sjpeg_data += 14; //seek to res info ... refer sjpeg format

        header->always_zero = 0;
        header->cf = LV_IMG_CF_RAW;

        header->w = *raw_sjpeg_data++;
        header->w |= *raw_sjpeg_data++ << 8;

        header->h = *raw_sjpeg_data++;
        header->h |= *raw_sjpeg_data++ << 8;
        return LV_RES_OK;
    }
    return LV_RES_INV;
}

static lv_res_t decode_sjpeg_header(SJPEG * sjpeg, lv_fs_file_t * fd)
{
    uint8_t * data = sjpeg->sjpeg_data;
    data += 14;

    sjpeg->sjpeg_x_res = *data++;
    sjpeg->sjpeg_x_res |= *data++ << 8;

    sjpeg->sjpeg_y_res = *data++;
    sjpeg->sjpeg_y_res |= *data++ << 8;

    sjpeg->sjpeg_total_frames = *data++;
    sjpeg->sjpeg_total_frames |= *data++ << 8;

    sjpeg->sjpeg_single_frame_height = *data++;
    sjpeg->sjpeg_single_frame_height |= *data++ << 8;

    if(!fd) {
        sjpeg->frame_base_array = lv_mem_alloc(sizeof(uint8_t *) * sjpeg->sjpeg_total_frames);
        if(!sjpeg->frame_base_array) {
            return LV_RES_INV;
        }

        sjpeg->frame_base_offset = NULL;

        uint8_t * img_frame_base = data +  sjpeg->sjpeg_total_frames * 2;
        sjpeg->frame_base_array[0] = img_frame_base;

        for(int i = 1; i <  sjpeg->sjpeg_total_frames; i++) {
            int offset = *data++;
            offset |= *data++ << 8;
            sjpeg->frame_base_array[i] = sjpeg->frame_base_array[i - 1] + offset;
        }
    }
    else {
        sjpeg->frame_base_array = NULL;//lv_mem_alloc( sizeof(uint8_t *) * sjpeg->sjpeg_total_frames );
        sjpeg->frame_base_offset = lv_mem_alloc(sizeof(int) * sjpeg->sjpeg_total_frames);
        if(! sjpeg->frame_base_offset) {
            return LV_RES_INV;
        }
        int img_frame_start_offset = (SJPEG_FRAME_INFO_ARRAY_OFFSET + sjpeg->sjpeg_total_frames * 2);
        sjpeg->frame_base_offset[0] = img_frame_start_offset; //pointer used to save integer for now...

        data = sjpeg->sjpeg_data;
        for(int i = 1; i <  sjpeg->sjpeg_total_frames; i++) {
            uint32_t rn = 0;
            lv_res_t res = lv_fs_read(fd, data, 2, &rn);
            if(res != LV_FS_RES_OK || rn != 2) {
                return LV_RES_INV;
            }

            int offset = *data++;
            offset |= *data++ << 8;
            sjpeg->frame_base_offset[i] = sjpeg->frame_base_offset[i - 1] + offset;
        }
    }
    sjpeg->sjpeg_cache_frame_index = -1;
    sjpeg->frame_cache = (void *)lv_mem_alloc(sjpeg->sjpeg_x_res * sjpeg->sjpeg_single_frame_height * 3/*2*/);
    if(! sjpeg->frame_cache) {
        return LV_RES_INV;
    }
    sjpeg->io.img_cache_buff = sjpeg->frame_cache;
    sjpeg->io.img_cache_x_res = sjpeg->sjpeg_x_res;
    sjpeg->workb = (uint8_t *)lv_mem_alloc(TJPGD_WORKBUFF_SIZE);
    if(! sjpeg->workb) {
        return LV_RES_INV;
    }

    sjpeg->tjpeg_jd = (JDEC *)lv_mem_alloc(sizeof(JDEC));
    if(! sjpeg->tjpeg_jd) {
        return LV_RES_INV;
    }
    return LV_RES_OK;
}

static lv_res_t decode_jpg_header(JDEC * jd_tmp, SJPEG * sjpeg, bool is_file)
{
    sjpeg->sjpeg_x_res = jd_tmp->width;
    sjpeg->sjpeg_y_res = jd_tmp->height;
    sjpeg->sjpeg_total_frames = 1;
    sjpeg->sjpeg_single_frame_height = jd_tmp->height;

    if(is_file) {
        sjpeg->frame_base_array = NULL;
        sjpeg->frame_base_offset =  lv_mem_alloc(sizeof(uint8_t *) * sjpeg->sjpeg_total_frames);
        if(! sjpeg->frame_base_offset) {
            return LV_RES_INV;
        }

        int img_frame_start_offset = 0;
        sjpeg->frame_base_offset[0] = img_frame_start_offset;
    }
    else {
        sjpeg->frame_base_array = lv_mem_alloc(sizeof(uint8_t *) * sjpeg->sjpeg_total_frames);
        if(! sjpeg->frame_base_array) {
            return LV_RES_INV;
        }
        sjpeg->frame_base_offset = NULL;

        uint8_t * img_frame_base = sjpeg->sjpeg_data;
        sjpeg->frame_base_array[0] = img_frame_base;
    }
    sjpeg->sjpeg_cache_frame_index = -1;
    sjpeg->frame_cache = (void *)lv_mem_alloc(sjpeg->sjpeg_x_res * sjpeg->sjpeg_single_frame_height * 3);
    if(! sjpeg->frame_cache) {
        return LV_RES_INV;
    }

    sjpeg->io.img_cache_buff = sjpeg->frame_cache;
    sjpeg->io.img_cache_x_res = sjpeg->sjpeg_x_res;
    sjpeg->workb = (uint8_t *)lv_mem_alloc(TJPGD_WORKBUFF_SIZE);
    if(! sjpeg->workb) {
        return LV_RES_INV;
    }

    sjpeg->tjpeg_jd = (JDEC *)lv_mem_alloc(sizeof(JDEC));
    if(! sjpeg->tjpeg_jd) {
        return LV_RES_INV;
    }
    return LV_RES_OK;
}

static lv_res_t decoder_open(lv_img_dec_dsc_t * dsc, const lv_img_dec_flags_t flags)
{
    uint8_t * caps = &dsc->caps;

    if(flags == LV_IMG_DEC_ONLYMETA) {
        if(dsc->input.src->type == LV_IMG_SRC_VARIABLE) {
            uint8_t * raw_sjpeg_data = (uint8_t *)dsc->input.src->data;
            const uint32_t raw_sjpeg_data_size = dsc->input.src->data_len;

            if(extract_sjpeg_meta(&dsc->header, raw_sjpeg_data) == LV_RES_OK) {
                set_caps(caps);
                return LV_RES_OK;
            }
            else if(is_jpg(raw_sjpeg_data, raw_sjpeg_data_size) == true) {
                set_caps(caps);
                dsc->header.always_zero = 0;
                dsc->header.cf = LV_IMG_CF_RAW;

                uint8_t * workb_temp = lv_mem_alloc(TJPGD_WORKBUFF_SIZE);
                if(!workb_temp) return LV_RES_INV;

                io_source_t io_source_temp;
                io_source_temp.type = SJPEG_IO_SOURCE_C_ARRAY;
                io_source_temp.raw_sjpg_data = raw_sjpeg_data;
                io_source_temp.raw_sjpg_data_size = raw_sjpeg_data_size;
                io_source_temp.raw_sjpg_data_next_read_pos = 0;

                JDEC jd_tmp;

                JRESULT rc = jd_prepare(&jd_tmp, input_func, workb_temp, (size_t)TJPGD_WORKBUFF_SIZE, &io_source_temp);
                lv_mem_free(workb_temp);
                if(rc == JDR_OK) {
                    dsc->header.w = jd_tmp.width;
                    dsc->header.h = jd_tmp.height;
                    return LV_RES_OK;
                }
                return LV_RES_INV;
            }
        }
        else if(dsc->input.src->type == LV_IMG_SRC_FILE) {
            if(!dsc->input.src->ext) return LV_RES_INV;
            if(!strcmp(dsc->input.src->ext, ".sjpg")) {
                uint8_t buff[32];
                memset(buff, 0, sizeof(buff));

                lv_fs_file_t file;
                lv_fs_res_t res = lv_fs_open(&file, (const char *)dsc->input.src->data, LV_FS_MODE_RD);
                if(res != LV_FS_RES_OK) return LV_RES_INV;

                uint32_t rn;
                res = lv_fs_read(&file, buff, sizeof(buff), &rn);
                lv_fs_close(&file);
                if(res != LV_FS_RES_OK || rn != sizeof(buff)) {
                    return LV_RES_INV;
                }
                return extract_sjpeg_meta(&dsc->header, buff);
            }
            else if(!strcmp(dsc->input.src->ext, ".jpg")) {
                lv_fs_file_t file;
                lv_fs_res_t res = lv_fs_open(&file, (const char *)dsc->input.src->data, LV_FS_MODE_RD);
                if(res != LV_FS_RES_OK) return LV_RES_INV;

                uint8_t * workb_temp = lv_mem_alloc(TJPGD_WORKBUFF_SIZE);
                if(!workb_temp) {
                    lv_fs_close(&file);
                    return LV_RES_INV;
                }

                io_source_t io_source_temp;
                io_source_temp.type = SJPEG_IO_SOURCE_DISK;
                io_source_temp.raw_sjpg_data_next_read_pos = 0;
                io_source_temp.img_cache_buff = NULL;
                io_source_temp.lv_file = file;
                JDEC jd_tmp;

                JRESULT rc = jd_prepare(&jd_tmp, input_func, workb_temp, (size_t)TJPGD_WORKBUFF_SIZE, &io_source_temp);
                lv_mem_free(workb_temp);
                lv_fs_close(&file);

                if(rc == JDR_OK) {
                    dsc->header.always_zero = 0;
                    dsc->header.cf = LV_IMG_CF_RAW;
                    dsc->header.w = jd_tmp.width;
                    dsc->header.h = jd_tmp.height;
                    return LV_RES_OK;
                }
            }
            return LV_RES_INV;
        }
    }

    lv_img_dec_ctx_t * dec_ctx = dsc->dec_ctx;
    if(dec_ctx == NULL) {
        LV_ZERO_ALLOC(dec_ctx);
        dsc->dec_ctx = dec_ctx;
        dec_ctx->auto_allocated = 1;
    }
    set_caps(&dsc->caps);
    SJPEG * sjpeg = (SJPEG *)dec_ctx->user_data;
    LV_ZERO_ALLOC(sjpeg);
    dec_ctx->user_data = sjpeg;

    if(dsc->input.src->type == LV_IMG_SRC_VARIABLE) {
        sjpeg->sjpeg_data = (uint8_t *)dsc->input.src->data;
        sjpeg->sjpeg_data_size = dsc->input.src->data_len;

        if(!strncmp((char *) sjpeg->sjpeg_data, "_SJPG__", strlen("_SJPG__"))) {
            if(decode_sjpeg_header(sjpeg, NULL) == LV_RES_INV) {
                decoder_close(dsc);
                return LV_RES_INV;
            }
            sjpeg->io.type = SJPEG_IO_SOURCE_C_ARRAY;
            sjpeg->io.lv_file.file_d = NULL;
        }
        else if(is_jpg(sjpeg->sjpeg_data, sjpeg->sjpeg_data_size) == true) {
            uint8_t * workb_temp = lv_mem_alloc(TJPGD_WORKBUFF_SIZE);
            if(!workb_temp) {
                decoder_close(dsc);
                return LV_RES_INV;
            }
            io_source_t io_source_temp;
            io_source_temp.type = SJPEG_IO_SOURCE_C_ARRAY;
            io_source_temp.raw_sjpg_data =  sjpeg->sjpeg_data;
            io_source_temp.raw_sjpg_data_size = sjpeg->sjpeg_data_size;
            io_source_temp.raw_sjpg_data_next_read_pos = 0;

            JDEC jd_tmp;
            JRESULT rc = jd_prepare(&jd_tmp, input_func, workb_temp, (size_t)TJPGD_WORKBUFF_SIZE, &io_source_temp);
            lv_mem_free(workb_temp);


            if(rc != JDR_OK) {
                decoder_close(dsc);
                return LV_RES_INV;
            }

            if(decode_jpg_header(&jd_tmp, sjpeg, false) == LV_RES_INV) {
                decoder_close(dsc);
                return LV_RES_INV;
            }
            sjpeg->io.type = SJPEG_IO_SOURCE_C_ARRAY;
            sjpeg->io.lv_file.file_d = NULL;
        }
        else {
            decoder_close(dsc);
            return LV_RES_INV;
        }
    }
    else if(dsc->input.src->type == LV_IMG_SRC_FILE) {
        /* If all fine, then the file will be kept open */
        if(!strcmp(dsc->input.src->ext, ".sjpg")) {
            uint8_t buff[22];
            memset(buff, 0, sizeof(buff));

            lv_fs_file_t lv_file;
            lv_fs_res_t res = lv_fs_open(&lv_file, (const char *)dsc->input.src->data, LV_FS_MODE_RD);
            if(res != LV_FS_RES_OK) {
                decoder_close(dsc);
                return LV_RES_INV;
            }

            sjpeg->io.lv_file = lv_file;
            sjpeg->io.type = SJPEG_IO_SOURCE_DISK;

            uint32_t rn;
            res = lv_fs_read(&lv_file, buff, 22, &rn);
            if(res != LV_FS_RES_OK || rn != 22) {
                decoder_close(dsc);
                return LV_RES_INV;
            }

            if(strcmp((char *)buff, "_SJPG__") == 0) {
                sjpeg->sjpeg_data = buff;
                sjpeg->sjpeg_data_size = rn;

                if(decode_sjpeg_header(sjpeg, &lv_file) == LV_RES_INV) {
                    decoder_close(dsc);
                    return LV_RES_INV;
                }
            }
            else {
                decoder_close(dsc);
                return LV_RES_INV;
            }
        }
        else if(!strcmp(dsc->input.src->ext, ".jpg")) {
            lv_fs_file_t lv_file;
            lv_fs_res_t res = lv_fs_open(&lv_file, (const char *)dsc->input.src->data, LV_FS_MODE_RD);
            if(res != LV_FS_RES_OK) {
                return LV_RES_INV;
            }

            sjpeg->io.lv_file = lv_file;
            sjpeg->io.type = SJPEG_IO_SOURCE_DISK;

            sjpeg->sjpeg_data = NULL;
            sjpeg->sjpeg_data_size = 0;

            uint8_t * workb_temp = lv_mem_alloc(TJPGD_WORKBUFF_SIZE);
            if(! workb_temp) {
                lv_fs_close(&lv_file);
                lv_sjpg_cleanup(sjpeg);
                return LV_RES_INV;
            }

            io_source_t io_source_temp;
            io_source_temp.type = SJPEG_IO_SOURCE_DISK;
            io_source_temp.raw_sjpg_data_next_read_pos = 0;
            io_source_temp.img_cache_buff = NULL;
            io_source_temp.lv_file = lv_file;

            JDEC jd_tmp;

            JRESULT rc = jd_prepare(&jd_tmp, input_func, workb_temp, (size_t)TJPGD_WORKBUFF_SIZE, &io_source_temp);

            lv_mem_free(workb_temp);
            if(rc != JDR_OK) {
                decoder_close(dsc);
                return LV_RES_INV;
            }
            if(decode_jpg_header(&jd_tmp, sjpeg, true) == LV_RES_INV) {
                decoder_close(dsc);
                return LV_RES_INV;
            }
        }
        else {
            decoder_close(dsc);
            return LV_RES_INV;
        }
    }
    dsc->header.always_zero = 0;
    dsc->header.cf = LV_IMG_CF_RAW;
    dsc->header.w = sjpeg->sjpeg_x_res;
    dsc->header.h = sjpeg->sjpeg_y_res;
    dsc->img_data = NULL;
    return LV_RES_OK;
}

static int img_data_cb(JDEC * jd, void * data, JRECT * rect)
{
    io_source_t * io = jd->device;
    uint8_t * cache = io->img_cache_buff;
    const int xres = io->img_cache_x_res;
    uint8_t * buf = data;
    const int INPUT_PIXEL_SIZE = 3;
    const int row_width = rect->right - rect->left + 1; // Row width in pixels.
    const int row_size = row_width * INPUT_PIXEL_SIZE;  // Row size (bytes).

    for(int y = rect->top; y <= rect->bottom; y++) {
        int row_offset = y * xres * INPUT_PIXEL_SIZE + rect->left * INPUT_PIXEL_SIZE;
        memcpy(cache + row_offset, buf, row_size);
        buf += row_size;
    }

    return 1;
}

static size_t input_func(JDEC * jd, uint8_t * buff, size_t ndata)
{
    io_source_t * io = jd->device;

    if(!io) return 0;

    if(io->type == SJPEG_IO_SOURCE_C_ARRAY) {
        const uint32_t bytes_left = io->raw_sjpg_data_size - io->raw_sjpg_data_next_read_pos;
        const uint32_t to_read = ndata <= bytes_left ? (uint32_t)ndata : bytes_left;
        if(to_read == 0)
            return 0;
        if(buff) {
            memcpy(buff, io->raw_sjpg_data + io->raw_sjpg_data_next_read_pos, to_read);
        }
        io->raw_sjpg_data_next_read_pos += to_read;
        return to_read;
    }
    else if(io->type == SJPEG_IO_SOURCE_DISK) {

        lv_fs_file_t * lv_file_p = &(io->lv_file);

        if(buff) {
            uint32_t rn = 0;
            lv_fs_read(lv_file_p, buff, (uint32_t)ndata, &rn);
            return rn;
        }
        else {
            uint32_t pos;
            lv_fs_tell(lv_file_p, &pos);
            lv_fs_seek(lv_file_p, (uint32_t)(ndata + pos),  LV_FS_SEEK_SET);
            return ndata;
        }
    }
    return 0;
}

/**
 * Decode `len` pixels starting from the given `x`, `y` coordinates and store them in `buf`.
 * Required only if the "open" function can't open the whole decoded pixel array. (dsc->img_data == NULL)
 * @param decoder pointer to the decoder the function associated with
 * @param dsc pointer to decoder descriptor
 * @param x start x coordinate
 * @param y start y coordinate
 * @param len number of pixels to decode
 * @param buf a buffer to store the decoded pixels
 * @return LV_RES_OK: ok; LV_RES_INV: failed
 */

static lv_res_t decoder_read_line(lv_img_dec_dsc_t * dsc, lv_coord_t x, lv_coord_t y,
                                  lv_coord_t len, uint8_t * buf)
{
    SJPEG * sjpeg = (SJPEG *) dsc->dec_ctx->user_data;
    if(dsc->input.src->type == LV_IMG_SRC_VARIABLE) {
        JRESULT rc;

        int sjpeg_req_frame_index = y / sjpeg->sjpeg_single_frame_height;

        /*If line not from cache, refresh cache */
        if(sjpeg_req_frame_index != sjpeg->sjpeg_cache_frame_index) {
            sjpeg->io.raw_sjpg_data = sjpeg->frame_base_array[ sjpeg_req_frame_index ];
            if(sjpeg_req_frame_index == (sjpeg->sjpeg_total_frames - 1)) {
                /*This is the last frame. */
                const uint32_t frame_offset = (uint32_t)(sjpeg->io.raw_sjpg_data - sjpeg->sjpeg_data);
                sjpeg->io.raw_sjpg_data_size = sjpeg->sjpeg_data_size - frame_offset;
            }
            else {
                sjpeg->io.raw_sjpg_data_size =
                    (uint32_t)(sjpeg->frame_base_array[sjpeg_req_frame_index + 1] - sjpeg->io.raw_sjpg_data);
            }
            sjpeg->io.raw_sjpg_data_next_read_pos = 0;
            rc = jd_prepare(sjpeg->tjpeg_jd, input_func, sjpeg->workb, (size_t)TJPGD_WORKBUFF_SIZE, &(sjpeg->io));
            if(rc != JDR_OK) return LV_RES_INV;
            rc = jd_decomp(sjpeg->tjpeg_jd, img_data_cb, 0);
            if(rc != JDR_OK) return LV_RES_INV;
            sjpeg->sjpeg_cache_frame_index = sjpeg_req_frame_index;
        }

        int offset = 0;
        uint8_t * cache = (uint8_t *)sjpeg->frame_cache + x * 3 + (y % sjpeg->sjpeg_single_frame_height) * sjpeg->sjpeg_x_res *
                          3;

#if  LV_COLOR_DEPTH == 32
        for(int i = 0; i < len; i++) {
            buf[offset + 3] = 0xff;
            buf[offset + 2] = *cache++;
            buf[offset + 1] = *cache++;
            buf[offset + 0] = *cache++;
            offset += 4;
        }

#elif  LV_COLOR_DEPTH == 16

        for(int i = 0; i < len; i++) {
            uint16_t col_16bit = (*cache++ & 0xf8) << 8;
            col_16bit |= (*cache++ & 0xFC) << 3;
            col_16bit |= (*cache++ >> 3);
#if  LV_BIG_ENDIAN_SYSTEM == 1 || LV_COLOR_16_SWAP == 1
            buf[offset++] = col_16bit >> 8;
            buf[offset++] = col_16bit & 0xff;
#else
            buf[offset++] = col_16bit & 0xff;
            buf[offset++] = col_16bit >> 8;
#endif // LV_BIG_ENDIAN_SYSTEM
        }

#elif  LV_COLOR_DEPTH == 8

        for(int i = 0; i < len; i++) {
            uint8_t col_8bit = (*cache++ & 0xC0);
            col_8bit |= (*cache++ & 0xe0) >> 2;
            col_8bit |= (*cache++ & 0xe0) >> 5;
            buf[offset++] = col_8bit;
        }
#else
#error Unsupported LV_COLOR_DEPTH


#endif // LV_COLOR_DEPTH
        return LV_RES_OK;
    }
    else if(dsc->input.src->type == LV_IMG_SRC_FILE) {
        JRESULT rc;
        int sjpeg_req_frame_index = y / sjpeg->sjpeg_single_frame_height;

        lv_fs_file_t * lv_file_p = &(sjpeg->io.lv_file);
        if(!lv_file_p) goto end;

        /*If line not from cache, refresh cache */
        if(sjpeg_req_frame_index != sjpeg->sjpeg_cache_frame_index) {
            sjpeg->io.raw_sjpg_data_next_read_pos = (int)(sjpeg->frame_base_offset [ sjpeg_req_frame_index ]);
            lv_fs_seek(&(sjpeg->io.lv_file), sjpeg->io.raw_sjpg_data_next_read_pos, LV_FS_SEEK_SET);

            rc = jd_prepare(sjpeg->tjpeg_jd, input_func, sjpeg->workb, (size_t)TJPGD_WORKBUFF_SIZE, &(sjpeg->io));
            if(rc != JDR_OK) return LV_RES_INV;

            rc = jd_decomp(sjpeg->tjpeg_jd, img_data_cb, 0);
            if(rc != JDR_OK) return LV_RES_INV;

            sjpeg->sjpeg_cache_frame_index = sjpeg_req_frame_index;
        }

        int offset = 0;
        uint8_t * cache = (uint8_t *)sjpeg->frame_cache + x * 3 + (y % sjpeg->sjpeg_single_frame_height) * sjpeg->sjpeg_x_res *
                          3;

#if LV_COLOR_DEPTH == 32
        for(int i = 0; i < len; i++) {
            buf[offset + 3] = 0xff;
            buf[offset + 2] = *cache++;
            buf[offset + 1] = *cache++;
            buf[offset + 0] = *cache++;
            offset += 4;
        }
#elif  LV_COLOR_DEPTH == 16

        for(int i = 0; i < len; i++) {
            uint16_t col_8bit = (*cache++ & 0xf8) << 8;
            col_8bit |= (*cache++ & 0xFC) << 3;
            col_8bit |= (*cache++ >> 3);
#if  LV_BIG_ENDIAN_SYSTEM == 1 || LV_COLOR_16_SWAP == 1
            buf[offset++] = col_8bit >> 8;
            buf[offset++] = col_8bit & 0xff;
#else
            buf[offset++] = col_8bit & 0xff;
            buf[offset++] = col_8bit >> 8;
#endif // LV_BIG_ENDIAN_SYSTEM
        }

#elif  LV_COLOR_DEPTH == 8

        for(int i = 0; i < len; i++) {
            uint8_t col_8bit = (*cache++ & 0xC0);
            col_8bit |= (*cache++ & 0xe0) >> 2;
            col_8bit |= (*cache++ & 0xe0) >> 5;
            buf[offset++] = col_8bit;
        }

#else
#error Unsupported LV_COLOR_DEPTH


#endif // LV_COLOR_DEPTH

        return LV_RES_OK;
    }
end:
    return LV_RES_INV;
}

/**
 * Free the allocated resources
 * @param decoder pointer to the decoder where this function belongs
 * @param dsc pointer to a descriptor which describes this decoding session
 */
static void decoder_close(lv_img_dec_dsc_t * dsc)
{
    /*Free all allocated data*/
    if(!dsc->dec_ctx || dsc->dec_ctx->auto_allocated == 0) {
        dsc->dec_ctx = NULL;
        return;
    }

    SJPEG * sjpeg = (SJPEG *) dsc->dec_ctx->user_data;
    if(!sjpeg) return;

    switch(dsc->input.src->type) {
        case LV_IMG_SRC_FILE:
            if(sjpeg->io.lv_file.file_d) {
                lv_fs_close(&(sjpeg->io.lv_file));
            }
            lv_sjpg_cleanup(sjpeg);
            break;

        case LV_IMG_SRC_VARIABLE:
            lv_sjpg_cleanup(sjpeg);
            break;
        default:
            break;
    }
    lv_mem_free(dsc->dec_ctx);
    dsc->dec_ctx = NULL;
}

static int is_jpg(const uint8_t * raw_data, size_t len)
{
    const uint8_t jpg_signature[] = {0xFF, 0xD8, 0xFF,  0xE0,  0x00,  0x10, 0x4A,  0x46, 0x49, 0x46};
    if(len < sizeof(jpg_signature)) return false;
    return memcmp(jpg_signature, raw_data, sizeof(jpg_signature)) == 0;
}

static void lv_sjpg_free(SJPEG * sjpeg)
{
    if(sjpeg->frame_cache) lv_mem_free(sjpeg->frame_cache);
    if(sjpeg->frame_base_array) lv_mem_free(sjpeg->frame_base_array);
    if(sjpeg->frame_base_offset) lv_mem_free(sjpeg->frame_base_offset);
    if(sjpeg->tjpeg_jd) lv_mem_free(sjpeg->tjpeg_jd);
    if(sjpeg->workb) lv_mem_free(sjpeg->workb);
}

static void lv_sjpg_cleanup(SJPEG * sjpeg)
{
    if(! sjpeg) return;

    lv_sjpg_free(sjpeg);
    lv_mem_free(sjpeg);
}

#endif /*LV_USE_SJPG*/
