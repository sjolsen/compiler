#include "../include/token_predicates.hh"



matcher identifier_p
{
	[] (char_range& token_range) -> token
	{
		token t;
		t.type = token_type::identifier;
		t.str =  std::string (token_range.begin (), token_range.end ());
		return t;
	},

	[] (char_range& text) -> char_range
	{
		if (!std::isalpha (*text.begin ()))
			return char_range ();
		return char_range (text.begin (),
		                   std::find_if_not (text.begin (), text.end (), (int (*) (int)) &std::isalnum));
	}
};
