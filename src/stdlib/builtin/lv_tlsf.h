#include "../../lv_conf_internal.h"
#if LV_USE_STDLIB_MALLOC == LV_STDLIB_BUILTIN

#ifndef LV_TLSF_H
#define LV_TLSF_H

/*
** Two Level Segregated Fit memory allocator, version 3.1.
** Written by Matthew Conte
**  http://tlsf.baisoku.org
**
** Based on the original documentation by Miguel Masmano:
**  http://www.gii.upv.es/tlsf/main/docs
**
** This implementation was written to the specification
** of the document, therefore no GPL restrictions apply.
**
** Copyright (c) 2006-2016, Matthew Conte
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the copyright holder nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL MATTHEW CONTE BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "../../osal/lv_os.h"
#include "../../misc/lv_ll.h"
#include "../../misc/lv_types.h"

#if defined(__cplusplus)
extern "C" {
#endif

/* lv_tlsf_t: a TLSF structure. Can contain 1 to N pools. */
/* lv_pool_t: a block of memory that TLSF can manage. */
typedef void * lv_tlsf_t;
typedef void * lv_pool_t;

/* Create/destroy a memory pool. */
lv_tlsf_t lv_tlsf_create(void * mem);
lv_tlsf_t lv_tlsf_create_with_pool(void * mem, size_t bytes);
void lv_tlsf_destroy(lv_tlsf_t tlsf);
lv_pool_t lv_tlsf_get_pool(lv_tlsf_t tlsf);

/* Add/remove memory pools. */
lv_pool_t lv_tlsf_add_pool(lv_tlsf_t tlsf, void * mem, size_t bytes);
void lv_tlsf_remove_pool(lv_tlsf_t tlsf, lv_pool_t pool);

/* malloc/memalign/realloc/free replacements. */
void * lv_tlsf_malloc(lv_tlsf_t tlsf, size_t bytes);
void * lv_tlsf_memalign(lv_tlsf_t tlsf, size_t align, size_t bytes);
void * lv_tlsf_realloc(lv_tlsf_t tlsf, void * ptr, size_t size);
size_t lv_tlsf_free(lv_tlsf_t tlsf, const void * ptr);

/* Returns internal block size, not original request size */
size_t lv_tlsf_block_size(void * ptr);

/* Overheads/limits of internal structures. */
size_t lv_tlsf_size(void);
size_t lv_tlsf_align_size(void);
size_t lv_tlsf_block_size_min(void);
size_t lv_tlsf_block_size_max(void);
size_t lv_tlsf_pool_overhead(void);
size_t lv_tlsf_alloc_overhead(void);

/* Debugging. */
typedef void (*lv_tlsf_walker)(void * ptr, size_t size, int used, void * user);
void lv_tlsf_walk_pool(lv_pool_t pool, lv_tlsf_walker walker, void * user);
/* Returns nonzero if any internal consistency check fails. */
int lv_tlsf_check(lv_tlsf_t tlsf);
int lv_tlsf_check_pool(lv_pool_t pool);

#if defined(__cplusplus)
};
#endif

#endif /*LV_TLSF_H*/

#endif /*LV_STDLIB_BUILTIN*/
