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


#ifndef NEMA_CMDLIST_H__
#define NEMA_CMDLIST_H__

#include "nema_sys_defs.h"
#include "nema_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CL_NOP      0x010000U
#define CL_PUSH     0x020000U
#define CL_RETURN   0x040000U
#define CL_ABORT    0x080000U

#define CL_BATCH_SHIFT      12
#define CL_BATCH_LOOP       0x8000

#define SUBMISSION_ID_MASK 0xffffff

#define CL_ALIGNMENT_MASK  (0x00000007U) // CL buffer must be 8 byte aligned

//---------------------------------------------------------------------------
typedef struct nema_cmdlist_t_ {
    nema_buffer_t bo;
    int size;                       /**< Number of entries in the command list */
    int offset;                     /**< Points to the next address to write */
    uint32_t flags;                 /**< Flags */
    int32_t  submission_id;
    struct nema_cmdlist_t_ *next;   /**< Points to next command list */
    struct nema_cmdlist_t_ *root;   /**< Points to the head of the list */
} nema_cmdlist_t;

/** \brief Create a new Command List into a preallocated space
 *
 * \param addr_virt  Command List's address (preallocated)
 * \param size_bytes Command List's size in bytes
 * \return The instance of the new Command List
 *
 */
nema_cmdlist_t nema_cl_create_prealloc(nema_buffer_t *bo);

/** \brief Create a new, non expandable Command List of specific size
 *
 * \param size_bytes Command List's size in bytes
 * \return The instance of the new Command List
 *
 */
nema_cmdlist_t nema_cl_create_sized(int size_bytes);

/** \brief Create a new expandable Command List
 *
 * \return The instance of the new Command List
 *
 */
nema_cmdlist_t nema_cl_create(void);

/** \brief Destroy/Free a Command List
 *
 * \param cl Pointer to the Command List
 *
 */
void nema_cl_destroy(nema_cmdlist_t *cl);

/** \brief Reset position of next command to be written to the beginning. Doesn't clear the List's contents.
 *
 * \param cl Pointer to the Command List
 *
 */
void nema_cl_rewind(nema_cmdlist_t *cl);

/** \brief Define in which Command List each subsequent commands are going to be inserted.
 *
 * \param cl Pointer to the Command List
 *
 */
void nema_cl_bind(nema_cmdlist_t *cl);

/** \brief Define in which Command List each subsequent commands are going to be inserted.
 *         Bind this command list as Circular. It never gets full, it never expands,
 *         it may get implicitly submitted, it cannot be reused. No other CL should be submitted
 *         while a circular CL is bound
 *
 * \param cl Pointer to the Command List
 *
 */
void nema_cl_bind_circular(nema_cmdlist_t *cl);

/** \brief Unbind current bound Command List, if any.
 *
 *
 */
void nema_cl_unbind(void);


/** \brief Get bound Command List
 *
 * \return Pointer to the bound Command List
 *
 */
nema_cmdlist_t *nema_cl_get_bound(void);

/** \private */
void nema_cl_submit_no_irq(nema_cmdlist_t *cl);

/** \brief Enqueue Command List to the Ring Buffer for execution
 *
 * \param cl Pointer to the Command List
 *
 */
void nema_cl_submit(nema_cmdlist_t *cl);

/** \brief Wait for Command List to finish
 *
 * \param cl Pointer to the Command List
 * \return 0 if no error has occurred
 *
 */
int nema_cl_wait(nema_cmdlist_t *cl);

/** \brief Add a command to the bound Command List
 *
 * \param reg Hardware register to be written
 * \param data Data to be written
 *
 */
void nema_cl_add_cmd(uint32_t reg, uint32_t data);

/** \brief Add multiple commands to the bound Command List
 *
 * \param cmd_no Numbers of commands to add
 * \param cmd Pointer to the commands to be added
 * \return 0 if no error has occurred
 *
 */
int nema_cl_add_multiple_cmds(int cmd_no, uint32_t *cmd);

/** private */
uint32_t * nema_cl_get_space(int cmd_no);

/** \brief Branch from the bound Command List to a different one. Return is implied.
 *
 * \param cl Pointer to the Command List to branch to
 *
 */
void nema_cl_branch(nema_cmdlist_t *cl);

/** \brief Jump from the bound Command List to a different one. No return is implied.
 *
 * \param cl Pointer to the Command List to jump to
 *
 */
void nema_cl_jump(nema_cmdlist_t *cl);

/** \brief Add an explicit return command to the bound Command List
 *
 *
 */
void nema_cl_return(void);

/** \brief Returns positive number if the Command List is almost full, otherwise returns 0.
 *
 * \param cl Pointer to the Command List
 *
 */
int nema_cl_almost_full(nema_cmdlist_t *cl);

/** \brief Check if there is enough space or expansion can be performed for
 * required commands.
 *
 * \param cmd_no Numbers of commands to be checked if they fit
 * \reurn zero is commands fit or expansion xan be performed else return negative
 */
int nema_cl_enough_space(int cmd_no);

#ifdef __cplusplus
}
#endif

#endif
