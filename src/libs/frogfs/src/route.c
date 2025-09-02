/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef LV_BUILD_FROGFS

#include "../../../lv_conf_internal.h"
#include "../../../stdlib/lv_string.h"
#include LV_STDDEF_INCLUDE

#include "frogfs/route.h"
#include "frogfs/frogfs.h"
#include "log.h"
#include "cwhttpd/route.h"
#include "cwhttpd/httpd.h"


#define TRY(X) ({ \
    ssize_t n = X; \
    if (n < 0) { \
        r = CWHTTPD_STATUS_FAIL; \
        goto cleanup; \
    } \
    n; \
})

#define FILE_CHUNK_LEN (1024)

static cwhttpd_status_t get_filepath(cwhttpd_conn_t *conn, char *path,
        size_t len, frogfs_stat_t *s, const char *index)
{
    size_t out_len = 0;
    const char *url = conn->request.url;
    const cwhttpd_route_t *route = conn->route;
    const char *rpath = route->path;

    while (*url && *rpath == *url) {
        rpath++;
        url++;
    }

    if (route->argc < 1) {
        if (len > 0) {
            lv_strncpy(path, url, len - 1);
            path[len - 1] = '\0';
            out_len = lv_strlen(path);
            if (path[out_len - 1] == '/') {
                if (index == NULL) {
                    path[out_len - 1] = '\0';
                    out_len -= 1;
                } else if (len - out_len > 0) {
                        lv_strncpy(path + out_len, index, len - out_len - 1);
                        path[len - out_len - 1] = '\0';
                        out_len = lv_strlen(path);
                }
            }
        }
    } else if (len > 0) {
        lv_strncpy(path, route->argv[0], len - 1);
        path[len - 1] = '\0';
        out_len = lv_strlen(path);
        if (path[out_len - 1] == '/' && len - out_len > 0) {
            lv_strncpy(path + out_len, url, len - out_len - 1);
            path[len - out_len - 1] = '\0';
            out_len = lv_strlen(path);

            if (path[out_len - 1] == '/') {
                if (index == NULL) {
                    path[out_len - 1] = '\0';
                    out_len -= 1;
                } else if (len - out_len > 0) {
                    lv_strncpy(path + out_len, index, len - out_len - 1);
                    path[len - out_len - 1] = '\0';
                }
            }
        }
    }

    if (!frogfs_stat(conn->inst->frogfs, path, s)) {
        return CWHTTPD_STATUS_NOTFOUND;
    }

    if ((index == NULL) && (s->type == FROGFS_TYPE_DIR) && len - out_len > 0) {
        lv_strncpy(path + out_len, "/", len - out_len - 1);
        path[len - out_len - 1] = '\0';
        return CWHTTPD_STATUS_OK;
    }

    if (s->type == FROGFS_TYPE_FILE) {
        return CWHTTPD_STATUS_OK;
    }

    if (s->type == FROGFS_TYPE_DIR && len - out_len > 0) {
        char *p;

        lv_strncpy(path + out_len, "/", len - out_len - 1);
        path[len - out_len - 1] = '\0';
        out_len = lv_strlen(path);

        p = path + out_len;
        if (len - out_len > 0) {
            lv_strncpy(path + out_len, index, len - out_len - 1);
            path[len - out_len - 1] = '\0';
            if (!frogfs_stat(conn->inst->frogfs, path, s)) {
                return CWHTTPD_STATUS_NOTFOUND;
            }
            if (s->type == FROGFS_TYPE_FILE) {
                *p = '\0';
                cwhttpd_redirect(conn, path);
                return CWHTTPD_STATUS_DONE;
            }
        }
    }

    return CWHTTPD_STATUS_NOTFOUND;
}

cwhttpd_status_t frogfs_route_get(cwhttpd_conn_t *conn)
{
    cwhttpd_status_t r = CWHTTPD_STATUS_DONE;

    /* Only process GET requests, otherwise fallthrough */
    if (conn->request.method != CWHTTPD_METHOD_GET) {
        return CWHTTPD_STATUS_NOTFOUND;
    }

    /* We can use buf here because its not needed until reading data */
    char buf[FILE_CHUNK_LEN];
    frogfs_stat_t st;
    cwhttpd_status_t status = get_filepath(conn, buf, sizeof(buf), &st,
            "index.html");
    if (status != CWHTTPD_STATUS_OK) {
        return status;
    }

    frogfs_fh_t *f = frogfs_open(conn->inst->frogfs, buf);
    if (f == NULL) {
        return CWHTTPD_STATUS_NOTFOUND;
    }

    const char *mimetype = cwhttpd_get_mimetype(buf);

    bool gzip_encoding = (st.flags & FROGFS_FLAG_GZIP);
    if (gzip_encoding) {
        /* Check the request Accept-Encoding header for gzip. Return a 404
         * response if not present */
        const char *header = cwhttpd_get_header(conn, "Accept-Encoding");
        if (header && lv_strstr(header, "gzip") == NULL) {
            LV_LOG_WARN(__func__, "client does not accept gzip!");
            frogfs_close(f);
            TRY(cwhttpd_response(conn, 404));
            TRY(cwhttpd_send_header(conn, "Content-Type", "text/plain"));
            TRY(cwhttpd_send(conn, "only gzip file available", -1));
            return CWHTTPD_STATUS_DONE;
        }
    }

    cwhttpd_set_chunked(conn, false);
    TRY(cwhttpd_response(conn, 200));
    if (gzip_encoding) {
        TRY(cwhttpd_send_header(conn, "Content-Encoding", "gzip"));
    }
    if (!(conn->priv.flags & HFL_SEND_CHUNKED)) {
        snprintf(buf, sizeof(buf), "%d", st.size);
        TRY(cwhttpd_send_header(conn, "Content-Length", buf));
    }
    if (mimetype) {
        TRY(cwhttpd_send_header(conn, "Content-Type", mimetype));
    }
    if (st.flags & FROGFS_FLAG_CACHE) {
        TRY(cwhttpd_send_cache_header(conn, NULL));
    }

    ssize_t len;
    TRY(cwhttpd_chunk_start(conn, st.size));
    while ((len = frogfs_read(f, buf, sizeof(buf))) > 0) {
        TRY(cwhttpd_send(conn, buf, len));
    }
    TRY(cwhttpd_chunk_end(conn));

cleanup:
    frogfs_close(f);
    return r;
}

cwhttpd_status_t frogfs_route_tpl(cwhttpd_conn_t *conn)
{
    cwhttpd_status_t r = CWHTTPD_STATUS_DONE;

    /* Only process GET requests, otherwise fallthrough */
    if (conn->request.method != CWHTTPD_METHOD_GET) {
        return CWHTTPD_STATUS_NOTFOUND;
    }

    /* We can use buf here because its not needed until reading data */
    char buf[FILE_CHUNK_LEN];
    frogfs_stat_t st;
    if (!get_filepath(conn, buf, sizeof(buf), &st, "index.tpl")) {
        return CWHTTPD_STATUS_NOTFOUND;
    }

    if (st.flags & FROGFS_FLAG_GZIP) {
        LV_LOG_ERROR(__func__, "template has gzip encoding");
        return CWHTTPD_STATUS_NOTFOUND;
    }

    const char *mimetype = cwhttpd_get_mimetype(buf);

    frogfs_fh_t *f = frogfs_open(conn->inst->frogfs, buf);
    if (f == NULL) {
        return CWHTTPD_STATUS_NOTFOUND;
    }

    cwhttpd_response(conn, 200);
    if (mimetype) {
        cwhttpd_send_header(conn, "Content-Type", mimetype);
    }
    cwhttpd_send_cache_header(conn, mimetype);

    cwhttpd_tpl_cb_t cb = conn->route->argv[1];
    void *user = NULL;
    size_t len;
    int token_pos = -1;
    char token[32];
    do {
        len = frogfs_read(f, buf, FILE_CHUNK_LEN);
        int raw_count = 0;
        uint8_t *p = (uint8_t *) buf;
        if (len > 0) {
            for (size_t i = 0; i < len; i++) {
                if (token_pos < 0) {
                    /* we're on ordinary text */
                    if (buf[i] == '%') {
                        /* send collected raw data */
                        if (raw_count != 0) {
                            TRY(cwhttpd_send(conn, p, raw_count));
                            raw_count = 0;
                        }
                        /* start collecting token chars */
                        token_pos = 0;
                    } else {
                        raw_count++;
                    }
                } else {
                    /* we're in token text */
                    if (buf[i] == '%') {
                        if (token_pos == 0) {
                            /* this is an escape sequence */
                            TRY(cwhttpd_send(conn, "%", 1));
                        } else {
                            /* this is a token */
                            token[token_pos] = '\0'; /* zero terminate */
                            cb(conn, token, &user);
                        }

                        /* collect normal characters again */
                        p = (uint8_t *) &buf[i + 1];
                        token_pos = -1;
                    } else {
                        if (token_pos < (sizeof(token) - 1)) {
                            token[token_pos++] = buf[i];
                        }
                    }
                }
            }
        }

        /* send remainder */
        if (raw_count != 0) {
            TRY(cwhttpd_send(conn, p, raw_count));
        }
    } while (len == FILE_CHUNK_LEN);

cleanup:
    /* we're done */
    cb(conn, NULL, &user);
    frogfs_close(f);
    return r;
}

cwhttpd_status_t frogfs_route_index(cwhttpd_conn_t *conn)
{
    cwhttpd_status_t r = CWHTTPD_STATUS_DONE;

    /* Only process GET requests, otherwise fallthrough */
    if (conn->request.method != CWHTTPD_METHOD_GET) {
        return CWHTTPD_STATUS_NOTFOUND;
    }

    char buf[FILE_CHUNK_LEN];
    frogfs_stat_t st;
    cwhttpd_status_t status = get_filepath(conn, buf, sizeof(buf), &st, NULL);
    if (status != CWHTTPD_STATUS_OK) {
        return status;
    }

    if (st.type != FROGFS_TYPE_DIR) {
        return CWHTTPD_STATUS_NOTFOUND;
    }

    size_t len = lv_strlen(conn->request.url);
    if (conn->request.url[len - 1] != '/') {
        lv_strncpy(buf, conn->request.url, sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';
        if (sizeof(buf) - len > 0) {
            lv_strncpy(buf + len, "/", sizeof(buf) - len - 1);
            buf[sizeof(buf) - len - 1] = '\0';
        }
        cwhttpd_redirect(conn, buf);
        return CWHTTPD_STATUS_DONE;
    }

    const char *parent = frogfs_path_from_index(conn->inst->frogfs, st.index);
    uint16_t start_index = 0;
    uint16_t current_index = start_index = st.index;
    bool files = false;

    cwhttpd_response(conn, 200);
    cwhttpd_send_header(conn, "Content-Type", "text/html");

    TRY(cwhttpd_sendf(conn,
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "<meta charset=\"UTF-8\">\n"
            "<title>Index of %H</title>\n"
            "</head>\n"
            "<body>\n"
            "<h1>Index of %H</h1>\n"
            "<pre>\n"
            "[DIR ]          <a href=\"./\">./</a>\n"
            "[DIR ]          <a href=\"../\">../</a>\n",
            conn->request.url, conn->request.url));

    do {
        const char *path = frogfs_path_from_index(conn->inst->frogfs, current_index);
        if (path != NULL) {
            frogfs_stat(conn->inst->frogfs, path, &st);
        }

        if (path == NULL && !files) {
            files = true;
            current_index = start_index;
            path = frogfs_path_from_index(conn->inst->frogfs, current_index);
            frogfs_stat(conn->inst->frogfs, path, &st);
        } else if (path == NULL && files) {
            break;
        }

        if (path == parent) {
            current_index++;
            continue;
        }

        const char *p = parent;
        while (*p && *p++ == *path++);

        if (!files && st.type == FROGFS_TYPE_DIR) {
            if (path[0] != '/') {
                files = true;
                current_index = start_index;
                continue;
            }
            path++;
            if (lv_strchr(path, '/')) {
                current_index++;
                continue;
            }
            TRY(cwhttpd_sendf(conn,
                    "[DIR ]          <a href=\"%H/\">%H/</a>\n", path, path));
            current_index++;
        } else if (files && st.type == FROGFS_TYPE_FILE) {
            if (path[0] != '/') {
                break;
            }
            path++;
            if (lv_strchr(path, '/')) {
                current_index++;
                continue;
            }
            TRY(cwhttpd_sendf(conn, "[FILE] %-8d <a href=\"%H\">%H</a>\n",
                    st.size, path, path));
            current_index++;
        }
        current_index++;
    } while (true);

    TRY(cwhttpd_send(conn, "</pre>\n</body>\n</html>\n", -1));

cleanup:
    return r;
}

#endif /*LV_BUILD_FROGFS*/
