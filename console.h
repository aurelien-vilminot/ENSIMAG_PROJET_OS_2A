#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "inttypes.h"

/*
 * This is the function called by printf to send its output to the screen. You
 * have to implement it in the kernel and in the user program.
 */
extern void console_putbytes(const char *s, int len);

extern void traite_car(char c);

#endif
