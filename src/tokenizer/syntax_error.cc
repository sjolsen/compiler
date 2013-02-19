#include <tokenizer/syntax_error.hh>

using namespace std;



syntax_error::syntax_error (const string& what_str,
                            typename char_range::iterator where_iter)
	: runtime_error (what_str),
	  _where_iter (where_iter)
{
}



syntax_error::syntax_error (const char* const what_str,
                            typename char_range::iterator where_iter)
	: runtime_error (what_str),
	  _where_iter (where_iter)
{
}



typename char_range::iterator
syntax_error::where () const noexcept
{
	return _where_iter;
}
