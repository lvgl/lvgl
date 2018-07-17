/**
 @file  :   lv_sys.h
 @Summary:  Initializes the system. OS/system dependent. Each system should 
            provide its own impl. See dir posix for impl. of the POSIX 
            system initialization.
*/

#ifndef __LV_SYS_H
#define __LV_SYS_H

void lv_sys_init(void (*exit_func_ptr)(void));

#endif
