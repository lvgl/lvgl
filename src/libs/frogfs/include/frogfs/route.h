/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef LV_BUILD_FROGFS

#include "cwhttpd/httpd.h"


/*************************
 * \section Frogfs Routes
 *************************/

/**
 * \brief       Frogfs GET route handler
 */
cwhttpd_status_t frogfs_route_get(cwhttpd_conn_t *conn);

/**
 * \brief       Frogfs template route handler
 */
cwhttpd_status_t frogfs_route_tpl(cwhttpd_conn_t *conn);

/**
 * \brief       Frogfs index route handler
 */
cwhttpd_status_t frogfs_route_index(cwhttpd_conn_t *conn);

#endif /*LV_BUILD_FROGFS*/

#ifdef __cplusplus
} /* extern "C" */
#endif
