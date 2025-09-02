/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef LV_BUILD_FROGFS

#include "../../../lv_conf_internal.h"
#include LV_STDDEF_INCLUDE
#include LV_STDINT_INCLUDE
#include "../../../stdlib/lv_string.h"

#define ZLIB_CONST
#include "zlib.h"

#include "frogfs_priv.h"
#include "log.h"
#include "frogfs_format.h"
#include "frogfs/frogfs.h"


#define BUFFER_LEN 16
#define STREAM(f) ((z_stream *)(f->decomp_priv))

static int open_zlib(frogfs_fh_t *f, unsigned int flags)
{
    int ret;

    z_stream *stream = lv_malloc(sizeof(z_stream));
    if (stream == NULL) {
        LV_LOG_ERROR("malloc failed");
        return -1;
    }
    lv_memset(stream, 0, sizeof(*stream));

    ret = inflateInit2(stream, MAX_WBITS | 32);
    if (ret != Z_OK) {
        LV_LOG_ERROR("error allocating zlib stream");
        return -1;
    }

    f->decomp_priv = stream;
    return 0;
}

static void close_zlib(frogfs_fh_t *f)
{
    z_stream *stream = STREAM(f);
    inflateEnd(stream);
    lv_free(stream);
    f->decomp_priv = NULL;
}

static ssize_t read_zlib(frogfs_fh_t *f, void *buf, size_t len)
{
    size_t start_in, start_out;
    int ret;

    if (STREAM(f)->total_out == f->real_sz) {
        return 0;
    }

    start_in = STREAM(f)->total_in;
    start_out = STREAM(f)->total_out;

    while (STREAM(f)->total_in < f->data_sz &&
            STREAM(f)->total_out - start_out < len) {
        STREAM(f)->next_in = f->data_ptr;
        STREAM(f)->avail_in = f->data_sz - \
                (f->data_ptr - f->data_start);
        STREAM(f)->next_out = buf;
        STREAM(f)->avail_out = len;

        ret = inflate(STREAM(f), Z_NO_FLUSH);
        if (ret < 0) {
            LV_LOG_ERROR("inflate");
            return -1;
        }
        f->data_ptr += STREAM(f)->total_in - start_in;
        if (ret == Z_STREAM_END) {
            break;
        }
    }

    return STREAM(f)->total_out - start_out;
}

static ssize_t seek_zlib(frogfs_fh_t *f, long offset, int mode)
{
    const frogfs_comp_t *comp = (const void *) f->file;
    ssize_t new_pos = STREAM(f)->total_out;

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

    if (new_pos < STREAM(f)->total_out) {
        f->data_ptr = f->data_start;
        inflateReset(STREAM(f));
    }

    while (new_pos > STREAM(f)->total_out) {
        uint8_t buf[BUFFER_LEN];
        size_t len = new_pos - STREAM(f)->total_out < BUFFER_LEN ?
                new_pos - STREAM(f)->total_out : BUFFER_LEN;

        ssize_t res = frogfs_read(f, buf, len);
        if (res < 0) {
            LV_LOG_ERROR("frogfs_read");
            return -1;
        }
    }

    return STREAM(f)->total_out;
}

static size_t tell_zlib(frogfs_fh_t *f)
{
    return STREAM(f)->total_out;
}

const frogfs_decomp_funcs_t frogfs_decomp_zlib = {
    .open = open_zlib,
    .close = close_zlib,
    .read = read_zlib,
    .seek = seek_zlib,
    .tell = tell_zlib,
};

#else
/*This typedef exists purely to keep -Wpedantic happy when the file is empty.*/
typedef int _keep_pedantic_happy;
#endif /*LV_BUILD_FROGFS*/
