/**
 * @file misc_conf.h
 * 
 */

#if 0 /*Remove this to enable the content*/

#ifndef MISC_CONF_H
#define MISC_CONF_H

/*====================
 * TICK INTERFACE
 *===================*/
/*Some modules (e.g. ptask) uses sys, tick*/
#define MISC_SYSTICK_INCLUDE    "lvgl/lv_hal/lv_hal_tick.h"
#define MISC_SYSTICK_GET        lv_tick_get
#define MISC_SYSTICK_ELAPS      lv_tick_elaps
#define MISC_SYSTICK_ADD_CB     lv_tick_add_cb
#define MISC_SYSTICK_REM_CB     lv_tick_rem_cb

/*=========================================
 *  Logging (add external log functions)
 *=========================================*/
#define USE_MISC_LOG    0       /*Planned*/
#if USE_MISC_LOG  !=  0
#define MISC_LOG_INCLUDE    "hw/dev/ui/log.h"   /*Include of log headers*/
#define MISC_LOG_MSG        SMSG                /*Log simple messages. (printf-like variable paremter list)*/
#define MISC_LOG_WARN       SWARN               /*Log warning messages. (printf-like variable paremter list)*/
#define MISC_LOG_ERR        SERR                /*Log error messages. (printf-like variable paremter list)*/
#endif

/*====================
 * Memory management
 *===================*/

/*----------------
 * Dynamic memory
 *----------------*/
#define USE_DYN_MEM     1
#if USE_DYN_MEM != 0
#define DM_AUTO_ZERO   0     /*Automatically fill-zero the allocated memory*/
#define DM_CUSTOM      0     /*1: use custom malloc/free, 0: use malloc/free provided by dyn_mem*/
#if DM_CUSTOM == 0
  #define DM_MEM_SIZE    (32U * 1024U) /*Size memory used by mem_alloc (in bytes)*/
  #define DM_MEM_ATTR                  /*Complier prefix for big array declaration*/
#else /*DM_CUSTOM != 0: Provide custom malloc/free functions*/
  #define DM_CUST_INCLUDE <stdlib.h>   /*Header for the dynamic memory function*/
  #define DM_CUST_ALLOC   malloc       /*Wrapper to malloc*/
  #define DM_CUST_FREE    free         /*Wrapper to free*/
#endif  /*DM_CUSTOM*/
#endif  /*USE_DYN_MEM*/

 /*--------------------------------------
  * Dynamic memory with always 0 % fragmentation
  * Not compatible with normal malloc/free
  *------------------------------------*/
 #define USE_DYN_MEM_DEFR     0
 #if USE_DYN_MEM_DEFR != 0
 #define DMD_MEM_SIZE    (32U * 1024U) /*Size memory used by mem_alloc (in bytes)*/
 #define DMD_AUTO_ZERO   1             /*Automatically fill-zero the allocated memory*/
 #define DMD_MEM_ATTR
 #endif  /*USE_DYN_MEM_DEFR*/

/*------------
 *   FIFO
 *-----------*/
#define USE_FIFO        0
#if USE_FIFO != 0
/* No settings*/
#endif /*USE_FIFO*/

/*----------------
 *   Linked list
 *----------------*/
#define USE_LINKED_LIST     1
#if USE_LINKED_LIST != 0
/* No settings*/
#endif /*USE_LINKED_LIST*/

/*====================
 * Operation system
 *===================*/

/*----------------
 * Periodic task
 *----------------*/
#define USE_PTASK       1
#if USE_PTASK != 0
#define PTASK_IDLE_PERIOD  500  /*500*/
#endif /*USE_PTASK*/

/*-----------------------
 * CPU Idle measurement
 *-----------------------*/
/*In hal/systick.c: MISC_SYSTICK_ADD_CB and MISC_SYSTICK_REM_CB is required)*/
#define USE_IDLE        0
#if USE_IDLE != 0
#define IDLE_REFR_RATE  500 /*ms*/
#endif /*USE_IDLE*/

/*===================
 *   File system
 *==================*/

/*----------------ü
 *  FS interface
 *----------------*/
#define USE_FSINT   1
#if USE_FSINT != 0
/*No settings*/
#endif  /*USE_FSINT*/

/*----------------
 *     uFS
 *----------------*/
#define USE_UFS   1
#if USE_UFS != 0
#define UFS_LETTER 'U'
#endif  /*USE_UFS*/

/*------------------------
 * FAT32 - FatFS wrappers
 * (used on MCU)
 *-----------------------*/
#define USE_FAT32   0
#if USE_FAT32 != 0
#define FAT32_LETTER 'S'/
#endif  /*USE_FAT32*/

/*---------------------
 * Linux File system
 *---------------------*/
#define USE_LINUXFS   0
#if USE_LINUXFS != 0
#define LINUXFS_LETTER 'L'
#define LINUXFS_ROOT_DIR    "./" /*See this directory as root folder*/
#endif  /*USE_LINUXFS*/

/*================
 *     Math
 *================*/

/*-----------------
 *  Trigonometry
 *----------------*/
#define USE_TRIGO   1
#if USE_TRIGO != 0
/*No settings*/
#endif

/*-----------------
 *  Math basic
 *----------------*/
#define USE_MATH_BASE   1
#if USE_MATH_BASE != 0
/*No settings*/
#endif


/*===================
 *  GRAPHICS (GFX)
 *==================*/

/*----------------
 *     Color
 *----------------*/
#define  USE_COLOR      1
#if USE_COLOR != 0
#define COLOR_DEPTH     24
#endif

/*----------------
 *     Area
 *----------------*/
#define USE_AREA    1
#if USE_AREA != 0
/*No settings*/
#endif

/*----------------
 *     Circle
 *----------------*/
#define USE_CIRC    1
#if USE_CIRC != 0
/*No settings*/
#endif


/*----------------
 *     Font
 *----------------*/
#define USE_FONT    1
#if USE_FONT != 0
#define FONT_ANTIALIAS                  0   /*1: filter all fonts to get half sized, but anti-aliased letters */

/*Built-in font usage*/
#define USE_FONT_DEJAVU_10              0
#define USE_FONT_DEJAVU_10_SUP          0
#define USE_FONT_DEJAVU_10_LATIN_EXT_A  0
#define USE_FONT_DEJAVU_10_LATIN_EXT_B  0
#define USE_FONT_DEJAVU_10_CYRILLIC     0
#define USE_FONT_SYMBOL_10_BASIC        0
#define USE_FONT_SYMBOL_10_FILE         0
#define USE_FONT_SYMBOL_10_FEEDBACK     0

#define USE_FONT_DEJAVU_20              0
#define USE_FONT_DEJAVU_20_SUP          0
#define USE_FONT_DEJAVU_20_LATIN_EXT_A  0
#define USE_FONT_DEJAVU_20_LATIN_EXT_B  0
#define USE_FONT_DEJAVU_20_CYRILLIC     0
#define USE_FONT_SYMBOL_20_BASIC        0
#define USE_FONT_SYMBOL_20_FILE         0
#define USE_FONT_SYMBOL_20_FEEDBACK     0

#define USE_FONT_DEJAVU_30              0
#define USE_FONT_DEJAVU_30_SUP          0
#define USE_FONT_DEJAVU_30_LATIN_EXT_A  0
#define USE_FONT_DEJAVU_30_LATIN_EXT_B  0
#define USE_FONT_DEJAVU_30_CYRILLIC     0
#define USE_FONT_SYMBOL_30_BASIC        0
#define USE_FONT_SYMBOL_30_FILE         0
#define USE_FONT_SYMBOL_30_FEEDBACK     0

#define USE_FONT_DEJAVU_40              1
#define USE_FONT_DEJAVU_40_SUP          0
#define USE_FONT_DEJAVU_40_LATIN_EXT_A  0
#define USE_FONT_DEJAVU_40_LATIN_EXT_B  0
#define USE_FONT_DEJAVU_40_CYRILLIC     0
#define USE_FONT_SYMBOL_40_BASIC        1
#define USE_FONT_SYMBOL_40_FILE         1
#define USE_FONT_SYMBOL_40_FEEDBACK     1

#define USE_FONT_DEJAVU_60              0
#define USE_FONT_DEJAVU_60_SUP          0
#define USE_FONT_DEJAVU_60_LATIN_EXT_A  0
#define USE_FONT_DEJAVU_60_LATIN_EXT_B  0
#define USE_FONT_DEJAVU_60_CYRILLIC     0
#define USE_FONT_SYMBOL_60_BASIC        0
#define USE_FONT_SYMBOL_60_FILE         0
#define USE_FONT_SYMBOL_60_FEEDBACK     0

#define USE_FONT_DEJAVU_80              0
#define USE_FONT_DEJAVU_80_SUP          0
#define USE_FONT_DEJAVU_80_LATIN_EXT_A  0
#define USE_FONT_DEJAVU_80_LATIN_EXT_B  0
#define USE_FONT_DEJAVU_80_CYRILLIC     0
#define USE_FONT_SYMBOL_80_BASIC        0
#define USE_FONT_SYMBOL_80_FILE         0
#define USE_FONT_SYMBOL_80_FEEDBACK     0

#define USE_FONT_DEJAVU_100              0
#define USE_FONT_DEJAVU_100_SUP          0
#define USE_FONT_DEJAVU_100_LATIN_EXT_A  0
#define USE_FONT_DEJAVU_100_LATIN_EXT_B  0
#define USE_FONT_DEJAVU_100_CYRILLIC     0
#define USE_FONT_SYMBOL_100_BASIC        0
#define USE_FONT_SYMBOL_100_FILE         0
#define USE_FONT_SYMBOL_100_FEEDBACK     0

#define USE_FONT_DEJAVU_120              0
#define USE_FONT_DEJAVU_120_SUP          0
#define USE_FONT_DEJAVU_120_LATIN_EXT_A  0
#define USE_FONT_DEJAVU_120_LATIN_EXT_B  0
#define USE_FONT_DEJAVU_120_CYRILLIC     0
#define USE_FONT_SYMBOL_120_BASIC        0
#define USE_FONT_SYMBOL_120_FILE         0
#define USE_FONT_SYMBOL_120_FEEDBACK     0

#endif /*USE_FONT*/

/*----------------
 *     Text
 *----------------*/
#define USE_TEXT    1
#if USE_TEXT != 0
#define TXT_BREAK_CHARS     " ,.;:-_"    /*Can break texts on these chars*/
#define TXT_UTF8            0            /*Enable UTF-8 support*/
#endif /*USE_TEXT*/

/*----------------
 *     Animation
 *----------------*/
#define USE_ANIM    1
#if USE_ANIM != 0
#define ANIM_REFR_PERIOD 50         /*ms*/
#endif

/*===================
 *     Others
 *==================*/

/*------------
 *    Slip
 *-----------*/
#define USE_SLIP        0
#if USE_SLIP != 0
#define SLIP_ESC        0x0C
#define SLIP_END        0xDB
#define SLIP_ESC_END    0xDC  /*Change END char in the stream to: ESC, ESC_END*/
#define SLIP_ESC_ESC    0xDD  /*Change ESC char in the stream to: ESC, ESC_ESC*/
#endif  /*USE_SLIP*/

/*----------------
 * String command
 *--------------*/
#define USE_STRCMD     0
#if USE_STRCMD != 0
/*No settings*/
#endif /*USE_STRCMD*/

#endif /*Remove this to enable the content*/
