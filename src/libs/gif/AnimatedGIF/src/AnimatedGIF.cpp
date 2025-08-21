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

// Here is all of the actual code...
#include "gif.inl"

//
// Memory initialization
//
int AnimatedGIF::open(uint8_t *pData, int iDataSize, GIF_DRAW_CALLBACK *pfnDraw)
{
    _gif.iError = GIF_SUCCESS;
    _gif.pfnRead = readMem;
    _gif.pfnSeek = seekMem;
    _gif.pfnDraw = pfnDraw;
    _gif.pfnOpen = NULL;
    _gif.pfnClose = NULL;
    _gif.GIFFile.iSize = iDataSize;
    _gif.GIFFile.pData = pData;
    return GIFInit(&_gif);
} /* open() */

#ifdef ARDUINO
int AnimatedGIF::openFLASH(uint8_t *pData, int iDataSize, GIF_DRAW_CALLBACK *pfnDraw)
{
    _gif.iError = GIF_SUCCESS;
    _gif.pfnRead = readFLASH;
    _gif.pfnSeek = seekMem;
    _gif.pfnDraw = pfnDraw;
    _gif.pfnOpen = NULL;
    _gif.pfnClose = NULL;
    _gif.GIFFile.iSize = iDataSize;
    _gif.GIFFile.pData = pData;
    return GIFInit(&_gif);
} /* openFLASH() */
#endif
//
// Returns the first comment block found (if any)
//
int AnimatedGIF::getComment(char *pDest)
{
int32_t iOldPos;

    iOldPos = _gif.GIFFile.iPos; // keep old position
    (*_gif.pfnSeek)(&_gif.GIFFile, _gif.iCommentPos);
    (*_gif.pfnRead)(&_gif.GIFFile, (uint8_t *)pDest, _gif.sCommentLen);
    (*_gif.pfnSeek)(&_gif.GIFFile, iOldPos);
    pDest[_gif.sCommentLen] = 0; // zero terminate the string
    return (int)_gif.sCommentLen;
} /* getComment() */

//  
// Allocate a block of memory to hold the entire canvas (as 8-bpp)
//
int AnimatedGIF::allocFrameBuf(GIF_ALLOC_CALLBACK *pfnAlloc)
{
    if (_gif.iCanvasWidth > 0 && _gif.iCanvasHeight > 0 && _gif.pFrameBuffer == NULL)
    {
        // Allocate a little extra space for the current line
        // as RGB565 or RGB888
        int iCanvasSize = _gif.iCanvasWidth * (_gif.iCanvasHeight+3);
        _gif.pFrameBuffer = (unsigned char *)(*pfnAlloc)(iCanvasSize);
        if (_gif.pFrameBuffer == NULL)
            return GIF_ERROR_MEMORY;
        return GIF_SUCCESS;
    }
    return GIF_INVALID_PARAMETER;
} /* allocFrameBuf() */
//
// Allocate a block of memory to hold the Turbo Buffer entire canvas (as 8-bpp)
// as well as 32k needed for faster decoding
//
int AnimatedGIF::allocTurboBuf(GIF_ALLOC_CALLBACK *pfnAlloc)
{
    if (_gif.iCanvasWidth > 0 && _gif.iCanvasHeight > 0 && _gif.pTurboBuffer == NULL)
    {
        // Allocate a little extra space for the current line
        // as RGB565 or RGB888
        int iTurboSize = TURBO_BUFFER_SIZE + (_gif.iCanvasWidth * _gif.iCanvasHeight);
        _gif.pTurboBuffer = (unsigned char *)(*pfnAlloc)(iTurboSize);
        if (_gif.pTurboBuffer == NULL)
            return GIF_ERROR_MEMORY;
        return GIF_SUCCESS;
    }
    return GIF_INVALID_PARAMETER;
} /* allocTurboBuf() */
//
// Set the frame buffer pointer
//
void AnimatedGIF::setFrameBuf(void *pFrameBuf)
{
    _gif.pFrameBuffer = (uint8_t*)pFrameBuf;
}
//
// Set the Turbo buffer pointer
//
void AnimatedGIF::setTurboBuf(void *pBuf)
{
    _gif.pTurboBuffer = (uint8_t *)pBuf;
} /* setTurboBuf() */
//
// Set the DRAW callback behavior to RAW (default)
// or COOKED (requires allocating a frame buffer)
//
int AnimatedGIF::setDrawType(int iType)
{
    if (iType != GIF_DRAW_RAW && iType != GIF_DRAW_COOKED)
        return GIF_INVALID_PARAMETER; // invalid drawing mode
    _gif.ucDrawType = (uint8_t)iType;
    return GIF_SUCCESS;
} /* setDrawType() */
//
// Release the memory used by the Turbo buffer
//
int AnimatedGIF::freeTurboBuf(GIF_FREE_CALLBACK *pfnFree)
{
    if (_gif.pTurboBuffer)
    {
        (*pfnFree)(_gif.pTurboBuffer);
        _gif.pTurboBuffer = NULL;
        return GIF_SUCCESS;
    }
    return GIF_INVALID_PARAMETER;
} /* freeTurboBuf() */
//
// Release the memory used by the frame buffer
//
int AnimatedGIF::freeFrameBuf(GIF_FREE_CALLBACK *pfnFree)
{
    if (_gif.pFrameBuffer)
    {
        (*pfnFree)(_gif.pFrameBuffer);
        _gif.pFrameBuffer = NULL;
        return GIF_SUCCESS;
    }
    return GIF_INVALID_PARAMETER;
} /* freeFrameBuf() */
//
// Return a pointer to the frame buffer (if it was allocated)
//
uint8_t * AnimatedGIF::getFrameBuf()
{
    return _gif.pFrameBuffer;
} /* getFrameBuf() */

//
// Return a pointer to the Turbo buffer (if it was allocated)
//
uint8_t * AnimatedGIF::getTurboBuf()
{
    return _gif.pTurboBuffer;
} /* getTurboBuf() */

int AnimatedGIF::getCanvasWidth()
{
    return _gif.iCanvasWidth;
} /* getCanvasWidth() */

int AnimatedGIF::getFrameWidth()
{
    return _gif.iWidth;
}
int AnimatedGIF::getFrameHeight()
{
    return _gif.iHeight;
}

int AnimatedGIF::getFrameXOff()
{
    return _gif.iX;
} /* AnimatedGIF() */

int AnimatedGIF::getFrameYOff()
{
    return _gif.iY;
}

int AnimatedGIF::getCanvasHeight()
{
    return _gif.iCanvasHeight;
} /* getCanvasHeight() */

int AnimatedGIF::getLoopCount()
{
    return _gif.iRepeatCount;
} /* getLoopCount() */

int AnimatedGIF::getInfo(GIFINFO *pInfo)
{
   return GIF_getInfo(&_gif, pInfo);
} /* getInfo() */

int AnimatedGIF::getLastError()
{
    return _gif.iError;
} /* getLastError() */

//
// File (SD/MMC) based initialization
//
int AnimatedGIF::open(const char *szFilename, GIF_OPEN_CALLBACK *pfnOpen, GIF_CLOSE_CALLBACK *pfnClose, GIF_READ_CALLBACK *pfnRead, GIF_SEEK_CALLBACK *pfnSeek, GIF_DRAW_CALLBACK *pfnDraw)
{
    _gif.iError = GIF_SUCCESS;
    _gif.pfnRead = pfnRead;
    _gif.pfnSeek = pfnSeek;
    _gif.pfnDraw = pfnDraw;
    _gif.pfnOpen = pfnOpen;
    _gif.pfnClose = pfnClose;
    _gif.GIFFile.fHandle = (*pfnOpen)(szFilename, &_gif.GIFFile.iSize);
    if (_gif.GIFFile.fHandle == NULL) {
       _gif.iError = GIF_FILE_NOT_OPEN;
       return 0;
    }
    return GIFInit(&_gif);

} /* open() */

void AnimatedGIF::close()
{
    if (_gif.pfnClose)
        (*_gif.pfnClose)(_gif.GIFFile.fHandle);
} /* close() */

void AnimatedGIF::reset()
{
    _gif.iError = GIF_SUCCESS;
    (*_gif.pfnSeek)(&_gif.GIFFile, 0);
} /* reset() */

void AnimatedGIF::begin(unsigned char ucPaletteType)
{
    memset(&_gif, 0, sizeof(_gif));
    if (ucPaletteType != GIF_PALETTE_RGB565_LE && ucPaletteType != GIF_PALETTE_RGB565_BE && ucPaletteType != GIF_PALETTE_RGB888)
        _gif.iError = GIF_INVALID_PARAMETER;
    _gif.ucPaletteType = ucPaletteType;
    _gif.ucDrawType = GIF_DRAW_RAW; // assume RAW pixel handling
    _gif.pFrameBuffer = NULL;
} /* begin() */
//
// Play a single frame
// returns:
// 1 = good result and more frames exist
// 0 = no more frames exist, a frame may or may not have been played: use getLastError() and look for GIF_SUCCESS to know if a frame was played
// -1 = error
int AnimatedGIF::playFrame(bool bSync, int *delayMilliseconds, void *pUser)
{
int rc;
#if !defined( __MACH__ ) && !defined( __LINUX__ )
long lTime = millis();
#endif

    if (_gif.GIFFile.iPos >= _gif.GIFFile.iSize-1) // no more data exists
    {
        (*_gif.pfnSeek)(&_gif.GIFFile, 0); // seek to start
    }
    if (GIFParseInfo(&_gif, 0))
    {
        _gif.pUser = pUser;
        if (_gif.iError == GIF_EMPTY_FRAME) // don't try to decode it
            return 0;
        if (_gif.pTurboBuffer) {
            rc = DecodeLZWTurbo(&_gif, 0);
        } else {
            rc = DecodeLZW(&_gif, 0);
        }
        if (rc != 0) // problem
            return -1;
    }
    else
    {
        // The file is "malformed" in that there is a bunch of non-image data after
        // the last frame. Return as if all is well, though if needed getLastError()
        // can be used to see if a frame was actually processed:
        // GIF_SUCCESS -> frame processed, GIF_EMPTY_FRAME -> no frame processed
        if (_gif.iError == GIF_EMPTY_FRAME)
        {
	    if (delayMilliseconds)
                *delayMilliseconds = 0;
            return 0;
        }
        return -1; // error parsing the frame info, we may be at the end of the file
    }
    // Return 1 for more frames or 0 if this was the last frame
    if (bSync)
    {
#if !defined( __MACH__ ) && !defined( __LINUX__ ) 
        lTime = millis() - lTime;
        if (lTime < _gif.iFrameDelay) // need to pause a bit
           delay(_gif.iFrameDelay - lTime);
#endif // __LINUX__
    }
    if (delayMilliseconds) // if not NULL, return the frame delay time
        *delayMilliseconds = _gif.iFrameDelay;
    return (_gif.GIFFile.iPos < _gif.GIFFile.iSize-10);
} /* playFrame() */

