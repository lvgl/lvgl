/* TSI 2023.xmo */
/*******************************************************************************
 * Copyright (c) 2023 Think Silicon Single Member PC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this header file and/or associated documentation files to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the
 * Materials, and to permit persons to whom the Materials are furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Materials.
 *
 * MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS
 * NEMAGFX API. THE UNMODIFIED, NORMATIVE VERSIONS OF THINK-SILICON NEMAGFX
 * SPECIFICATIONS AND HEADER INFORMATION ARE LOCATED AT:
 *   https://think-silicon.com/products/software/nemagfx-api
 *
 *  The software is provided 'as is', without warranty of any kind, express or
 *  implied, including but not limited to the warranties of merchantability,
 *  fitness for a particular purpose and noninfringement. In no event shall
 *  Think Silicon Single Member PC be liable for any claim, damages or other
 *  liability, whether in an action of contract, tort or otherwise, arising
 *  from, out of or in connection with the software or the use or other dealings
 *  in the software.
 ******************************************************************************/

#ifndef TSI_MALLOC_H__
#define TSI_MALLOC_H__

#ifdef __cplusplus
extern "C" {
#endif


#define tsi_malloc_init(base_virt, base_phys, size, reset) \
		tsi_malloc_init_pool(0, base_virt, base_phys, size, reset)

#define tsi_malloc(size) tsi_malloc_pool(0, size)

int   tsi_malloc_init_pool(	int pool,
						   	void *base_virt,
						   	uintptr_t base_phys,
						   	int size,
						   	int reset);

int   tsi_malloc_init_pool_aligned(	int pool,
						   	        void *base_virt,
						   	        uintptr_t base_phys,
						   	        int size,
						   	        int reset,
							        int alignment); /*alignment must be multiple of 4, otherwise it will be overwritten internaly to be multiple of 4*/

void *tsi_malloc_pool(int pool, int size);
void  tsi_free(void *ptr);
uintptr_t tsi_virt2phys(void *addr);
void *tsi_phys2virt(uintptr_t addr);


#ifdef __cplusplus
}
#endif

#endif
