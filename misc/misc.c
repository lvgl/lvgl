/**
 * @file misc.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"

#include "os/idle.h"
#include "mem/dyn_mem.h"
#include "os/ptask.h"
#include "fs/fsint.h"
#include "fs/ufs/ufs.h"
#include "fs/fat32/fat32.h"
#include "fs/linuxfs/linuxfs.h"
#include "gfx/font.h"
#include "gfx/anim.h"

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
 * Initialize the enabled misc. modules
 */
void misc_init(void)
{
    static bool inited = false;
    
    if(inited) return;

#if USE_IDLE != 0
    idle_init();    /*Init. it as soon as possible*/
#endif
    
#if USE_DYN_MEM != 0
    dm_init();
#endif    
    
#if USE_PTASK != 0
    ptask_init();
#endif    
    
#if USE_FSINT != 0  /*Init is befor other FS inits*/
    fs_init();
#endif 
    
#if USE_UFS != 0
    ufs_init();
#endif 
    
#if USE_FAT32 != 0
    fat32_init();
#endif

#if USE_LINUXFS != 0
    linuxfs_init();
#endif
    
#if USE_FONT != 0
    font_init();
#endif

#if USE_ANIM != 0
    anim_init();
#endif

    inited = true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
