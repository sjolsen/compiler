#ifndef TOKENDEF_HH
#define TOKENDEF_HH

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
	equivalenet,
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



struct token
{
	token_type type;

	std::string str;
	int value;
	symbol op;
};



#endif
