// MSL's character input and output, as the C library of CodeWarrior for DS 2.0 has it: the two functions of the
// file the game links. This project's own decompilation of the game's code, written against that MSL's headers.
#include <stdio.h>
#include <ansi_files.h>
#include <buffer_io.h>
#include <char_io.h>
#include <critical_regions.h>

int __get_char(FILE *file)
{
	int state;
	int ioresult;

	file->buffer_len = 0;
	if (file->state.error || file->mode.file_kind == __closed_file)
		return EOF;
	state = file->state.io_state;
	if (state == __writing || !(file->mode.io_mode & __read)) {
		set_error(file);
		return EOF;
	}
	if (state >= __rereading) {
		file->state.io_state = state - 1;
		if (state == __rereading)
			file->buffer_len = file->saved_buffer_len;
		return file->ungetc_buffer[state - __rereading];
	}
	file->state.io_state = __reading;
	ioresult = __load_buffer(file, NULL, __align_buffer);
	if (ioresult || !file->buffer_len) {
		if (ioresult == __io_error)
			set_error(file);
		else
			set_eof(file);
		return EOF;
	}
	file->buffer_len--;
	return *file->buffer_ptr++;
}

char *fgets(char *s, int n, FILE *file)
{
	char *p = s;
	int c;
	int crtrgn;

	if (file == stdin)
		crtrgn = stdin_access;
	else if (file == stdout)
		crtrgn = stdout_access;
	else if (file == stderr)
		crtrgn = stderr_access;
	else
		crtrgn = files_access;
	if (--n < 0)
		return NULL;
	__begin_critical_region(crtrgn);
	if (n) {
		do {
			c = __getc(file);
			if (c == EOF) {
				if (file->state.eof && p != s)
					break;
				__end_critical_region(crtrgn);
				return NULL;
			}
			*p++ = c;
			if (c == '\n')
				break;
		} while (--n);
	}
	__end_critical_region(crtrgn);
	*p = 0;
	return s;
}
