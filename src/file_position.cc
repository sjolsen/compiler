#include "../include/file_position.hh"



file_position::file_position (std::string& file,
                              typename std::string::iterator position,
                              char delim)
{
	first = std::count (file.begin (), position, delim) + 1;
	second = std::find (std::reverse_iterator <decltype (position)> (position),
	                    std::reverse_iterator <decltype (file.begin ())> (file.begin ()),
	                    delim)
	         - std::reverse_iterator <decltype (position)> (position);
}
