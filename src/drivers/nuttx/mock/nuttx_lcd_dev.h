/**
 * @file nuttx_lcd_dev.h
 *
 */

#ifndef NUTTX_LCD_DEV_H
#define NUTTX_LCD_DEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "nuttx_video_fb.h"

/*********************
 *      DEFINES
 *********************/

#define _LCDIOC(x) (x)

#define LCDDEVIO_PUTRUN       _LCDIOC(0)  /* Arg: const struct lcddev_run_s* */
#define LCDDEVIO_PUTAREA      _LCDIOC(1)  /* Arg: const struct lcddev_area_s* */
#define LCDDEVIO_GETRUN       _LCDIOC(2)  /* Arg: struct lcddev_run_s* */
#define LCDDEVIO_GETAREA      _LCDIOC(3)  /* Arg: struct lcddev_area_s* */
#define LCDDEVIO_GETPOWER     _LCDIOC(4)  /* Arg: int* */
#define LCDDEVIO_SETPOWER     _LCDIOC(5)  /* Arg: int */
#define LCDDEVIO_GETCONTRAST  _LCDIOC(6)  /* Arg: int* */
#define LCDDEVIO_SETCONTRAST  _LCDIOC(7)  /* Arg: unsigned int */
#define LCDDEVIO_GETPLANEINFO _LCDIOC(8)  /* Arg: struct lcd_planeinfo_s* */
#define LCDDEVIO_GETVIDEOINFO _LCDIOC(9)  /* Arg: struct fb_videoinfo_s* */
#define LCDDEVIO_SETPLANENO   _LCDIOC(10) /* Arg: int */

#define LCDDEVIO_GETAREAALIGN _LCDIOC(17) /* Arg: struct lcddev_area_align_s* */

/**********************
 *      TYPEDEFS
 **********************/

struct lcddev_area_s {
    fb_coord_t row_start, row_end;
    fb_coord_t col_start, col_end;
    fb_coord_t stride;               /* row stride in bytes */
    uint8_t * data;
};

/* Some special LCD drivers require input data to be aligned.
 * Such as starting row and column, width, height, data address, etc.
 */

struct lcddev_area_align_s {
    uint16_t row_start_align; /* Start row index alignment */
    uint16_t height_align;    /* Height alignment */
    uint16_t col_start_align; /* Start column index alignment */
    uint16_t width_align;     /* Width alignment */
    uint16_t buf_align;       /* Buffer addr alignment */
};

/* This structure describes one color plane.  Some YUV formats may support
 * up to 4 planes (although they probably wouldn't be used on LCD hardware).
 * The framebuffer driver provides the video memory address in its
 * corresponding fb_planeinfo_s structure.  The LCD driver, instead, provides
 * methods to transfer data to/from the LCD color plane.
 */

struct lcd_planeinfo_s {
    /* This is working memory allocated by the LCD driver for each LCD device
    * and for each color plane.  This memory will hold one raster line of
    * data. The size of the allocated run buffer must therefore be at least
    * (bpp * xres / 8).  Actual alignment of the buffer must conform to the
    * bitwidth of the underlying pixel type.
    *
    * If there are multiple planes, they may share the same working buffer
    * because different planes will not be operate on concurrently.  However,
    * if there are multiple LCD devices, they must each have unique run
    * buffers.
    */

    uint8_t * buffer;

    /* This is the number of bits in one pixel.  This may be one of {1, 2, 4,
     * 8, 16, 24, or 32} unless support for one or more of those resolutions
     * has been disabled.
     */

    uint8_t  bpp;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*NUTTX_LCD_DEV_H*/
