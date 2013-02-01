#include "../include/tokendef.hh"

using namespace std;



string to_string (token t)
{
	switch (t.type)
	{
	case token_type::identifier:
		return "<identifier, " + t.str + '>';
	case token_type::keyword:
		return "<keyword, " + t.str + '>';
	case token_type::int_literal:
		return "<int_literal, " + to_string (t.value) + '>';
	case token_type::char_literal:
		return "<char_literal, " + string (1, t.cvalue) + '>';
	case token_type::string_literal:
		return "<string_literal, " + t.str + '>';
	case token_type::symbol:
		return "<symbol, " + to_string (t.op) + '>';
	};
}



string to_string (symbol s)
{
	switch (s)
	{
	case symbol::plus:
		return "+";
	case symbol::minus:
		return "-";
	case symbol::asterisk:
		return "*";
	case symbol::solidus:
		return "/";
	case symbol::less_than:
		return "<";
	case symbol::greater_than:
		return ">";
	case symbol::lt_equiv:
		return "<=";
	case symbol::gt_equiv:
		return ">=";
	case symbol::equivalent:
		return "==";
	case symbol::not_equiv:
		return "!=";
	case symbol::equal:
		return "=";
	case symbol::bang:
		return "!";
	case symbol::lbracket:
		return "[";
	case symbol::rbracket:
		return "]";
	case symbol::lbrace:
		return "{";
	case symbol::rbrace:
		return "}";
	case symbol::lparen:
		return "(";
	case symbol::rparen:
		return ")";
	case symbol::comma:
		return ",";
	case symbol::semicolon:
		return ";";
	};
}
