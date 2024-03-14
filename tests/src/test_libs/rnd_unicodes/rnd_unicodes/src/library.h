/*
 * MIT License
 *
 * Copyright (c) 2023 Benign X
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef RND_UNICODES_LIBRARY_H
#define RND_UNICODES_LIBRARY_H

#include <stdint.h>

typedef uint32_t unicode_t;
typedef uint8_t utf8_t;

extern const uint32_t ALPHANUM_AND_CJK_TABLE[];
extern const uint32_t ALPHANUM_AND_CJK_TABLE_LEN;

extern const uint32_t ONLY_CJK_TABLE[];
extern const uint32_t ONLY_CJK_TABLE_LEN;

extern const uint32_t ALPHANUM_TABLE[];
extern const uint32_t ALPHANUM_TABLE_LEN;

uint32_t RANDOM_CALL_PROCESS(void);

int random_utf8_chars(utf8_t * buf, int buf_len, const unicode_t * ranges, uint32_t range_num, int char_num);

#endif //RND_UNICODES_LIBRARY_H
