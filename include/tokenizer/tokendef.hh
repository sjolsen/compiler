#ifndef TOKENDEF_HH
#define TOKENDEF_HH

#include "../text_processing.hh"
#include <string>



enum class token_type
{
	identifier,
	keyword,
	int_literal,
	char_literal,
	string_literal,
	symbol
};



enum class symbol
{
	plus,
	minus,
	asterisk,
	solidus,
	less_than,
	greater_than,
	lt_equiv,
	gt_equiv,
	equivalent,
	not_equiv,
	equal,
	bang,
	lbracket,
	rbracket,
	lbrace,
	rbrace,
	lparen,
	rparen,
	comma,
	semicolon
};



std::string to_string (symbol s);



struct token
{
	token_type type;

	std::string str;
	union
	{
		int value;
		char cvalue;
		symbol op;
	};

	file_position pos;

	token () = default;
};



std::string to_string (token t);



#endif
