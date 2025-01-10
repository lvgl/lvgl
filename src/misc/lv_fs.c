/**
 * @file lv_fs.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_fs_private.h"

#include "../misc/lv_assert.h"
#include "../misc/lv_profiler.h"
#include "../stdlib/lv_string.h"
#include "lv_ll.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/

#if LV_FS_DEFAULT_DRIVER_LETTER != '\0' && (LV_FS_DEFAULT_DRIVER_LETTER < 'A' || 'Z' < LV_FS_DEFAULT_DRIVER_LETTER)
    #error "When enabled, LV_FS_DEFAULT_DRIVER_LETTER needs to be a capital ASCII letter (A-Z)"
#endif

#define fsdrv_ll_p &(LV_GLOBAL_DEFAULT()->fsdrv_ll)

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    char driver_letter;
    const char * real_path;
} resolved_path_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static resolved_path_t lv_fs_resolve_path(const char * path);
static lv_fs_res_t lv_fs_read_cached(lv_fs_file_t * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t lv_fs_write_cached(lv_fs_file_t * file_p, const void * buf, uint32_t btw, uint32_t * bw);
static lv_fs_res_t lv_fs_seek_cached(lv_fs_file_t * file_p, uint32_t pos, lv_fs_whence_t whence);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_fs_init(void)
{
    lv_ll_init(fsdrv_ll_p, sizeof(lv_fs_drv_t *));
}

void lv_fs_deinit(void)
{
    lv_ll_clear(fsdrv_ll_p);
}

bool lv_fs_is_ready(char letter)
{
    lv_fs_drv_t * drv = lv_fs_get_drv(letter);

    if(drv == NULL) return false; /*An unknown driver in not ready*/

    if(drv->ready_cb == NULL) return true; /*Assume the driver is always ready if no handler provided*/

    return drv->ready_cb(drv);
}

lv_fs_res_t lv_fs_open(lv_fs_file_t * file_p, const char * path, lv_fs_mode_t mode)
{
    if(path == NULL) {
        LV_LOG_WARN("Can't open file: path is NULL");
        return LV_FS_RES_INV_PARAM;
    }

    resolved_path_t resolved_path = lv_fs_resolve_path(path);

    lv_fs_drv_t * drv = lv_fs_get_drv(resolved_path.driver_letter);

    if(drv == NULL) {
        LV_LOG_WARN("Can't open file (%s): unknown driver letter", path);
        return LV_FS_RES_NOT_EX;
    }

    if(drv->ready_cb) {
        if(drv->ready_cb(drv) == false) {
            LV_LOG_WARN("Can't open file (%s): driver not ready", path);
            return LV_FS_RES_HW_ERR;
        }
    }

    if(drv->open_cb == NULL) {
        LV_LOG_WARN("Can't open file (%s): open function not exists", path);
        return LV_FS_RES_NOT_IMP;
    }

    LV_PROFILER_FS_BEGIN;

    file_p->drv = drv;

    /* For memory-mapped files we set the file handle to our file descriptor so that we can access the cache from the file operations */
    if(drv->cache_size == LV_FS_CACHE_FROM_BUFFER) {
        file_p->file_d = file_p;
    }
    else {
        void * file_d = drv->open_cb(drv, resolved_path.real_path, mode);
        if(file_d == NULL || file_d == (void *)(-1)) {
            LV_PROFILER_FS_END;
            return LV_FS_RES_UNKNOWN;
        }
        file_p->file_d = file_d;
    }

    if(drv->cache_size) {
        file_p->cache = lv_malloc_zeroed(sizeof(lv_fs_file_cache_t));
        LV_ASSERT_MALLOC(file_p->cache);

        /* If this is a memory-mapped file, then set "cache" to the memory buffer */
        if(drv->cache_size == LV_FS_CACHE_FROM_BUFFER) {
            lv_fs_path_ex_t * path_ex = (lv_fs_path_ex_t *)path;
            file_p->cache->buffer = (void *)path_ex->buffer;
            file_p->cache->start = 0;
            file_p->cache->file_position = 0;
            file_p->cache->end = path_ex->size;
        }
        /*Set an invalid range by default*/
        else {
            file_p->cache->start = UINT32_MAX;
            file_p->cache->end = UINT32_MAX - 1;
        }
    }

    LV_PROFILER_FS_END;

    return LV_FS_RES_OK;
}

void lv_fs_make_path_from_buffer(lv_fs_path_ex_t * path, char letter, const void * buf, uint32_t size)
{
    path->path[0] = letter;
    path->path[1] = ':';
    path->path[2] = 0;
    path->buffer = buf;
    path->size = size;
}

lv_fs_res_t lv_fs_close(lv_fs_file_t * file_p)
{
    if(file_p->drv == NULL) {
        return LV_FS_RES_INV_PARAM;
    }

    if(file_p->drv->close_cb == NULL) {
        return LV_FS_RES_NOT_IMP;
    }

    LV_PROFILER_FS_BEGIN;

    lv_fs_res_t res = file_p->drv->close_cb(file_p->drv, file_p->file_d);

    if(file_p->drv->cache_size && file_p->cache) {
        /* Only free cache if it was pre-allocated (for memory-mapped files it is never allocated) */
        if(file_p->drv->cache_size != LV_FS_CACHE_FROM_BUFFER && file_p->cache->buffer) {
            lv_free(file_p->cache->buffer);
        }

        lv_free(file_p->cache);
    }

    file_p->file_d = NULL;
    file_p->drv    = NULL;
    file_p->cache  = NULL;

    LV_PROFILER_FS_END;

    return res;
}

lv_fs_res_t lv_fs_read(lv_fs_file_t * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    if(br != NULL) *br = 0;
    if(file_p->drv == NULL) return LV_FS_RES_INV_PARAM;

    if(file_p->drv->cache_size) {
        if(file_p->drv->read_cb == NULL || file_p->drv->seek_cb == NULL) return LV_FS_RES_NOT_IMP;
    }
    else {
        if(file_p->drv->read_cb == NULL) return LV_FS_RES_NOT_IMP;
    }

    LV_PROFILER_FS_BEGIN;

    uint32_t br_tmp = 0;
    lv_fs_res_t res;

    if(file_p->drv->cache_size) {
        res = lv_fs_read_cached(file_p, buf, btr, &br_tmp);
    }
    else {
        res = file_p->drv->read_cb(file_p->drv, file_p->file_d, buf, btr, &br_tmp);
    }

    if(br != NULL) *br = br_tmp;

    LV_PROFILER_FS_END;

    return res;
}

lv_fs_res_t lv_fs_write(lv_fs_file_t * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    if(bw != NULL) *bw = 0;

    if(file_p->drv == NULL) {
        return LV_FS_RES_INV_PARAM;
    }

    if(file_p->drv->cache_size) {
        if(file_p->drv->write_cb == NULL || file_p->drv->seek_cb == NULL) return LV_FS_RES_NOT_IMP;
    }
    else {
        if(file_p->drv->write_cb == NULL) return LV_FS_RES_NOT_IMP;
    }

    LV_PROFILER_FS_BEGIN;

    lv_fs_res_t res;
    uint32_t bw_tmp = 0;
    if(file_p->drv->cache_size) {
        res = lv_fs_write_cached(file_p, buf, btw, &bw_tmp);
    }
    else {
        res = file_p->drv->write_cb(file_p->drv, file_p->file_d, buf, btw, &bw_tmp);
    }
    if(bw != NULL) *bw = bw_tmp;

    LV_PROFILER_FS_END;
    return res;
}

lv_fs_res_t lv_fs_seek(lv_fs_file_t * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    if(file_p->drv == NULL) {
        return LV_FS_RES_INV_PARAM;
    }

    if(file_p->drv->cache_size) {
        if(file_p->drv->seek_cb == NULL || file_p->drv->tell_cb == NULL) return LV_FS_RES_NOT_IMP;
    }
    else {
        if(file_p->drv->seek_cb == NULL) return LV_FS_RES_NOT_IMP;
    }

    LV_PROFILER_FS_BEGIN;

    lv_fs_res_t res;
    if(file_p->drv->cache_size) {
        res = lv_fs_seek_cached(file_p, pos, whence);
    }
    else {
        res = file_p->drv->seek_cb(file_p->drv, file_p->file_d, pos, whence);
    }

    LV_PROFILER_FS_END;

    return res;
}

lv_fs_res_t lv_fs_tell(lv_fs_file_t * file_p, uint32_t * pos)
{
    if(file_p->drv == NULL) {
        *pos = 0;
        return LV_FS_RES_INV_PARAM;
    }

    if(file_p->drv->cache_size == 0 && file_p->drv->tell_cb == NULL) {
        *pos = 0;
        return LV_FS_RES_NOT_IMP;
    }

    LV_PROFILER_FS_BEGIN;

    lv_fs_res_t res;
    if(file_p->drv->cache_size) {
        *pos = file_p->cache->file_position;
        res = LV_FS_RES_OK;
    }
    else {
        res = file_p->drv->tell_cb(file_p->drv, file_p->file_d, pos);
    }

    LV_PROFILER_FS_END;

    return res;
}

lv_fs_res_t lv_fs_dir_open(lv_fs_dir_t * rddir_p, const char * path)
{
    if(path == NULL) return LV_FS_RES_INV_PARAM;

    resolved_path_t resolved_path = lv_fs_resolve_path(path);

    lv_fs_drv_t * drv = lv_fs_get_drv(resolved_path.driver_letter);

    if(drv == NULL) {
        return LV_FS_RES_NOT_EX;
    }

    if(drv->ready_cb) {
        if(drv->ready_cb(drv) == false) {
            return LV_FS_RES_HW_ERR;
        }
    }

    if(drv->dir_open_cb == NULL) {
        return LV_FS_RES_NOT_IMP;
    }

    LV_PROFILER_FS_BEGIN;

    void * dir_d = drv->dir_open_cb(drv, resolved_path.real_path);

    if(dir_d == NULL || dir_d == (void *)(-1)) {
        LV_PROFILER_FS_END;
        return LV_FS_RES_UNKNOWN;
    }

    rddir_p->drv = drv;
    rddir_p->dir_d = dir_d;

    LV_PROFILER_FS_END;

    return LV_FS_RES_OK;
}

lv_fs_res_t lv_fs_dir_read(lv_fs_dir_t * rddir_p, char * fn, uint32_t fn_len)
{
    if(fn_len == 0) {
        return LV_FS_RES_INV_PARAM;
    }

    if(rddir_p->drv == NULL || rddir_p->dir_d == NULL) {
        fn[0] = '\0';
        return LV_FS_RES_INV_PARAM;
    }

    if(rddir_p->drv->dir_read_cb == NULL) {
        fn[0] = '\0';
        return LV_FS_RES_NOT_IMP;
    }

    LV_PROFILER_FS_BEGIN;

    lv_fs_res_t res = rddir_p->drv->dir_read_cb(rddir_p->drv, rddir_p->dir_d, fn, fn_len);

    LV_PROFILER_FS_END;

    return res;
}

lv_fs_res_t lv_fs_dir_close(lv_fs_dir_t * rddir_p)
{
    if(rddir_p->drv == NULL || rddir_p->dir_d == NULL) {
        return LV_FS_RES_INV_PARAM;
    }

    if(rddir_p->drv->dir_close_cb == NULL) {
        return LV_FS_RES_NOT_IMP;
    }

    LV_PROFILER_FS_BEGIN;

    lv_fs_res_t res = rddir_p->drv->dir_close_cb(rddir_p->drv, rddir_p->dir_d);

    rddir_p->dir_d = NULL;
    rddir_p->drv   = NULL;

    LV_PROFILER_FS_END;

    return res;
}

void lv_fs_drv_init(lv_fs_drv_t * drv)
{
    lv_memzero(drv, sizeof(lv_fs_drv_t));
}

void lv_fs_drv_register(lv_fs_drv_t * drv_p)
{
    /*Save the new driver*/
    lv_fs_drv_t ** new_drv;
    new_drv = lv_ll_ins_head(fsdrv_ll_p);
    LV_ASSERT_MALLOC(new_drv);
    if(new_drv == NULL) return;

    *new_drv = drv_p;
}

lv_fs_drv_t * lv_fs_get_drv(char letter)
{
    lv_fs_drv_t ** drv;

    LV_LL_READ(fsdrv_ll_p, drv) {
        if((*drv)->letter == letter) {
            return *drv;
        }
    }

    return NULL;
}

char * lv_fs_get_letters(char * buf)
{
    lv_fs_drv_t ** drv;
    uint8_t i = 0;

    LV_LL_READ(fsdrv_ll_p, drv) {
        buf[i] = (*drv)->letter;
        i++;
    }

    buf[i] = '\0';

    return buf;
}

const char * lv_fs_get_ext(const char * fn)
{
    size_t i;
    for(i = lv_strlen(fn); i > 0; i--) {
        if(fn[i] == '.') {
            return &fn[i + 1];
        }
        else if(fn[i] == '/' || fn[i] == '\\') {
            return ""; /*No extension if a '\' or '/' found*/
        }
    }

    return ""; /*Empty string if no '.' in the file name.*/
}

char * lv_fs_up(char * path)
{
    size_t len = lv_strlen(path);
    if(len == 0) return path;

    len--; /*Go before the trailing '\0'*/

    /*Ignore trailing '/' or '\'*/
    while(path[len] == '/' || path[len] == '\\') {
        path[len] = '\0';
        if(len > 0)
            len--;
        else
            return path;
    }

    size_t i;
    for(i = len; i > 0; i--) {
        if(path[i] == '/' || path[i] == '\\') break;
    }

    if(i > 0) path[i] = '\0';

    return path;
}

const char * lv_fs_get_last(const char * path)
{
    size_t len = lv_strlen(path);
    if(len == 0) return path;

    len--; /*Go before the trailing '\0'*/

    /*Ignore trailing '/' or '\'*/
    while(path[len] == '/' || path[len] == '\\') {
        if(len > 0)
            len--;
        else
            return path;
    }

    size_t i;
    for(i = len; i > 0; i--) {
        if(path[i] == '/' || path[i] == '\\') break;
    }

    /*No '/' or '\' in the path so return with path itself*/
    if(i == 0) return path;

    return &path[i + 1];
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Extract the drive letter and the real path from LVGL's "abstracted file system" path string
 * @param path path string (E.g. S:/folder/file.txt)
 */
static resolved_path_t lv_fs_resolve_path(const char * path)
{
    resolved_path_t resolved;

#if LV_FS_DEFAULT_DRIVER_LETTER != '\0' /*When using default drive letter, strict format (X:) is mandatory*/
    bool has_drive_prefix = ('A' <= path[0]) && (path[0] <= 'Z') && (path[1] == ':');

    if(has_drive_prefix) {
        resolved.driver_letter = path[0];
        resolved.real_path = path + 2;
    }
    else {
        resolved.driver_letter = LV_FS_DEFAULT_DRIVER_LETTER;
        resolved.real_path = path;
    }
# else /*Lean rules for backward compatibility*/
    resolved.driver_letter = path[0];

    if(*path != '\0') {
        path++; /*Ignore the driver letter*/
        if(*path == ':') path++;
    }

    resolved.real_path = path;
#endif

    return resolved;
}

static lv_fs_res_t lv_fs_read_cached(lv_fs_file_t * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    lv_fs_res_t res = LV_FS_RES_OK;
    uint32_t file_position = file_p->cache->file_position;
    uint32_t start = file_p->cache->start;
    uint32_t end = file_p->cache->end;
    char * buffer = file_p->cache->buffer;
    uint32_t buffer_size = file_p->drv->cache_size;

    if(start <= file_position && file_position <= end) {
        /* Data can be read from cache buffer */
        uint32_t buffer_remaining_length = (uint32_t)end - file_position + 1;
        uint32_t buffer_offset = (end - start) - buffer_remaining_length + 1;

        /* Do not allow reading beyond the actual memory block for memory-mapped files */
        if(file_p->drv->cache_size == LV_FS_CACHE_FROM_BUFFER) {
            if(btr > buffer_remaining_length)
                btr = buffer_remaining_length - 1;
        }

        if(btr <= buffer_remaining_length) {
            /*Data is in cache buffer, and buffer end not reached, no need to read from FS*/
            lv_memcpy(buf, buffer + buffer_offset, btr);
            *br = btr;
        }
        else {
            /*First part of data is in cache buffer, but we need to read rest of data from FS*/
            lv_memcpy(buf, buffer + buffer_offset, buffer_remaining_length);

            file_p->drv->seek_cb(file_p->drv, file_p->file_d, file_p->cache->end + 1,
                                 LV_FS_SEEK_SET);

            uint32_t bytes_read_to_buffer = 0;
            if(btr - buffer_remaining_length > buffer_size) {
                /*If remaining data chuck is bigger than buffer size, then do not use cache, instead read it directly from FS*/
                res = file_p->drv->read_cb(file_p->drv, file_p->file_d, (char *)buf + buffer_remaining_length,
                                           btr - buffer_remaining_length, &bytes_read_to_buffer);
            }
            else {
                /*If remaining data chunk is smaller than buffer size, then read into cache buffer*/
                res = file_p->drv->read_cb(file_p->drv, file_p->file_d, buffer, buffer_size, &bytes_read_to_buffer);
                file_p->cache->start = file_p->cache->end + 1;
                file_p->cache->end = file_p->cache->start + bytes_read_to_buffer - 1;

                uint16_t data_chunk_remaining = LV_MIN(btr - buffer_remaining_length, bytes_read_to_buffer);
                lv_memcpy((char *)buf + buffer_remaining_length, buffer, data_chunk_remaining);
            }
            *br = LV_MIN(buffer_remaining_length + bytes_read_to_buffer, btr);
        }
    }
    else {
        file_p->drv->seek_cb(file_p->drv, file_p->file_d, file_p->cache->file_position,
                             LV_FS_SEEK_SET);

        /*Data is not in cache buffer*/
        if(btr > buffer_size) {
            /*If bigger data is requested, then do not use cache, instead read it directly*/
            res = file_p->drv->read_cb(file_p->drv, file_p->file_d, (void *)buf, btr, br);
        }
        else {
            /*If small data is requested, then read from FS into cache buffer*/
            if(buffer == NULL) {
                file_p->cache->buffer = lv_malloc(buffer_size);
                LV_ASSERT_MALLOC(file_p->cache->buffer);
                buffer = file_p->cache->buffer;
            }

            uint32_t bytes_read_to_buffer = 0;
            res = file_p->drv->read_cb(file_p->drv, file_p->file_d, (void *)buffer, buffer_size, &bytes_read_to_buffer);
            file_p->cache->start = file_position;
            file_p->cache->end = file_p->cache->start + bytes_read_to_buffer - 1;

            *br = LV_MIN(btr, bytes_read_to_buffer);
            lv_memcpy(buf, buffer, *br);

        }
    }

    if(res == LV_FS_RES_OK) {
        file_p->cache->file_position += *br;
    }

    return res;
}

static lv_fs_res_t lv_fs_write_cached(lv_fs_file_t * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    lv_fs_res_t res = LV_FS_RES_OK;

    /*Need to do FS seek before writing data to FS*/
    res = file_p->drv->seek_cb(file_p->drv, file_p->file_d, file_p->cache->file_position, LV_FS_SEEK_SET);
    if(res != LV_FS_RES_OK) return res;

    res = file_p->drv->write_cb(file_p->drv, file_p->file_d, buf, btw, bw);
    if(res != LV_FS_RES_OK) return res;

    if(file_p->cache->end >= file_p->cache->start) {
        uint32_t start_position = file_p->cache->file_position;
        uint32_t end_position = file_p->cache->file_position + *bw - 1;
        char * cache_buffer = file_p->cache->buffer;
        const char * write_buffer = buf;

        if(start_position <= file_p->cache->start && end_position >= file_p->cache->end) {
            lv_memcpy(cache_buffer,
                      write_buffer + (file_p->cache->start - start_position),
                      file_p->cache->end + 1 - file_p->cache->start);
        }
        else if(start_position >= file_p->cache->start && end_position <= file_p->cache->end) {
            lv_memcpy(cache_buffer + (start_position - file_p->cache->start),
                      write_buffer,
                      end_position + 1 - start_position);
        }
        else if(end_position >= file_p->cache->start && end_position <= file_p->cache->end) {
            lv_memcpy(cache_buffer,
                      write_buffer + (file_p->cache->start - start_position),
                      end_position + 1 - file_p->cache->start);
        }
        else if(start_position >= file_p->cache->start && start_position <= file_p->cache->end) {
            lv_memcpy(cache_buffer + (start_position - file_p->cache->start),
                      write_buffer,
                      file_p->cache->end + 1 - start_position);
        }
    }

    file_p->cache->file_position += *bw;

    return res;
}

static lv_fs_res_t lv_fs_seek_cached(lv_fs_file_t * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    lv_fs_res_t res = LV_FS_RES_OK;
    switch(whence) {
        case LV_FS_SEEK_SET: {
                file_p->cache->file_position = pos;
                break;
            }
        case LV_FS_SEEK_CUR: {
                file_p->cache->file_position += pos;
                break;
            }
        case LV_FS_SEEK_END: {
                /*Because we don't know the file size, we do a little trick: do a FS seek, then get the new file position from FS*/
                res = file_p->drv->seek_cb(file_p->drv, file_p->file_d, pos, whence);
                if(res == LV_FS_RES_OK) {
                    uint32_t tmp_position;
                    res = file_p->drv->tell_cb(file_p->drv, file_p->file_d, &tmp_position);

                    if(res == LV_FS_RES_OK) {
                        file_p->cache->file_position = tmp_position;
                    }
                }
                break;
            }
    }

    return res;
}
