/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef LV_BUILD_FROGFS

#include "../../../lv_conf_internal.h"
#include LV_STDDEF_INCLUDE
#include LV_STDINT_INCLUDE
#include "../../../stdlib/lv_string.h"

#include "miniz.h"

#include "frogfs_priv.h"
#include "log.h"
#include "frogfs_format.h"
#include "frogfs/frogfs.h"


#define BUFFER_LEN 16

typedef struct {
    const void *data;
    tinfl_decompressor inflator;
    uint8_t buf[TINFL_LZ_DICT_SIZE];
    size_t buf_pos;
    size_t buf_len;
    size_t out_pos;
} priv_data_t;

static int open_miniz(frogfs_fh_t *f, unsigned int flags)
{
    priv_data_t *priv = lv_malloc(sizeof(priv_data_t));
    if (priv == NULL) {
        LV_LOG_ERROR("malloc failed");
        return -1;
    }

    const char *p = f->data_start;
    priv->data = f->data_start;
    if (*p == 0x78 && (*(p + 1) == 0x01 || *(p + 1) == 0x5e ||
            *(p + 1) == 0x9c || *(p + 1) == 0xda)) {
        /* zlib */
        priv->data += 2;
    } else if (*p == 0x1f && *(p + 1) == 0x8b) {
        /* gzip */
        if (*(p + 2) != 8) {
            LV_LOG_ERROR("unsupported gzip compression method");
            return -1;
        }
        priv->data += 10;
        if (p[3] & 4) {
            priv->data += 2;
        }
        if (p[3] & 8) {
            while (*(char *) priv->data) {
                priv->data++;
            }
            priv->data++;
        }
        if (p[3] & 16) {
            while (*(char *) priv->data) {
                priv->data++;
            }
            priv->data++;
        }
        if (p[3] & 2) {
            priv->data += 2;
        }
    } else {
        /* assume raw deflate stream */
    }

    f->data_ptr = priv->data;
    tinfl_init(&priv->inflator);
    priv->buf_pos = 0;
    priv->buf_len = 0;
    priv->out_pos = 0;
    f->decomp_priv = priv;
    return 0;
}

static void close_miniz(frogfs_fh_t *f)
{
    priv_data_t *priv = f->decomp_priv;
    lv_free(priv);
    f->decomp_priv = NULL;
}

static ssize_t read_miniz(frogfs_fh_t *f, void *buf, size_t len)
{
    priv_data_t *priv = f->decomp_priv;
    tinfl_status status;
    size_t start_len = len;
    size_t in_bytes;
    size_t out_bytes;

    while (len) {
        size_t chunk = len < priv->buf_len - priv->buf_pos ? len :
                priv->buf_len - priv->buf_pos;
        lv_memcpy(buf, priv->buf + priv->buf_pos, chunk);
        priv->buf_pos += chunk;
        priv->out_pos += chunk;
        buf += chunk;
        len -= chunk;

        if (priv->buf_len == priv->buf_pos) {
            priv->buf_len = 0;
            priv->buf_pos = 0;
        }

        in_bytes = f->data_sz - (f->data_ptr - priv->data);
        out_bytes = sizeof(priv->buf) - priv->buf_len;
        status = tinfl_decompress(&priv->inflator, f->data_ptr, &in_bytes,
                priv->buf, &priv->buf[priv->buf_len], &out_bytes, 0);
        f->data_ptr += in_bytes;
        priv->buf_len += out_bytes;

        if (status < TINFL_STATUS_DONE) {
            LV_LOG_ERROR("tinfl_decompress");
            return -1;
        }

        if (priv->buf_len - priv->buf_pos == 0) {
            break;
        }
    }

    return start_len - len;
}

static ssize_t seek_miniz(frogfs_fh_t *f, long offset, int mode)
{
    priv_data_t *priv = f->decomp_priv;
    const frogfs_comp_t *comp = (const void *) f->file;
    ssize_t new_pos = priv->out_pos;

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

    if (new_pos < priv->out_pos) {
        f->data_ptr = priv->data;
        tinfl_init(&priv->inflator);
        priv->buf_len = 0;
        priv->buf_pos = 0;
        priv->out_pos = 0;
    }

    while (new_pos > priv->out_pos) {
        uint8_t buf[BUFFER_LEN];
        size_t len = new_pos - priv->out_pos < BUFFER_LEN ?
                new_pos - priv->out_pos : BUFFER_LEN;

        ssize_t res = frogfs_read(f, buf, len);
        if (res < 0) {
            LV_LOG_ERROR("frogfs_read");
            return -1;
        }
    }

    return priv->out_pos;
}

static size_t tell_miniz(frogfs_fh_t *f)
{
    priv_data_t *priv = f->decomp_priv;
    return priv->out_pos;
}

const frogfs_decomp_funcs_t frogfs_decomp_miniz = {
    .open = open_miniz,
    .close = close_miniz,
    .read = read_miniz,
    .seek = seek_miniz,
    .tell = tell_miniz,
};

#else
/*This typedef exists purely to keep -Wpedantic happy when the file is empty.*/
typedef int _keep_pedantic_happy;
#endif /*LV_BUILD_FROGFS*/
