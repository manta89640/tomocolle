// MSL's abort and exit, as the C library of CodeWarrior for DS 2.0 has them for ARM: the three functions of the file
// the game links. This project's own decompilation of the game's code, written against that MSL's headers.
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <abort_exit.h>
#include <critical_regions.h>

#define max_funcs 64

// Declared in this order so that they come out in the original's: __console_exit, __stdio_exit, __atexit_curr_func,
// __aborting, then the table
void (*__console_exit)(void);
int __aborting;
void (*__stdio_exit)(void);
static int __atexit_curr_func;
static void (*__atexit_funcs[max_funcs])(void);

void abort(void)
{
	raise(SIGABRT);
	__aborting = 1;
	exit(EXIT_FAILURE);
}

void exit(int status)
{
	if (!__aborting) {
		__destroy_global_chain();
		if (__stdio_exit) {
			(*__stdio_exit)();
			__stdio_exit = 0;
		}
	}
	__exit(status);
}

void __exit(int status)
{
	__begin_critical_region(atexit_funcs_access);
	while (__atexit_curr_func > 0)
		(*__atexit_funcs[--__atexit_curr_func])();
	__end_critical_region(atexit_funcs_access);
	if (__console_exit) {
		(*__console_exit)();
		__console_exit = 0;
	}
	fflush(NULL);
	_ExitProcess();
}
