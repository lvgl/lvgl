/**
 * @file lv_fs_fatfs.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_fs_if.h"

#if LV_USE_FS_IF
#if LV_FS_IF_FATFS != '\0'
#include "ff.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void fs_init(void);

static void * fs_open (lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode);
static lv_fs_res_t fs_close (lv_fs_drv_t * drv, void * file_p);
static lv_fs_res_t fs_read (lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br);
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw);
static lv_fs_res_t fs_seek (lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence);
static lv_fs_res_t fs_tell (lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p);
static void * fs_dir_open (lv_fs_drv_t * drv, const char *path);
static lv_fs_res_t fs_dir_read (lv_fs_drv_t * drv, void * dir_p, char *fn);
static lv_fs_res_t fs_dir_close (lv_fs_drv_t * drv, void * dir_p);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_fs_if_fatfs_init(void)
{
    /*----------------------------------------------------
     * Initialize your storage device and File System
     * -------------------------------------------------*/
    fs_init();

    /*---------------------------------------------------
     * Register the file system interface  in LittlevGL
     *--------------------------------------------------*/

    /* Add a simple drive to open images */
    static lv_fs_drv_t fs_drv;                         /*A driver descriptor*/
	lv_fs_drv_init(&fs_drv);

    /*Set up fields...*/
    fs_drv.letter = LV_FS_IF_FATFS;
    fs_drv.open_cb = fs_open;
    fs_drv.close_cb = fs_close;
    fs_drv.read_cb = fs_read;
    fs_drv.write_cb = fs_write;
    fs_drv.seek_cb = fs_seek;
    fs_drv.tell_cb = fs_tell;

    fs_drv.dir_close_cb = fs_dir_close;
    fs_drv.dir_open_cb = fs_dir_open;
    fs_drv.dir_read_cb = fs_dir_read;

    lv_fs_drv_register(&fs_drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/* Initialize your Storage device and File system. */
static void fs_init(void)
{
    /* Initialize the SD card and FatFS itself.
     * Better to do it in your code to keep this library utouched for easy updating*/
}

/**
 * Open a file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable
 * @param path path to the file beginning with the driver letter (e.g. S:/folder/file.txt)
 * @param mode read: FS_MODE_RD, write: FS_MODE_WR, both: FS_MODE_RD | FS_MODE_WR
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static void * fs_open (lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
    uint8_t flags = 0;

    if(mode == LV_FS_MODE_WR) flags = FA_WRITE | FA_OPEN_ALWAYS;
    else if(mode == LV_FS_MODE_RD) flags = FA_READ;
    else if(mode == (LV_FS_MODE_WR | LV_FS_MODE_RD)) flags = FA_READ | FA_WRITE | FA_OPEN_ALWAYS;

    FIL * f = lv_mem_alloc(sizeof(FIL));
    if(f == NULL) return NULL;

    FRESULT res = f_open(f, path, flags);

    if(res == FR_OK) {
    	f_lseek(f, 0);
    	return f;
    } else {
        lv_mem_free(f);
    	return NULL;
    }
}


/**
 * Close an opened file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable. (opened with lv_ufs_open)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_close (lv_fs_drv_t * drv, void * file_p)
{
    f_close(file_p);
    lv_mem_free(file_p);
    return LV_FS_RES_OK;
}

/**
 * Read data from an opened file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable.
 * @param buf pointer to a memory block where to store the read data
 * @param btr number of Bytes To Read
 * @param br the real number of read bytes (Byte Read)
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_read (lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    FRESULT res = f_read(file_p, buf, btr, (UINT*)br);
    if(res == FR_OK) return LV_FS_RES_OK;
    else return LV_FS_RES_UNKNOWN;
}

/**
 * Write into a file
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable
 * @param buf pointer to a buffer with the bytes to write
 * @param btr Bytes To Write
 * @param br the number of real written bytes (Bytes Written). NULL if unused.
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_write(lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
	FRESULT res = f_write(file_p, buf, btw, (UINT*)bw);
    if(res == FR_OK) return LV_FS_RES_OK;
    else return LV_FS_RES_UNKNOWN;
}

/**
 * Set the read write pointer. Also expand the file size if necessary.
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable. (opened with lv_ufs_open )
 * @param pos the new position of read write pointer
 * @param whence only LV_SEEK_SET is supported
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_seek (lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
    switch (whence)
    {
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
 * @param drv pointer to a driver where this function belongs
 * @param file_p pointer to a file_t variable.
 * @param pos_p pointer to to store the result
 * @return LV_FS_RES_OK: no error, the file is read
 *         any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_tell (lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
	*pos_p = f_tell(((FIL *)file_p));
    return LV_FS_RES_OK;
}

/**
 * Initialize a 'fs_read_dir_t' variable for directory reading
 * @param drv pointer to a driver where this function belongs
 * @param dir_p pointer to a 'fs_read_dir_t' variable
 * @param path path to a directory
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static void * fs_dir_open (lv_fs_drv_t * drv, const char *path)
{
    DIR * d = lv_mem_alloc(sizeof(DIR));
    if(d == NULL) return NULL;

    FRESULT res = f_opendir(d, path);
    if(res != FR_OK) {
        lv_mem_free(d);
        d = NULL;
    }
    return d;
}

/**
 * Read the next filename form a directory.
 * The name of the directories will begin with '/'
 * @param drv pointer to a driver where this function belongs
 * @param dir_p pointer to an initialized 'fs_read_dir_t' variable
 * @param fn pointer to a buffer to store the filename
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_read (lv_fs_drv_t * drv, void * dir_p, char *fn)
{
	FRESULT res;
	FILINFO fno;
	fn[0] = '\0';

    do {
    	res = f_readdir(dir_p, &fno);
    	if(res != FR_OK) return LV_FS_RES_UNKNOWN;

		if(fno.fattrib & AM_DIR) {
			fn[0] = '/';
			strcpy(&fn[1], fno.fname);
		}
		else strcpy(fn, fno.fname);

    } while(strcmp(fn, "/.") == 0 || strcmp(fn, "/..") == 0);

    return LV_FS_RES_OK;
}

/**
 * Close the directory reading
 * @param drv pointer to a driver where this function belongs
 * @param dir_p pointer to an initialized 'fs_read_dir_t' variable
 * @return LV_FS_RES_OK or any error from lv_fs_res_t enum
 */
static lv_fs_res_t fs_dir_close (lv_fs_drv_t * drv, void * dir_p)
{
	f_closedir(dir_p);
    lv_mem_free(dir_p);
    return LV_FS_RES_OK;
}

#endif	/*LV_USE_FS_IF*/
#endif  /*LV_FS_IF_FATFS*/
