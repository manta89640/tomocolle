// MSL's signals, as the C library of CodeWarrior for DS 2.0 has them: raise, the one function of the file the game
// links, and the table of handlers. This project's own decompilation of the game's code, written against that MSL's
// headers.
#include <signal.h>
#include <stdlib.h>
#include <critical_regions.h>

__signal_func_ptr signal_funcs[__msl_signal_max];

int raise(int sig)
{
	__signal_func_ptr temp;

	if (sig < 1 || sig > __msl_signal_max)
		return -1;
	__begin_critical_region(signal_funcs_access);
	temp = signal_funcs[sig - 1];
	if (temp != SIG_IGN)
		signal_funcs[sig - 1] = SIG_DFL;
	__end_critical_region(signal_funcs_access);
	if (temp == SIG_IGN || (temp == SIG_DFL && sig == SIGABRT))
		return 0;
	if (temp == SIG_DFL)
		exit(0);
	(*temp)(sig);
	return 0;
}
