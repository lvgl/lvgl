/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifdef LV_BUILD_FROGFS


#if defined(ESP_PLATFORM)
# include "sdkconfig.h"
#endif

#if !defined(CONFIG_FROGFS_USE_MINIZ)
#define CONFIG_FROGFS_USE_MINIZ 0
#endif

#if !defined(CONFIG_FROGFS_USE_ZLIB)
#define CONFIG_FROGFS_USE_ZLIB 0
#endif

#if !defined(CONFIG_FROGFS_USE_HEATSHRINK)
#define CONFIG_FROGFS_USE_HEATSHRINK 0
#endif

#if !defined(CONFIG_FROGFS_LOG_LEVEL_NONE) || \
    !defined(CONFIG_FROGFS_LOG_LEVEL_ERROR) || \
    !defined(CONFIG_FROGFS_LOG_LEVEL_WARN) || \
    !defined(CONFIG_FROGFS_LOG_LEVEL_INFO) || \
    !defined(CONFIG_FROGFS_LOG_LEVEL_DEBUG) || \
    !defined(CONFIG_FROGFS_LOG_LEVEL_VERBOSE)
#define CONFIG_FROGFS_LEVEL_WARN 1
#endif

#endif /*LV_BUILD_FROGFS*/
