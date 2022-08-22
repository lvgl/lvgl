///////////////////////////////////////////////////////////////////////////////
// \author (c) Marco Paland (info@paland.com)
//             2014-2019, PALANDesign Hannover, Germany
//
// \license The MIT License (MIT)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// \brief Tiny printf, sprintf and snprintf implementation, optimized for speed on
//        embedded systems with a very limited resources.
//        Use this instead of bloated standard/newlib printf.
//        These routines are thread safe and reentrant.
//
///////////////////////////////////////////////////////////////////////////////

/*Original repository: https://github.com/mpaland/printf*/

#ifndef _LV_PRINTF_H_
#define _LV_PRINTF_H_

#if defined(__has_include)
    #if __has_include(<inttypes.h>)
        #include <inttypes.h>
        /* platform-specific printf format for int32_t, usually "d" or "ld" */
        #define LV_PRId32 PRId32
        #define LV_PRIu32 PRIu32
        #define LV_PRIx32 PRIx32
        #define LV_PRIX32 PRIX32
    #else
        #define LV_PRId32 "d"
        #define LV_PRIu32 "u"
        #define LV_PRIx32 "x"
        #define LV_PRIX32 "X"
    #endif
#else
    /* hope this is correct for ports without __has_include or without inttypes.h */
    #define LV_PRId32 "d"
    #define LV_PRIu32 "u"
    #define LV_PRIx32 "x"
    #define LV_PRIX32 "X"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "../lv_conf_internal.h"
#include <stdarg.h>
#include <stddef.h>

#include "lv_types.h"

typedef struct {
    const char * fmt;
    va_list * va;
} lv_vaformat_t;

int lv_snprintf_builtin(char * buffer, size_t count, const char * format, ...);
int lv_vsnprintf_builtin(char * buffer, size_t count, const char * format, va_list va);

#define lv_snprintf LV_SNPRINTF
#define lv_vsnprintf LV_VSNPRINTF


#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif  // _LV_PRINTF_H_
