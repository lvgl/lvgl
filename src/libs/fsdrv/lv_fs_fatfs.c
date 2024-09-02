/**
 * @file lv_fs_fatfs.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"

#if LV_USE_FS_FATFS
#include "ff.h"

#include "../../core/lv_global.h"
/*********************
 *      DEFINES
 *********************/

#ifdef ESP_PLATFORM
    #define DIR FF_DIR  /* ESP IDF typedefs `DIR` as `FF_DIR` in its version of ff.h. Use `FF_DIR` in LVGL too */
#endif

#if LV_FS_FATFS_LETTER == '\0'
    #error "LV_FS_FATFS_LETTER must be set to a valid value"
#else
    #if (LV_FS_FATFS_LETTER < 'A') || (LV_FS_FATFS_LETTER > 'Z')
        #if LV_FS_DEFAULT_DRIVE_LETTER != '\0' /*When using default drive letter, strict format (X:) is mandatory*/
            #error "LV_FS_FATFS_LETTER must be an upper case ASCII letter"
        #else /*Lean rules for backward compatibility*/
            #warning LV_FS_FATFS_LETTER should be an upper case ASCII letter. \
            Using a slash symbol as drive letter should be replaced with LV_FS_DEFAULT_DRIVE_LETTER mechanism
        #endif
    #endif
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void fs_init(void);

static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path);
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * dir_p, char * fn, uint32_t fn_len);
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * dir_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_fs_fatfs_init(void)
{
    /*----------------------------------------------------
     * Initialize your storage device and File System
     * -------------------------------------------------*/
    fs_init();

    /*---------------------------------------------------
     * Register the file system interface in LVGL
     *--------------------------------------------------*/

    lv_fs_drv_t * fs_drv_p = &(LV_GLOBAL_DEFAULT()->fatfs_fs_drv);
    lv_fs_drv_init(fs_drv_p);

    /*Set up fields...*/
    fs_drv_p->letter = LV_FS_FATFS_LETTER;
    fs_drv_p->cache_size = LV_FS_FATFS_CACHE_SIZE;

    fs_drv_p->open_cb = fs_open;
    fs_drv_p->close_cb = fs_close;
    fs_drv_p->read_cb = fs_read;
    fs_drv_p->write_cb = fs_write;
    fs_drv_p->seek_cb = fs_seek;
    fs_drv_p->tell_cb = fs_tell;

    fs_drv_p->dir_close_cb = fs_dir_close;
    fs_drv_p->dir_open_cb = fs_dir_open;
    fs_drv_p->dir_read_cb = fs_dir_read;

    lv_fs_drv_register(fs_drv_p);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your Storage device and File system.*/
static void fs_init(void)
{
    /*Initialize the SD card and FatFS itself.
     *Better to do it in your code to keep this library untouched for easy updating*/
}

/**
 * Open a file
 * @param drv   pointer to a driver where this function belongs
 * @param path  path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode  read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return pointer to FIL struct or NULL in case of fail
 */
static void * fs_open(lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    LV_UNUSED(drv);
    uint8_t flags = 0;

    if(mode == LV_FS_MODE_WR) flags = FA_WRITE | FA_OPEN_ALWAYS;
    else if(mode == LV_FS_MODE_RD) flags = FA_READ;
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) flags = FA_READ | FA_WRITE | FA_OPEN_ALWAYS;

    FIL * f = lv_malloc(sizeof(FIL));
    if(f == NULL) return NULL;

    FRESULT res = f_open(f, path, flags);
    if(res == FR_OK) {
        return f;
    }
    else {
        lv_free(f);
        return NULL;
    }
}

/**
 * Close an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a FIL variable. (opened with fs_open)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_close(lv_fs_drv_t * drv, void * file_p)
{
    LV_UNUSED(drv);
    f_close(file_p);
    lv_free(file_p);
    return LV_FS_RES_OK;
}

/**
 * Read data from an opened file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a FIL variable.
 * @param buf       pointer to a memory block where to store the read data
 * @param btr       number of Bytes To Read
 * @param br        the real number of read bytes (Byte Read)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_read(lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    LV_UNUSED(drv);
    FRESULT res = f_read(file_p, buf, btr, (UINT *)br);
    if(res == FR_OK) return LV_FS_RES_OK;
    else return LV_FS_RES_UNKNOWN;
}

/**
 * Write into a file
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a FIL variable
 * @param buf       pointer to a buffer with the bytes to write
 * @param btw       Bytes To Write
 * @param bw        the number of real written bytes (Bytes Written). NULL if unused.
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
    LV_UNUSED(drv);
    FRESULT res = f_write(file_p, buf, btw, (UINT *)bw);
    if(res == FR_OK) return LV_FS_RES_OK;
    else return LV_FS_RES_UNKNOWN;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a FIL variable. (opened with fs_open )
 * @param pos       the new position of read write pointer
 * @param whence    only LV_SEEK_SET is supported
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek(lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    LV_UNUSED(drv);
    switch(whence) {
        case LV_FS_SEEK_SET:
            f_lseek(file_p, pos);
            break;
        case LV_FS_SEEK_CUR:
            f_lseek(file_p, f_tell((FIL *)file_p) + pos);
            break;
        case LV_FS_SEEK_END:
            f_lseek(file_p, f_size((FIL *)file_p) + pos);
            break;
        default:
            break;
    }
    return LV_FS_RES_OK;
}

/**
 * Give the position of the read write pointer
 * @param drv       pointer to a driver where this function belongs
 * @param file_p    pointer to a FIL variable
 * @param pos_p     pointer to store the result
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell(lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
    LV_UNUSED(drv);
    *pos_p = f_tell((FIL *)file_p);
    return LV_FS_RES_OK;
}

/**
 * Initialize a 'DIR' variable for directory reading
 * @param drv   pointer to a driver where this function belongs
 * @param path  path to a directory
 * @return pointer to an initialized 'DIR' variable
 */
static void * fs_dir_open(lv_fs_drv_t * drv, const char * path)
{
    LV_UNUSED(drv);
    DIR * d = lv_malloc(sizeof(DIR));
    if(d == NULL) return NULL;

    FRESULT res = f_opendir(d, path);
    if(res != FR_OK) {
        lv_free(d);
        d = NULL;
    }
    return d;
}

/**
 * Read the next filename from a directory.
 * The name of the directories will begin with '/'
 * @param drv       pointer to a driver where this function belongs
 * @param dir_p     pointer to an initialized 'DIR' variable
 * @param fn        pointer to a buffer to store the filename
 * @param fn_len    length of the buffer to store the filename
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_read(lv_fs_drv_t * drv, void * dir_p, char * fn, uint32_t fn_len)
{
    LV_UNUSED(drv);
    if(fn_len == 0) return LV_FS_RES_INV_PARAM;

    FRESULT res;
    FILINFO fno;
    fn[0] = '\0';

    do {
        res = f_readdir(dir_p, &fno);
        if(res != FR_OK) return LV_FS_RES_UNKNOWN;

        if(fno.fname[0] == 0) break; /* End of the directory */

        if(fno.fattrib & AM_DIR) {
            lv_snprintf(fn, fn_len, "/%s", fno.fname);
        }
        else lv_strlcpy(fn, fno.fname, fn_len);

    } while(lv_strcmp(fn, "/.") == 0 || lv_strcmp(fn, "/..") == 0);

    return LV_FS_RES_OK;
}

/**
 * Close the directory reading
 * @param drv   pointer to a driver where this function belongs
 * @param dir_p pointer to an initialized 'DIR' variable
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_close(lv_fs_drv_t * drv, void * dir_p)
{
    LV_UNUSED(drv);
    f_closedir(dir_p);
    lv_free(dir_p);
    return LV_FS_RES_OK;
}

#else /*LV_USE_FS_FATFS == 0*/

#if defined(LV_FS_FATFS_LETTER) && LV_FS_FATFS_LETTER != '\0'
    #warning "LV_USE_FS_FATFS is not enabled but LV_FS_FATFS_LETTER is set"
#endif

#endif /*LV_USE_FS_FATFS*/
