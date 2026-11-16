// MSL's file functions, as the C library of CodeWarrior for DS 2.0 has them: fflush, the one function of the file
// the game links. This project's own decompilation of the game's code, written against that MSL's headers.
#include <stdio.h>
#include <ansi_files.h>
#include <buffer_io.h>
#include <file_io.h>

int fflush(FILE *file)
{
	if (file == NULL)
		return __flush_all();
	if (file->state.error || file->mode.file_kind == __closed_file)
		return EOF;
	if (file->mode.io_mode == __read)
		return 0;
	if (file->state.io_state >= __rereading)
		file->state.io_state = __reading;
	if (file->state.io_state == __reading)
		file->buffer_len = 0;
	if (file->state.io_state != __writing) {
		file->state.io_state = __neutral;
		return 0;
	}
	if (__flush_buffer(file, NULL)) {
		set_error(file);
		return EOF;
	}
	file->state.io_state = __neutral;
	file->position = 0;
	file->buffer_len = 0;
	return 0;
}
