#include <tokenizer/syntax_error.hh>

using namespace std;



syntax_error::syntax_error (const string& what_str,
                            file_position where_pos)
	: runtime_error (what_str),
	  _where_pos (where_pos)
{
}



syntax_error::syntax_error (const char* const what_str,
                            file_position where_pos)
	: runtime_error (what_str),
	  _where_pos (where_pos)
{
}



file_position syntax_error::where () const noexcept
{
	return _where_pos;
}
