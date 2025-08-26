/****************************************************************************
*
*    The MIT License (MIT)
*
*    Copyright (c) 2014 - 2022 Vivante Corporation
*
*    Permission is hereby granted, free of charge, to any person obtaining a
*    copy of this software and associated documentation files (the "Software"),
*    to deal in the Software without restriction, including without limitation
*    the rights to use, copy, modify, merge, publish, distribute, sublicense,
*    and/or sell copies of the Software, and to permit persons to whom the
*    Software is furnished to do so, subject to the following conditions:
*
*    The above copyright notice and this permission notice shall be included in
*    all copies or substantial portions of the Software.
*
*    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
*    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
*    DEALINGS IN THE SOFTWARE.
*
*****************************************************************************
*
*    The GPL License (GPL)
*
*    Copyright (C) 2014 - 2022 Vivante Corporation
*
*    This program is free software; you can redistribute it and/or
*    modify it under the terms of the GNU General Public License
*    as published by the Free Software Foundation; either version 2
*    of the License, or (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program; if not, write to the Free Software Foundation,
*    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*****************************************************************************
*
*    Note: This software is released under dual MIT and GPL licenses. A
*    recipient may use this file under the terms of either the MIT license or
*    GPL License. If you wish to use only one license not the other, you can
*    indicate your decision by deleting one of the above license notices in your
*    version of this file.
*
*****************************************************************************/

#ifndef VG_LITE_HAL_H
#define VG_LITE_HAL_H

#include "../../../lv_conf_internal.h"
#if LV_USE_VG_LITE_DRIVER

#define VGLITE_MEM_ALIGNMENT      128

#define VGLITE_EVENT_FRAME_END    2

#ifdef __cplusplus
extern "C" {
#endif
/*!
 @brief Wait a number of milliseconds.

 @discussion
 The VGLite hardware requires some waiting when changing clock frequencies or issuing a reset. This is the wrapper function
 for the delay function.

 @param milliseconds
 The number of milliseconds to wait.
 */
void vg_lite_hal_delay(uint32_t milliseconds);

/*!
 @brief Initialize the hardware.

 @discussion
 The VGLite kernel knows how to program its own hardware, but in any SOC there might be additional control required for
 turning on the power or initializing the clocks. This function gets called by the VGLite kernel before the VGLite graphics
 hardware gets initialized by the VGLite kernel itself and allows for SOC power management control.

 The implementer should make sure that on exit of this function the power and clock to the VGLite graphics hardware is
 turned on and stable.
 */
void vg_lite_hal_initialize(void);

/*!
 @brief Uninitialize the hardware.

 @discussion
 The VGLite kernel knows how to program its own hardware, but in any SOC there might be additional control required for
 turning off the power or uninitializing the clocks. This function gets called by the VGLite kernel after the VGLite
 graphics hardware gets uninitialized by the VGLite kernel itself and allows for SOC power management control.

 On exit of this function it is okay to have the power and/or clock to the VGLite graphics hardware turned off.
 */
void vg_lite_hal_deinitialize(void);

/*!
 @brief Allocate contiguous video memory.

 @discussion
 Any memory the VGLite graphics hardware will see should be allocated as contiguous memory. Any allocated memory will be
 addressed through an opaque handle, usually a pointer to an opaque structure. The porting layer can put any information it
 needs inside this structure.

 @param size
 The number of bytes to allocate.

 @param pool
 select the reserved memory pool

 @param logical
 A pointer to a variable that will receive the logical address of the allocated memory for the CPU.

 @param gpu
 A pointer to a variable that will receive the physical address of the allocated memory for the VGLite graphics hardware.

 @result
 A pointer to an opaque structure that will be used as the memory handle. NULL should be returned if there is not
 enough memory.
 */
vg_lite_error_t vg_lite_hal_allocate_contiguous(unsigned long size, vg_lite_vidmem_pool_t pool, void ** logical,
                                                void ** klogical, uint32_t * physical, void ** node);

/*!
 @brief Free contiguous video memory.

 @discussion
 Free the memory allocated by {@link vg_lite_hal_allocate_contiguous}. After this function returns, the associated memory
 handle is no longer a valid handle.

 @param memory_handle
 A pointer to an opaque structure returned by {@link vg_lite_hal_allocate_contiguous}.
 */
void vg_lite_hal_free_contiguous(void * memory_handle);

/*!
 @brief remove unfree node when continuously allocate buffer without free buffer.

 @discussion
 Free the node allocated by {@link kmalloc}. After this function returns, the associated memory
 handle is no longer a valid handle.
 */
void vg_lite_hal_free_os_heap(void);

/*!
 @brief Map contiguous logical or physical memory into the VGLite graphics hardware space.

 @discussion
 Any memory, like a frame buffer or some pre-allocated image or path data, needs to be mapped into the VGLite graphics
 hardware address space and wrapped by a memory handle. This allows the VGLite graphics hardware access that memory
 directly.

 Either a logical or a physical address should be passed in to map.

 @param size
 The number of bytes to map.

 @param logical
 The logical address of the memory region to map or NULL if the logical address is not known.

 @param physical
 The physical address of the memory region to map if logical is NULL.

 @param gpu
 A pointer to a variable that will receive the VGLite graphics hardware addressable address of the mapped region.

 @result
 A pointer to an opaque structure that will be used as the memory handle. NULL should be returned if there is
 not enough system resources to map the region.
 */
void * vg_lite_hal_map(uint32_t flags, uint32_t bytes, void * logical, uint32_t physical, int32_t dma_buf_fd,
                       uint32_t * gpu);

/*!
 @brief Unmap a previously mapped region.

 @discussion
 If a mapped region by {@link vg_lite_hal_map} is no longer needed, it should be unmapped to free up any allocated system
 resources used when mapping the region.

 @param memory_handle
 A pointer to an opaque structure returned by {@link vg_lite_hal_map}.
 */
void vg_lite_hal_unmap(void * memory_handle);

/*!
 @brief Execute a memory barrier.

 @discussion
 Some systems require a a memory barrier to make sure all store operations in the CPU have been handled. This is the wrapper
 function for a memory barrier.
 */
void vg_lite_hal_barrier(void);

/*!
 @brief Read data from a register from the VGLite graphics hardware.

 @discussion
 In order to communicate with the VGLite graphics hardware, the kernel needs to read and write to some hardware registers.
 In each SOC those registers could be allocated at a different space in the physical memory map.

 @param address
 The relative address of the VGLite graphics hardware register to read from.

 @result
 The 32-bit value returned from reading the register.
 */
uint32_t vg_lite_hal_peek(uint32_t address);

/*!
 @brief Write data to a register from the VGLite graphics hardware.

 @discussion
 In order to communicate with the VGLite graphics hardware, the kernel needs to read and write to some hardware registers.
 In each SOC those registers could be allocated at a different space in the physical memory map.

 @param address
 The relative address of the VGLite graphics hardware register to write to.

 @param data
 The data to write to the VGLite graphics hardware register.
 */
void vg_lite_hal_poke(uint32_t address, uint32_t data);

/*!
 @brief query the remaining allocate contiguous video memory.

 @param data
 The data to get the remaining allocate contiguous video memory bytes.
 */
vg_lite_error_t vg_lite_hal_query_mem(vg_lite_kernel_mem_t * mem);

/*!
 @brief Map contiguous physical memory into the user space.

 @param node
 This node have 3 attributes, bytes means the number of bytes to map.
 physical means the physical address of the memory region to map.logical means
 the return logical address of the memory region after map.
 */
vg_lite_error_t vg_lite_hal_map_memory(vg_lite_kernel_map_memory_t * node);

/*!
 @brief Unmap a previously mapped region.

 @param node
 This node have 2 attributes, bytes means the number of bytes to unmap.logical means
 the logical address of the memory region to unmap.
 */
vg_lite_error_t vg_lite_hal_unmap_memory(vg_lite_kernel_unmap_memory_t * node);

/*!
 @brief Wait until an interrupt from the VGLite graphics hardware has been received.

 @discussion
 Currently, the VGLite API is synchronous. This means that after each call it will wait until the VGLite graphics hardware
 has completed. The VGLite graphics hardware will send an interrupt when it is finished, and this function will wait until
 that interrupt has been received by the operating system.

 A timeout value is specified in order if the kernel wants to wait for a specific number of milliseconds fir the interrupt to
 occur. If the interrupt does not occur in the specified timeout, a timeout error will be returned.

 @param timeout
 The number of milliseconds to wait for the interrupt before returning a timeout error. If timeout = 0xFFFFFFFF
 then {@link vg_lite_hal_wait_interrupt} will wait forever for the interrupt.

 @param mask
 Irq event mask to wait for.

 @result
 A boolean value indicating whether the interrupt was received (1) or not (0).
 */
int32_t vg_lite_hal_wait_interrupt(uint32_t timeout, uint32_t mask, uint32_t * value);

/*!
 @brief After call vg_lite_hal_map(), flush cpu cache according the direction
 spicified by parameter cache_op.
 */
vg_lite_error_t vg_lite_hal_operation_cache(void * handle, vg_lite_cache_op_t cache_op);

/*!
 @brief export memory to dma buf, and get the dma buf fd
 */
vg_lite_error_t vg_lite_hal_memory_export(int32_t * fd);

/*!
 @brief print message
 */
void vg_lite_hal_print(char * format, ...);

/*!
 @brief trace message
 */
void vg_lite_hal_trace(char * format, ...);

/*!
 @brief error number to string
 */
const char * vg_lite_hal_Status2Name(vg_lite_error_t status);

/*!
 @brief allocate virtual memory from os
 */
vg_lite_error_t vg_lite_hal_allocate(uint32_t size, void ** memory);

/*!
 @brief free virtual memory
 */
vg_lite_error_t vg_lite_hal_free(void * memory);

/*!
 @brief set gpu execute state
 */
void vg_lite_set_gpu_execute_state(vg_lite_gpu_execute_state_t state);

#ifdef __cplusplus
}
#endif

#endif /* LV_USE_VG_LITE_DRIVER */

#endif /* VG_LITE_HAL_H */


