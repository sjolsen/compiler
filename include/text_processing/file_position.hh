#ifndef FILE_POSITION_HH
#define FILE_POSITION_HH

#include "char_range.hh"
#include <cstddef>



struct file_position // first represents line, second, col
{
	const char_range _file;
	const char_range::iterator _pos;
	const std::size_t _line;
	const std::size_t _col;

	file_position (char_range file,
	               char_range::iterator position);
};



#endif
