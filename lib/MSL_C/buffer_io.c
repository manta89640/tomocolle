// MSL's stream buffers, as the C library of CodeWarrior for DS 2.0 has them: the five functions of the file the game
// links. This project's own decompilation of the game's code, written against that MSL's headers.
#include <buffer_io.h>

void __convert_from_newlines(unsigned char *p, size_t *n)
{
}

void __convert_to_newlines(unsigned char *p, size_t *n)
{
}

void __prep_buffer(FILE *file)
{
	file->buffer_ptr = file->buffer;
	file->buffer_len = file->buffer_size;
	file->buffer_len -= file->position & file->buffer_alignment;
	file->buffer_pos = file->position;
}

int __load_buffer(FILE *file, size_t *bytes_loaded, int alignment)
{
	int ioresult;

	__prep_buffer(file);
	if (alignment == __dont_align_buffer)
		file->buffer_len = file->buffer_size;
	ioresult = (*file->read_proc)(file->handle, file->buffer, &file->buffer_len, file->ref_con);
	if (ioresult == __io_EOF)
		file->buffer_len = 0;
	if (bytes_loaded)
		*bytes_loaded = file->buffer_len;
	if (ioresult)
		return ioresult;
	file->position += file->buffer_len;
	if (!file->mode.binary_io)
		__convert_to_newlines(file->buffer, &file->buffer_len);
	return __no_io_error;
}

int __flush_buffer(FILE *file, size_t *bytes_flushed)
{
	size_t buffer_len;
	int ioresult;

	buffer_len = file->buffer_ptr - file->buffer;
	if (buffer_len) {
		file->buffer_len = buffer_len;
		if (!file->mode.binary_io)
			__convert_from_newlines(file->buffer, &file->buffer_len);
		ioresult = (*file->write_proc)(file->handle, file->buffer, &file->buffer_len, file->ref_con);
		if (bytes_flushed)
			*bytes_flushed = file->buffer_len;
		if (ioresult)
			return ioresult;
		file->position += file->buffer_len;
	}
	__prep_buffer(file);
	return __no_io_error;
}
