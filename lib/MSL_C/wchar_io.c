// MSL's wide character input and output, as the C library of CodeWarrior for DS 2.0 has it: the two functions the
// game links, which set and read a stream's orientation. This project's own decompilation of the game's code,
// written against that MSL's headers.
#include <wchar.h>
#include <stdio.h>

int __fwide(FILE *file, int mode)
{
	return fwide(file, mode);
}

int fwide(FILE *file, int mode)
{
	int orientation;
	int result;

	if (file == NULL || file->mode.file_kind == __closed_file)
		return 0;

	orientation = file->mode.file_orientation;
	switch (orientation) {
	case __unoriented:
		if (mode > 0)
			file->mode.file_orientation = __wide_oriented;
		else if (mode < 0)
			file->mode.file_orientation = __char_oriented;
		result = mode;
		break;
	case __char_oriented:
		result = -1;
		break;
	case __wide_oriented:
		result = 1;
		break;
	}
	return result;
}
