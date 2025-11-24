
/****************************************************************************
 * Apache NuttX
 * fs/romfs/fs_romfsutil.c
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

/**
 * @file romfs.c
 *
 */
#include "../../../lvgl.h"

#if LV_USE_FS_ROMFS

#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include "romfs.h"

#ifndef NAME_MAX
    #define NAME_MAX 255
#endif

typedef struct {
    uint32_t word0;
    uint32_t word1;
    uint32_t size;
    uint32_t checksum;
} romfs_super_block_t;

typedef struct {
    uint32_t next;
    uint32_t spec;
    uint32_t size;
    uint32_t checksum;
    char name[NAME_MAX + 1];     /* The name to the entry */
} romfs_node_t;

struct romfs_entryname {
    const char * re_name;
    size_t re_len;
};

#define NODEINFO_NINCR 4

#define __ftype(node) (__le(node.next) & 0x7)

static uint32_t __le(uint32_t v)
{
#ifdef ROMFS_ENDIAN_LITTLE
    return (v >> 24) | (v << 24) | ((v >> 8) & 0xff00) | ((v << 8) & 0xff0000);
#else
    return v;
#endif
}

static uint32_t __checksum(rom_fs_t * fs)
{
    uint32_t buf[128];
    uint32_t sum = 0;
    uint32_t chksum_size = fs->size >= 512 ? 512 : fs->size;
    fs->backend_read(fs, 0, buf, chksum_size);
    int i;

    for(i = 0; i < chksum_size / 4; i++)
        sum += __le(buf[i]);

    return sum;
}

static void get_file_name(rom_fs_t * fs, uint32_t offset, char * fname, int size)
{
    int len = 0;
    do {
        fs->backend_read(fs, offset + len, fname + len, ROMFS_ALIGNMENT);  // fname, only 16 chars available
        len += ROMFS_ALIGNMENT;
    } while(fname[len - 1] != '\0' && len < size);
}

static int romfs_cachenode(rom_fs_t * fs, uint32_t offset, uint32_t next,
                           uint32_t size, const char * name,
                           romfs_nodeinfo_t ** pnodeinfo);

static int cache_dentry_inited = 0;

static void * init_dentry_cache(void * arg)
{
    rom_fs_t * fs = (rom_fs_t *)arg;
    char name[NAME_MAX + 1];
    get_file_name(fs, ROMFS_ALIGNMENT, name, sizeof(name));

    romfs_cachenode(fs, ROMFS_ALIGNUP(ROMFS_ALIGNMENT + strlen(name) + 1), RFNEXT_DIRECTORY,
                    0, "", &fs->root);

    cache_dentry_inited = 1;

    return NULL;
}

int romfs_init(rom_fs_t * fs)
{
    romfs_super_block_t super;

    fs->backend_read(fs, 0, &super, sizeof(super));
    if(memcmp(&super, "-rom1fs-", 8) != 0) {
        LV_LOG_ERROR("romfs_init: not a romfs file\n");
        return -1;
    }

    fs->size = __le(super.size);
    if(__checksum(fs) != 0) {
        LV_LOG_ERROR("romfs_init: checksum error\n");
        return -1;
    }

    init_dentry_cache(fs);

    return 0;
}

static int romfs_nodeinfo_search(const void * a, const void * b)
{
    struct romfs_nodeinfo * nodeinfo = *(struct romfs_nodeinfo **)b;
    const struct romfs_entryname * entry = a;
    const char * name2 = nodeinfo->name;
    size_t len = nodeinfo->namesize;
    int ret;

    if(len > entry->re_len) {
        len = entry->re_len;
    }

    ret = strncmp(entry->re_name, name2, len);
    if(!ret) {
        if(entry->re_name[len] == '/' || entry->re_name[len] == '\0') {
            return name2[len] == '\0' ? 0 : -1;
        }
        else {
            return 1;
        }
    }

    return ret;
}

static int romfs_nodeinfo_compare(const void * a, const void * b)
{
    struct romfs_nodeinfo * nodeinfo = *(struct romfs_nodeinfo **)a;
    struct romfs_entryname entry;

    entry.re_name = nodeinfo->name;
    entry.re_len = nodeinfo->namesize;

    return romfs_nodeinfo_search(&entry, b);
}

static inline int romfs_searchdir(rom_fs_t * fs, const char * entryname,
                                  int entrylen,
                                  struct romfs_nodeinfo * nodeinfo)
{
    struct romfs_nodeinfo ** cnodeinfo;
    struct romfs_entryname entry;

    entry.re_name = entryname;
    entry.re_len = entrylen;
    cnodeinfo = bsearch(&entry, nodeinfo->child, nodeinfo->count,
                        sizeof(*nodeinfo->child), romfs_nodeinfo_search);
    if(cnodeinfo) {
        memcpy(nodeinfo, *cnodeinfo, sizeof(*nodeinfo));
        return 0;
    }
    // LV_LOG_ERROR("romfs_searchdir: %s not found\n", entryname);
    return -1;
}

static int romfs_finddirentry(rom_fs_t * fs, struct romfs_nodeinfo * nodeinfo,
                              const char * path)
{
    const char * entryname;
    const char * terminator;
    int entrylen;
    int ret;

    /* Start with the first element after the root directory */

    memcpy(nodeinfo, fs->root, sizeof(*nodeinfo));

    /* The root directory is a special case */

    if(!path || path[0] == '\0') {
        return 0;
    }

    /* Then loop for each directory/file component in the full path */

    entryname = path;
    terminator = NULL;

    for(;;) {
        /* Find the start of the next path component */

        while(*entryname == '/')
            entryname++;

        /* Find the end of the next path component */

        terminator = strchr(entryname, '/');
        if(!terminator) {
            entrylen = strlen(entryname);
        }
        else {
            entrylen = terminator - entryname;
        }

        if(entrylen == 0) {
            return 0;
        }
        /* Long path segment names will be truncated to NAME_MAX */

        if(entrylen > NAME_MAX) {
            entrylen = NAME_MAX;
        }

        /* Then find the entry in the current directory with the
        * matching name.
        */

        ret = romfs_searchdir(fs, entryname, entrylen, nodeinfo);
        if(ret < 0) {
            return ret;
        }

        /* Was that the last path component? */

        if(!terminator) {
            /* Yes.. return success */

            return ROMFS_OK;
        }

        /* No... If that was not the last path component, then it had
        * better have been a directory
        */

        if(!IS_DIRECTORY(nodeinfo->next)) {
            return -ENOTDIR;
        }

        /* Setup to search the next directory for the next component
        * of the path
        */

        entryname = terminator;
    }

    return ROMFS_ERROR; /* Won't get here */
}

static int romfs_cachenode(rom_fs_t * fs, uint32_t offset, uint32_t next,
                           uint32_t size, const char * name,
                           romfs_nodeinfo_t ** pnodeinfo)
{
    romfs_nodeinfo_t ** child;
    romfs_nodeinfo_t * nodeinfo;
    char * childname;

    uint32_t linkoffset;
    uint32_t info;
    uint16_t num = 0;
    size_t nsize;
    int ret;

    nsize = strlen(name);
    nodeinfo = malloc(sizeof(struct romfs_nodeinfo) + nsize);

    if(nodeinfo == NULL) {
        LV_LOG_ERROR("romfs_cachenode: malloc failed for %s\n", name);
        return -ENOMEM;
    }

    memset(nodeinfo, 0, sizeof(struct romfs_nodeinfo) + nsize);
    *pnodeinfo = nodeinfo;
    nodeinfo->offset = offset;
    nodeinfo->next = next;
    nodeinfo->namesize = nsize;
    strlcpy(nodeinfo->name, name, nsize + 1);
    // LV_LOG_WARN("romfs_cachenode: cached: %s\n", name);

    if(!IS_DIRECTORY(next)) {
        nodeinfo->size = size;
        return 0;
    }

    child = nodeinfo->child;
    do {
        romfs_node_t node;
        memset(&node, 0, sizeof(node));
        fs->backend_read(fs, offset, &node, sizeof(node));

        next = __le(node.next);
        info = __le(node.spec);
        size = __le(node.size);
        childname = node.name;
        if(strlen(childname) > NAME_MAX) {
            childname[NAME_MAX] = '\0';
        }
        linkoffset = offset;

        if(strcmp(childname, ".") != 0 && strcmp(childname, "..") != 0) {
            if(child == NULL || nodeinfo->count == num - 1) {
                void * tmp =
                    realloc(nodeinfo->child,
                            (num + NODEINFO_NINCR) * sizeof(*nodeinfo->child));
                if(tmp == NULL) {
                    LV_LOG_ERROR("romfs_cachenode: realloc failed for %s\n", childname);
                    return -ENOMEM;
                }

                nodeinfo->child = tmp;
                memset(nodeinfo->child + num, 0,
                       NODEINFO_NINCR * sizeof(*nodeinfo->child));
                num += NODEINFO_NINCR;
            }

            child = &nodeinfo->child[nodeinfo->count++];
            if(IS_DIRECTORY(next)) {
                linkoffset = info;
            }

            ret = romfs_cachenode(fs, linkoffset, next, size, childname, child);
            if(ret < 0) {
                LV_LOG_ERROR("romfs_cachenode: failed to cache %s\n", childname);
                nodeinfo->count--;
                return ret;
            }
        }

        next &= RFNEXT_OFFSETMASK;
        offset = next;
    } while(next != 0);

    if(nodeinfo->count > 1) {
        qsort(nodeinfo->child, nodeinfo->count, sizeof(*nodeinfo->child),
              romfs_nodeinfo_compare);
    }

    return 0;
}

static void romfs_freenode(struct romfs_nodeinfo * nodeinfo)
{
    int i;
    if(IS_DIRECTORY(nodeinfo->next)) {
        for(i = 0; i < nodeinfo->count; i++) {
            romfs_freenode(nodeinfo->child[i]);
        }

        free(nodeinfo->child);
    }

    free(nodeinfo);
}

int romfs_open(rom_fs_t * fs, struct romfs_file * file, const char * path)
{
    int ret = -1;

    if(fs == NULL) {
        LV_LOG_ERROR("romfs_open: fs is NULL");
        return -1;
    }

    while(!cache_dentry_inited) {
        LV_LOG_ERROR("romfs_open: cache not initialized");
        return -EAGAIN;
    }

    struct romfs_nodeinfo nodeinfo;

    memset(&nodeinfo, 0, sizeof(nodeinfo));

    ret = romfs_finddirentry(fs, &nodeinfo, path);
    if(ret < 0) {
        // LV_LOG_ERROR("romfs_open: finddirentry failed for %s\n", path);
        return ret;
    }

    file->size = nodeinfo.size;
    file->offset =
        ROMFS_ALIGNUP(nodeinfo.offset + 16 + nodeinfo.namesize + 1);
    file->curr_pos = 0;

    return 0;
}

int romfs_close(rom_fs_t * fs, struct romfs_file * file)
{
    if(file->offset == 0) {
        LV_LOG_ERROR("romfs_close: file not opened");
        return -1;
    }
    file->curr_pos = 0;
    file->offset = 0;
    return 0;
}

int romfs_read(rom_fs_t * fs, struct romfs_file * file, void * buf, uint32_t nbytes)
{
    int max_to_read;

    if(file->offset == 0) {
        LV_LOG_ERROR("romfs_read: file not opened");
        return -1;
    }

    max_to_read = file->size - file->curr_pos;
    if(max_to_read < 0)
        return 0;
    if(max_to_read > nbytes)
        max_to_read = nbytes;
    fs->backend_read(fs, file->offset + file->curr_pos, buf, max_to_read);
    file->curr_pos += max_to_read;

    return max_to_read;
}

off_t romfs_tell(rom_fs_t * fs, struct romfs_file * file)
{
    return file->curr_pos;
}

off_t romfs_lseek(rom_fs_t * fs, struct romfs_file * file, uint32_t offset, int whence)
{
    off_t position;

    switch(whence) {
        case SEEK_SET: /* The offset is set to offset bytes. */
            position = offset;
            break;

        case SEEK_CUR: /* The offset is set to its current location plus
                * offset bytes. */
            position = offset + file->curr_pos;
            break;

        case SEEK_END: /* The offset is set to the size of the file plus
                * offset bytes. */
            position = offset + file->size;
            break;

        default:
            return -EINVAL;
    }

    if(position > file->size) {
        position = file->size;
    }

    file->curr_pos = position;

    return position;
}

static int romfs_exit(rom_fs_t * fs)
{
    romfs_freenode(fs->root);
    fs->root = NULL;
    fs->private_data = NULL;
    cache_dentry_inited = 0;

    return 0;
}

static size_t file_backend_read(rom_fs_t * fs, uint32_t offset, void * buf, uint32_t size)
{
    int fd = (int)(fs->private_data);
    if(fs->private_data == NULL || fd < 0) {
        LV_LOG_ERROR("romfs_filebackend_read: fs or fd is invalid");
        return -1;
    }
    lseek(fd, fs->start + offset, SEEK_SET);
    return read(fd, buf, size);
}

rom_fs_t * romfs_filebackend_init(const char * path, uint32_t offset)
{
    rom_fs_t * fs = malloc(sizeof(rom_fs_t));

    int fd = open(path, O_RDONLY);

    if(fd < 0) {
        LV_LOG_ERROR("romfs_filebackend_init: failed to open file: %s\n", path);
        return NULL;
    }

    LV_LOG_WARN("romfs_filebackend_init: path: %s, offset: %lu, fd: %d\n", path, offset, fd);
    fs->start = offset;
    fs->private_data = (void *)fd;
    fs->backend_read = file_backend_read;

    if(romfs_init(fs) < 0) {
        close(fd);
        free(fs);
        LV_LOG_ERROR("romfs_filebackend_init: romfs_init failed\n");
        return NULL;
    }

    return fs;
}

void romfs_filebackend_exit(rom_fs_t * fs)
{
    if(fs == NULL) {
        return;
    }

    int fd = (int)(uintptr_t)(fs->private_data);
    if(fd > 0) {
        close(fd);
        fs->private_data = NULL;
    }
    romfs_exit(fs);

    free(fs);
}
#endif /* LV_USE_FS_ROMFS */
