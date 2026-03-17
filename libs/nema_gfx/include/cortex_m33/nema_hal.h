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


#ifndef NEMA_HAL_H__
#define NEMA_HAL_H__

#include "nema_sys_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct nema_buffer_t_ {
    int       size;                 /**< Size of buffer */
    int       fd;                   /**< File Descriptor of buffer */
    void     *base_virt;            /**< Virtual address of buffer */
    uintptr_t base_phys;            /**< Physical address of buffer */
} nema_buffer_t;

/** \brief Initialize system. Implementor defined. Called in nema_init()
 *
 * \param void
 * \return 0 if no errors occurred
 * \see nema_init()
 *
 */
int32_t nema_sys_init(void);


/** \brief Wait for interrupt from the GPU
 *
 * \param void
 * \return 0 on success
 *
 */
int nema_wait_irq(void);

/** \brief Wait for a Command List to finish
 *
 * \param cl_id Command List ID
 * \return 0 on success
 *
 */
int nema_wait_irq_cl(int cl_id);

/** \brief Wait for a Breakpoint
 *
 * \param cl_id Breakpoint ID
 * \return 0 on success
 *
 */
int nema_wait_irq_brk(int brk_id);

/** \brief Read Hardware register
 *
 * \param reg Register to read
 * \return Value read from the register
 * \see nema_reg_write
 *
 */
uint32_t nema_reg_read(uint32_t reg);

/** \brief Write Hardware Register
 *
 * \param reg Register to write
 * \param value Value to be written
 * \return void()
 * \see nema_reg_read()
 *
 */
void nema_reg_write(uint32_t reg, uint32_t value);

/** \brief Create memory buffer
 *
 * \param size Size of buffer in bytes
 * \return nema_buffer_t struct
 *
 */
nema_buffer_t nema_buffer_create(int size);

/** \brief Create memory buffer at a specific pool
 *
 * \param pool ID of the desired memory pool
 * \param size Size of buffer in bytes
 * \return nema_buffer_t struct
 *
 */
nema_buffer_t nema_buffer_create_pool(int pool, int size);

/** \brief Maps buffer
 *
 * \param bo Pointer to buffer struct
 * \return Virtual pointer of the buffer (same as in bo->base_virt)
 *
 */
void *nema_buffer_map(nema_buffer_t *bo);

/** \brief Unmaps buffer
 *
 * \param bo Pointer to buffer struct
 * \return void
 *
 */
void nema_buffer_unmap(nema_buffer_t *bo);

/** \brief Destroy/deallocate buffer
 *
 * \param bo Pointer to buffer struct
 * \return void
 *
 */
void nema_buffer_destroy(nema_buffer_t *bo);

/** \brief Get physical (GPU) base address of a given buffer
 *
 * \param bo Pointer to buffer struct
 * \return Physical base address of a given buffer
 *
 */
uintptr_t nema_buffer_phys(nema_buffer_t *bo);

/** \brief Write-back buffer from cache to main memory
 *
 * \param bo Pointer to buffer struct
 * \return void
 *
 */
void nema_buffer_flush(nema_buffer_t * bo);

/** \brief Allocate memory for CPU to use (typically, standard malloc() is called)
 *
 * \param size Size in bytes
 * \return Pointer to allocated memory (virtual)
 * \see nema_host_free()
 *
 */
void *nema_host_malloc(size_t size);

/** \brief Free memory previously allocated with nema_host_malloc()
 *
 * \param ptr Pointer to allocated memory (virtual)
 * \return void
 * \see nema_host_malloc()
 *
 */
void  nema_host_free(void *ptr );

/** \private */
typedef struct nema_ringbuffer_t_ {
    nema_buffer_t bo;
    int      offset;    //number of 32-bit entries
    int	     last_submission_id;
} nema_ringbuffer_t;


/** \brief Initialize Ring Buffer. Should be called from inside nema_sys_init().
 *   This is a private function, the user should never call it.
 *
 * \param *rb 	Pointer to nema_ring_buffer_t struct
 * \param reset Resets the Ring Buffer if non-zero
 * \return 		Negative number on error
 * \see nema_sys_init()
 *
 */
/** \private */
int nema_rb_init(nema_ringbuffer_t *rb, int reset);

#define MUTEX_RB     0
#define MUTEX_MALLOC 1
#define MUTEX_FLUSH  2
#define MUTEX_MAX    2

/** \brief Mutex Lock for multiple processes/threads
 *
 * \param MUTEX_RB or MUTEX_MALLOC
 * \return int
 *
 */
int nema_mutex_lock(int mutex_id);

/** \brief Mutex Unlock for multiple processes/threads
 *
 * \param MUTEX_RB or MUTEX_MALLOC
 * \return int
 *
 */
int nema_mutex_unlock(int mutex_id);

#ifdef __cplusplus
}
#endif

#endif
