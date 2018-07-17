/**
 @file  :  lv_debug.h
 @Summary:  Macros for debugging/logging

 FULL_DUMP (enables dump globally) or DUMP (enables file-level dump) must be 
 defined before including this header file for debug output to be enabled.

 To be able to use the DBG macro do the following:
 1. Either add   '-DFULL_DUMP'  to your CFLAGS ...
    (NOTE: This will enable all DBG macros found in the code!)

     -=OR=-

    add :
     #define DUMP
     #define TRACE
     #include "lv_debug.h"
   ... to the source file where you want to use the DBG macros.

 3. Then use the macros inside that file:
      int func1(void)
      {
          TRACE_IN();
          int err_code = func2();
          if(err_code)
          {
              TRACE_RET();
              return err_code;
          }
          TRACE_OUT();
      }

      int func2(void)
      {
          int err_code = 123;
          DBG("errcode = 0x%x\n", err_code);
          RETURN_ERR( err_code );
      }
 */

#ifndef __LV_DEBUG_H
#define __LV_DEBUG_H

/*Enable lv_debug operation based if debugging or tracing is enabled.*/
#if defined( DUMP ) || defined( FULL_DUMP ) || defined(TRACE) || defined(FULL_TRACE)

#include <features.h>
#include <stdio.h>

/*disable this if you don't want to flush on every macro call*/
#define flush_stdout()  fflush(stdout)

/*The impl of the DBG and TRACE_... macros is system-dependent 
  and is provided by one of the headers below. 
  NOTE: We will use a generic impl for all *nixes for now, 
  until the need arises to specialize some of them*/
#if defined( __linux__ )
#include "lvgl/lv_sys/posix/lv_debug_posix_generic.h"

#elif defined( __unix__ )
#include "lvgl/lv_sys/posix/lv_debug_posix_generic.h"

#else
#error   "Write a lv_debug impl for your system"
#endif

/*  The DBG_V macro is for "spammy" messages, which we usually don't want
    to see, unless specifically enabled with DUMP_VERBOSE. In most cases 
    these are DBG messages appearing in a loop... */
#if defined( DUMP_VERBOSE )
#define DBG_V           DBG
#else
#define DBG_V(fmt, args...)
#endif

/*  These macros are based on DBG and hence are not system-specific and can 
    be defined here. The DBG macro is implemented in the system-specific headers 
    included above (e.g. lv_debug_unix_generic.h)*/
#define RETURN_ERR(e)   do{ DBG("ERROR 0x%x\n", (e)); return (e);}while(0)
#define RETURN_ERR_V(e) do{ if(e) { DBG("ERROR 0x%x\n", (e)) } else { DBG("No error code when ERROR was expected!\n") }; return (e);}while(0)
#define EXIT_ERR(e)     do{ DBG("ERROR 0x%x\n", (e)); return;}while(0)
#define BREAK_ERR(e)    DBG("ERROR 0x%x\n", (e)); break;

/*This var holds current indentation level for the TRACE macros*/
#if defined(TRACE) || defined(FULL_TRACE)
#if defined(TRACE_LEVEL_VAR_DEFINED)
extern  int g_trace_lvl;
#else
#define TRACE_LEVEL_VAR_DEFINED
int g_trace_lvl;
#endif
#endif


#else  /*neither DUMP nor TRACE are defined --> all macros resolve to noop*/

#define DBG(fmt, args...)
#define DBG_V(fmt, args...)

#define TRACE_IN()          {
#define TRACE_IN_wFrom()    {
#define TRACE_OUT()         }
#define TRACE_RET()
#define TRACE_MSG(fmt, args...)

#define RETURN_ERR(e)
#define RETURN_ERR_V(e)
#define EXIT_ERR(e)
#define BREAK_ERR(e)

#endif /* defined( DUMP ) || defined( FULL_DUMP ) || defined(TRACE) || defined(FULL_TRACE) */

#endif /* __LV_DEBUG_H */
