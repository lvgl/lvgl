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
#ifndef __LV_QR_SCAN_H__
#define __LV_QR_SCAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"
#if LV_USE_QRSCAN

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * create the scanner
 * @return the scanner handle, NULL means failed
 */
void * lv_qrscan_create(void);

/**
 * destroy the scanner
 * @param handle pointer to the initialized scanner
 */
void lv_qrscan_destory(void * handle);

/**
 * decode the gray image buffer and store result in the out buff
 * @param handle pointer to the initialized scanner
 * @param width width of the gray image
 * @param height height of the gray image
 * @param in pointer to the gray image buffer
 * @param out address of pointer to the buffer which used to store the result
 * @return 0: scan successfully
 *         -ENOMEM: no memory
 *         -ENOBUFS: buffer full
 *         -EINVAL: invalid parameter
 *         -EPERM: the scanner is not available
 */
int lv_qrscan_scan(void * handle, int width, int height,
                   uint8_t * in, char ** out);

/**
 * convert to gray image from YUYV
 * @param src pointer to the YUYV buffer
 * @param src_pitch pitch between lines(YUYV: w*4)
 * @param w width of the YUYV image
 * @param h height of the YUYV image
 * @param dst pointer to the gray image buffer
 * @param dst_pitch pitch between lines(gray: w)
 */
void yuyv_to_gray(const uint8_t * src, int src_pitch,
                  int w, int h, uint8_t * dst, int dst_pitch);

/**
 * convert to gray image from bgra8888
 * @param src pointer to the bgra8888 buffer
 * @param src_pitch pitch between lines(bgra8888: w*4)
 * @param w width of the bgra8888 image
 * @param h height of the bgra8888 image
 * @param dst pointer to the gray image buffer
 * @param dst_pitch pitch between lines(gray: w)
 */
void bgra8888_to_gray(const uint8_t * src, int src_pitch,
                      int w, int h, uint8_t * dst, int dst_pitch);

/**
 * convert to gray image from rgb565
 * @param src pointer to the rgb565 buffer
 * @param w width of the rgb565 image
 * @param h height of the rgb565 image
 * @param dst pointer to the gray image buffer
 */
void rgb565_to_gray(const uint8_t * src, int w, int h, uint8_t * dst);

/**
 * convert to gray image from rgb332
 * @param src pointer to the rgb332 buffer
 * @param w width of the rgb332 image
 * @param h height of the rgb332 image
 * @param dst pointer to the gray image buffer
 */
void rgb332_to_gray(const uint8_t * src, int w, int h, uint8_t * dst);

#endif /* LV_USE_QRSCAN  */

#ifdef __cplusplus
}
#endif

#endif /* __LV_QR_SCAN_H__ */
