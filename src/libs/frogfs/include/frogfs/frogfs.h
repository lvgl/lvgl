/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../../lv_conf_internal.h"
#include LV_STDDEF_INCLUDE
#include LV_STDINT_INCLUDE
#include "frogfs_types.h"

/**
 * \brief       Magic number used in the frogfs file header
 */
#define FROGFS_MAGIC 0x474F5246 /** FROG */

/**
 * \brief       Major version this source distribution supports
 */
#define FROGFS_VER_MAJOR 1

/**
 * \brief       Minor version this source distribution supports
 */
#define FROGFS_VER_MINOR 0

/**
 * \brief       Flag for \a frogfs_open to open any file as raw. Useful to
 *              pass compressed data over a transport such as HTTP.
 */
#define FROGFS_OPEN_RAW (1 << 0)

/**
 * \brief       Enum of frogfs entry types
 */
typedef enum frogfs_entry_type_t {
    FROGFS_ENTRY_TYPE_DIR,
    FROGFS_ENTRY_TYPE_FILE,
} frogfs_entry_type_t;

/**
 * \brief       Compression algorithm ids
 */
typedef enum frogfs_comp_algo_t {
    FROGFS_COMP_ALGO_NONE,
    FROGFS_COMP_ALGO_ZLIB,
    FROGFS_COMP_ALGO_HEATSHRINK,
    FROGFS_COMP_ALGO_GZIP,
} frogfs_comp_algo_t;

/**
 * \brief       Configuration for the \a frogfs_init function
 */
typedef struct frogfs_config_t {
    const void *addr; /**< address of an frogfs filesystem in memory */
} frogfs_config_t;

/**
 * \brief       A frogfs filesystem handle
 */
typedef struct frogfs_fs_t frogfs_fs_t;

/**
 * \brief       Structure filled by the \a frogfs_stat function
 */
typedef struct frogfs_stat_t {
    frogfs_entry_type_t type; /**< entry type */
    size_t size; /**< uncompressed file size */
    frogfs_comp_algo_t compression; /**< compression type */
    size_t compressed_sz; /**< compressed file size */
} frogfs_stat_t;

/**
 * \brief       Fiilesystem entry pointer
*/
typedef struct frogfs_entry_t frogfs_entry_t;
typedef struct frogfs_dh_t frogfs_dh_t;
typedef struct frogfs_fh_t frogfs_fh_t;

#if !defined(FROGFS_PRIVATE_STRUCTS)
/**
 * \brief       A frogfs directory handle
 */
struct frogfs_dh_t {
    const frogfs_fs_t *fs; /**< filesystem handle */
    frogfs_entry_t *entry; /**< directory entry */
};

/**
 * \brief       A frogfs file handle
 */
struct frogfs_fh_t {
    const frogfs_fs_t *fs; /**< filesystem handle */
    frogfs_entry_t *entry; /**< file entry */
};
#endif

/**
 * \brief      Initialize and return a \a frogfs_fs_t instance
 * \param[in]  config   frogfs configuration
 * \return              \a frogfs_fs_t pointer or \a NULL on error
 */
frogfs_fs_t *frogfs_init(const frogfs_config_t *conf);

/**
 * \brief      Tear down a \a frogfs_fs_t instance
 * \param[in]  fs       \a frogfs_fs_t pointer
 */
void frogfs_deinit(frogfs_fs_t *fs);

/**
 * \brief       Get frogfs entry for path
 * \param[in]   fs      \a frogfs_fs_t pointer
 * \param[in]   path    path string
 * \return              \a frogfs_entry_t pointer or \a NULL if path was not
 *                      found
 */
const frogfs_entry_t *frogfs_get_entry(const frogfs_fs_t *fs,
        const char *path);

/**
 * \brief       Get name for frogfs entry
 * \param[in]   entry   \a frogfs_entry_t pointer
 * \return              name string, caller is expected to free
 */
char *frogfs_get_name(const frogfs_entry_t *entry);

/**
 * \brief       Get full path for frogfs entry
 * \param[in]   fs      \a frogfs_fs_t pointer
 * \param[in]   entry   \a frogfs_entry_t pointer
 * \return              full path string or \a NULL if entry is NULL, caller is
 *                      expected to free
 */
char *frogfs_get_path(const frogfs_fs_t *fs, const frogfs_entry_t *entry);

/**
 * \brief       Return if entry is a directory
 * \param[in]   entry   \a frogfs_entry_t pointer
 * \return              1 if directory, 0 otherwise
 */
int frogfs_is_dir(const frogfs_entry_t *entry);

/**
 * \brief       Return if entry is a file
 * \param[in]   entry   \a frogfs_entry_t pointer
 * \return              1 if file, 0 otherwise
 */
int frogfs_is_file(const frogfs_entry_t *entry);

/**
 * \brief       Get information about a frogfs entry
 * \param[in]   fs      \a frogfs_fs_t pointer
 * \param[in]   entry   \a frogfs_entry_t pointer
 * \param[out]  st      \a frogfs_stat_t structure
 */
void frogfs_stat(const frogfs_fs_t *fs, const frogfs_entry_t *entry,
        frogfs_stat_t *st);

/**
 * \brief       Open a frogfs entry as a file from a \a frogfs_fs_t instance
 * \param[in]   fs      \a frogfs_fs_t poitner
 * \param[in]   entry   \a frogfs_entry_t pointer
 * \param[in]   flags   open flags
 * \return              \a frogfs_fh_t or \a NULL if not found
 */
frogfs_fh_t *frogfs_open(const frogfs_fs_t *fs, const frogfs_entry_t *entry,
        unsigned int flags);

/**
 * \brief       Close an open file entry
 * \param[in]   f       \a frogfs_fh_t pointer
 */
void frogfs_close(frogfs_fh_t *fh);

/**
 * \brief       Determine if file handle is opened raw.
 * \param[in]   f       \a frogfs_fh_t pointer
 * \return              1 if file is open raw, 0 otherwise
*/
int frogfs_is_raw(frogfs_fh_t *fh);

/**
 * \brief       Read data from an open file entry
 * \param[in]   f       \a frogfs_fh_t pointer
 * \param[out]  buf     buffer to read into
 * \param[in]   len     maximum number of bytes to read
 * \return              actual number of bytes read, zero if end of file
 *                      reached
 */
ssize_t frogfs_read(frogfs_fh_t *fh, void *buf, size_t len);

/**
 * \brief       Seek to a position within an open file entry
 * \param[in]   f       \a frogfs_fh_t pointer
 * \param[in]   offset  file position (relative or absolute)
 * \param[in]   mode    \a SEEK_SET, \a SEEK_CUR, or \a SEEK_END
 * \return              current position in file or < 0 upon error
 */
ssize_t frogfs_seek(frogfs_fh_t *fh, long offset, int mode);

/**
 * \brief       Get the current position in an open file entry
 * \param[in]   f       \a frogfs_fh_t pointer
 * \return              current position in file or < 0 upon error
 */
size_t frogfs_tell(frogfs_fh_t *fh);

/**
 * \brief       Get raw memory for raw file entry
 * \param[in]   f       \a frogfs_fh_t pointer
 * \param[out]  buf     pointer pointer to buf
 * \return              length of raw data
 */
size_t frogfs_access(frogfs_fh_t *fh, const void **buf);

/**
 * \brief       Open a directory for reading child entrys
 * \param[in]   fs      \a frogfs_fs_t pointer
 * \param[in]   entry   \a frogfs_entry_t pointer to root director
 * \return              \a frogfs_dh_t pointer or \a NULL if invalid
 */
frogfs_dh_t *frogfs_opendir(frogfs_fs_t *fs, const frogfs_entry_t *entry);

/**
 * \brief       Close a directory
 * \param[in]   d       \a frogfs_dh_t pointer
 */
void frogfs_closedir(frogfs_dh_t *dh);

/**
 * \brief       Get the next child entry in directory
 * \param[in]   d       \a frogfs_dh_t pointer
 * \return              \a frogfs_entry_t pointer or \a NULL if end has been
 *                      reached
 */
const frogfs_entry_t *frogfs_readdir(frogfs_dh_t *dh);

/**
 * \brief       Set dir entry index to a value returned by \a frogfs_telldir
 *              for the current \a frogfs_dh_t pointer
 * \param[in]   d       \a frogfs_dh_t pointer
 * \param[in]   loc     entry index
 */
void frogfs_seekdir(frogfs_dh_t *dh, long loc);

/**
 * \brief       Return the current entry index for a directory
 * \param[in]   d       \a frogfs_dh_t pointer
 * \return              entry index
 */
long frogfs_telldir(frogfs_dh_t *dh);

#ifdef __cplusplus
} /* extern "C" */
#endif
