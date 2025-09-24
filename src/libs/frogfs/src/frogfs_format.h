/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include "../../../lv_conf_internal.h"
#include LV_STDINT_INCLUDE


/**
 * \brief       Is entry a directory?
 */
#define FROGFS_IS_DIR(e) (e->u.child_count < 0xFF00)

/**
 * \brief       Is entry a file?
 */
#define FROGFS_IS_FILE(e) (e->u.child_count >= 0xFF00)

/**
 * \brief       Is entry a compressed file?
 */
#define FROGFS_IS_COMP(e) (e->u.child_count > 0xFF00)

/**
 * \brief       Filesystem header
 */
typedef struct frogfs_head_t {
    uint32_t magic; /**< filesystem magic */
    uint8_t ver_major; /**< major version */
    uint8_t ver_minor; /**< minor version */
    uint16_t num_entries; /** entry count */
    uint32_t bin_sz; /**< binary length */
} frogfs_head_t;

/**
 * \brief       Hash table entry
 */
typedef struct frogfs_hash_t {
    uint32_t hash; /**< path hash */
    uint32_t offs; /**< object offset */
} frogfs_hash_t;

/**
 * \brief       Entry header
 */
struct frogfs_entry_t {
    uint32_t parent; /**< parent entry offset */
    union {
        uint16_t child_count; /**< child entry count */
        uint8_t compression; /**< compression algorithm */
    } u;
    uint8_t seg_sz; /**< path segment size (before alignment) */
    uint8_t opts; /**< compression opts */
};

/**
 * \brief       Directory object header
 */
typedef struct frogfs_dir_t {
    const frogfs_entry_t entry;
    uint32_t children[];
} frogfs_dir_t;

/**
 * \brief       File object header
 */
typedef struct frogfs_file_t {
    const frogfs_entry_t entry;
    uint32_t data_offs;
    uint32_t data_sz;
} frogfs_file_t;

/**
 * \brief       Compressed file object header
 */
typedef struct frogfs_comp_t {
    const frogfs_entry_t entry;
    uint32_t data_offs;
    uint32_t data_sz; /**< data size (before alignment) */
    uint32_t real_sz; /**< expanded size */
} frogfs_comp_t;

/**
 * \brief       Filesystem footer
 */
typedef struct frogfs_foot_t {
    uint32_t crc32; /**< crc32 of entire file without this field */
} frogfs_foot_t;
