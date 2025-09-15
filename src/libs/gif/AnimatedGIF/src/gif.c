//
// GIF Animator
// written by Larry Bank
// bitbank@pobox.com
// Arduino port started 7/5/2020
// Original GIF code written 20+ years ago :)
// The goal of this code is to decode images up to 480x320
// using no more than 22K of RAM (if sent directly to an LCD display)
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
#include "AnimatedGIF.h"

static const unsigned char cGIFBits[9] = {1,4,4,4,8,8,8,8,8}; // convert odd bpp values to ones we can handle

// forward references
static int GIFInit(GIFIMAGE *pGIF);
static int GIFParseInfo(GIFIMAGE *pPage, int bInfoOnly);
static int GIFGetMoreData(GIFIMAGE *pPage);
static void GIFMakePels(GIFIMAGE *pPage, unsigned int code);
static int DecodeLZW(GIFIMAGE *pImage, int iOptions);
static int DecodeLZWTurbo(GIFIMAGE *pImage, int iOptions);
static int32_t readMem(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen);
static int32_t seekMem(GIFFILE *pFile, int32_t iPosition);
int GIF_getInfo(GIFIMAGE *pPage, GIFINFO *pInfo);
static int32_t readFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen);
static int32_t seekFile(GIFFILE *pFile, int32_t iPosition);
static void closeFile(lv_fs_file_t *handle);

// C API
int GIF_openRAM(GIFIMAGE *pGIF, uint8_t *pData, int iDataSize, GIF_DRAW_CALLBACK *pfnDraw)
{
    pGIF->iError = GIF_SUCCESS;
    pGIF->pfnRead = readMem;
    pGIF->pfnSeek = seekMem;
    pGIF->pfnDraw = pfnDraw;
    pGIF->pfnOpen = NULL;
    pGIF->pfnClose = NULL;
    pGIF->GIFFile.iSize = iDataSize;
    pGIF->GIFFile.pData = pData;
    return GIFInit(pGIF);
} /* GIF_openRAM() */

int GIF_openFile(GIFIMAGE *pGIF, const char *szFilename, GIF_DRAW_CALLBACK *pfnDraw)
{
    pGIF->iError = GIF_SUCCESS;
    pGIF->pfnRead = readFile;
    pGIF->pfnSeek = seekFile;
    pGIF->pfnDraw = pfnDraw;
    pGIF->pfnOpen = NULL;
    pGIF->pfnClose = closeFile;
    if (LV_FS_RES_OK != lv_fs_open(&pGIF->GIFFile.fHandle, szFilename, LV_FS_MODE_RD))
       return 0;
    lv_fs_seek(&pGIF->GIFFile.fHandle, 0, LV_FS_SEEK_END);
    uint32_t pos;
    lv_fs_tell(&pGIF->GIFFile.fHandle, &pos);
    pGIF->GIFFile.iSize = pos;
    lv_fs_seek(&pGIF->GIFFile.fHandle, 0, LV_FS_SEEK_SET);
    return GIFInit(pGIF);
} /* GIF_openFile() */

void GIF_close(GIFIMAGE *pGIF)
{
    if (pGIF->pfnClose)
        (*pGIF->pfnClose)(&pGIF->GIFFile.fHandle);
} /* GIF_close() */

void GIF_begin(GIFIMAGE *pGIF, unsigned char ucPaletteType)
{
    lv_memset(pGIF, 0, sizeof(GIFIMAGE));
    pGIF->ucPaletteType = ucPaletteType;
} /* GIF_begin() */

void GIF_reset(GIFIMAGE *pGIF)
{
    (*pGIF->pfnSeek)(&pGIF->GIFFile, 0);
} /* GIF_reset() */

//
// Return value:
// 1 = good decode, more frames exist
// 0 = good decode, no more frames
// -1 = error
//
int GIF_playFrame(GIFIMAGE *pGIF, int *delayMilliseconds, void *pUser)
{
int rc;

    if (delayMilliseconds)
       *delayMilliseconds = 0; // clear any old valid
    if (pGIF->GIFFile.iPos >= pGIF->GIFFile.iSize-1) // no more data exists
    {
        (*pGIF->pfnSeek)(&pGIF->GIFFile, 0); // seek to start
    }
    if (GIFParseInfo(pGIF, 0))
    {
        pGIF->pUser = pUser;
        if (pGIF->iError == GIF_EMPTY_FRAME) // don't try to decode it
            return 0;
        if (pGIF->pTurboBuffer) { // the presence of the Turbo buffer indicates Turbo mode
            rc = DecodeLZWTurbo(pGIF, 0);
        } else {
            rc = DecodeLZW(pGIF, 0);
        }
        if (rc != 0) // problem
            return 0;
    }
    else
    {
        return 0; // error parsing the frame info, we may be at the end of the file
    }
    // Return 1 for more frames or 0 if this was the last frame
    if (delayMilliseconds) // if not NULL, return the frame delay time
        *delayMilliseconds = pGIF->iFrameDelay;
    return (pGIF->GIFFile.iPos < pGIF->GIFFile.iSize-1);
} /* GIF_playFrame() */

int GIF_getCanvasWidth(GIFIMAGE *pGIF)
{
    return pGIF->iCanvasWidth;
} /* GIF_getCanvasWidth() */

int GIF_getCanvasHeight(GIFIMAGE *pGIF)
{
    return pGIF->iCanvasHeight;
} /* GIF_getCanvasHeight() */

int GIF_getLoopCount(GIFIMAGE *pGIF)
{
    return pGIF->iRepeatCount;
} /* GIF_getLoopCount() */

int GIF_getComment(GIFIMAGE *pGIF, char *pDest)
{
int32_t iOldPos;

    iOldPos = pGIF->GIFFile.iPos; // keep old position
    (*pGIF->pfnSeek)(&pGIF->GIFFile, pGIF->iCommentPos);
    (*pGIF->pfnRead)(&pGIF->GIFFile, (uint8_t *)pDest, pGIF->sCommentLen);
    (*pGIF->pfnSeek)(&pGIF->GIFFile, iOldPos);
    pDest[pGIF->sCommentLen] = 0; // zero terminate the string
    return (int)pGIF->sCommentLen;

} /* GIF_getComment() */

int GIF_getLastError(GIFIMAGE *pGIF)
{
    return pGIF->iError;
} /* GIF_getLastError() */

//
// Helper functions for memory based images
//
static int32_t readMem(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
    int32_t iBytesRead;

    iBytesRead = iLen;
    if ((pFile->iSize - pFile->iPos) < iLen)
       iBytesRead = pFile->iSize - pFile->iPos;
    if (iBytesRead <= 0)
       return 0;
    lv_memmove(pBuf, &pFile->pData[pFile->iPos], iBytesRead);
    pFile->iPos += iBytesRead;
    return iBytesRead;
} /* readMem() */

static int32_t seekMem(GIFFILE *pFile, int32_t iPosition)
{
    if (iPosition < 0) iPosition = 0;
    else if (iPosition >= pFile->iSize) iPosition = pFile->iSize-1;
    pFile->iPos = iPosition;
    return iPosition;
} /* seekMem() */

static void closeFile(lv_fs_file_t *handle)
{
    lv_fs_close(handle);
} /* closeFile() */

static int32_t seekFile(GIFFILE *pFile, int32_t iPosition)
{
    if (iPosition < 0) iPosition = 0;
    else if (iPosition >= pFile->iSize) iPosition = pFile->iSize-1;
    pFile->iPos = iPosition;
    lv_fs_seek(&pFile->fHandle, iPosition, LV_FS_SEEK_SET);
    return iPosition;
} /* seekMem() */

static int32_t readFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
    int32_t iBytesRead;

    iBytesRead = iLen;
    if ((pFile->iSize - pFile->iPos) < iLen)
       iBytesRead = pFile->iSize - pFile->iPos;
    if (iBytesRead <= 0)
       return 0;
    uint32_t br;
    lv_fs_read(&pFile->fHandle, pBuf, iBytesRead, &br);
    iBytesRead = br;
    pFile->iPos += iBytesRead;
    return iBytesRead;
} /* readFile() */

//
// The following functions are written in plain C and have no
// 3rd party dependencies, not even the C runtime library
//
//
// Initialize a GIF file and callback access from a file on SD or memory
// returns 1 for success, 0 for failure
// Fills in the canvas size of the GIFIMAGE structure
//
static int GIFInit(GIFIMAGE *pGIF)
{
    pGIF->GIFFile.iPos = 0; // start at beginning of file
    if (!GIFParseInfo(pGIF, 1)) // gather info for the first frame
       return 0; // something went wrong; not a GIF file?
    (*pGIF->pfnSeek)(&pGIF->GIFFile, 0); // seek back to start of the file
    if (pGIF->iCanvasWidth > MAX_WIDTH || pGIF->iCanvasHeight > 32767) { // too big or corrupt
        pGIF->iError = GIF_TOO_WIDE;
        return 0;
    }
  return 1;
} /* GIFInit() */

//
// Parse the GIF header, gather the size and palette info
// If called with bInfoOnly set to true, it will test for a valid file
// and return the canvas size only
// Returns 1 for success, 0 for failure
//
static int GIFParseInfo(GIFIMAGE *pPage, int bInfoOnly)
{
    int i, j, iColorTableBits;
    int iBytesRead;
    unsigned char c, *p;
    int32_t iOffset = 0;
    int32_t iStartPos = pPage->GIFFile.iPos; // starting file position
    int iReadSize;

    pPage->bUseLocalPalette = 0; // assume no local palette
    pPage->bEndOfFrame = 0; // we're just getting started
    pPage->iFrameDelay = 0; // may not have a gfx extension block
    pPage->iRepeatCount = -1; // assume NETSCAPE loop count is not specified
    iReadSize = MAX_CHUNK_SIZE;
    // If you try to read past the EOF, the SD lib will return garbage data
    if (iStartPos + iReadSize > pPage->GIFFile.iSize)
       iReadSize = (pPage->GIFFile.iSize - iStartPos - 1);
    p = pPage->ucFileBuf;
    iBytesRead =  (*pPage->pfnRead)(&pPage->GIFFile, pPage->ucFileBuf, iReadSize); // 255 is plenty for now

    if (iBytesRead != iReadSize) // we're at the end of the file
    {
       pPage->iError = GIF_EARLY_EOF;
       return 0;
    }
    if (iStartPos == 0) // start of the file
    { // canvas size
        if (lv_memcmp(p, "GIF89", 5) != 0 && lv_memcmp(p, "GIF87", 5) != 0) // not a GIF file
        {
           pPage->iError = GIF_BAD_FILE;
           return 0;
        }
        pPage->iCanvasWidth = pPage->iWidth = INTELSHORT(&p[6]);
        pPage->iCanvasHeight = pPage->iHeight = INTELSHORT(&p[8]);
        pPage->iBpp = ((p[10] & 0x70) >> 4) + 1;
        iColorTableBits = (p[10] & 7) + 1; // Log2(size) of the color table
        pPage->ucBackground = p[11]; // background color
        pPage->ucGIFBits = 0;
        iOffset = 13;
        if (p[10] & 0x80) // global color table?
        { // by default, convert to byte-reversed RGB565 for immediate use
            // Read enough additional data for the color table
            iBytesRead += (*pPage->pfnRead)(&pPage->GIFFile, &pPage->ucFileBuf[iBytesRead], 3*(1<<iColorTableBits));
            if (pPage->ucPaletteType == GIF_PALETTE_RGB565_LE || pPage->ucPaletteType == GIF_PALETTE_RGB565_BE) {
                for (i=0; i<(1<<iColorTableBits); i++) {
                    uint16_t usRGB565;
                    usRGB565 = ((p[iOffset] >> 3) << 11); // R
                    usRGB565 |= ((p[iOffset+1] >> 2) << 5); // G
                    usRGB565 |= (p[iOffset+2] >> 3); // B
                    if (pPage->ucPaletteType == GIF_PALETTE_RGB565_LE)
                        pPage->pPalette[i] = usRGB565;
                    else
                        pPage->pPalette[i] = (usRGB565 << 8) | (usRGB565 >> 8); // SPI wants MSB first
                    iOffset += 3;
                }
            } else if (pPage->ucPaletteType == GIF_PALETTE_1BPP || pPage->ucPaletteType == GIF_PALETTE_1BPP_OLED) {
                uint8_t *pPal1 = (uint8_t*)pPage->pPalette;
                for (i=0; i<(1<<iColorTableBits); i++) {
                    uint16_t usGray;
                    usGray = p[iOffset]; // R
                    usGray += p[iOffset+1]*2; // G is twice as important
                    usGray += p[iOffset+2]; // B
                    pPal1[i] = (usGray >= 512); // bright enough = 1
                    iOffset += 3;
                }
            } else { // just copy it as-is (RGB888 & RGB8888 output)
                lv_memcpy(pPage->pPalette, &p[iOffset], (1<<iColorTableBits) * 3);
                iOffset += (1 << iColorTableBits) * 3;
            }
        }
    }
    while (p[iOffset] != ',' && p[iOffset] != ';') /* Wait for image separator */
    {
        if (p[iOffset] == '!') /* Extension block */
        {
            iOffset++;
            switch(p[iOffset++]) /* Block type */
            {
                case 0xf9: /* Graphic extension */
                    if (p[iOffset] == 4) // correct length
                    {
                        pPage->ucGIFBits = p[iOffset+1]; // packed fields
                        pPage->iFrameDelay = (INTELSHORT(&p[iOffset+2]))*10; // delay in ms
                        if (pPage->iFrameDelay <= 1) // 0-1 is going to make it run at 60fps; use 100 (10fps) as a reasonable substitute
                           pPage->iFrameDelay = 100;
                        if (pPage->ucGIFBits & 1) // transparent color is used
                            pPage->ucTransparent = p[iOffset+4]; // transparent color index
                        iOffset += 6;
                    }
                    //                     else   // error
                    break;
                case 0xff: /* App extension */
                    c = 1;
                    while (c) /* Skip all data sub-blocks */
                    {
                        c = p[iOffset++]; /* Block length */
                        if ((iBytesRead - iOffset) < (c+32)) // need to read more data first
                        {
                            lv_memmove(pPage->ucFileBuf, &pPage->ucFileBuf[iOffset], (iBytesRead-iOffset)); // move existing data down
                            iBytesRead -= iOffset;
                            iStartPos += iOffset;
                            iOffset = 0;
                            iBytesRead += (*pPage->pfnRead)(&pPage->GIFFile, &pPage->ucFileBuf[iBytesRead], c+32);
                        }
                        if (c == 11) // fixed block length
                        { // Netscape app block contains the repeat count
                            if (lv_memcmp(&p[iOffset], "NETSCAPE2.0", 11) == 0)
                            {
                                if (p[iOffset+11] == 3 && p[iOffset+12] == 1) // loop count
                                    pPage->iRepeatCount = INTELSHORT(&p[iOffset+13]);
                            }
                        }
                        iOffset += (int)c; /* Skip to next sub-block */
                    }
                    break;
                case 0x01: /* Text extension */
                    c = 1;
                    j = 0;
                    while (c) /* Skip all data sub-blocks */
                    {
                        c = p[iOffset++]; /* Block length */
                        if (j == 0) // use only first block
                        {
                            j = c;
                            if (j > 127)   // max comment length = 127
                                j = 127;
                            //                           memcpy(pPage->szInfo1, &p[iOffset], j);
                            //                           pPage->szInfo1[j] = '\0';
                            j = 1;
                        }
                        iOffset += (int)c; /* Skip this sub-block */
                    }
                    break;
                case 0xfe: /* Comment */
                    c = 1;
                    while (c) /* Skip all data sub-blocks */
                    {
                        c = p[iOffset++]; /* Block length */
                        if ((iBytesRead - iOffset) < (c+32)) // need to read more data first
                        {
                            lv_memmove(pPage->ucFileBuf, &pPage->ucFileBuf[iOffset], (iBytesRead-iOffset)); // move existing data down
                            iBytesRead -= iOffset;
                            iStartPos += iOffset;
                            iOffset = 0;
                            iBytesRead += (*pPage->pfnRead)(&pPage->GIFFile, &pPage->ucFileBuf[iBytesRead], c+32);
                        }
                        if (pPage->iCommentPos == 0) // Save first block info
                        {
                            pPage->iCommentPos = iStartPos + iOffset;
                            pPage->sCommentLen = c;
                        }
                        iOffset += (int)c; /* Skip this sub-block */
                    }
                    break;
                default:
                    /* Bad header info */
                    pPage->iError = GIF_DECODE_ERROR;
                    return 0;
            } /* switch */
        }
        else // invalid byte, stop decoding
        {
            if (pPage->GIFFile.iSize - iStartPos < 32) // non-image bytes at end of file?
                pPage->iError = GIF_EMPTY_FRAME;
            else
                /* Bad header info */
                pPage->iError = GIF_DECODE_ERROR;
            return 0;
        }
    } /* while */
    if (bInfoOnly)
       return 1; // we've got the info we needed, leave
    if (p[iOffset] == ';') { // end of file, quit and return a correct error code
        pPage->iError = GIF_EMPTY_FRAME;
        return 1;
    }

    if (p[iOffset] == ',')
        iOffset++;
    // This particular frame's size and position on the main frame (if animated)
    pPage->iX = INTELSHORT(&p[iOffset]);
    pPage->iY = INTELSHORT(&p[iOffset+2]);
    pPage->iWidth = INTELSHORT(&p[iOffset+4]);
    pPage->iHeight = INTELSHORT(&p[iOffset+6]);
    if (pPage->iWidth > pPage->iCanvasWidth || pPage->iHeight > pPage->iCanvasHeight ||
        pPage->iWidth + pPage->iX > pPage->iCanvasWidth || pPage->iHeight + pPage->iY > pPage->iCanvasHeight) {
        pPage->iError = GIF_DECODE_ERROR; // must be a corrupt file to encounter this error here
        return 0;
    }
    iOffset += 8;

    /* Image descriptor
     7 6 5 4 3 2 1 0    M=0 - use global color map, ignore pixel
     M I 0 0 0 pixel    M=1 - local color map follows, use pixel
     I=0 - Image in sequential order
     I=1 - Image in interlaced order
     pixel+1 = # bits per pixel for this image
     */
    pPage->ucMap = p[iOffset++];
    if (pPage->ucMap & 0x80) // local color table?
    {// by default, convert to byte-reversed RGB565 for immediate use
        j = (1<<((pPage->ucMap & 7)+1));
        // Read enough additional data for the color table
        iBytesRead += (*pPage->pfnRead)(&pPage->GIFFile, &pPage->ucFileBuf[iBytesRead], j*3);
        if (pPage->ucPaletteType == GIF_PALETTE_RGB565_LE || pPage->ucPaletteType == GIF_PALETTE_RGB565_BE)
        {
            for (i=0; i<j; i++)
            {
                uint16_t usRGB565;
                usRGB565 = ((p[iOffset] >> 3) << 11); // R
                usRGB565 |= ((p[iOffset+1] >> 2) << 5); // G
                usRGB565 |= (p[iOffset+2] >> 3); // B
                if (pPage->ucPaletteType == GIF_PALETTE_RGB565_LE)
                    pPage->pLocalPalette[i] = usRGB565;
                else
                    pPage->pLocalPalette[i] = (usRGB565 << 8) | (usRGB565 >> 8); // SPI wants MSB first
                iOffset += 3;
            }
        } else if (pPage->ucPaletteType == GIF_PALETTE_1BPP || pPage->ucPaletteType == GIF_PALETTE_1BPP_OLED) {
            uint8_t *pPal1 = (uint8_t*)pPage->pLocalPalette;
            for (i=0; i<j; i++) {
                uint16_t usGray;
                usGray = p[iOffset]; // R
                usGray += p[iOffset+1]*2; // G is twice as important
                usGray += p[iOffset+2]; // B
                pPal1[i] = (usGray >= 512); // bright enough = 1
                iOffset += 3;
            }
        } else { // just copy it as-is
            lv_memcpy(pPage->pLocalPalette, &p[iOffset], j * 3);
            iOffset += j*3;
        }
        pPage->bUseLocalPalette = 1;
    }
    pPage->ucCodeStart = p[iOffset++]; /* initial code size */
    /* Since GIF can be 1-8 bpp, we only allow 1,4,8 */
    pPage->iBpp = cGIFBits[pPage->ucCodeStart];
    // we are re-using the same buffer turning GIF file data
    // into "pure" LZW
   pPage->iLZWSize = 0; // we're starting with no LZW data yet
   c = 1; // get chunk length
   while (c && iOffset < iBytesRead)
   {
//     Serial.printf("iOffset=%d, iBytesRead=%d\n", iOffset, iBytesRead);
     c = p[iOffset++]; // get chunk length
//     Serial.printf("Chunk size = %d\n", c);
     if (c <= (iBytesRead - iOffset))
     {
       lv_memcpy(&pPage->ucLZW[pPage->iLZWSize], &p[iOffset], c);
       pPage->iLZWSize += c;
       iOffset += c;
     }
     else // partial chunk in our buffer
     {
       int iPartialLen = (iBytesRead - iOffset);
       lv_memcpy(&pPage->ucLZW[pPage->iLZWSize], &p[iOffset], iPartialLen);
       pPage->iLZWSize += iPartialLen;
       iOffset += iPartialLen;
       (*pPage->pfnRead)(&pPage->GIFFile, &pPage->ucLZW[pPage->iLZWSize], c - iPartialLen);
       pPage->iLZWSize += (c - iPartialLen);
     }
     if (c == 0)
        pPage->bEndOfFrame = 1; // signal not to read beyond the end of the frame
   }
// seeking on an SD card is VERY VERY SLOW, so use the data we've already read by de-chunking it
// in this case, there's too much data, so we have to seek backwards a bit
   if (iOffset < iBytesRead)
   {
//     Serial.printf("Need to seek back %d bytes\n", iBytesRead - iOffset);
     (*pPage->pfnSeek)(&pPage->GIFFile, iStartPos + iOffset); // position file to new spot
   }
    return 1; // we are now at the start of the chunk data
} /* GIFParseInfo() */
//
// Gather info about an animated GIF file
//
int GIF_getInfo(GIFIMAGE *pPage, GIFINFO *pInfo)
{
    int iOff, iNumFrames;
    int iDelay, iMaxDelay, iMinDelay, iTotalDelay;
    int iReadAmount;
    int iDataAvailable = 0;
    int iDataRemaining = 0;
 //   uint32_t lFileOff = 0;
    int bDone = 0;
    int bExt;
    uint8_t c, *cBuf;

    iMaxDelay = iTotalDelay = 0;
    iMinDelay = 10000;
    iNumFrames = 1;
    iDataRemaining = pPage->GIFFile.iSize;
    cBuf = (uint8_t *) pPage->ucFileBuf;
    (*pPage->pfnSeek)(&pPage->GIFFile, 0);
    iDataAvailable = (*pPage->pfnRead)(&pPage->GIFFile, cBuf, FILE_BUF_SIZE);
    iDataRemaining -= iDataAvailable;
   // lFileOff += iDataAvailable;
    iOff = 10;
    c = cBuf[iOff]; // get info bits
    iOff += 3;   /* Skip flags, background color & aspect ratio */
    if (c & 0x80) /* Deal with global color table */
    {
        c &= 7;  /* Get the number of colors defined */
        iOff += (2<<c)*3; /* skip color table */
    }
    while (!bDone) // && iNumFrames < MAX_FRAMES)
    {
        bExt = 1; /* skip extension blocks */
        while (bExt && iOff < iDataAvailable)
        {
            if ((iDataAvailable - iOff) < 258) // need to read more data first
            {
                lv_memmove(cBuf, &cBuf[iOff], (iDataAvailable-iOff)); // move existing data down
                iDataAvailable -= iOff;
                iOff = 0;
                iReadAmount = (*pPage->pfnRead)(&pPage->GIFFile, &cBuf[iDataAvailable], FILE_BUF_SIZE-iDataAvailable);
                iDataAvailable += iReadAmount;
                iDataRemaining -= iReadAmount;
               // lFileOff += iReadAmount;
            }
            switch(cBuf[iOff])
            {
                case 0x3b: /* End of file */
                    /* we were fooled into thinking there were more pages */
                    iNumFrames--;
                    goto gifpagesz;
    // F9 = Graphic Control Extension (fixed length of 4 bytes)
    // FE = Comment Extension
    // FF = Application Extension
    // 01 = Plain Text Extension
                case 0x21: /* Extension block */
                    if (cBuf[iOff+1] == 0xf9 && cBuf[iOff+2] == 4) // Graphic Control Extension
                    {
                       //cBuf[iOff+3]; // page disposition flags
                        iDelay = cBuf[iOff+4]; // delay low byte
                        iDelay |= ((uint16_t)(cBuf[iOff+5]) << 8); // delay high byte
                        if (iDelay < 2) // too fast, provide a default
                            iDelay = 2;
                        iDelay *= 10; // turn JIFFIES into milliseconds
                        iTotalDelay += iDelay;
                        if (iDelay > iMaxDelay) iMaxDelay = iDelay;
                        else if (iDelay < iMinDelay) iMinDelay = iDelay;
                       // (cBuf[iOff+6]; // transparent color index
                    }
                    iOff += 2; /* skip to length */
                    iOff += (int)cBuf[iOff]; /* Skip the data block */
                    iOff++;
                   // block terminator or optional sub blocks
                    c = cBuf[iOff++]; /* Skip any sub-blocks */
                    while (c)
                       {
                       iOff += (int)c;
                       c = cBuf[iOff++];
                       if ((iDataAvailable - iOff) < (c+258)) // need to read more data first
                        {
                            lv_memmove(cBuf, &cBuf[iOff], (iDataAvailable-iOff)); // move existing data down
                            iDataAvailable -= iOff;
                            iOff = 0;
                            iReadAmount = (*pPage->pfnRead)(&pPage->GIFFile, &cBuf[iDataAvailable], FILE_BUF_SIZE-iDataAvailable);
                            iDataAvailable += iReadAmount;
                            iDataRemaining -= iReadAmount;
                           // lFileOff += iReadAmount;
                        }
                    }
                    if (c != 0) // problem, we went past the end
                    {
                        iNumFrames--; // possible corrupt data; stop
                        goto gifpagesz;
                    }
                    break;
                case 0x2c: /* Start of image data */
                    bExt = 0; /* Stop doing extension blocks */
                    break;
                default:
                   /* Corrupt data, stop here */
                    iNumFrames--;
                    goto gifpagesz;
            } // switch
        } // while
        if (iOff >= iDataAvailable) // problem
        {
             iNumFrames--; // possible corrupt data; stop
             goto gifpagesz;
        }
          /* Start of image data */
        c = cBuf[iOff+9]; /* Get the flags byte */
        iOff += 10; /* Skip image position and size */
        if (c & 0x80) /* Local color table */
        {
            c &= 7;
            iOff += (2<<c)*3;
        }
        iOff++; /* Skip LZW code size byte */
        if ((iDataAvailable - iOff) < (c+258)) // need to read more data first
         {
             if (iOff < iDataAvailable) {
                 lv_memmove(cBuf, &cBuf[iOff], (iDataAvailable-iOff)); // move existing data down
                 iDataAvailable -= iOff;
                 iOff = 0;
             } else { // already points beyond end
                 iOff -= iDataAvailable;
                 iDataAvailable = 0;
             }
             iReadAmount = (*pPage->pfnRead)(&pPage->GIFFile, &cBuf[iDataAvailable], FILE_BUF_SIZE-iDataAvailable);
             iDataAvailable += iReadAmount;
             iDataRemaining -= iReadAmount;
            // lFileOff += iReadAmount;
         }
        c = cBuf[iOff++];
        while (c) /* While there are more data blocks */
        {
            if (iOff > (3*FILE_BUF_SIZE/4) && iDataRemaining > 0) /* Near end of buffer, re-align */
            {
                lv_memmove(cBuf, &cBuf[iOff], (iDataAvailable-iOff)); // move existing data down
                iDataAvailable -= iOff;
                iOff = 0;
                iReadAmount = (FILE_BUF_SIZE - iDataAvailable);
                if (iReadAmount > iDataRemaining)
                    iReadAmount = iDataRemaining;
                iReadAmount = (*pPage->pfnRead)(&pPage->GIFFile, &cBuf[iDataAvailable], iReadAmount);
                iDataAvailable += iReadAmount;
                iDataRemaining -= iReadAmount;
                // lFileOff += iReadAmount;
            }
            iOff += (int)c;  /* Skip this data block */
//            if ((int)lFileOff + iOff > pPage->GIFFile.iSize) // past end of file, stop
//            {
//                iNumFrames--; // don't count this page
//                break; // last page is corrupted, don't use it
//            }
            c = cBuf[iOff++]; /* Get length of next */
        }
        /* End of image data, check for more pages... */
        if (cBuf[iOff] == 0x3b || (iDataRemaining == 0 && (iDataAvailable - iOff) < 32))
        {
            bDone = 1; /* End of file has been reached */
        }
        else /* More pages to scan */
        {
            iNumFrames++;
             // read new page data starting at this offset
            if (pPage->GIFFile.iSize > FILE_BUF_SIZE && iDataRemaining > 0) // since we didn't read the whole file in one shot
            {
                lv_memmove(cBuf, &cBuf[iOff], (iDataAvailable-iOff)); // move existing data down
                iDataAvailable -= iOff;
                iOff = 0;
                iReadAmount = (FILE_BUF_SIZE - iDataAvailable);
                if (iReadAmount > iDataRemaining)
                    iReadAmount = iDataRemaining;
                iReadAmount = (*pPage->pfnRead)(&pPage->GIFFile, &cBuf[iDataAvailable], iReadAmount);
                iDataAvailable += iReadAmount;
                iDataRemaining -= iReadAmount;
               // lFileOff += iReadAmount;
            }
        }
    } /* while !bDone */
gifpagesz:
    pInfo->iFrameCount = iNumFrames;
    pInfo->iMaxDelay = iMaxDelay;
    pInfo->iMinDelay = iMinDelay;
    pInfo->iDuration = iTotalDelay;
    return 1;
} /* GIF_getInfo() */

//
// Unpack more chunk data for decoding
// returns 1 to signify more data available for this image
// 0 indicates there is no more data
//
static int GIFGetMoreData(GIFIMAGE *pPage)
{
    int iDelta = (pPage->iLZWSize - pPage->iLZWOff);
    int iLZWBufSize;
    unsigned char c = 1;

    // Turbo mode uses combined buffers to read more compressed data
    iLZWBufSize = (pPage->pTurboBuffer) ? LZW_BUF_SIZE_TURBO : LZW_BUF_SIZE;
    // move any existing data down
    if (pPage->bEndOfFrame ||  iDelta >= (iLZWBufSize - MAX_CHUNK_SIZE) || iDelta <= 0)
        return 1; // frame is finished or buffer is already full; no need to read more data
    if (pPage->iLZWOff != 0)
    {
// NB: memcpy() fails on some systems because the src and dest ptrs overlap
// so copy the bytes in a simple loop to avoid problems
      for (int i=0; i<pPage->iLZWSize - pPage->iLZWOff; i++) {
         pPage->ucLZW[i] = pPage->ucLZW[i + pPage->iLZWOff];
      }
      pPage->iLZWSize -= pPage->iLZWOff;
      pPage->iLZWOff = 0;
    }
    while (c && pPage->GIFFile.iPos < pPage->GIFFile.iSize && pPage->iLZWSize < (iLZWBufSize-MAX_CHUNK_SIZE))
    {
        (*pPage->pfnRead)(&pPage->GIFFile, &c, 1); // current length
        (*pPage->pfnRead)(&pPage->GIFFile, &pPage->ucLZW[pPage->iLZWSize], c);
        pPage->iLZWSize += c;
    }
    if (c == 0) // end of frame
        pPage->bEndOfFrame = 1;
    return (c != 0 && pPage->GIFFile.iPos < pPage->GIFFile.iSize); // more data available?
} /* GIFGetMoreData() */
//
// Draw and convert pixels when the user wants fully rendered output
//
static void DrawCooked(GIFIMAGE *pPage, GIFDRAW *pDraw, void *pDest)
{
    uint8_t c, *s, *d8, *pEnd;
    uint8_t *pActivePalette;

    pActivePalette = (pPage->bUseLocalPalette) ? (uint8_t *)pPage->pLocalPalette : (uint8_t *)pPage->pPalette;
    // d8 points to the line in the full sized canvas where the new opaque pixels will be merged
    d8 = &pPage->pFrameBuffer[pDraw->iX + (pDraw->iY + pDraw->y) * pPage->iCanvasWidth];
    s = pDraw->pPixels; // s points to the newly decoded pixels of this line of the current frame
    pEnd = s + pDraw->iWidth; // faster way to loop over the source pixels - eliminates a counter variable

    if (pPage->ucPaletteType == GIF_PALETTE_1BPP || pPage->ucPaletteType == GIF_PALETTE_1BPP_OLED) { // 1-bit mono
        uint8_t *d = NULL;
        uint8_t *pPal = pActivePalette;
        uint8_t uc, ucMask;
        int iPitch = 0;
         if (pPage->ucPaletteType == GIF_PALETTE_1BPP) { // horizontal pixels
             d = pPage->pFrameBuffer;
             iPitch = (pPage->iCanvasWidth+7)/8;
             d += (pPage->iCanvasWidth * pPage->iCanvasHeight);
             d += pDraw->iX/8; // starting column
             d += (pDraw->iY + pDraw->y) * iPitch;
             // Apply the new pixels to the main image and generate 1-bpp output
             if (pDraw->ucHasTransparency) { // if transparency used
                 uint8_t ucTransparent = pDraw->ucTransparent;
                 if (pDraw->ucDisposalMethod == 2) { // restore to background color
                     uint8_t u8BG = pPal[pDraw->ucBackground];
                     if (u8BG == 1) u8BG = 0xff; // set all bits to use mask
                     uc = *d; ucMask = (0x80 >> (pDraw->iX & 7));;
                     while (s < pEnd) {
                         c = *s++;
                         if (c != ucTransparent) {
                             if (pPal[c])
                                uc |= ucMask;
                             else
                                uc &= ~ucMask;
                             *d8++ = c;
                         } else {
                             uc |= (u8BG & ucMask); // transparent pixel is restored to background color
                             *d8++ = pDraw->ucBackground;
                         }
                         ucMask >>= 1;
                         if (ucMask == 0) { // write the completed byte
                             *d++ = uc;
                             uc = *d;
                             ucMask = 0x80;
                         }
                     }
                     *d = uc; // write last partial byte
                 } else { // no disposal, just write non-transparent pixels
                     uc = *d; ucMask = (0x80 >> (pDraw->iX & 7));
                     while (s < pEnd) {
                         c = *s++;
                         if (c != ucTransparent) {
                             if (pPal[c])
                                 uc |= ucMask;
                             else
                                 uc &= ~ucMask;
                             *d8 = c;
                         }
                         d8++;
                         ucMask >>= 1;
                         if (ucMask == 0) {
                             *d++ = uc;
                             uc = *d;
                             ucMask = 0x80;
                         }
                     }
                     *d = uc;
                 }
             } else { // convert everything as opaque
                 uc = *d; ucMask = (0x80 >> (pDraw->iX & 7)); // left pixel is MSB
                 while (s < pEnd) {
                     c = *d8++ = *s++; // just write the new opaque pixels over the old
                     if (pPal[c]) // if non-zero, set white pixel
                        uc |= ucMask;
                     else
                        uc &= ~ucMask;
                     ucMask >>= 1;
                     if (ucMask == 0) { // time to write the current byte
                        *d++ = uc;
                        uc = *d;
                        ucMask = 0x80;
                     }
                 }
                 *d = uc;
             }
         } else { // vertical pixels
             d = pPage->pFrameBuffer;
             d += (pPage->iCanvasWidth * pPage->iCanvasHeight);
             d += pDraw->iX; // starting column
             d += ((pDraw->iY + pDraw->y)>>3) * pPage->iCanvasWidth;
             ucMask = 1 << ((pDraw->iY + pDraw->y) & 7);
             // Apply the new pixels to the main image and generate 1-bpp output
             if (pDraw->ucHasTransparency) { // if transparency used
                 uint8_t ucTransparent = pDraw->ucTransparent;
                 if (pDraw->ucDisposalMethod == 2) { // restore to background color
                     uint8_t u8BG = pPal[pDraw->ucBackground];
                     u8BG *= ucMask; // set the right bit
                     while (s < pEnd) {
                         c = *s++;
                         uc = *d & ~ucMask; // clear old pixel
                         if (c != ucTransparent) {
                             uc |= (pPal[c] * ucMask);
                             *d8++ = c;
                         } else {
                             uc |= u8BG; // transparent pixel is restored to background color
                             *d8++ = pDraw->ucBackground;
                         }
                         *d++ = uc; // write back the updated pixel
                     }
                 } else { // no disposal, just write non-transparent pixels
                     while (s < pEnd) {
                         c = *s++;
                         uc = *d & ~ucMask;
                         if (c != ucTransparent) {
                             *d = uc | (pPal[c] * ucMask);
                             *d8 = c;
                         }
                         d++;
                         d8++;
                     }
                 }
             } else { // convert everything as opaque
                 while (s < pEnd) {
                     c = *d8++ = *s++; // just write the new opaque pixels over the old
                     uc = *d & ~ucMask;
                     *d++ = uc | (pPal[c] * ucMask);
                 }
             }
         }
    } else if (pPage->ucPaletteType == GIF_PALETTE_RGB565_LE || pPage->ucPaletteType == GIF_PALETTE_RGB565_BE) {
        uint16_t *d, *pPal = (uint16_t *)pActivePalette;
        d = (uint16_t *)pDest; // dest pointer to the cooked pixels
        // Apply the new pixels to the main image
        if (pDraw->ucHasTransparency) { // if transparency used
            uint8_t ucTransparent = pDraw->ucTransparent;
            if (pDraw->ucDisposalMethod == 2) { // restore to background color
                uint16_t u16BG = pPal[pDraw->ucBackground];
                while (s < pEnd) {
                    c = *s++;
                    if (c != ucTransparent) {
                        *d++ = pPal[c];
                        *d8++ = c;
                    } else {
                        *d++ = u16BG; // transparent pixel is restored to background color
                        *d8++ = pDraw->ucBackground;
                    }
                }
            } else { // no disposal, just write non-transparent pixels
                while (s < pEnd) {
                    c = *s++;
                    if (c != ucTransparent) {
                        *d = pPal[c];
                        *d8 = c;
                    }
                    d++;
                    d8++;
                }
            }
        } else { // convert all pixels through the palette without transparency
#if REGISTER_WIDTH == 64
            // parallelize the writes
            // optimizing for the write buffer helps; reading 4 bytes at a time vs 1 doesn't on M1
            while (s < pEnd + 4) { // group 4 pixels
                BIGUINT bu;
                uint8_t s0, s1, s2, s3;
                uint16_t d1, d2, d3;
                *(uint32_t *)d8 = *(uint32_t *)s; // just copy new opaque pixels over the old
                s0 = s[0]; s1 = s[1]; s2 = s[2]; s3 = s[3];
                bu = pPal[s0]; // not much difference on Apple M1
                d1 = pPal[s1]; // but other processors may gain
                d2 = pPal[s2]; // from unrolling the reads
                d3 = pPal[s3];
                bu |= (BIGUINT)d1 << 16;
                bu |= (BIGUINT)d2 << 32;
                bu |= (BIGUINT)d3 << 48;
                s += 4;
                d8 += 4;
                *(BIGUINT *)d = bu;
                d += 4;
            }
#endif
            while (s < pEnd) {
                c = *d8++ = *s++; // just write the new opaque pixels over the old
                *d++ = pPal[c]; // and create the cooked pixels through the palette
            }
        }
    } else { // 24bpp or 32bpp
        uint8_t pixel, *d, *pPal;
        int x;
        d = (uint8_t *)pDest;
        pPal = pActivePalette;
        if (pDraw->ucHasTransparency) {
            uint8_t ucTransparent = pDraw->ucTransparent;
            if (pDraw->ucDisposalMethod == 2) { // restore to background color
                uint8_t * bg = &pPal[pDraw->ucBackground * 3];
                if (pPage->ucPaletteType == GIF_PALETTE_RGB888) {
                    while (s < pEnd) {
                        pixel = *s++;
                        if (pixel != ucTransparent) {
                            *d8++ = pixel;
                            d[0] = pPal[(pixel * 3) + 2];
                            d[1] = pPal[(pixel * 3) + 1];
                            d[2] = pPal[(pixel * 3) + 0];
                            d += 3;
                        } else {
                            *d8++ = pDraw->ucBackground;
                            d[0] = bg[2];
                            d[1] = bg[1];
                            d[2] = bg[0];
                            d += 3;
                        }
                    }
                } else { /* GIF_PALETTE_RGB8888 */
                    while (s < pEnd) {
                        pixel = *s++;
                        if (pixel != ucTransparent) {
                            *d8++ = pixel;
                            d[0] = pPal[(pixel * 3) + 2];
                            d[1] = pPal[(pixel * 3) + 1];
                            d[2] = pPal[(pixel * 3) + 0];
                            d[3] = 0xFF;
                            d += 4;
                        } else {
                            *d8++ = pDraw->ucBackground;
                            d[3] = 0x00;
                            d += 4;
                        }
                    }
                }
            } else { // no disposal, just write non-transparent pixels
                if (pPage->ucPaletteType == GIF_PALETTE_RGB888) {
                    for (x=0; x<pPage->iWidth; x++) {
                        pixel = *s++;
                        if (pixel != ucTransparent) {
                            *d8 = pixel;
                            d[0] = pPal[(pixel * 3) + 2]; // convert to RGB888 pixels
                            d[1] = pPal[(pixel * 3) + 1];
                            d[2] = pPal[(pixel * 3) + 0];
                        }
                        d8++;
                        d += 3;
                    }
                } else { // must be RGBA32
                    for (x=0; x<pPage->iWidth; x++) {
                        pixel = *s++;
                        if (pixel != ucTransparent) {
                            *d8 = pixel;
                            d[0] = pPal[(pixel * 3) + 2]; // convert to RGB8888 pixels
                            d[1] = pPal[(pixel * 3) + 1];
                            d[2] = pPal[(pixel * 3) + 0];
                            d[3] = 0xff;
                        }
                        d8++;
                        d += 4;
                    }
                }
            }
        } else { // no transparency
            if (pPage->ucPaletteType == GIF_PALETTE_RGB888) {
                for (x=0; x<pPage->iWidth; x++) {
                    pixel = *d8++ = *s++;
                    *d++ = pPal[(pixel * 3) + 2]; // convert to RGB888 pixels
                    *d++ = pPal[(pixel * 3) + 1];
                    *d++ = pPal[(pixel * 3) + 0];
                }
            } else { // must be RGBA32
                for (x=0; x<pPage->iWidth; x++) {
                    pixel = *d8++ = *s++;
                    *d++ = pPal[(pixel * 3) + 2]; // convert to RGB8888 pixels
                    *d++ = pPal[(pixel * 3) + 1];
                    *d++ = pPal[(pixel * 3) + 0];
                    *d++ = 0xff;
                }
            }
        } // opaque
    }
} /* DrawCooked() */

//
// Handle transparent pixels and disposal method
// Used only when a frame buffer is allocated
//
static void DrawNewPixels(GIFIMAGE *pPage, GIFDRAW *pDraw)
{
    uint8_t *d, *s;
    int x, iPitch = pPage->iCanvasWidth;

    s = pDraw->pPixels;
    d = &pPage->pFrameBuffer[pDraw->iX + (pDraw->y + pDraw->iY)  * iPitch]; // dest pointer in our complete canvas buffer

    // Apply the new pixels to the main image
    if (pDraw->ucHasTransparency) { // if transparency used
        uint8_t c, ucTransparent = pDraw->ucTransparent;
        if (pDraw->ucDisposalMethod == 2) {
            lv_memset(d, pDraw->ucBackground, pDraw->iWidth); // start with background color
        }
        for (x=0; x<pDraw->iWidth; x++) {
            c = *s++;
            if (c != ucTransparent)
                *d = c;
            d++;
        }
    } else { // disposal method doesn't matter when there aren't any transparent pixels
        lv_memcpy(d, s, pDraw->iWidth); // just overwrite the old pixels
    }
} /* DrawNewPixels() */
//
// LZWCopyBytes
//
// Output the bytes for a single code (checks for buffer len)
//
static int LZWCopyBytes(unsigned char *buf, int iOffset, uint32_t *pSymbols, uint16_t *pLengths)
{
int iLen;
uint8_t c, *s, *d, *pEnd;
uint32_t u32Offset;

    iLen = *pLengths;
    u32Offset = *pSymbols;
    // The string data frequently writes past the end of the framebuffer (past last pixel)
    // ...but with the placement of our code tables AFTER the framebuffer, it doesn't matter
    // Adding a check for buffer overrun here slows everything down about 10%
    s = &buf[u32Offset & 0x7fffff];
    d = &buf[iOffset];
    pEnd = &d[iLen];
    while (d < pEnd) // most frequent are 1-8 bytes in length, copy 4 or 8 bytes in these cases too
    {
#ifdef ALLOWS_UNALIGNED
// This is a significant perf improvement compared to copying 1 byte at a time
// even though it will often copy too many bytes
        BIGUINT tmp = *(BIGUINT *) s;
        s += sizeof(BIGUINT);
        *(BIGUINT *)d = tmp;
        d += sizeof(BIGUINT);
#else
// CPUs which enforce unaligned address exceptions must do it 1 byte at a time
        *d++ = *s++;
#endif
    }
    if (u32Offset & 0x800000) // was a newly used code
    {
        d = pEnd; // in case we overshot
        c = (uint8_t)(u32Offset >> 24);
        iLen++;
        // since the code with extension byte has now been written to the output, fix the code
        *pSymbols = iOffset;
//        pSymbols[SYM_EXTRAS] = 0xffffffff;
        *d = c;
        *pLengths = (uint16_t)iLen;
    }
    return iLen;
} /* LZWCopyBytes() */
//
// Macro to extract a variable length code
//
#define GET_CODE_TURBO if (bitnum > (REGISTER_WIDTH - MAX_CODE_SIZE/*codesize*/)) { p += (bitnum >> 3); \
            bitnum &= 7; ulBits = INTELLONG(p); } \
        code = ((ulBits >> bitnum) & sMask);  \
        bitnum += codesize;

//
// DecodeLZWTurbo
//
// Theory of operation:
//
// The 'traditional' LZW decoder maintains a dictionary with a linked list of codes.
// These codes build into longer chains as more data is decoded. To output the pixels,
// the linked list is traversed backwards from the last node to the first, then these
// pixels are copied in reverse order to the output bitmap.
//
// My decoder takes a different approach. The output image becomes the dictionary and
// the tables keep track of where in the output image the 'run' begins and its length.
// ** NB **
// These tables cannot be 16-bit values because a single dictionary's output can be
// bigger than 64K
//
// I also work with the compressed data differently. Most decoders wind their way through
// the chunked data by constantly checking if the current chunk has run out of data. I
// take a different approach since modern machines have plenty of memory - I 'de-chunk'
// the data first so that the inner loop can just decode as fast as possible. I also keep
// a set of codes in a 64-bit local variable to minimize memory reads.
//
// These 2 changes result in a much faster decoder. For poorly compressed images, the
// speed gain is about 2.5x compared to giflib. For well compressed images (long runs)
// the speed can be as much as 30x faster. This is because it doesn't have to walk
// backwards through the linked list of codes when outputting pixels. It also doesn't
// have to copy pixels in reverse order, then unwind them.
//
static int DecodeLZWTurbo(GIFIMAGE *pImage, int iOptions)
{
int i, bitnum;
int iUncompressedLen;
uint32_t code, oldcode, codesize, nextcode, nextlim;
uint32_t cc, eoi;
uint32_t sMask;
uint8_t c, *p, *buf, codestart, *pHighWater;
BIGUINT ulBits;
int iLen, iColors;
int iErr = GIF_SUCCESS;
int iOffset;
uint32_t *pSymbols;
uint16_t *pLengths;

    (void)iOptions;
    pImage->iYCount = pImage->iHeight; // count down the lines
    pImage->iXCount = pImage->iWidth;
    bitnum = 0;
    pHighWater = pImage->ucLZW + LZW_HIGHWATER_TURBO;
    pImage->iLZWOff = 0; // Offset into compressed data
    GIFGetMoreData(pImage); // Read some data to start
    codestart = pImage->ucCodeStart;
    iColors = 1 << codestart;
    sMask = UINT32_MAX << (codestart+1);
    sMask = 0xffffffff - sMask;
    cc = (sMask >> 1) + 1; /* Clear code */
    eoi = cc + 1;
    iUncompressedLen = (pImage->iWidth * pImage->iHeight);
    buf = (uint8_t *)pImage->pTurboBuffer;
    pSymbols = (uint32_t *)&buf[iUncompressedLen+256]; // we need 32-bits (really 23) for the offsets
    pLengths = (uint16_t *)&pSymbols[4096]; // but only 16-bits for the length of any single string
    iOffset = 0; // output data offset
    p = pImage->ucLZW; // un-chunked LZW data
    ulBits = INTELLONG(p); // start by reading some LZW data
    // set up the default symbols (0..iColors-1)
   for (i = 0; i<iColors; i++) {
       pSymbols[i] = iUncompressedLen + i; // root symbols
       pLengths[i] = 1;
       buf[iUncompressedLen + i] = (unsigned char) i;
   }
init_codetable:
   codesize = codestart + 1;
   sMask = UINT32_MAX << (codestart+1);
   sMask = 0xffffffff - sMask;
   nextcode = cc + 2;
   nextlim = (1 << codesize);
    GET_CODE_TURBO
    if (code == cc) { // we just reset the dictionary; get another code
        GET_CODE_TURBO
    }
    buf[iOffset++] = (unsigned char) code; // first code after a dictionary reset is just stored
    oldcode = code;
    GET_CODE_TURBO
    while (code != eoi && iOffset < iUncompressedLen) { /* Loop through all the data */
        if (code == cc) { /* Clear code? */
           goto init_codetable;
        }
        if (code != eoi) {
            if (nextcode < nextlim) { // for deferred cc case, don't let it overwrite the last entry (fff)
                if (code != nextcode) { // most probable case
                    iLen = LZWCopyBytes(buf, iOffset, &pSymbols[code], &pLengths[code]);
                    pSymbols[nextcode] = (pSymbols[oldcode] | 0x800000 | (buf[iOffset] << 24));
                    pLengths[nextcode] = pLengths[oldcode];
                    iOffset += iLen;
                } else { // new code
                    iLen = LZWCopyBytes(buf, iOffset, &pSymbols[oldcode], &pLengths[oldcode]);
                    pLengths[nextcode] = iLen+1;
                    pSymbols[nextcode] = iOffset;
                    c = buf[iOffset];
                    iOffset += iLen;
                    buf[iOffset++] = c; // repeat first character of old code on the end
                }
            } else { // Deferred CC case - continue to use codes, but don't generate new ones
                iLen = LZWCopyBytes(buf, iOffset, &pSymbols[code], &pLengths[code]);
                iOffset += iLen;
            }
            nextcode++;
            if (nextcode >= nextlim && codesize < MAX_CODE_SIZE) {
                codesize++;
                nextlim <<= 1;
                sMask = (sMask << 1) | 1;
            }
            if (p >= pHighWater) {
                pImage->iLZWOff = (int)(p - pImage->ucLZW); // restore object member var
                GIFGetMoreData(pImage); // We need to read more LZW data
                p = &pImage->ucLZW[pImage->iLZWOff];
            }
            oldcode = code;
            GET_CODE_TURBO
        } /* while not end of LZW code stream */
    } // while not end of frame
    if (pImage->ucDrawType == GIF_DRAW_COOKED && pImage->pfnDraw && pImage->pFrameBuffer) { // convert each line through the palette
        GIFDRAW gd;
        gd.iX = pImage->iX;
        gd.iY = pImage->iY;
        gd.iWidth = pImage->iWidth;
        gd.iHeight = pImage->iHeight;
        gd.pPalette = (pImage->bUseLocalPalette) ? pImage->pLocalPalette : pImage->pPalette;
        gd.pPalette24 = (uint8_t *)gd.pPalette; // just cast the pointer for RGB888
        gd.ucIsGlobalPalette = pImage->bUseLocalPalette==1?0:1;
        gd.pUser = pImage->pUser;
        gd.ucPaletteType = pImage->ucPaletteType;
        for (int y=0; y<pImage->iHeight; y++) {
            gd.y = y;
            gd.pPixels = &buf[(y * pImage->iWidth)]; // source pixels
            // Ugly logic to handle the interlaced line position, but it
            // saves having to have another set of state variables
            if (pImage->ucMap & 0x40) { // interlaced?
               int height = pImage->iHeight-1;
               if (gd.y > height / 2)
                  gd.y = gd.y * 2 - (height | 1);
               else if (gd.y > height / 4)
                  gd.y = gd.y * 4 - ((height & ~1) | 2);
               else if (gd.y > height / 8)
                  gd.y = gd.y * 8 - ((height & ~3) | 4);
               else
                  gd.y = gd.y * 8;
            }
            gd.ucDisposalMethod = (pImage->ucGIFBits & 0x1c)>>2;
            gd.ucTransparent = pImage->ucTransparent;
            gd.ucHasTransparency = pImage->ucGIFBits & 1;
            gd.ucBackground = pImage->ucBackground;
            gd.iCanvasWidth = pImage->iCanvasWidth;
            DrawCooked(pImage, &gd, &buf[pImage->iCanvasHeight * pImage->iCanvasWidth]); // dest = past end of canvas
            gd.pPixels = &buf[pImage->iCanvasHeight * pImage->iCanvasWidth]; // point to the line we just converted
            (*pImage->pfnDraw)(&gd); // callback to handle this line
        }
    }
    return iErr;
} /* DecodeLZWTurbo() */

//
// GIFMakePels
//
static void GIFMakePels(GIFIMAGE *pPage, unsigned int code)
{
    int iPixCount;
    unsigned short *giftabs;
    unsigned char *buf, *s, *pEnd, *gifpels;
    /* Copy this string of sequential pixels to output buffer */
    //   iPixCount = 0;
    pEnd = pPage->ucFileBuf;
    s = pEnd + FILE_BUF_SIZE; /* Pixels will come out in reversed order */
    buf = pPage->ucLineBuf + (pPage->iWidth - pPage->iXCount);
    giftabs = pPage->usGIFTable;
    gifpels = &pPage->ucGIFPixels[PIXEL_LAST];
    while (code < LINK_UNUSED)
    {
        if (s == pEnd) /* Houston, we have a problem */
        {
            return; /* Exit with error */
        }
        *(--s) = gifpels[code];
        code = giftabs[code];
    }
    iPixCount = (int)(intptr_t)(pEnd + FILE_BUF_SIZE - s);
    while (iPixCount && pPage->iYCount > 0)
    {
        if (pPage->iXCount > iPixCount)  /* Pixels fit completely on the line */
        {
            pEnd = buf + iPixCount;
            while (buf < pEnd) {
#ifdef ALLOWS_UNALIGNED
// This is a significant perf improvement compared to copying 1 byte at a time
// even though it will often copy too many bytes. Since we're not at the end of
// the line, it's okay to copy a few extra pixels.
                BIGUINT tmp = *(BIGUINT *) s;
                s += sizeof(BIGUINT);
                *(BIGUINT *)buf = tmp;
                buf += sizeof(BIGUINT);
#else
                *buf++ = *s++;
#endif
            }
            pPage->iXCount -= iPixCount;
            //         iPixCount = 0;
            if (pPage->iLZWOff >= LZW_HIGHWATER)
                GIFGetMoreData(pPage); // We need to read more LZW data
            return;
        }
        else  /* Pixels cross into next line */
        {
            GIFDRAW gd;
            pEnd = buf + pPage->iXCount;
            while (buf < pEnd)
            {
                *buf++ = *s++;
            }
            iPixCount -= pPage->iXCount;
            pPage->iXCount = pPage->iWidth; /* Reset pixel count */
            // Prepare GIDRAW structure for callback
            gd.iX = pPage->iX;
            gd.iY = pPage->iY;
            gd.iWidth = pPage->iWidth;
            gd.iHeight = pPage->iHeight;
            gd.pPixels = pPage->ucLineBuf;
            gd.pPalette = (pPage->bUseLocalPalette) ? pPage->pLocalPalette : pPage->pPalette;
            gd.pPalette24 = (uint8_t *)gd.pPalette; // just cast the pointer for RGB888
            gd.ucIsGlobalPalette = pPage->bUseLocalPalette==1?0:1;
            gd.y = pPage->iHeight - pPage->iYCount;
            // Ugly logic to handle the interlaced line position, but it
            // saves having to have another set of state variables
            if (pPage->ucMap & 0x40) { // interlaced?
               int height = pPage->iHeight-1;
               if (gd.y > height / 2)
                  gd.y = gd.y * 2 - (height | 1);
               else if (gd.y > height / 4)
                  gd.y = gd.y * 4 - ((height & ~1) | 2);
               else if (gd.y > height / 8)
                  gd.y = gd.y * 8 - ((height & ~3) | 4);
               else
                  gd.y = gd.y * 8;
            }
            gd.ucDisposalMethod = (pPage->ucGIFBits & 0x1c)>>2;
            gd.ucTransparent = pPage->ucTransparent;
            gd.ucHasTransparency = pPage->ucGIFBits & 1;
            gd.ucBackground = pPage->ucBackground;
            gd.iCanvasWidth = pPage->iCanvasWidth;
            gd.pUser = pPage->pUser;
            gd.ucPaletteType = pPage->ucPaletteType;
            if (pPage->pFrameBuffer) // update the frame buffer
            {
                int iPitch = 0, iBpp = 1, iOffset = pPage->iCanvasWidth * pPage->iCanvasHeight;
                if (pPage->ucDrawType == GIF_DRAW_COOKED) {
                    if (!pPage->pfnDraw) { // no draw callback, prepare the full frame
                        switch (pPage->ucPaletteType) {
                            case GIF_PALETTE_1BPP:
                                iPitch = (pPage->iCanvasWidth + 7) / 8;
                                break;
                            case GIF_PALETTE_RGB565_BE:
                            case GIF_PALETTE_RGB565_LE:
                                iPitch = (pPage->iCanvasWidth * 2);
                                iBpp = 2;
                                break;
                            case GIF_PALETTE_RGB888:
                                iPitch = pPage->iCanvasWidth * 3;
                                iBpp = 3;
                                break;
                            case GIF_PALETTE_RGB8888:
                                iPitch = pPage->iCanvasWidth * 4;
                                iBpp = 4;
                                break;
                        }
                        iOffset += (iBpp * pPage->iX) + ((gd.y + pPage->iY) * iPitch);
                    }
                    DrawCooked(pPage, &gd, &pPage->pFrameBuffer[iOffset]);
                    // pass the cooked pixel pointer to the GIFDraw callback
                    gd.pPixels = &pPage->pFrameBuffer[iOffset];
                } else { // the user will manage converting them through the palette
                    DrawNewPixels(pPage, &gd); // merge the new opaque pixels
                }
            }
            if (pPage->pfnDraw) {
                (*pPage->pfnDraw)(&gd); // callback to handle this line
            }
            pPage->iYCount--;
            buf = pPage->ucLineBuf;
            if (pPage->iLZWOff >= LZW_HIGHWATER)
                GIFGetMoreData(pPage); // We need to read more LZW data
        }
    } /* while */
    if (pPage->iLZWOff >= LZW_HIGHWATER)
        GIFGetMoreData(pPage); // We need to read more LZW data
    return;
} /* GIFMakePels() */
//
// Macro to extract a variable length code
//
#define GET_CODE if (bitnum > (REGISTER_WIDTH - codesize)) { pImage->iLZWOff += (bitnum >> 3); \
            bitnum &= 7; ulBits = INTELLONG(&p[pImage->iLZWOff]); } \
        code = (unsigned short) (ulBits >> bitnum); /* Read a REGISTER_WIDTH chunk */ \
        code &= sMask; bitnum += codesize;
//
// Decode LZW into an image
//
static int DecodeLZW(GIFIMAGE *pImage, int iOptions)
{
    int i, bitnum;
    unsigned short oldcode, codesize, nextcode, nextlim;
    unsigned short *giftabs, cc, eoi;
    signed short sMask;
    unsigned char c, *gifpels, *p;
    //    int iStripSize;
    //unsigned char **index;
    BIGUINT ulBits;
    unsigned short code;
    (void)iOptions; // not used for now
    // if output can be used for string table, do it faster
    //       if (bGIF && (OutPage->cBitsperpixel == 8 && ((OutPage->iWidth & 3) == 0)))
    //          return PILFastLZW(InPage, OutPage, bGIF, iOptions);
    if (pImage->ucDrawType == GIF_DRAW_COOKED && pImage->pFrameBuffer == NULL && pImage->pfnDraw == NULL) { // without a framebuffer and a GIFDRAW callback, we cannot continue
        pImage->iError = GIF_INVALID_PARAMETER;
        return 1; // indicate a problem
    }
    // If the user selected RAW output and there is no GIFDRAW callback, that won't work either
    if (pImage->ucDrawType == GIF_DRAW_RAW && pImage->pfnDraw == NULL) {
        pImage->iError = GIF_INVALID_PARAMETER;
        return 1; // indicate a problem
    }
    p = pImage->ucLZW; // un-chunked LZW data
    sMask = 0xffff << (pImage->ucCodeStart + 1);
    sMask = 0xffff - sMask;
    cc = (sMask >> 1) + 1; /* Clear code */
    eoi = cc + 1;
    giftabs = pImage->usGIFTable;
    gifpels = pImage->ucGIFPixels;
    pImage->iYCount = pImage->iHeight; // count down the lines
    pImage->iXCount = pImage->iWidth;
    bitnum = 0;
    pImage->iLZWOff = 0; // Offset into compressed data
    GIFGetMoreData(pImage); // Read some data to start

    // Initialize code table
    // this part only needs to be initialized once
    for (i = 0; i < cc; i++)
    {
        gifpels[PIXEL_FIRST + i] = gifpels[PIXEL_LAST + i] = (unsigned short) i;
        giftabs[i] = LINK_END;
    }
init_codetable:
    codesize = pImage->ucCodeStart + 1;
    sMask = 0xffff << (pImage->ucCodeStart + 1);
    sMask = 0xffff - sMask;
    nextcode = cc + 2;
    nextlim = (unsigned short) ((1 << codesize));
    // This part of the table needs to be reset multiple times
    lv_memset(&giftabs[cc], (uint8_t) LINK_UNUSED, sizeof(pImage->usGIFTable) - sizeof(giftabs[0])*cc);
    ulBits = INTELLONG(&p[pImage->iLZWOff]); // start by reading 4 bytes of LZW data
    GET_CODE
    if (code == cc) // we just reset the dictionary, so get another code
    {
      GET_CODE
    }
    c = oldcode = code;
    GIFMakePels(pImage, code); // first code is output as the first pixel
    // Main decode loop
    while (code != eoi && pImage->iYCount > 0) // && y < pImage->iHeight+1) /* Loop through all lines of the image (or strip) */
    {
        GET_CODE
        if (code == cc) /* Clear code?, and not first code */
            goto init_codetable;
        if (code != eoi)
        {
                if (nextcode < nextlim) // for deferred cc case, don't let it overwrite the last entry (fff)
                {
                    giftabs[nextcode] = oldcode;
                    gifpels[PIXEL_FIRST + nextcode] = c; // oldcode pixel value
                    gifpels[PIXEL_LAST + nextcode] = c = gifpels[PIXEL_FIRST + code];
                }
                nextcode++;
                if (nextcode >= nextlim && codesize < MAX_CODE_SIZE)
                {
                    codesize++;
                    nextlim <<= 1;
                    sMask = nextlim - 1;
                }
            GIFMakePels(pImage, code);
            oldcode = code;
        }
    } /* while not end of LZW code stream */
    return 0;
//gif_forced_error:
//    free(pImage->pPixels);
//    pImage->pPixels = NULL;
//    return -1;
} /* DecodeLZW() */


