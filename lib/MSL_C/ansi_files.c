// MSL's standard streams, as the C library of CodeWarrior for DS 2.0 has them: the table of stdin, stdout and stderr,
// their buffers, and __flush_all, the one function of the file the game links. This project's own decompilation of
// the game's code, written against that MSL's headers.
//
// The console is buffered: the original object has the three 256-byte buffers of ansi_files.h, and in the table
// stdin and stdout are line buffered and stderr unbuffered.
#define _MSL_BUFFERED_CONSOLE 1
#include <stdio.h>
#include <ansi_files.h>
#include <console_io.h>

FILE __files[_STATIC_FILES] = {
	{0, {__must_exist, __read, console_buff_mode, __console_file, __unoriented, 0}, {__neutral, 0, 0, 0}, 0, 0,
	 {0, 0}, {0, 0}, 0, stdin_buff, console_buff_size, stdin_buff, 0, 0, 0, 0, NULL, __read_console,
	 __write_console, __close_console, NULL},
	{1, {__must_exist, __write, console_buff_mode, __console_file, __unoriented, 0}, {__neutral, 0, 0, 0}, 0, 0,
	 {0, 0}, {0, 0}, 0, stdout_buff, console_buff_size, stdout_buff, 0, 0, 0, 0, NULL, __read_console,
	 __write_console, __close_console, NULL},
	{2, {__must_exist, __write, _IONBF, __console_file, __unoriented, 0}, {__neutral, 0, 0, 0}, 0, 0,
	 {0, 0}, {0, 0}, 0, stderr_buff, console_buff_size, stderr_buff, 0, 0, 0, 0, NULL, __read_console,
	 __write_console, __close_console, NULL},
};

int __flush_all(void)
{
	int result = 0;
	FILE *file = &__files[0];
	int i = 1;

	do {
		if (file->mode.file_kind != __closed_file && fflush(file))
			result = EOF;
		file = (i < _STATIC_FILES) ? &__files[i++] : NULL;
	} while (file);
	return result;
}
