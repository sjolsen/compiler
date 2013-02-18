#ifndef FILE_POSITION_HH
#define FILE_POSITION_HH

#include "char_range.hh"
#include <iostream>
#include <cstddef>



struct file_position // first represents line, second, col
{
	char_range _file;
	char_range::iterator _pos;
	std::size_t _line;
	std::size_t _col;

	file_position (char_range file,
	               char_range::iterator position);
	file_position () = default;
};



std::string to_string (file_position pos);
std::ostream& operator << (std::ostream& os, file_position pos);
char_range containing_line (file_position pos);



#endif
