/*
 * Copyright (C) 2023 Xiaomi Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_qrscan.h"
#if LV_USE_QRSCAN

#include "quirc.h"
#include "quirc_internal.h"

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void * lv_qrscan_create(void)
{
    return quirc_new();
}

void lv_qrscan_destory(void * handle)
{
    quirc_destroy(handle);
}

int lv_qrscan_scan(void * handle, int width, int height,
                   uint8_t * in, char ** out)
{
    struct quirc * qr = handle;
    char * msg = NULL;
    size_t len = 0;
    uint8_t * buf;
    int count;
    int i;

    if(*out || !in) {
        LV_LOG_ERROR("error: invalid parameter\n");
        return -1;
    }

    if(quirc_resize(qr, width, height) < 0) {
        LV_LOG_ERROR("couldn't allocate QR buffer");
        return -1;
    }

    buf = quirc_begin(qr, &width, &height);
    qr->image = in; /* override to avoid copying */
    quirc_end(qr);

    count = quirc_count(qr);

    for(i = 0; i < count; i++) {
        struct quirc_code code;
        struct quirc_data data;

        quirc_extract(qr, i, &code);

        quirc_decode_error_t err = quirc_decode(&code, &data);
        if(err == QUIRC_ERROR_DATA_ECC) {
            quirc_flip(&code);
            err = quirc_decode(&code, &data);
        }

        if(!err) {
            size_t paylen = strlen((char *)data.payload) + 1;
            char * tmp = lv_mem_realloc(msg, len + paylen + 1);
            if(tmp == NULL) {
                lv_mem_free(msg);
                return -1;
            }
            msg = tmp;

            lv_memcpy(msg + len, data.payload, paylen);
            len += paylen;
            msg[len] = '\0';
        }
        else {
            LV_LOG_ERROR("error: %s\n", quirc_strerror(err));
        }
    }

    *out = msg;

    qr->image = buf; /* restore the original address */

    return 0;
}

void yuyv_to_gray(const uint8_t * src, int src_pitch,
                  int w, int h, uint8_t * dst, int dst_pitch)
{
    int y;

    for(y = 0; y < h; y++) {
        const uint8_t * yuv = src + y * src_pitch;
        uint8_t * gray = dst + y * dst_pitch;
        int x;

        for(x = 0; x < w; x += 2) {
            *(gray++) = yuv[0];
            *(gray++) = yuv[2];
            yuv += 4;
        }
    }
}

void bgra8888_to_gray(const uint8_t * src, int src_pitch,
                      int w, int h, uint8_t * dst, int dst_pitch)
{
    int y;

    for(y = 0; y < h; y++) {
        const uint8_t * rgb32 = src + src_pitch * y;
        uint8_t * gray = dst + y * dst_pitch;
        int i;

        for(i = 0; i < w; i++) {
            /* packed BGRA 8:8:8:8, 32bpp, (msb)8B 8G 8R 8A(lsb) */
            int r = (int)rgb32[2];
            int g = (int)rgb32[1];
            int b = (int)rgb32[0];
            int sum = 66 * r + 129 * g + 25 * b + 0x1080;

            *(gray++) = sum >> 8;
            rgb32 += 4;
        }
    }
}

void rgb565_to_gray(const uint8_t * src, int w, int h, uint8_t * dst)
{
    int y;

    for(y = 0; y < h; y++) {
        const uint16_t * rgb565 = (uint16_t *)(src + y * w * 2);
        uint8_t * gray = dst + y * w;
        int i;

        for(i = 0; i < w; i++) {
            /* packed RGB 5:6:5, 16bpp, (msb)5R 6G 5B(lsb) */
            int b = (int)(rgb565[i] >> 11) & 0x1F;
            int g = (int)(rgb565[i] >> 5) & 0x3F;
            int r = (int)rgb565[i] & 0x1F;
            int sum = 66 * r + 129 * g + 25 * b + 0x1080;

            *(gray++) = sum >> 8;
        }
    }
}

void rgb332_to_gray(const uint8_t * src, int w, int h, uint8_t * dst)
{
    int y;

    for(y = 0; y < h; y++) {
        const uint8_t * rgb332 = src + y * w;
        uint8_t * gray = dst + y * w;
        int i;

        for(i = 0; i < w; i++) {
            /* packed RGB 3:3:2,  8bpp, (msb)3R 3G 2B(lsb) */
            int r = (int)(rgb332[i] >> 5) & 0x7;
            int g = (int)(rgb332[i] >> 2) & 0x7;
            int b = (int)rgb332[i] & 0x3;
            int sum = 66 * r + 129 * g + 25 * b + 0x1080;

            *(gray++) = sum >> 8;
        }
    }
}


#endif /* LV_USE_QRSCAN */
