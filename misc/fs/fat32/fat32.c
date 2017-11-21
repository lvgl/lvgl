/**
 * @file fat32.c
 * Functions to give an API to a Fat32 storage device which is compatible 
 * with the fs_int module. 
 * It uses the FatFS as FAT32.
 * For more information about FatFS see:
 * http://elm-chan.org/fsw/ff/00index_e.html
 */

/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_FAT32 != 0

#include <string.h>
#include <stdio.h>
#include "fat32.h"
#include "ff.h"
#include "hw/dev/ext_mem/sdcard.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static fs_res_t fat32_res_trans(FRESULT fat32_res);

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
 * Create a driver for the fat32 device and initilaize it.
 */
void fat32_init(void)
{
    /*Create the driver*/
    fs_drv_t fat_drv;
    memset(&fat_drv, 0, sizeof(fs_drv_t));    /*Initialization*/
        
    fat_drv.file_size = sizeof(FIL);
    fat_drv.rddir_size = sizeof(DIR);
    fat_drv.letter = FAT32_LETTER;
    fat_drv.ready = fat32_ready;
    
    fat_drv.open = fat32_open;
    fat_drv.close = fat32_close;
#if _FS_MINIMIZE < 1
    fat_drv.remove = fat32_remove;
#else
    fat_drv.remove = NULL;
#endif
    fat_drv.read = fat32_read;
#if _FS_READONLY == 0
    fat_drv.write = fat32_write;
#else
    fat_drv.write = NULL;
#endif
#if _FS_MINIMIZE < 3
    fat_drv.seek = fat32_seek;
#else
    fat_drv.seek = NULL;
#endif
    fat_drv.tell = fat32_tell;
    fat_drv.size = fat32_size;
    fat_drv.trunc = fat32_trunc;
#if _FS_MINIMIZE < 2
    fat_drv.rddir_init = fat32_readdir_init;
    fat_drv.rddir = fat32_readdir;
    fat_drv.rddir_close = fat32_readdir_close;
#else
    fat_drv.rddir_init = NULL;
    fat_drv.rddir = NULL;
    fat_drv.rddir_close = NULL;
#endif

#if _FS_MINIMIZE == 0
    fat_drv.free = fat32_free;
#else
    fat_drv.free = NULL;
#endif
    fs_add_drv(&fat_drv);
}

/**
 * Give the state of the fat32 storage device 
 * @return true if the device is initialized and can be used else false
 */
bool fat32_ready(void)
{
    return sdcard_ready();
}

/**
 * Open a file
 * @param file_p pointer to a FIL type variable
 * @param path path of the file (e.g. "dir1/dir2/file1.txt")
 * @param mode open mode (FS_MODE_RD or FS_MODE_WR or both with | (or connection))
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t fat32_open (void * file_p, const char * path, fs_mode_t mode)
{
    uint8_t fat32_mode = 0;
    if(mode & FS_MODE_RD) fat32_mode |= FA_READ;
#if _FS_READONLY == 0
    if(mode & FS_MODE_WR) fat32_mode |= FA_WRITE | FA_OPEN_ALWAYS;
#endif
    
    /*In ready only mode the write is not implemented*/
    if(mode == 0) return FS_RES_NOT_IMP;
    
    
    FRESULT fat32_res;
    fat32_res = f_open(file_p, path, fat32_mode);
    return fat32_res_trans(fat32_res);    

}

/**
 * Close an already opened file
 * @param file_p pointer to a FIL type variable
 * @return FS_RES_OK or any error from fs_res_t
 */
fs_res_t fat32_close (void * file_p)
{
    FRESULT fat32_res;
    fat32_res = f_close(file_p);
    return fat32_res_trans(fat32_res);    
}

/**
 * Remove (delete) a file
 * @param path the path of the file ("dir1/file1.txt") 
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t fat32_remove(const char * path)
{
#if _FS_MINIMIZE < 1
    FRESULT fat32_res;
    fat32_res = f_unlink(path);
    return fat32_res_trans(fat32_res);       
#else
    return FS_RES_NOT_IMP;
#endif
}
   
/**
 * Read data from an opened file
 * @param file_p pointer to a FIL type variable
 * @param buf pointer a buffer to store the read bytes
 * @param btr the number of Bytes To Read 
 * @param br the number of real read bytes (Bytes Read)
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t fat32_read (void * file_p, void * buf, uint32_t btr, uint32_t * br)
{
    FRESULT fat32_res;
    fat32_res = f_read(file_p, buf, btr, (UINT *)br);
    return fat32_res_trans(fat32_res);   
}

/**
 * Write data to an opened file
 * @param file_p pointer to a FIL type variable
 * @param buf pointer to buffer where the data to write is located
 * @param btw the number of Bytes To Write
 * @param bw the number of real written bytes (Bytes Written)
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t fat32_write (void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{   
#if _FS_READONLY == 0
    FRESULT fat32_res;
    fat32_res = f_write(file_p, buf, btw, (UINT *)bw);
    return fat32_res_trans(fat32_res);     
#else
    return FS_RES_NOT_IMP;
#endif
}

/**
 * Position the read write pointer to given position
 * @param file_p pointer to a FIL type variable
 * @param pos the new position expressed in bytes index (0: start of file)
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t fat32_seek (void * file_p, uint32_t pos)
{
#if _FS_MINIMIZE < 3
    FRESULT fat32_res;
    fat32_res = f_lseek(file_p, pos);
    return fat32_res_trans(fat32_res);  
#else
    return FS_RES_NOT_IMP;
#endif 
}

/**
 * Give the position of the read write pointer
 * @param file_p pointer to a FIL type variable
 * @param pos_p pointer to store the position of the read write pointer
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t fat32_tell (void * file_p, uint32_t * pos_p)
{
    *pos_p = f_tell((FIL *)file_p);
    
    return FS_RES_OK;
}

/**
 * Truncate the file size to the current position of read write pointer 
 * @param file_p pointer to a FIL type variable
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t fat32_trunc (void * file_p)
{
#if _FS_MINIMIZE < 1
    FRESULT fat32_res;
    fat32_res = f_truncate(file_p);
    return fat32_res_trans(fat32_res);  
#else
    return FS_RES_NOT_IMP;
#endif  
}

/**
 * Give the size of a file
 * @param file_p pointer to a FIL type variable
 * @param size_p pointer to store the size
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t fat32_size (void * file_p, uint32_t * size_p)
{
    *size_p = f_size((FIL *)file_p);
    return FS_RES_OK;   
}


/**
 * Initialize a variable for directory reading
 * @param rddir_p pointer to a 'DIR' variable
 * @param path path to a directory
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t fat32_readdir_init(void * rddir_p, const char * path)
{   
#if _FS_MINIMIZE < 2
    FRESULT res = f_opendir(rddir_p, path);
    
    return fat32_res_trans(res);
#else
    return FS_RES_NOT_IMP;
#endif
}

/**
 * Read the next filename form a directory. 
 * The name of the directories will begin with '/'
 * @param rddir_p pointer to an initialized 'DIR' variable
 * @param fn pointer to a buffer to store the filename
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t fat32_readdir(void * rddir_p, char * fn)
{
    
#if _FS_MINIMIZE < 2
    FRESULT res;
    FILINFO fno;
    char lfn_buf[FSINT_MAX_FN_LENGTH];
    fno.lfname = lfn_buf;
    fno.lfsize = FSINT_MAX_FN_LENGTH;
    res = f_readdir(rddir_p, &fno);
    if(res == FR_OK && fno.fname[0] != '\0') {
        if (fno.fattrib & AM_DIR) {              /* It is a directory */
#if _USE_LFN != 0
            sprintf(fn, "/%s", fno.lfname[0] != '\0' ? fno.lfname : fno.fname);
#else
            sprintf(fn, "/%s", fno.fname);
#endif
        } else {                                 /* It is a file. */
#if _USE_LFN != 0
            strcpy(fn, fno.lfname[0] != '\0' ? fno.lfname : fno.fname);
#else
             strcpy(fn, fno.fname);
#endif
        }
    } else {
        fn[0] = '\0';
    }
    
    return fat32_res_trans(res);
#else
    return FS_RES_NOT_IMP;
#endif
}

/**
 * Close the directory reading
 * @param rddir_p pointer to an initialized 'DIR' variable
 * @return FS_RES_OK or any error from fs_res_t enum
 */
fs_res_t fat32_readdir_close(void * rddir_p)
{
#if _FS_MINIMIZE < 2
    f_closedir(rddir_p);
    return FS_RES_OK;
#else
    return FS_RES_NOT_IMP;
#endif
}

/**
 * Give the size of a drive
 * @param total_p pointer to store the total size [kB]
 * @param free_p pointer to store the free size [kB]
 * @return FS_RES_OK or any error from 'fs_res_t'
 */
fs_res_t fat32_free (uint32_t * total_p, uint32_t * free_p)
{
#if _FS_READONLY == 0 && _FS_MINIMIZE == 0
	FATFS *fs;
    FRESULT fat32_res;
	uint32_t fre_clust = 0;
	uint32_t fre_sect=0;
	uint32_t tot_sect=0;

	fat32_res = f_getfree("", (unsigned long*)&fre_clust, &fs);
	tot_sect=(fs->n_fatent - 2) * fs->csize;
	fre_sect=fre_clust*fs->csize;
#if _MAX_SS!=512
	tot_sect*=fs1->ssize / 512;
	fre_sect*=fs1->ssize / 512;
#endif	  
	*total_p=tot_sect >> 1;
	*free_p=fre_sect >> 1;
    return fat32_res_trans(fat32_res); 
#else
    return FS_RES_NOT_IMP;
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Translate the return codes to fs_res_t
 * @param fat32_res the original fat_32 result
 * @return the converted, fs_res_t return code
 */

static fs_res_t fat32_res_trans(FRESULT fat32_res)
{
    switch(fat32_res) {
        case FR_OK:         		/* (0) Succeeded */
            return FS_RES_OK;
                    
        case FR_DISK_ERR:   		/* (1) A hard error occurred in the low level disk I/O layer */
        case FR_NOT_READY:      	/* (3) The physical drive cannot work */
            return FS_RES_HW_ERR;
            
        case FR_INT_ERR:        	/* (2) Assertion failed */
        case FR_NOT_ENABLED:    	/* (12) The volume has no work area */
        case FR_NO_FILESYSTEM:  	/* (13) There is no valid FAT volume */
            return FS_RES_FS_ERR;
            
        case FR_NO_FILE:        	/* (4) Could not find the file */
        case FR_NO_PATH:        	/* (5) Could not find the path */
        case FR_INVALID_NAME:   	/* (6) The path name format is invalid */
        case FR_INVALID_DRIVE:  	/* (11) The logical drive number is invalid */
            return FS_RES_NOT_EX;
                    
        case FR_DENIED:     		/* (7) Access denied due to prohibited access or directory full */
        case FR_EXIST:  			/* (8) Access denied due to prohibited access */
        case FR_WRITE_PROTECTED:    /* (10) The physical drive is write protected */
        case FR_LOCKED:             /* (16) The operation is rejected according to the file sharing policy */
            return FS_RES_DENIED; 
            
        case FR_INVALID_OBJECT: 	/* (9) The file/directory object is invalid */
        case FR_INVALID_PARAMETER:	/* (19) Given parameter is invalid */
            return FS_RES_INV_PARAM;
            
        case FR_TIMEOUT:            /* (15) Could not get a grant to access the volume within defined period */
            return FS_RES_TOUT;
            
        case FR_TOO_MANY_OPEN_FILES:/* (18) Number of open files > _FS_LOCK */
        case FR_NOT_ENOUGH_CORE:	/* (17) LFN working buffer could not be allocated */
            return FS_RES_OUT_OF_MEM; 
            
        case FR_MKFS_ABORTED:       /* (14) The f_mkfs() aborted due to any parameter error */
            return FS_RES_UNKNOWN;
    
    }
    
    return FS_RES_UNKNOWN;
}

#endif
