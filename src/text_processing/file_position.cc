#include "../../include/text_processing/file_position.hh"
#include <algorithm>

using namespace std;



file_position::file_position (char_range file,
                              char_range::iterator position)
	: _file (file),
	  _pos (position),
	  _line (count (begin (_file), position, '\n') + 1),
	  _col (find (reverse_iterator <decltype (position)> (position),
	              reverse_iterator <decltype (begin (_file))> (begin (_file)),
	              '\n')
	        - reverse_iterator <decltype (position)> (position) + 1)
{
}
