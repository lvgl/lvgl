/****************************************************************************
 * Apache NuttX
 * fs/romfs/fs_romfs.h
 *
 * Copyright 2020 The Apache Software Foundation
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef ROMFS_H
#define ROMFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#if LV_USE_FS_ROMFS
// configurations
#define ROMFS_ENDIAN_LITTLE
//#define ROMFS_ENDIAN_BIG

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* Volume header (multi-byte values are big-endian) */

#define ROMFS_VHDR_ROM1FS  0  /*  0-7:  "-rom1fs-" */
#define ROMFS_VHDR_SIZE    8    /*  8-11: Number of accessible bytes in this fs. */
#define ROMFS_VHDR_CHKSUM  12 /* 12-15: Checksum of the first 512 bytes. */
#define ROMFS_VHDR_VOLNAME 16 /* 16-..: Zero terminated volume name, padded to 16 byte boundary. */

#define ROMFS_VHDR_MAGIC   "-rom1fs-" /* File header offset (multi-byte values are big-endian) */

#define ROMFS_FHDR_NEXT    0 /*  0-3:  Offset of the next file header
                                *        (zero if no more files) */
#define ROMFS_FHDR_INFO    4 /*  4-7:  Info for directories/hard links/
                                *        devices */
#define ROMFS_FHDR_SIZE    8 /*  8-11: Size of this file in bytes */
#define ROMFS_FHDR_CHKSUM  12 /* 12-15: Checksum covering the meta data,
                                *        including the file name, and
                                *        padding. */
#define ROMFS_FHDR_NAME    16 /* 16-..: Zero terminated volume name, padded
                                *        to 16 byte boundary. */

/* Bits 0-3 of the rf_next offset provide mode information.  These are the
 * values specified in
 */

#define RFNEXT_MODEMASK 7       /* Bits 0-2: Mode; bit 3: Executable */
#define RFNEXT_ALLMODEMASK 15   /* Bits 0-3: All mode bits */
#define RFNEXT_OFFSETMASK (~15) /* Bits n-3: Offset to next entry */

#define RFNEXT_HARDLINK 0  /* rf_info = Link destination file header */
#define RFNEXT_DIRECTORY 1 /* rf_info = First file's header */
#define RFNEXT_FILE 2      /* rf_info = Unused, must be zero */
#define RFNEXT_SOFTLINK 3  /* rf_info = Unused, must be zero */
#define RFNEXT_BLOCKDEV 4  /* rf_info = 16/16 bits major/minor number */
#define RFNEXT_CHARDEV 5   /* rf_info = 16/16 bits major/minor number */
#define RFNEXT_SOCKET 6    /* rf_info = Unused, must be zero */
#define RFNEXT_FIFO 7      /* rf_info = Unused, must be zero */
#define RFNEXT_EXEC 8      /* Modifier of RFNEXT_DIRECTORY and RFNEXT_FILE */

#define IS_MODE(rfn, mode) ((((uint32_t)(rfn)) & RFNEXT_MODEMASK) == (mode))
#define IS_HARDLINK(rfn) IS_MODE(rfn, RFNEXT_HARDLINK)
#define IS_DIRECTORY(rfn) IS_MODE(rfn, RFNEXT_DIRECTORY)
#define IS_FILE(rfn) IS_MODE(rfn, RFNEXT_FILE)
#define IS_SOFTLINK(rfn) IS_MODE(rfn, RFNEXT_SOFTLINK)
#define IS_BLOCKDEV(rfn) IS_MODE(rfn, RFNEXT_BLOCKDEV)
#define IS_CHARDEV(rfn) IS_MODE(rfn, RFNEXT_CHARDEV)
#define IS_SOCKET(rfn) IS_MODE(rfn, RFNEXT_SOCKET)
#define IS_FIFO(rfn) IS_MODE(rfn, RFNEXT_FIFO)
#define IS_EXECUTABLE(rfn) (((rfn)&RFNEXT_EXEC) != 0)

/* Alignment macros */

#define ROMFS_ALIGNMENT 16
#define ROMFS_MAXPADDING (ROMFS_ALIGNMENT - 1)
#define ROMFS_ALIGNMASK (~ROMFS_MAXPADDING)
#define ROMFS_ALIGNUP(addr) \
    ((((uint32_t)(addr)) + ROMFS_MAXPADDING) & ROMFS_ALIGNMASK)
#define ROMFS_ALIGNDOWN(addr) (((uint32_t)(addr)) & ROMFS_ALIGNMASK)

#define ROMFS_OK 1
#define ROMFS_ERROR -1

struct rom_fs;

typedef struct romfs_nodeinfo {
    struct romfs_nodeinfo ** child;      /* The node array for link to lower level */
    uint16_t count;   /* The count of node in child level */
    uint32_t offset;  /* Offset of real file header */
    uint32_t next;    /* Offset of the next file header+flags */
    uint32_t size;    /* Size (if file) */
    uint8_t namesize; /* The length of name of the entry */
    char name[1];     /* The name to the entry */
} romfs_nodeinfo_t;

typedef size_t (*romfs_backend_read_t)(struct rom_fs * fs, uint32_t offset, void * buf, uint32_t size);

typedef struct rom_fs {
    uint32_t start;
    uint32_t size;
    void * private_data;
    romfs_backend_read_t backend_read;
    struct romfs_nodeinfo * root; /* The node for root node */
} rom_fs_t;

typedef struct romfs_dir {
    uint32_t offset;
} romfs_dir_t;

typedef struct romfs_file {
    uint32_t size, curr_pos, offset;
} romfs_file_t;

int romfs_init(rom_fs_t * fs);                 // mount a volume

int romfs_open(rom_fs_t * fs, struct romfs_file * file, const char * path);
int romfs_close(rom_fs_t * fs, struct romfs_file * file);
int romfs_read(rom_fs_t * fs, struct romfs_file * file, void * buff, uint32_t nbytes);

off_t romfs_lseek(rom_fs_t * fs, struct romfs_file * file, uint32_t offset,
                  int seek_opt);

off_t romfs_tell(rom_fs_t * fs, struct romfs_file * file);

rom_fs_t * romfs_filebackend_init(const char * path, uint32_t offset);
void romfs_filebackend_exit(rom_fs_t * fs);
#endif /* LV_USE_FS_ROMFS */

#ifdef __cplusplus
}
#endif

#endif
