/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef LV_BUILD_FROGFS

#include "../../../lv_conf_internal.h"
#include LV_STDDEF_INCLUDE
#include LV_STDINT_INCLUDE
#include "../../../stdlib/lv_string.h"

#include "heatshrink_decoder.h"

#include "frogfs_priv.h"
#include "log.h"
#include "frogfs_format.h"
#include "frogfs/frogfs.h"


#define BUFFER_LEN 16
#define PRIV(f) ((decomp_priv_t *)(f->decomp_priv))

typedef struct {
    heatshrink_decoder *hsd;
    size_t file_pos;
} decomp_priv_t;

static int open_heatshrink(frogfs_fh_t *f, unsigned int flags)
{
    const frogfs_comp_t *comp = (const frogfs_comp_t *) f->file;

    decomp_priv_t *data = lv_malloc(sizeof(decomp_priv_t));
    if (data == NULL) {
        LV_LOG_ERROR("malloc failed");
        return -1;
    }
    lv_memset(data, 0, sizeof(*data));

    uint8_t args = comp->entry.opts;
    uint8_t window = args & 0xf;
    uint8_t lookahead = args >> 4;

    data->hsd = heatshrink_decoder_alloc(BUFFER_LEN, window, lookahead);
    if (data->hsd == NULL) {
        LV_LOG_ERROR("error allocating heatshrink decoder");
        lv_free(data);
        return -1;
    }

    f->decomp_priv = data;
    return 0;
}

static void close_heatshrink(frogfs_fh_t *f)
{
    heatshrink_decoder_free(PRIV(f)->hsd);
    lv_free(PRIV(f));
    f->decomp_priv = NULL;
}

static ssize_t read_heatshrink(frogfs_fh_t *f, void *buf, size_t len)
{
    size_t rlen, decoded = 0;

    while (decoded < len) {
        /* feed data into the decoder */
        size_t remain = f->data_sz - (f->data_ptr - f->data_start);
        if (remain > 0) {
            HSD_sink_res res = heatshrink_decoder_sink(PRIV(f)->hsd,
                    (uint8_t *) f->data_ptr, (remain > BUFFER_LEN) ?
                    BUFFER_LEN : remain, &rlen);
            if (res < 0) {
                LV_LOG_ERROR("heatshrink_decoder_sink");
                return -1;
            }
            f->data_ptr += rlen;
        }

        /* poll decoder for data */
        HSD_poll_res res = heatshrink_decoder_poll(PRIV(f)->hsd,
                (uint8_t *) buf, len - decoded, &rlen);
        if (res < 0) {
            LV_LOG_ERROR("heatshrink_decoder_poll");
            return -1;
        }
        PRIV(f)->file_pos += rlen;
        buf += rlen;
        decoded += rlen;

        /* end of input data */
        if (remain == 0) {
            if (PRIV(f)->file_pos == f->real_sz) {
                HSD_finish_res res = heatshrink_decoder_finish(PRIV(f)->hsd);
                if (res < 0) {
                    LV_LOG_ERROR("heatshink_decoder_finish");
                    return -1;
                }
                LV_LOG_TRACE("heatshrink_decoder_finish");
            }
            return decoded;
        }
    }

    return len;
}

static ssize_t seek_heatshrink(frogfs_fh_t *f, long offset, int mode)
{
    const frogfs_comp_t *comp = (const frogfs_comp_t *) f->file;
    ssize_t new_pos = PRIV(f)->file_pos;

    if (mode == SEEK_SET) {
        if (offset < 0) {
            return -1;
        }
        if (offset > comp->real_sz) {
            offset = comp->real_sz;
        }
        new_pos = offset;
    } else if (mode == SEEK_CUR) {
        if (new_pos + offset < 0) {
            new_pos = 0;
        } else if (new_pos > comp->real_sz) {
            new_pos = comp->real_sz;
        } else {
            new_pos += offset;
        }
    } else if (mode == SEEK_END) {
        if (offset > 0) {
            return -1;
        }
        if (offset < -(ssize_t) comp->real_sz) {
            offset = 0;
        }
        new_pos = comp->real_sz + offset;
    } else {
        return -1;
    }

    if (new_pos < PRIV(f)->file_pos) {
        f->data_ptr = f->data_start;
        PRIV(f)->file_pos = 0;
        heatshrink_decoder_reset(PRIV(f)->hsd);
    }

    while (new_pos > PRIV(f)->file_pos) {
        uint8_t buf[BUFFER_LEN];
        size_t len = new_pos - PRIV(f)->file_pos < BUFFER_LEN ?
                new_pos - PRIV(f)->file_pos : BUFFER_LEN;

        ssize_t res = frogfs_read(f, buf, len);
        if (res < 0) {
            LV_LOG_ERROR("frogfs_read");
            return -1;
        }
    }

    return PRIV(f)->file_pos;
}

static size_t tell_heatshrink(frogfs_fh_t *f)
{
    return PRIV(f)->file_pos;
}

const frogfs_decomp_funcs_t frogfs_decomp_heatshrink = {
    .open = open_heatshrink,
    .close = close_heatshrink,
    .read = read_heatshrink,
    .seek = seek_heatshrink,
    .tell = tell_heatshrink,
};

#endif /*LV_BUILD_FROGFS*/
