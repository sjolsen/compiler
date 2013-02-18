#include "../../include/text_processing/file_position.hh"
#include <algorithm>

using namespace std;



file_position::file_position (char_range file,
                              char_range::iterator position)
	: _file (file),
	  _pos (position),
	  _line (count (begin (_file), position, '\n') + 1),
	  _col (find (typename char_range::reverse_iterator (position),
	              _file.rend (),
	              '\n')
	        - typename char_range::reverse_iterator (position) + 1)
{
}



string to_string (file_position pos)
{
	return ('(' + to_string (pos._line) + ", " + to_string (pos._col) + ')');
}



ostream& operator << (ostream& os, file_position pos)
{
	return os << to_string (pos);
}



char_range containing_line (file_position pos)
{
	auto line_begin = find (typename char_range::reverse_iterator (pos._pos),
	                        pos._file.rend (),
	                        '\n').base ();
	auto line_end = find (pos._pos, end (pos._file), '\n');
	return char_range (line_begin, line_end);
}
