/**
 * @file  :  lv_debug.h
 Summary:  Macros for debugging/logging

 FULL_DUMP (enables dump globally) or DUMP (enables file-level dump) must be 
 defined before including this header file for debug output to be enabled.

 To be able to use the DBG macro do the following:
 1. Either add   '-DFULL_DUMP'  to your CFLAGS ...

 2. ... OR add 
     #define DUMP
     #include lv_debug.h
   ... to the source file where you want to use the DBG macros

 3. Then use the macro in the code:
      err_code = func();
      DBG("errcode = 0x%x\n");
         -= OR =- 
      RETURN_ERR( errcode );
 */

#ifndef __LV_DEBUG_GENERIC_UNIX_H
#define __LV_DEBUG_GENERIC_UNIX_H

#include <stdio.h>

/* Define several variants of the DBG macro */
#if defined( DUMP ) || defined( FULL_DUMP )

/* Variant 1. DBG macros print time stamp + location in code + user message */
#if defined(PRINT_TIME_IN_DBG)
#include <sys/timeb.h>
#include <time.h>
#define DBG(fmt, args...)    do{                    \
            struct tm * p_tm;                       \
            struct timeb var_timeb;                 \
            ftime( &var_timeb );                    \
            p_tm = localtime( &var_timeb.time );    \
            printf("%02d:%02d:%03d [%s %s:%d] : " fmt, p_tm->tm_min, p_tm->tm_sec, var_timeb.millitm, __FUNCTION__,__FILE__, __LINE__, ##args);flush_stdout(); }while(0)

/* Variant 2. DBG macros print caller thread + location in code + user message */
#elif defined(PRINT_THREAD_IN_DBG)
#include <pthread.h>
#define DBG(fmt, args...)   printf("0x%lx [%s %s:%d]: " fmt, pthread_self(), __FUNCTION__, __FILE__, __LINE__, ##args);flush_stdout()


/* Variant 3. DBG macros print location in code (func, file:line)  + user message */
#elif defined(PRINT_FILE_IN_DBG)
#define DBG(fmt, args...)   printf("[%s %s:%d]: " fmt, __FUNCTION__, __FILE__, __LINE__, ##args);fflush(NULL)

/* Variant 4. DBG macros print originating func  + user message  [default if DUMP defined] */
#else
#define DBG(fmt, args...)   printf("[%s]: " fmt, __FUNCTION__, ##args);fflush(NULL)
#endif  /* PRINT_TIME_IN_DBG */


/* Variant 4. DBG macros disabled */
#else

#define DBG(fmt, args...)

#endif   /* end of DBG macro variants */


/* Define several variants of the TRACE_... macros */
#if defined(TRACE) || defined(FULL_TRACE)

/* Variant 1. TRACE macros print time stamp + location in code + user message */
#if defined(PRINT_TIME_IN_TRACE)

#include <sys/timeb.h>
#include <time.h>
#define TRACE_IN()      { do{                       \
            struct tm * p_tm;                       \
            struct timeb var_timeb;                 \
            ftime( &var_timeb );                    \
            p_tm = localtime( &var_timeb.time );    \
            printf("TRACE IN  %*s> %s  [%02d:%02d:%03d] %s:%d\n", (++g_trace_lvl)*2, "", __FUNCTION__, p_tm->tm_min, p_tm->tm_sec, var_timeb.millitm, __FILE__, __LINE__);flush_stdout(); }while(0)

#define TRACE_OUT()      do{                        \
            struct tm * p_tm;                       \
            struct timeb var_timeb;                 \
            ftime( &var_timeb );                    \
            p_tm = localtime( &var_timeb.time );    \
            printf("TRACE OUT %*s< %s  [%02d:%02d:%03d] %s:%d\n", (--g_trace_lvl+1)*2, "", __FUNCTION__, p_tm->tm_min, p_tm->tm_sec, var_timeb.millitm, __FILE__, __LINE__);flush_stdout(); }while(0); }

#define TRACE_RET()      do{                        \
            struct tm * p_tm;                       \
            struct timeb var_timeb;                 \
            ftime( &var_timeb );                    \
            p_tm = localtime( &var_timeb.time );    \
            printf("TRACE_RET %*s< %s  [%02d:%02d:%03d] %s:%d\n", (--g_trace_lvl+1)*2, "", __FUNCTION__,  p_tm->tm_min,  p_tm->tm_sec, var_timeb.millitm, __FILE__, __LINE__);flush_stdout(); }while(0)

#define TRACE_MSG(fmt, args...)    do{              \
            struct tm * p_tm;                       \
            struct timeb var_timeb;                 \
            ftime( &var_timeb );                    \
            p_tm = localtime( &var_timeb.time );    \
            printf("TRACE msg %*s  %s  [%02d:%02d:%03d] %s:%d : " fmt, (g_trace_lvl)*2, "", __FUNCTION__,  p_tm->tm_min, p_tm->tm_sec, var_timeb.millitm, __FILE__, __LINE__, ##args);flush_stdout(); }while(0)

/* Variant 2. TRACE macros print caller thread + location in code + user message */
#elif defined(PRINT_THREAD_IN_TRACE)
#include <pthread.h>
#define TRACE_IN()          {   printf("TRACE IN  %*s> %s  [0x%lx] %s:%d\n", (++g_trace_lvl)*2, "", __FUNCTION__, pthread_self(),__FILE__, __LINE__);flush_stdout()
#define TRACE_OUT()             printf("TRACE OUT %*s< %s  [0x%lx] %s:%d\n", (--g_trace_lvl+1)*2, "", __FUNCTION__, pthread_self(),__FILE__, __LINE__);flush_stdout(); }
#define TRACE_RET()             printf("TRACE_RET %*s< %s  [0x%lx] %s:%d\n", (--g_trace_lvl+1)*2, "", __FUNCTION__, pthread_self(),__FILE__, __LINE__);flush_stdout()
#define TRACE_MSG(fmt, args...) printf("TRACE msg %*s  %s  [0x%lx] %s:%d : " fmt, (g_trace_lvl)*2, "", __FUNCTION__, pthread_self(), __FILE__, __LINE__, ##args);flush_stdout()


/* Variant 3. TRACE macros print location in code + user message */
#elif defined(PRINT_FILE_IN_TRACE)
#define TRACE_IN()          {   printf("TRACE IN  %*s> %s  %s:%d\n", (++g_trace_lvl)*2, "", __FUNCTION__, __FILE__, __LINE__);flush_stdout()
#define TRACE_IN_wFrom()    {   printf("TRACE IN  %*s> %s  %s:%d   (from %s:%d)\n", (++g_trace_lvl)*2, "", __FUNCTION__, __FILE__, __LINE__, fiLe, LiNe);flush_stdout()
#define TRACE_OUT()             printf("TRACE OUT %*s< %s  %s:%d\n", (--g_trace_lvl+1)*2, "", __FUNCTION__, __FILE__, __LINE__);flush_stdout(); }
#define TRACE_RET()             printf("TRACE_RET %*s< %s  %s:%d\n", (--g_trace_lvl+1)*2, "", __FUNCTION__, __FILE__, __LINE__);flush_stdout()
#define TRACE_MSG(fmt, args...) printf("TRACE msg %*s  %s  %s:%d : " fmt, (g_trace_lvl)*2, "", __FUNCTION__, __FILE__, __LINE__, ##args);flush_stdout()


/* Variant 4. TRACE macros print short location in code + user message */
#else
#define TRACE_IN()          {   printf("TRACE IN  %*s> %s:%d\n", (++g_trace_lvl)*2, "", __FUNCTION__,    __LINE__);flush_stdout()
#define TRACE_IN_wFrom()    {   printf("TRACE IN  %*s> %s:%d  (from %s:%d)\n", (++g_trace_lvl)*2, "", __FUNCTION__,    __LINE__, fiLe, LiNe);flush_stdout()
#define TRACE_OUT()             printf("TRACE OUT %*s< %s:%d\n", (--g_trace_lvl+1)*2, "", __FUNCTION__,  __LINE__);flush_stdout(); }
#define TRACE_RET()             printf("TRACE_RET %*s< %s:%d\n", (--g_trace_lvl+1)*2, "", __FUNCTION__,  __LINE__);flush_stdout()
#define TRACE_MSG(fmt, args...) printf("TRACE msg %*s  %s:%d : " fmt, (g_trace_lvl)*2, "", __FUNCTION__, __LINE__, ##args);flush_stdout()
#endif  // PRINT_TIME_IN_TRACE


/* Variant 5. TRACE macros disabled */
#else

#define TRACE_IN()          {
#define TRACE_IN_wFrom()    {
#define TRACE_OUT()         }
#define TRACE_RET()
#define TRACE_MSG(fmt, args...)

#endif /* ... end of TRACE macro variants */

#endif /* __LV_DEBUG_GENERIC_UNIX_H */
