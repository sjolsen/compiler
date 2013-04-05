#include <tokenizer/error.hh>

using namespace std;



error::error (const string& what_str,
                            file_position where_pos)
	: runtime_error (what_str),
	  _where_pos (where_pos)
{
}



error::error (const char* const what_str,
                            file_position where_pos)
	: runtime_error (what_str),
	  _where_pos (where_pos)
{
}



file_position error::where () const noexcept
{
	return _where_pos;
}
