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

#include "library.h"

__attribute__((weak)) uint32_t RANDOM_CALL_PROCESS(void)
{
    /*Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs"*/
    static uint32_t x = 0x114514;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    return x;
}

const uint32_t ALPHANUM_AND_CJK_TABLE[] = {
    0x4e00, 0x9fa5, // CJK Unified Ideographs
    'A', 'Z',
    'a', 'z',
    '0', '9',
};
const uint32_t ALPHANUM_AND_CJK_TABLE_LEN = sizeof(ALPHANUM_AND_CJK_TABLE) / sizeof(unicode_t) / 2;

const uint32_t ONLY_CJK_TABLE[] = {
    0x4e00, 0x9fa5, // CJK Unified Ideographs
};
const uint32_t ONLY_CJK_TABLE_LEN = sizeof(ALPHANUM_AND_CJK_TABLE) / sizeof(unicode_t) / 2;

const uint32_t ALPHANUM_TABLE[] = {
    'A', 'Z',
    'a', 'z',
    '0', '9',
};
const uint32_t ALPHANUM_TABLE_LEN = sizeof(ALPHANUM_TABLE) / sizeof(unicode_t) / 2;

static int unicode_to_utf8_bytes_len(unicode_t unicode)
{
    if(unicode < 0x80) {
        return 1;
    }
    else if(unicode < 0x800) {
        return 2;
    }
    else if(unicode < 0x10000) {
        return 3;
    }
    else if(unicode < 0x200000) {
        return 4;
    }
    else if(unicode < 0x4000000) {
        return 5;
    }
    else {
        return 6;
    }
}

static int unicode_to_uft8(utf8_t * buf, uint32_t buf_len, unicode_t unicode)
{
    uint32_t unicode_len = unicode_to_utf8_bytes_len(unicode);
    if(buf_len < unicode_len) {
        return -1;
    }

    int buf_index = 0;

    switch(unicode_len) {
        case 1:
            buf[buf_index++] = (uint8_t) unicode;
            break;
        case 2:
            buf[buf_index++] = (uint8_t)(0xc0 | (unicode >> 6));
            buf[buf_index++] = (uint8_t)(0x80 | (unicode & 0x3f));
            break;
        case 3:
            buf[buf_index++] = (uint8_t)(0xe0 | (unicode >> 12));
            buf[buf_index++] = (uint8_t)(0x80 | ((unicode >> 6) & 0x3f));
            buf[buf_index++] = (uint8_t)(0x80 | (unicode & 0x3f));
            break;
        case 4:
            buf[buf_index++] = (uint8_t)(0xf0 | (unicode >> 18));
            buf[buf_index++] = (uint8_t)(0x80 | ((unicode >> 12) & 0x3f));
            buf[buf_index++] = (uint8_t)(0x80 | ((unicode >> 6) & 0x3f));
            buf[buf_index++] = (uint8_t)(0x80 | (unicode & 0x3f));
            break;
        case 5:
            buf[buf_index++] = (uint8_t)(0xf8 | (unicode >> 24));
            buf[buf_index++] = (uint8_t)(0x80 | ((unicode >> 18) & 0x3f));
            buf[buf_index++] = (uint8_t)(0x80 | ((unicode >> 12) & 0x3f));
            buf[buf_index++] = (uint8_t)(0x80 | ((unicode >> 6) & 0x3f));
            buf[buf_index++] = (uint8_t)(0x80 | (unicode & 0x3f));
            break;
        case 6:
            buf[buf_index++] = (uint8_t)(0xfc | (unicode >> 30));
            buf[buf_index++] = (uint8_t)(0x80 | ((unicode >> 24) & 0x3f));
            buf[buf_index++] = (uint8_t)(0x80 | ((unicode >> 18) & 0x3f));
            buf[buf_index++] = (uint8_t)(0x80 | ((unicode >> 12) & 0x3f));
            buf[buf_index++] = (uint8_t)(0x80 | ((unicode >> 6) & 0x3f));
            buf[buf_index++] = (uint8_t)(0x80 | (unicode & 0x3f));
            break;
        default:
            return -1;
    }
    return buf_index;
}

static int random_one_utf8_char(utf8_t * buf, int buf_len, unicode_t char_range_min, unicode_t char_range_max)
{
    if(buf_len < 1) {
        return -1;
    }

    uint32_t r = RANDOM_CALL_PROCESS() % (char_range_max - char_range_min + 1) + char_range_min;

    return unicode_to_uft8(buf, buf_len, r);
}

int random_utf8_chars(utf8_t * buf, int buf_len, const unicode_t * ranges, uint32_t range_num, int char_num)
{
    if(buf_len < char_num) {
        return -1;
    }

    int buf_index = 0;
    for(int i = 0; i < char_num && buf_index < buf_len; i++) {
        int range_index = RANDOM_CALL_PROCESS() % range_num;
        int ret = random_one_utf8_char(buf + buf_index,
                                       buf_len - buf_index,
                                       ranges[2 * range_index],
                                       ranges[2 * range_index + 1]);
        if(ret < 0) {
            return -1;
        }
        buf_index += ret;
    }

    buf[buf_index] = '\0';
    return buf_index;
}
