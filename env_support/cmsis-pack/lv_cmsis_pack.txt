/****************************************************************************
*  Copyright 2022 Gorgon Meducer (Email:embedded_zhuoran@hotmail.com)       *
*                                                                           *
*  Licensed under the Apache License, Version 2.0 (the "License");          *
*  you may not use this file except in compliance with the License.         *
*  You may obtain a copy of the License at                                  *
*                                                                           *
*     http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                           *
*  Unless required by applicable law or agreed to in writing, software      *
*  distributed under the License is distributed on an "AS IS" BASIS,        *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
*  See the License for the specific language governing permissions and      *
*  limitations under the License.                                           *
*                                                                           *
****************************************************************************/

/**
 * @file lv_cmsis_pack.c
 *
 * @brief This file will only be used by cmsis-pack.
 */

/*********************
 *      INCLUDES
 *********************/
#include "RTE_Components.h"
#include <time.h>

 /*********************
 *      DEFINES
 *********************/

 /**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

 /**********************
 *  STATIC VARIABLES
 **********************/

 /**********************
 *      MACROS
 **********************/

 /*! \name The macros to identify the compiler */
/*! @{ */

/*! \note for IAR */
#undef __IS_COMPILER_IAR__
#if defined(__IAR_SYSTEMS_ICC__)
#   define __IS_COMPILER_IAR__                  1
#endif

/*! \note for arm compiler 5 */
#undef __IS_COMPILER_ARM_COMPILER_5__
#if ((__ARMCC_VERSION >= 5000000) && (__ARMCC_VERSION < 6000000))
#   define __IS_COMPILER_ARM_COMPILER_5__       1
#endif
/*! @} */

/*! \note for arm compiler 6 */

#undef __IS_COMPILER_ARM_COMPILER_6__
#if defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#   define __IS_COMPILER_ARM_COMPILER_6__       1
#endif

#undef __IS_COMPILER_ARM_COMPILER__
#if defined(__IS_COMPILER_ARM_COMPILER_5__) && __IS_COMPILER_ARM_COMPILER_5__   \
||  defined(__IS_COMPILER_ARM_COMPILER_6__) && __IS_COMPILER_ARM_COMPILER_6__

#   define __IS_COMPILER_ARM_COMPILER__         1

#endif


#undef  __IS_COMPILER_LLVM__
#if defined(__clang__) && !__IS_COMPILER_ARM_COMPILER_6__
#   define __IS_COMPILER_LLVM__                 1
#else
/*! \note for gcc */
#   undef __IS_COMPILER_GCC__
#   if defined(__GNUC__) && !(  defined(__IS_COMPILER_ARM_COMPILER__)           \
                            ||  defined(__IS_COMPILER_LLVM__))
#       define __IS_COMPILER_GCC__              1
#   endif
/*! @} */
#endif
/*! @} */


 /**********************
 *   GLOBAL FUNCTIONS
 **********************/

/* only arm compilers will use microLib that doesn't implement time() */
#if defined(__MICROLIB)
__attribute__((weak))
_ARMABI time_t time(time_t * time)
{
    return (time_t)(-1);
}
#endif

/* when people don't want to use src/extra */
__attribute__((weak))
void lv_extra_init(void)
{
}

