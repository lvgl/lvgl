/**
 * @file picolibc_stdio.h
 * A stdout that goes to semihosting, for the picolibc targets.
 *
 * picolibc's tinystdio references `stdout` from puts() whether or not anything here
 * prints, so an image that links its libc has to define one or fail at link time. Pointing
 * it at semihosting means a stray print shows up instead of vanishing.
 */
#ifndef PICOLIBC_STDIO_H
#define PICOLIBC_STDIO_H

#include <stdio.h>
#include "plat.h"

static int sh_putc(char c, FILE * f)
{
    char s[2];
    (void)f;
    s[0] = c;
    s[1] = '\0';
    sh_write0(s);
    return c;
}

static FILE sh_stdout = FDEV_SETUP_STREAM(sh_putc, NULL, NULL, _FDEV_SETUP_WRITE);
FILE * const stdout = &sh_stdout;
FILE * const stderr = &sh_stdout;

#endif /*PICOLIBC_STDIO_H*/
