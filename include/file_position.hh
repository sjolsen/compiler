#ifndef FILE_POSITION_HH
#define FILE_POSITION_HH

#include <utility>
#include <algorithm>



struct file_position // first represents line, second, col
	: std::pair <std::size_t, std::size_t>
{
	file_position (std::string& file,
	               typename std::string::iterator position,
	               char delim = '\n');
};



#endif
