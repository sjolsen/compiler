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
