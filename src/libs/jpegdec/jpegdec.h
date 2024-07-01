//
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
//
#ifndef __JPEGDEC__
#define __JPEGDEC__
#include "../../../lvgl.h"
#if LV_USE_JPEGDEC
// fixed to Linux for LVGL
#define __LINUX__
#if defined( __MACH__ ) || defined( __LINUX__ ) || defined( __MCUXPRESSO ) || defined( ESP_PLATFORM )
    #include "../../stdlib/lv_string.h"
    #include <stdint.h>
#else
    #include <Arduino.h>
    #if !defined(HAL_ESP32_HAL_H_) && defined(__has_include) && __has_include(<FS.h>)
        #include <FS.h>
    #endif
#endif
#ifndef PROGMEM
    #define memcpy_P memcpy
    #define PROGMEM
#endif
// Cortex-M4/M7 allow unaligned access to SRAM
#if defined(HAL_ESP32_HAL_H_) || defined(TEENSYDUINO) || defined(ARM_MATH_CM4) || defined(ARM_MATH_CM7) || defined (__x86_64__) || defined(TEENSYDUINO)
    #define ALLOWS_UNALIGNED
#endif

#if defined (__aarch64__) || defined (__arm64__)
    #define HAS_NEON
    #define ALLOWS_UNALIGNED
#endif // __aarch64
//
// JPEG Decoder
// Written by Larry Bank
// Copyright (c) 2020 BitBank Software, Inc.
//
// Designed to decode baseline JPEG images (8 or 24-bpp)
// using less than 22K of RAM
//

/* Defines and variables */
#define FILE_HIGHWATER 1536
#define JPEG_FILE_BUF_SIZE 2048
#define HUFF_TABLEN  273
#define HUFF11SIZE (1<<11)
#define DC_TABLE_SIZE 1024
#define DCTSIZE 64
#define MAX_MCU_COUNT 6
#define MAX_COMPS_IN_SCAN 4
#define MAX_BUFFERED_PIXELS 2048
#define MCU_SKIP -8

// Decoder options
#define JPEG_AUTO_ROTATE 1
#define JPEG_SCALE_HALF 2
#define JPEG_SCALE_QUARTER 4
#define JPEG_SCALE_EIGHTH 8
#define JPEG_LE_PIXELS 16
#define JPEG_EXIF_THUMBNAIL 32
#define JPEG_LUMA_ONLY 64

#define MCU0 (DCTSIZE * 0)
#define MCU1 (DCTSIZE * 1)
#define MCU2 (DCTSIZE * 2)
#define MCU3 (DCTSIZE * 3)
#define MCU4 (DCTSIZE * 4)
#define MCU5 (DCTSIZE * 5)

#if defined(__arm64__) || defined(__aarch64__) || defined (__x86_64__)
    #define REGISTER_WIDTH 64
    typedef uint64_t my_ulong;
    typedef int64_t my_long;
#else
    #define REGISTER_WIDTH 32
    typedef uint32_t my_ulong;
    typedef int32_t my_long;
#endif

// Pixel types (defaults to little endian RGB565)
enum {
    RGB565_LITTLE_ENDIAN = 0,
    RGB565_BIG_ENDIAN,
    RGB8888,
    EIGHT_BIT_GRAYSCALE,
    FOUR_BIT_DITHERED,
    TWO_BIT_DITHERED,
    ONE_BIT_DITHERED,
    INVALID_PIXEL_TYPE
};

enum {
    JPEG_MEM_RAM = 0,
    JPEG_MEM_FLASH
};

// Error codes returned by getLastError()
enum {
    JPEG_SUCCESS = 0,
    JPEG_INVALID_PARAMETER,
    JPEG_DECODE_ERROR,
    JPEG_UNSUPPORTED_FEATURE,
    JPEG_INVALID_FILE
};

typedef struct buffered_bits {
    unsigned char * pBuf; // buffer pointer
    my_ulong ulBits; // buffered bits
    uint32_t ulBitOff; // current bit offset
} BUFFERED_BITS;

typedef struct jpeg_file_tag {
    int32_t iPos; // current file position
    int32_t iSize; // file size
    uint8_t * pData; // memory file pointer
    void * fHandle; // class pointer to File/SdFat or whatever you want
} JPEGFILE;

typedef struct jpeg_draw_tag {
    int x, y; // upper left corner of current MCU
    int iWidth, iHeight; // size of this pixel block
    int iWidthUsed; // clipped size for odd/edges
    int iBpp; // bit depth of the pixels (8 or 16)
    uint16_t * pPixels; // 16-bit pixels
    void * pUser;
} JPEGDRAW;

// Callback function prototypes
typedef int32_t (JPEG_READ_CALLBACK)(JPEGFILE * pFile, uint8_t * pBuf, int32_t iLen);
typedef int32_t (JPEG_SEEK_CALLBACK)(JPEGFILE * pFile, int32_t iPosition);
typedef int (JPEG_DRAW_CALLBACK)(JPEGDRAW * pDraw);
typedef void * (JPEG_OPEN_CALLBACK)(const char * szFilename, int32_t * pFileSize);
typedef void (JPEG_CLOSE_CALLBACK)(void * pHandle);

/* JPEG color component info */
typedef struct _jpegcompinfo {
    // These values are fixed over the whole image
    // For compression, they must be supplied by the user interface
    // for decompression, they are read from the SOF marker.
    unsigned char component_needed;  /*  do we need the value of this component? */
    unsigned char component_id;     /* identifier for this component (0..255) */
    unsigned char component_index;  /* its index in SOF or cinfo->comp_info[] */
    //unsigned char h_samp_factor;    /* horizontal sampling factor (1..4) */
    //unsigned char v_samp_factor;    /* vertical sampling factor (1..4) */
    unsigned char quant_tbl_no;     /* quantization table selector (0..3) */
    // These values may vary between scans
    // For compression, they must be supplied by the user interface
    // for decompression, they are read from the SOS marker.
    unsigned char dc_tbl_no;        /* DC entropy table selector (0..3) */
    unsigned char ac_tbl_no;        /* AC entropy table selector (0..3) */
    // These values are computed during compression or decompression startup
    //int true_comp_width;  /* component's image width in samples */
    //int true_comp_height; /* component's image height in samples */
    // the above are the logical dimensions of the downsampled image
    // These values are computed before starting a scan of the component
    //int MCU_width;        /* number of blocks per MCU, horizontally */
    //int MCU_height;       /* number of blocks per MCU, vertically */
    //int MCU_blocks;       /* MCU_width * MCU_height */
    //int downsampled_width; /* image width in samples, after expansion */
    //int downsampled_height; /* image height in samples, after expansion */
    // the above are the true_comp_xxx values rounded up to multiples of
    // the MCU dimensions; these are the working dimensions of the array
    // as it is passed through the DCT or IDCT step.  NOTE: these values
    // differ depending on whether the component is interleaved or not!!
    // This flag is used only for decompression.  In cases where some of the
    // components will be ignored (eg grayscale output from YCbCr image),
    // we can skip IDCT etc. computations for the unused components.
} JPEGCOMPINFO;

//
// our private structure to hold a JPEG image decode state
//
typedef struct jpeg_image_tag {
    int iWidth, iHeight; // image size
    int iThumbWidth, iThumbHeight; // thumbnail size (if present)
    int iThumbData; // offset to image data
    int iXOffset, iYOffset; // placement on the display
    int iCropX, iCropY, iCropCX, iCropCY; // crop area
    uint8_t ucBpp, ucSubSample, ucHuffTableUsed;
    uint8_t ucMode, ucOrientation, ucHasThumb, b11Bit;
    uint8_t ucComponentsInScan, cApproxBitsLow, cApproxBitsHigh;
    uint8_t iScanStart, iScanEnd, ucFF, ucNumComponents;
    uint8_t ucACTable, ucDCTable;
    uint8_t ucMemType, ucPixelType;
    uint16_t u16MCUFlags;
    int iEXIF; // Offset to EXIF 'TIFF' file
    int iError;
    int iOptions;
    int iVLCOff; // current VLC data offset
    int iVLCSize; // current quantity of data in the VLC buffer
    int iResInterval, iResCount; // restart interval
    int iMaxMCUs; // max MCUs of pixels per JPEGDraw call
    JPEG_READ_CALLBACK * pfnRead;
    JPEG_SEEK_CALLBACK * pfnSeek;
    JPEG_DRAW_CALLBACK * pfnDraw;
    JPEG_OPEN_CALLBACK * pfnOpen;
    JPEG_CLOSE_CALLBACK * pfnClose;
    JPEGCOMPINFO JPCI[MAX_COMPS_IN_SCAN]; /* Max color components */
    JPEGFILE JPEGFile;
    BUFFERED_BITS bb;
    void * pUser;
    uint8_t * pDitherBuffer; // provided externally to do Floyd-Steinberg dithering
    uint16_t * usPixels; // needs to be 16-byte aligned for S3 SIMD
    uint16_t usUnalignedPixels[MAX_BUFFERED_PIXELS + 8];
    int16_t * sMCUs; // needs to be 16-byte aligned for S3 SIMD
    int16_t sUnalignedMCUs[8 + (DCTSIZE * MAX_MCU_COUNT)]; // 4:2:0 needs 6 DCT blocks per MCU
    void * pFramebuffer;
    int16_t sQuantTable[DCTSIZE * 4]; // quantization tables
    uint8_t ucFileBuf[JPEG_FILE_BUF_SIZE]; // holds temp data and pixel stack
    uint8_t ucHuffDC[DC_TABLE_SIZE * 2]; // up to 2 'short' tables
    uint16_t usHuffAC[HUFF11SIZE * 2];
} JPEGIMAGE;

#ifdef __cplusplus
#if defined(__has_include) && __has_include(<FS.h>)
    #include "FS.h"
#endif
#define JPEG_STATIC static
//
// The JPEGDEC class wraps portable C code which does the actual work
//
class JPEGDEC
{
    public:
        int openRAM(uint8_t * pData, int iDataSize, JPEG_DRAW_CALLBACK * pfnDraw);
        int openFLASH(uint8_t * pData, int iDataSize, JPEG_DRAW_CALLBACK * pfnDraw);
        int open(const char * szFilename, JPEG_OPEN_CALLBACK * pfnOpen, JPEG_CLOSE_CALLBACK * pfnClose,
                 JPEG_READ_CALLBACK * pfnRead, JPEG_SEEK_CALLBACK * pfnSeek, JPEG_DRAW_CALLBACK * pfnDraw);
        int open(void * fHandle, int iDataSize, JPEG_CLOSE_CALLBACK * pfnClose, JPEG_READ_CALLBACK * pfnRead,
                 JPEG_SEEK_CALLBACK * pfnSeek, JPEG_DRAW_CALLBACK * pfnDraw);
        void setFramebuffer(void * pFramebuffer);
        void setCropArea(int x, int y, int w, int h);
        void getCropArea(int * x, int * y, int * w, int * h);

#ifdef FS_H
        int open(File & file, JPEG_DRAW_CALLBACK * pfnDraw);
#endif
        void close();
        int decode(int x, int y, int iOptions);
        int decodeDither(uint8_t * pDither, int iOptions);
        int decodeDither(int x, int y, uint8_t * pDither, int iOptions);
        int getOrientation();
        int getWidth();
        int getHeight();
        int getBpp();
        void setUserPointer(void * p);
        int getSubSample();
        int hasThumb();
        int getThumbWidth();
        int getThumbHeight();
        int getLastError();
        void setPixelType(int iType); // defaults to little endian
        void setMaxOutputSize(int iMaxMCUs);

    private:
        JPEGIMAGE _jpeg;
};
#else
#define JPEG_STATIC
int JPEG_openRAM(JPEGIMAGE * pJPEG, uint8_t * pData, int iDataSize, JPEG_DRAW_CALLBACK * pfnDraw);
void JPEG_setFramebuffer(JPEGIMAGE * pJPEG, void * pFramebuffer);
void JPEG_setCropArea(JPEGIMAGE * pJPEG, int x, int y, int w, int h);
void JPEG_getCropArea(JPEGIMAGE * pJPEG, int * x, int * y, int * w, int * h);
int JPEG_getWidth(JPEGIMAGE * pJPEG);
int JPEG_getHeight(JPEGIMAGE * pJPEG);
int JPEG_decode(JPEGIMAGE * pJPEG, int x, int y, int iOptions);
int JPEG_decodeDither(JPEGIMAGE * pJPEG, uint8_t * pDither, int iOptions);
void JPEG_close(JPEGIMAGE * pJPEG);
int JPEG_getLastError(JPEGIMAGE * pJPEG);
int JPEG_getOrientation(JPEGIMAGE * pJPEG);
int JPEG_getBpp(JPEGIMAGE * pJPEG);
int JPEG_getSubSample(JPEGIMAGE * pJPEG);
int JPEG_hasThumb(JPEGIMAGE * pJPEG);
int JPEG_getThumbWidth(JPEGIMAGE * pJPEG);
int JPEG_getThumbHeight(JPEGIMAGE * pJPEG);
int JPEG_getLastError(JPEGIMAGE * pJPEG);
void JPEG_setPixelType(JPEGIMAGE * pJPEG, int iType); // defaults to little endian
void JPEG_setMaxOutputSize(JPEGIMAGE * pJPEG, int iMaxMCUs);
#endif // __cplusplus

#ifdef ALLOWS_UNALIGNED
    #define INTELSHORT(p) (*(uint16_t *)p)
    #define INTELLONG(p) (*(uint32_t *)p)
    #define MOTOSHORT(p) __builtin_bswap16(*(uint16_t *)p)
    #if REGISTER_WIDTH == 64
        #define MOTOLONG(p) __builtin_bswap64(*(uint64_t *)p)
    #else
        #define MOTOLONG(p) __builtin_bswap32(*(uint32_t *)p)
    #endif
#else
    // Due to unaligned memory causing an exception, we have to do these macros the slow way
    #define INTELSHORT(p) ((*p) + (*(p+1)<<8))
    #define INTELLONG(p) ((*p) + (*(p+1)<<8) + (*(p+2)<<16) + (*(p+3)<<24))
    #define MOTOSHORT(p) (((*(p))<<8) + (*(p+1)))
    #define MOTOLONG(p) (((*p)<<24) + ((*(p+1))<<16) + ((*(p+2))<<8) + (*(p+3)))
#endif // ALLOWS_UNALIGNED
#endif // LV_USE_JPEGDEC
#endif // __JPEGDEC__
