/**
 * @file nuttx_video_fb.h
 *
 */

#ifndef LV_NUTTX_VIDEO_FB_H
#define LV_NUTTX_VIDEO_FB_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include <stdint.h>
#include <stddef.h>

/*********************
 *      DEFINES
 *********************/

#define _FBIOC(x) (x)

/* Color format definitions.  This pretty much defines the color pixel
 * processing organization of the video controller.
 */

/* Monochrome Formats *******************************************************/

#define FB_FMT_Y1             0         /* BPP=1, monochrome */
#define FB_FMT_Y2             1         /* BPP=2, 2-bit uncompressed greyscale */
#define FB_FMT_Y4             2         /* BPP=4, 4-bit uncompressed greyscale */
#define FB_FMT_Y8             3         /* BPP=8, 8-bit uncompressed greyscale */
#define FB_FMT_Y16            4         /* BPP=16, 16-bit uncompressed greyscale */
#define FB_FMT_GREY           FB_FMT_Y8 /* BPP=8 */
#define FB_FMT_Y800           FB_FMT_Y8 /* BPP=8 */

#define FB_ISMONO(f)          (((f) >= FB_FMT_Y1) && (f) <= FB_FMT_Y16)

/* RGB video formats ********************************************************/

/* Standard RGB */

#define FB_FMT_RGB4           5           /* BPP=4 */
#define FB_FMT_RGB8           6           /* BPP=8 RGB palette index */
#define FB_FMT_RGB8_222       7           /* BPP=8  R=2, G=2, B=2 */
#define FB_FMT_RGB8_332       8           /* BPP=8  R=3, G=3, B=2 */
#define FB_FMT_RGB12_444      9           /* BPP=12 R=4, G=4, B=4 */
#define FB_FMT_RGB16_555      10          /* BPP=16 R=5, G=5, B=5 (1 unused bit) */
#define FB_FMT_RGB16_565      11          /* BPP=16 R=5, G=6, B=5 */
#define FB_FMT_RGB24          12          /* BPP=24 */
#define FB_FMT_RGB32          13          /* BPP=32 */

/* Run length encoded RGB */

#define FB_FMT_RGBRLE4        14          /* BPP=4 */
#define FB_FMT_RGBRLE8        15          /* BPP=8 */

/* Raw RGB */

#define FB_FMT_RGBRAW         16          /* BPP=? */

/* Raw RGB with arbitrary sample packing within a pixel. Packing and
 * precision of R, G and B components is determined by bit masks for each.
 */

#define FB_FMT_RGBBTFLD16     17          /* BPP=16 */
#define FB_FMT_RGBBTFLD24     18          /* BPP=24 */
#define FB_FMT_RGBBTFLD32     19          /* BPP=32 */
#define FB_FMT_RGBA16         20          /* BPP=16 Raw RGB with alpha */
#define FB_FMT_RGBA32         21          /* BPP=32 Raw RGB with alpha */

/* Raw RGB with a transparency field. Layout is as for standard RGB at 16 and
 * 32 bits per pixel but the msb in each pixel indicates whether the pixel is
 * transparent or not.
 */

#define FB_FMT_RGBT16         22          /* BPP=16 */
#define FB_FMT_RGBT32         23          /* BPP=32 */

#define FB_ISRGB(f)           (((f) >= FB_FMT_RGB4) && (f) <= FB_FMT_RGBT32)

/* Packed YUV Formats *******************************************************/

#define FB_FMT_AYUV           24          /* BPP=32  Combined YUV and alpha */
#define FB_FMT_CLJR           25          /* BPP=8   4 pixels packed into a uint32_t.
                                           *         YUV 4:1:1 with l< 8 bits
                                           *         per YUV sample */
#define FB_FMT_CYUV           26          /* BPP=16  UYVY except that height is
                                           *         reversed */
#define FB_FMT_IRAW           27          /* BPP=?   Intel uncompressed YUV */
#define FB_FMT_IUYV           28          /* BPP=16  Interlaced UYVY (line order
                                           *         0,2,4,.., 1,3,5...) */
#define FB_FMT_IY41           29          /* BPP=12  Interlaced Y41P (line order
                                           *         0,2,4,.., 1,3,5...) */
#define FB_FMT_IYU2           30          /* BPP=24 */
#define FB_FMT_HDYC           31          /* BPP=16  UYVY except uses the BT709
                                           *         color space  */
#define FB_FMT_UYVP           32          /* BPP=24? YCbCr 4:2:2, 10-bits per
                                           *         component in U0Y0V0Y1 order */
#define FB_FMT_UYVY           33          /* BPP=16  YUV 4:2:2 */
#define FB_FMT_UYNV           FB_FMT_UYVY /* BPP=16  */
#define FB_FMT_Y422           FB_FMT_UYVY /* BPP=16  */
#define FB_FMT_V210           34          /* BPP=32  10-bit 4:2:2 YCrCb */
#define FB_FMT_V422           35          /* BPP=16  Upside down version of UYVY */
#define FB_FMT_V655           36          /* BPP=16? 16-bit YUV 4:2:2 */
#define FB_FMT_VYUY           37          /* BPP=?   ATI Packed YUV Data */
#define FB_FMT_YUYV           38          /* BPP=16  YUV 4:2:2 */
#define FB_FMT_YUY2           FB_FMT_YUYV /* BPP=16  YUV 4:2:2 */
#define FB_FMT_YUNV           FB_FMT_YUYV /* BPP=16  YUV 4:2:2 */
#define FB_FMT_YVYU           39          /* BPP=16  YUV 4:2:2 */
#define FB_FMT_Y41P           40          /* BPP=12  YUV 4:1:1 */
#define FB_FMT_Y411           41          /* BPP=12  YUV 4:1:1 */
#define FB_FMT_Y211           42          /* BPP=8  */
#define FB_FMT_Y41T           43          /* BPP=12  Y41P LSB for transparency */
#define FB_FMT_Y42T           44          /* BPP=16  UYVY LSB for transparency */
#define FB_FMT_YUVP           45          /* BPP=24? YCbCr 4:2:2 Y0U0Y1V0 order */

#define FB_ISYUVPACKED(f)     (((f) >= FB_FMT_AYUV) && (f) <= FB_FMT_YUVP)

/* Packed Planar YUV Formats ************************************************/

#define FB_FMT_YVU9           46          /* BPP=9   8-bit Y followed by 8-bit
                                           *         4x4 VU */
#define FB_FMT_YUV9           47          /* BPP=9? */
#define FB_FMT_IF09           48          /* BPP=9.5 YVU9 + 4x4 plane of delta
                                           *         relative to tframe. */
#define FB_FMT_YV16           49          /* BPP=16  8-bit Y followed by 8-bit
                                           *         2x1 VU */
#define FB_FMT_YV12           50          /* BPP=12  8-bit Y followed by 8-bit
                                           *         2x2 VU */
#define FB_FMT_I420           51          /* BPP=12  8-bit Y followed by 8-bit
                                           *         2x2 UV */
#define FB_FMT_IYUV           FB_FMT_I420 /* BPP=12 */
#define FB_FMT_NV12           52          /* BPP=12  8-bit Y followed by an
                                           *         interleaved 2x2 UV */
#define FB_FMT_NV21           53          /* BPP=12  NV12 with UV reversed */
#define FB_FMT_IMC1           54          /* BPP=12  YV12 except UV planes same
                                           *         stride as Y */
#define FB_FMT_IMC2           55          /* BPP=12  IMC1 except UV lines
                                           *         interleaved at half stride
                                           *         boundaries */
#define FB_FMT_IMC3           56          /* BPP=12  As IMC1 except that UV
                                           *         swapped */
#define FB_FMT_IMC4           57          /* BPP=12  As IMC2  except that UV
                                           *         swapped */
#define FB_FMT_CLPL           58          /* BPP=12  YV12 but including a level
                                           *         of indirection. */
#define FB_FMT_Y41B           59          /* BPP=12?  4:1:1 planar. */
#define FB_FMT_Y42B           60          /* BPP=16?  YUV 4:2:2 planar. */
#define FB_FMT_CXY1           61          /* BPP=12 */
#define FB_FMT_CXY2           62          /* BPP=16 */

#define FB_ISYUVPLANAR(f)     (((f) >= FB_FMT_YVU9) && (f) <= FB_FMT_CXY2)
#define FB_ISYUV(f)           (FB_ISYUVPACKED(f) || FB_ISYUVPLANAR(f))

/* Hardware cursor control **************************************************/

#ifdef CONFIG_FB_HWCURSOR
#  define FB_CUR_ENABLE       0x01        /* Enable the cursor */
#  define FB_CUR_SETIMAGE     0x02        /* Set the cursor image */
#  define FB_CUR_SETPOSITION  0x04        /* Set the position of the cursor */
#  define FB_CUR_SETSIZE      0x08        /* Set the size of the cursor */
#  define FB_CUR_XOR          0x10        /* Use XOR vs COPY ROP on image */
#endif

/* Hardware overlay acceleration ********************************************/

#define FB_NO_OVERLAY         -1

#ifdef CONFIG_FB_OVERLAY
#  define FB_ACCL_TRANSP      0x01        /* Hardware tranparency support */
#  define FB_ACCL_CHROMA      0x02        /* Hardware chromakey support */
#  define FB_ACCL_COLOR       0x04        /* Hardware color support */
#  define FB_ACCL_AREA        0x08        /* Hardware support area selection */

#ifdef CONFIG_FB_OVERLAY_BLIT
#  define FB_ACCL_BLIT        0x10        /* Hardware blit support */
#  define FB_ACCL_BLEND       0x20        /* Hardware blend support */
#endif

/* Overlay transparency mode ************************************************/

#  define FB_CONST_ALPHA      0x00         /* Transparency by alpha value */
#  define FB_PIXEL_ALPHA      0x01         /* Transparency by pixel alpha value */

#endif /* CONFIG_FB_OVERLAY */

/* FB character driver IOCTL commands ***************************************/

/* ioctls */

#define FBIOGET_VIDEOINFO     _FBIOC(0x0001)  /* Get color plane info */
/* Argument: writable struct
 *           fb_videoinfo_s */
#define FBIOGET_PLANEINFO     _FBIOC(0x0002)  /* Get video plane info */
/* Argument: writable struct
 *           fb_planeinfo_s */

#ifdef CONFIG_FB_CMAP
#  define FBIOGET_CMAP        _FBIOC(0x0003)  /* Get RGB color mapping */
/* Argument: writable struct
 *           fb_cmap_s */
#  define FBIOPUT_CMAP        _FBIOC(0x0004)  /* Put RGB color mapping */
/* Argument: read-only struct
 *           fb_cmap_s */
#endif

#ifdef CONFIG_FB_HWCURSOR
#  define FBIOGET_CURSOR      _FBIOC(0x0005)  /* Get cursor attributes */
/* Argument: writable struct
 *           fb_cursorattrib_s */
#  define FBIOPUT_CURSOR      _FBIOC(0x0006)  /* Set cursor attributes */
/* Argument: read-only struct
 *           fb_setcursor_s */
#endif

#ifdef CONFIG_FB_UPDATE
#  define FBIO_UPDATE         _FBIOC(0x0007)  /* Update a rectangular region in
                                               * the framebuffer
                                               * Argument: read-only struct
                                               *           fb_area_s */
#endif

#ifdef CONFIG_FB_SYNC
#  define FBIO_WAITFORVSYNC   _FBIOC(0x0008)  /* Wait for vertical sync */
#endif

#ifdef CONFIG_FB_OVERLAY
#  define FBIOGET_OVERLAYINFO _FBIOC(0x0009)  /* Get video overlay info */
/* Argument: writable struct
 *           fb_overlayinfo_s */
#  define FBIO_SELECT_OVERLAY _FBIOC(0x000a)  /* Select overlay */
/* Argument: read-only
 *           unsigned long */
#  define FBIOSET_TRANSP      _FBIOC(0x000b)  /* Set opacity or transparency
                                               * Argument: read-only struct
                                               *           fb_overlayinfo_s */
#  define FBIOSET_CHROMAKEY   _FBIOC(0x000c)  /* Set chroma key
                                               * Argument: read-only struct
                                               *           fb_overlayinfo_s */
#  define FBIOSET_COLOR       _FBIOC(0x000d)  /* Set color
                                               * Argument: read-only struct
                                               *           fb_overlayinfo_s */
#  define FBIOSET_BLANK       _FBIOC(0x000e)  /* Blank or unblank
                                               * Argument: read-only struct
                                               *           fb_overlayinfo_s */
#  define FBIOSET_AREA        _FBIOC(0x000f)  /* Set active overlay area
                                               * Argument: read-only struct
                                               *           fb_overlayinfo_s */
#  define FBIOSET_DESTAREA    _FBIOC(0x0010)  /* Set destination area on
                                               * primary FB.
                                               * Argument: read-only struct
                                               *           fb_overlayinfo_s */

#ifdef CONFIG_FB_OVERLAY_BLIT
#  define FBIOSET_BLIT        _FBIOC(0x0011)  /* Blit area between overlays
                                               * Argument: read-only struct
                                               *           fb_overlayblit_s */
#  define FBIOSET_BLEND       _FBIOC(0x0012)  /* Blend area between overlays
                                               * Argument: read-only struct
                                               *           fb_overlayblend_s */
#endif

#define FBIOPAN_OVERLAY       _FBIOC(0x0013)  /* Pan display for overlay
                                               * Argument: read-only struct
                                               *           fb_overlayinfo_s */

#endif /* CONFIG_FB_OVERLAY */

/* Specific Controls ********************************************************/

#define FBIOSET_POWER         _FBIOC(0x0014)  /* Set panel power
                                               * Argument:             int */
#define FBIOGET_POWER         _FBIOC(0x0015)  /* Get panel current power
                                               * Argument:            int* */
#define FBIOSET_FRAMERATE     _FBIOC(0x0016)  /* Set frame rate
                                               * Argument:             int */
#define FBIOGET_FRAMERATE     _FBIOC(0x0017)  /* Get frame rate
                                               * Argument:            int* */

#define FBIOPAN_DISPLAY       _FBIOC(0x0018)  /* Pan display
                                               * Argument: read-only struct
                                               *           fb_planeinfo_s* */

#define FBIOPAN_CLEAR         _FBIOC(0x0019)  /* Pan clear */
/* Argument: read-only
 *           unsigned long */

#define FBIOSET_VSYNCOFFSET   _FBIOC(0x001a)  /* Set VSync offset in usec
                                               * Argument:             int */

/* Linux Support ************************************************************/

#define FBIOGET_VSCREENINFO   _FBIOC(0x001b)  /* Get video variable info */
/* Argument: writable struct
 *           fb_var_screeninfo */
#define FBIOGET_FSCREENINFO   _FBIOC(0x001c)  /* Get video fix info */
/* Argument: writable struct
 *           fb_fix_screeninfo */

/**********************
 *      TYPEDEFS
 **********************/

typedef uint16_t fb_coord_t;

struct fb_videoinfo_s {
    uint8_t    fmt;               /* see FB_FMT_*  */
    fb_coord_t xres;              /* Horizontal resolution in pixel columns */
    fb_coord_t yres;              /* Vertical resolution in pixel rows */
    uint8_t    nplanes;           /* Number of color planes supported */
    uint8_t    noverlays;         /* Number of overlays supported */
    uint8_t    moduleinfo[128];   /* Module information filled by vendor */
};

struct fb_planeinfo_s {
    void * fbmem;            /* Start of frame buffer memory */
    size_t     fblen;        /* Length of frame buffer memory in bytes */
    fb_coord_t stride;       /* Length of a line in bytes */
    uint8_t    display;      /* Display number */
    uint8_t    bpp;          /* Bits per pixel */
    uint32_t   xres_virtual; /* Virtual Horizontal resolution in pixel columns */
    uint32_t   yres_virtual; /* Virtual Vertical resolution in pixel rows */
    uint32_t   xoffset;      /* Offset from virtual to visible resolution */
    uint32_t   yoffset;      /* Offset from virtual to visible resolution */
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

#endif /*LV_NUTTX_VIDEO_FB_H*/
