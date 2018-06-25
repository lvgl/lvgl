/**
 * @file lv_sys_posix_generic.c
 *
 *  Initialize a POSIX system.
 *  Provide a means to detect/debug/process signals.
 */
#include <features.h>
#if !defined _POSIX_SOURCE
#error This file should be included in builds of POSIX systems only!
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*********************
 *      DEFINES
 *********************/
#ifndef     LV_SIGNALS
#warning    "LV_SIGNALS not defined! Is lv_conf.h included?"
#endif

#define DBGSIGNALS      (LV_SIGNALS && 0)  /* && 0 to disable */
#define DBGSIGNALS_V    (DBGSIGNALS && 0)  /* verbose */

#ifdef  DBGSIGNALS
#include <stdio.h>
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if LV_SIGNALS
static void sig_handler_func(int signum, siginfo_t * pinfo, void * p);
#endif
static void exit_func(void);

/**********************
 *  STATIC VARIABLES
 **********************/
volatile    char    exiting = 0;;
static      void    (*user_deinit_func)(void);  /*Store deinit func provided by user*/

#if defined(DBGSIGNALS) && DBGSIGNALS
static  char        txt_buf[4*1024] = { 0 };

const char * sig_names [] = {
"SIGHUP",
"SIGINT",
"SIGQUIT",
"SIGILL",
"SIGTRAP",
"SIGABRT",
"SIGIOT",
"SIGBUS",
"SIGFPE",
"SIGKILL",
"SIGUSR1",
"SIGSEGV",
"SIGUSR2",
"SIGPIPE",
"SIGALRM",
"SIGTERM",
"SIGSTKFLT",
"SIGCHLD",
"SIGCONT",
"SIGSTOP",
"SIGTSTP",
"SIGTTIN",
"SIGTTOU",
"SIGURG",
"SIGXCPU",
"SIGXFSZ",
"SIGVTALRM",
"SIGPROF",
"SIGWINCH",
"SIGIO/SIGPOLL",
"SIGPWR",
"SIGSYS/SIGUNUSED",
};
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL VARIABLES
 **********************/

/*********************
 *   GLOBAL FUNCTIONS
 **********************/
/**
 * Init system: exit handler, signal handlers ...
 */
void lv_sys_init(void (*user_deinit_func_param)(void))
{
#if LV_SIGNALS
    sigset_t set;
    struct sigaction    siga = { 0 };

#if DBGSIGNALS
    {
        pid_t p = getpid();
        printf("\n\tPID:  %d\n\n", p); /*disp PID to test sending various signals to process*/
    }
#endif

    sigfillset(&set); /* Block all other signals during execution of sig_handler_func */
    siga.sa_sigaction = sig_handler_func;
    siga.sa_mask = set;
    siga.sa_flags = SA_SIGINFO; /* | SA_NODEFER  --> No! Multiple Ctrl-C keys will break the handler! */

    sigaction(SIGINT, &siga, NULL);
    sigaction(SIGQUIT, &siga, NULL);
#endif

    if( user_deinit_func_param )
    {
        user_deinit_func = user_deinit_func_param;
    }

    atexit(exit_func);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/
static void exit_func(void)
{
#if defined(DBGSIGNALS) && DBGSIGNALS
    if( *txt_buf )
    {
        puts( "Exiting due to signal:" );
        puts( "-------------------------------" );
        puts( txt_buf );
        puts( "-------------------------------" );
    }
#endif

    if( user_deinit_func )
    {
        user_deinit_func();
    }
}


#if LV_SIGNALS
/* TODO: Add proper signal handling during lv_init() and hal_init(), i.e.
   Setup a signal halndler for error processing and block all signals
      (some like SIGSKILL and possibly SEGV can not be masked)
   Monitor progress of initialization to know what was initialized
   If a signal is caught in the err handler - uninit app in reverse steps.
   Once the system is initialized properly  setup a SIGINT (Ctrl-C) handler
   and unblock all signals.
*/
#if defined(DBGSIGNALS) && DBGSIGNALS
static void sig_handler_func(int signum, siginfo_t * pinfo, void * p)
{
    int             exit_app = 0;
    int             have_err = -1;
/*  ucontext_t *    pcontext = (ucontext_t *) p; */

    if( SIGINT == signum || SIGQUIT == signum )
    {
        putchar('\n');
        exit_app = 1;
        have_err = 0;
    }

    if( SI_USER == pinfo->si_code || SI_QUEUE == pinfo->si_code )
    {
        sprintf(txt_buf,
            "%s (%d / 0x%X)\n"      \
            "si_errno        %d\n"  \
            "si_code         %d\n"  \
            "si_pid          %d\n"  \
            "si_uid          %d\n"
            , sig_names[ signum ]
            , signum
            , signum
            , pinfo->si_errno
            , pinfo->si_code
            , pinfo->si_pid
            , pinfo->si_uid
        );
        have_err = 0;
    }
    else
    {
        sprintf(txt_buf,
            "%s \t(%d / 0x%X)\n" \
            "si_errno \t%d\n"   \
            "si_code  \t%d - "  \
            , sig_names[ signum ]
            , signum
            , signum
            , pinfo->si_errno
            , pinfo->si_code
        );

        switch( pinfo->si_code )
        {
            case SI_KERNEL:
                strcat(txt_buf,"\nSI_KERNEL: \tsent by kernel");
                break;
            case SI_TIMER:
                strcat(txt_buf,"\nSI_TIMER: \tsent by timer expiration");
                break;
            case SI_MESGQ:
                strcat(txt_buf,"\nSI_MESGQ: \tsent by real time mesq state change");
                break;
            case SI_ASYNCIO:
                strcat(txt_buf,"\nSI_ASYNCIO: \tsent by AIO completion");
                break;
            case SI_SIGIO:
                strcat(txt_buf,"\nSI_SIGIO: \tsent by queued SIGIO");
                break;
            case SI_TKILL:
                strcat(txt_buf,"\nSI_TKILL: \tsent by tkill system call");
                break;
/*        case SI_DETHREAD:
              strcat(txt_buf,"\nSI_DETHREAD: sent by execve() killing subsidiary threads");
*/
            default:
                strcat(txt_buf,"\nSI_???: sent by ???");
                break;
        }

        if( SIGSEGV == pinfo->si_signo )
        {
           switch( pinfo->si_code )
           {
               char segv_details[128];
               case SEGV_MAPERR:
                   sprintf(segv_details, "\nSEGV_MAPERR (__SI_FAULT|1)  address %p not mapped to object", pinfo->si_addr);
                   strcat(txt_buf, segv_details);
                   break;
               case SEGV_ACCERR:
                   sprintf(segv_details, "\nSEGV_ACCERR (__SI_FAULT|2)  invalid permissions for mapped object at address %p", pinfo->si_addr);
                   strcat(txt_buf, segv_details);
                   break;
               default:
                   strcat(txt_buf,"\nSEGV_???");
           }

           if( !pinfo->si_pid ) /* if is a real SEGV, sent by kernel */
           {
               exit_app = 1;
           }
        }
        else if( SIGBUS == pinfo->si_signo )
        {
           switch( pinfo->si_code )
           {
               case BUS_ADRALN:
                   strcat(txt_buf,"\nBUS_ADRALN (__SI_FAULT|1)  invalid address alignment" );
                   break;
               case BUS_ADRERR:
                   strcat(txt_buf,"\nBUS_ADRERR (__SI_FAULT|2)  non-existent physical address" );
                   break;
               case BUS_OBJERR:
                   strcat(txt_buf,"\nBUS_OBJERR (__SI_FAULT|3)  object specific hardware error/" );
                   break;
               default:
                   strcat(txt_buf,"\nBUS_???");
           }
        }
        else if( SIGTRAP == pinfo->si_signo )
        {
            switch( pinfo->si_code )
            {
#ifdef TRAP_BRKPT
                case TRAP_BRKPT:
                    strcat(txt_buf,"\nTRAP_BRKPT     process breakpoint" );
                    break;
#endif
#ifdef TRAP_TRACE
                case TRAP_TRACE:
                    strcat(txt_buf,"\nTRAP_TRACE     process trace trap" );
                    break;
#endif
#ifdef TRAP_BRANCH
                case TRAP_BRANCH:
                    strcat(txt_buf,"\nTRAP_BRANCH   process taken branch trap (since Linux 2.4)" );
                    break;
#endif
#ifdef TRAP_HWBKPT
                case TRAP_HWBKPT:
                    strcat(txt_buf,"\nTRAP_HWBKPT   hardware breakpoint/watchpoint" );
                    break;
#endif
                default:
                    strcat(txt_buf,"\nTRAP_???");
            }
        }
        else if( SIGPOLL == pinfo->si_signo )
        {
            strcat(txt_buf,"\nSIGPOLL:" );
            switch( pinfo->si_code )
            {
                case POLL_IN:
                    strcat(txt_buf,"\nPOLL_IN     data input available" );
                    break;
                case POLL_OUT:
                    strcat(txt_buf,"\nPOLL_OUT    output buffers available" );
                    break;
                case POLL_MSG:
                    strcat(txt_buf,"\nPOLL_MSG    input message available" );
                    break;
                case POLL_ERR:
                    strcat(txt_buf,"\nPOLL_ERR    I/O error" );
                    break;
                case POLL_PRI:
                    strcat(txt_buf,"\nPOLL_PRI    high priority input available" );
                    break;
                case POLL_HUP:
                    strcat(txt_buf,"\nPOLL_HUP    high priority input available" );
                    break;
                default:
                    strcat(txt_buf,"\nPOLL_???");
            }
        }
        else if( SIGCHLD == pinfo->si_signo )
        {
            strcat(txt_buf,"\nSIGCHLD:" );
        }
    }

#if DBGSIGNALS_V
    /* Enable these if need more info about the signal caught */
    sprintf(txt_buf, "pcontext        %p\n", pcontext);           /* ?? */
    sprintf(txt_buf, "si_errno        %d\n", pinfo->si_errno);    /* An errno value - unused on Linux*/
    sprintf(txt_buf, "si_pid          %d\n", pinfo->si_pid);      /* Sending process ID */
    sprintf(txt_buf, "si_uid          %d\n", pinfo->si_uid);      /* Real user ID of sending process */
    sprintf(txt_buf, "_rt.si_sigval   %d\n", pinfo->_sifields._rt.si_sigval);   /* Signal value */
    sprintf(txt_buf, "si_status       %d\n", pinfo->si_status);   /* Exit value or signal */
    sprintf(txt_buf, "si_utime        %ld\n",pinfo->si_utime);    /* User time consumed */
    sprintf(txt_buf, "si_stime        %ld\n",pinfo->si_stime);    /* System time consumed */
    sprintf(txt_buf, "si_value        %d\n", pinfo->si_value.sival_int);    /* Signal value */
    sprintf(txt_buf, "si_int          %d\n", pinfo->si_int);      /* POSIX.1b signal */
    sprintf(txt_buf, "si_ptr          %p\n", pinfo->si_ptr);      /* POSIX.1b signal */
    sprintf(txt_buf, "si_overrun      %d\n", pinfo->si_overrun);  /* Timer overrun count; POSIX.1b timers */
    sprintf(txt_buf, "si_timer        %d\n", pinfo->si_timerid);  /* Timer ID; POSIX.1b timers */
    sprintf(txt_buf, "si_addr         %p\n", pinfo->si_addr);     /* Memory location which caused fault */
    sprintf(txt_buf, "si_band         %ld\n",pinfo->si_band);     /* Band event (was int in
                                                                     glibc 2.3.2 and earlier) */
    sprintf(txt_buf, "si_fd           %d\n", pinfo->si_fd);       /* File descriptor */
#endif

    if( exit_app )
    {
        exit(have_err);
    }
}
#else
static void sig_handler_func(int signum, siginfo_t * pinfo, void * p)
{
    if( !exiting )
    {
        exiting = 1;
        putchar(':');
        putchar(')');
        putchar('\n');
        assert( SIGINT == signum || SIGQUIT == signum );
        exit(0);
    }
}
#endif /* DBGSIGNALS */
#endif /* LV_SIGNALS */
