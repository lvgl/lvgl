// Copyright 2020 BitBank Software, Inc. All Rights Reserved.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//    http://www.apache.org/licenses/LICENSE-2.0
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//===========================================================================

#ifndef __ANIMATEDGIF__
#define __ANIMATEDGIF__

#include "../../../../misc/lv_fs.h"
#include "../../../../lv_conf_internal.h"
#include LV_STDINT_INCLUDE
#include LV_LIMITS_INCLUDE
#include "../../../../stdlib/lv_string.h"

//
// GIF Animator
// Written by Larry Bank
// Copyright (c) 2020 BitBank Software, Inc.
// bitbank@pobox.com
//
// Designed to decode images up to 480x320 on MCUs
// using less than 22K of RAM
// ...and decode any sized image when more RAM is available
//
// ** NEW **
// Turbo mode added Feb 18, 2024. This option decodes images
// up to 30x faster if there is enough RAM (48K + full framebuffer)
//

/* GIF Defines and variables */
#define MAX_CHUNK_SIZE 255
//
// These 2 macros can be changed to limit the amount of RAM
// required by the decoder. For example, decoding 1-bit images to
// a 128x32 display will not need a max code size of 12 nor a palette
// with 256 entries
//
#define TURBO_BUFFER_SIZE 0x6100

// If you intend to decode generic GIFs, you want this value to be 12. If you are using GIFs solely for animations in
// your own project, and you control the GIFs you intend to play, then you can save additional RAM here:
// the decoder must reserve a minimum of 4 byte * (1<<MAX_CODE_SIZE) for the dictionary, but based on implementation
// actually reserves 5 byte * (1<<MAX_CODE_SIZE). Small or low colour GIFs may inherently not require a large
// dictionary. For larger GIFs, the en(!)coder can "voluntarily" choose not to utilize the entire dictionary. I.e.,
// by preparing (specially encoding) the GIFs, you can save >10kB RAM, but you will not be able to decode arbitrary
// images anymore. One application to craft such GIFs can be found here (use option -d)
// https://create.stephan-brumme.com/flexigif-lossless-gif-lzw-optimization/
#define MAX_CODE_SIZE 12

#define MAX_COLORS 256
#define MAX_WIDTH 480
#define LZW_BUF_SIZE (6*MAX_CHUNK_SIZE)
#define LZW_HIGHWATER (4*MAX_CHUNK_SIZE)
// This buffer is used to store the pixel sequence in reverse order
// it needs to be large enough to hold the longest possible
// sequence (1<<MAX_CODE_SIZE)
#define FILE_BUF_SIZE (1<<MAX_CODE_SIZE)

#define PIXEL_FIRST 0
#define PIXEL_LAST (1<<MAX_CODE_SIZE)
#define LINK_UNUSED 5911 // 0x1717 to use memset
#define LINK_END 5912
#define MAX_HASH 5003
// expanded LZW buffer for Turbo mode
#define LZW_BUF_SIZE_TURBO (LZW_BUF_SIZE + (2<<MAX_CODE_SIZE) + (PIXEL_LAST*2) + MAX_WIDTH)
#define LZW_HIGHWATER_TURBO ((LZW_BUF_SIZE_TURBO * 14) / 16)

//
// Pixel types
//
enum {
   GIF_PALETTE_RGB565_LE = 0, // little endian (default)
   GIF_PALETTE_RGB565_BE,     // big endian
   GIF_PALETTE_RGB888,        // original 24-bpp entries
   GIF_PALETTE_RGB8888,       // 32-bit (alpha = 0xff or 0x00)
   GIF_PALETTE_1BPP,          // 1-bit per pixel (horizontal, MSB on left)
   GIF_PALETTE_1BPP_OLED      // 1-bit per pixel (vertical, LSB on top)
};
// for compatibility with older code
#define LITTLE_ENDIAN_PIXELS GIF_PALETTE_RGB565_LE
#define BIG_ENDIAN_PIXELS GIF_PALETTE_RGB565_BE
//
// Draw types
//
// RAW = 8-bit palettized pixels requiring transparent pixel handling and conversion through the palette.
//       Each line is sent to the GIFDraw callback as 8-bit pixels. If a framebuffer exists, the lines will be
//       written there too. The GIFDraw callback is optional if there is a framebuffer allocated.
//
// COOKED = 16/24/32-bpp fully rendered pixels ready for display. This requires a full frame buffer with extra
//          room for the fully rendered pixels at the end of the 8-bit pixel buffer. For example, a 160x120
//          canvas size with 24-bit output would require (160*120 + 3*160) bytes.
//          Each prepared line is sent to the GIFDraw callback as a row of 16/24/32-bit pixels.
//
enum {
   GIF_DRAW_RAW = 0,
   GIF_DRAW_COOKED
};

enum {
   GIF_SUCCESS = 0,
   GIF_DECODE_ERROR,
   GIF_TOO_WIDE,
   GIF_INVALID_PARAMETER,
   GIF_UNSUPPORTED_FEATURE,
   GIF_FILE_NOT_OPEN,
   GIF_EARLY_EOF,
   GIF_EMPTY_FRAME,
   GIF_BAD_FILE,
   GIF_ERROR_MEMORY
};

typedef struct gif_file_tag
{
  int32_t iPos; // current file position
  int32_t iSize; // file size
  uint8_t *pData; // memory file pointer
  lv_fs_file_t fHandle; // class pointer to File/SdFat or whatever you want
} GIFFILE;

typedef struct gif_info_tag
{
  int32_t iFrameCount; // total frames in file
  int32_t iDuration; // duration of animation in milliseconds
  int32_t iMaxDelay; // maximum frame delay
  int32_t iMinDelay; // minimum frame delay
} GIFINFO;

typedef struct gif_draw_tag
{
    int iX, iY; // Corner offset of this frame on the canvas
    int y; // current line being drawn (0 = top line of image)
    int iWidth, iHeight; // size of this frame
    int iCanvasWidth; // need this to know where to place output in a fully cooked bitmap
    void *pUser; // user supplied pointer
    uint8_t *pPixels; // 8-bit source pixels for this line
    uint16_t *pPalette; // little or big-endian RGB565 palette entries (default)
    uint8_t *pPalette24; // RGB888 palette (optional)
    uint8_t ucTransparent; // transparent color
    uint8_t ucHasTransparency; // flag indicating the transparent color is in use
    uint8_t ucDisposalMethod; // frame disposal method
    uint8_t ucBackground; // background color
    uint8_t ucPaletteType; // type of palette entries
    uint8_t ucIsGlobalPalette; // Flag to indicate that a global palette, rather than a local palette is being used
} GIFDRAW;

// Callback function prototypes
typedef int32_t (GIF_READ_CALLBACK)(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen);
typedef int32_t (GIF_SEEK_CALLBACK)(GIFFILE *pFile, int32_t iPosition);
typedef void (GIF_DRAW_CALLBACK)(GIFDRAW *pDraw);
typedef void * (GIF_OPEN_CALLBACK)(const char *szFilename, int32_t *pFileSize);
typedef void (GIF_CLOSE_CALLBACK)(lv_fs_file_t *pHandle);
typedef void * (GIF_ALLOC_CALLBACK)(uint32_t iSize);
typedef void (GIF_FREE_CALLBACK)(void *buffer);
//
// our private structure to hold a GIF image decode state
//
typedef struct gif_image_tag
{
    uint16_t iWidth, iHeight, iCanvasWidth, iCanvasHeight;
    uint16_t iX, iY; // GIF corner offset
    uint16_t iBpp;
    int16_t iError; // last error
    uint16_t iFrameDelay; // delay in milliseconds for this frame
    int16_t iRepeatCount; // NETSCAPE animation repeat count. 0=forever
    uint16_t iXCount, iYCount; // decoding position in image (countdown values)
    int iLZWOff; // current LZW data offset
    int iLZWSize; // current quantity of data in the LZW buffer
    int iCommentPos; // file offset of start of comment data
    short sCommentLen; // length of comment
    unsigned char bEndOfFrame;
    unsigned char ucGIFBits, ucBackground, ucTransparent, ucCodeStart, ucMap, bUseLocalPalette;
    unsigned char ucPaletteType; // RGB565 or RGB888
    unsigned char ucDrawType; // RAW or COOKED
    GIF_READ_CALLBACK *pfnRead;
    GIF_SEEK_CALLBACK *pfnSeek;
    GIF_DRAW_CALLBACK *pfnDraw;
    GIF_OPEN_CALLBACK *pfnOpen;
    GIF_CLOSE_CALLBACK *pfnClose;
    GIFFILE GIFFile;
    void *pUser;
    unsigned char *pFrameBuffer;
    unsigned char *pTurboBuffer;
    unsigned char *pPixels, *pOldPixels;
    unsigned char ucFileBuf[FILE_BUF_SIZE]; // holds temp data and pixel stack
    unsigned short pPalette[(MAX_COLORS * 3)/2]; // can hold RGB565 or RGB888 - set in begin()
    unsigned short pLocalPalette[(MAX_COLORS * 3)/2]; // color palettes for GIF images
    unsigned char ucLZW[LZW_BUF_SIZE]; // holds de-chunked LZW data
    // These next 3 are used in Turbo mode to have a larger ucLZW buffer
    unsigned short usGIFTable[1<<MAX_CODE_SIZE];
    unsigned char ucGIFPixels[(PIXEL_LAST*2)];
    unsigned char ucLineBuf[MAX_WIDTH]; // current line
} GIFIMAGE;

// C interface
    int GIF_openRAM(GIFIMAGE *pGIF, uint8_t *pData, int iDataSize, GIF_DRAW_CALLBACK *pfnDraw);
    int GIF_openFile(GIFIMAGE *pGIF, const char *szFilename, GIF_DRAW_CALLBACK *pfnDraw);
    void GIF_close(GIFIMAGE *pGIF);
    void GIF_begin(GIFIMAGE *pGIF, unsigned char ucPaletteType);
    void GIF_reset(GIFIMAGE *pGIF);
    int GIF_playFrame(GIFIMAGE *pGIF, int *delayMilliseconds, void *pUser);
    int GIF_getCanvasWidth(GIFIMAGE *pGIF);
    int GIF_getCanvasHeight(GIFIMAGE *pGIF);
    int GIF_getComment(GIFIMAGE *pGIF, char *destBuffer);
    int GIF_getInfo(GIFIMAGE *pGIF, GIFINFO *pInfo);
    int GIF_getLastError(GIFIMAGE *pGIF);
    int GIF_getLoopCount(GIFIMAGE *pGIF);

#define REGISTER_WIDTH 32
#ifdef ALLOWS_UNALIGNED
    #define INTELSHORT(p) (*(uint16_t *)p)
    #define INTELLONG(p) (*(uint32_t *)p)
#else
    // Due to unaligned memory causing an exception, we have to do these macros the slow way
    #define INTELSHORT(p) ((*p) + (*(p+1)<<8))
    #define INTELLONG(p) ((*p) + (*(p+1)<<8) + (*(p+2)<<16) + (*(p+3)<<24))
#endif // ALLOWS_UNALIGNED
#define BIGINT int32_t
#define BIGUINT uint32_t

#endif // __ANIMATEDGIF__
