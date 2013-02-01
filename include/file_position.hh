#ifndef FILE_POSITION_HH
#define FILE_POSITION_HH

#include "char_range.hh"
#include <utility>
#include <cstddef>



struct file_position // first represents line, second, col
	: std::pair <std::size_t, std::size_t>
{
	file_position (char_range file,
	               char_range::iterator position,
	               char delim = '\n');
};



#endif
