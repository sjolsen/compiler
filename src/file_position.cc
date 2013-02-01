#include "../include/file_position.hh"
#include <algorithm>

using namespace std;



file_position::file_position (char_range file,
                              char_range::iterator position,
                              char delim)
{
	first = count (begin (file), position, delim) + 1;
	second = find (reverse_iterator <decltype (position)> (position),
	               reverse_iterator <decltype (begin (file))> (begin (file)),
	               delim)
	         - reverse_iterator <decltype (position)> (position);
}
