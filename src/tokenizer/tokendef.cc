#include <tokenizer/tokendef.hh>

using namespace std;



string to_string (token t)
{
	switch (t.type)
	{
	case token_type::identifier:
		return "<identifier, "     + t.str               + ", " + to_string (t.pos) + '>';
	case token_type::keyword:
		return "<keyword, "        + t.str               + ", " + to_string (t.pos) + '>';
	case token_type::int_literal:
		return "<int_literal, "    + to_string (t.value) + ", " + to_string (t.pos) + '>';
	case token_type::char_literal:
		return "<char_literal, "   + t.literal           + ", " + to_string (t.pos) + '>';
	case token_type::string_literal:
		return "<string_literal, " + t.literal           + ", " + to_string (t.pos) + '>';
	case token_type::symbol:
		return "<symbol, "         + to_string (t.op)    + ", " + to_string (t.pos) + '>';
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



bool operator == (const token& a,
                  const token& b)
{
	if (a.type != b.type)
		return false;

	switch (a.type)
	{
	case token_type::identifier:
	case token_type::keyword:
	case token_type::string_literal:
		return a.str == b.str;
	case token_type::int_literal:
	case token_type::char_literal:
		return a.value == b.value;
	case token_type::symbol:
		return a.op == b.op;
	};
}



bool operator != (const token& a,
                  const token& b)
{
	return !(a == b);
}
