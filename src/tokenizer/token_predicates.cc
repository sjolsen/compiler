#include "../../include/tokenizer/token_predicates.hh"
#include <cctype>
#include <algorithm>
#include <unordered_map>
#include <iterator>

//#define SYNTAX_NO_BANG

using namespace std;



matcher identifier_p
{
	[] (char_range token_range) -> token
	{
		token t;
		t.type = token_type::identifier;
		t.str = string (begin (token_range), end (token_range));
		return t;
	},

	[] (char_range text) -> char_range
	{
		if (*begin (text) == '_')
			throw syntax_error ("Identifiers may not contain underscores",
			                    begin (text));
		if (!isalpha (*begin (text)))
			return char_range ();

		auto token_end = find_if_not (begin (text), end (text), (int (*) (int)) &isalnum);
		if (token_end != end (text) && *token_end == '_')
			throw syntax_error ("Identifiers may not contain underscores",
			                    begin (text));

		return char_range (begin (text), token_end);

	}
};



const array <const string, 8> keywords = {{"if",
                                           "else",
                                           "while",
                                           "int",
                                           "string",
                                           "char",
                                           "return",
                                           "void"}};

matcher keyword_p
{
	[] (char_range token_range) -> token
	{
		token t;
		t.type = token_type::keyword;
		t.str = string (begin (token_range), end (token_range));
		return t;
	},

	[] (char_range text) -> char_range
	{
		auto token_range = identifier_p (text);
		if (token_range)
			for (const string& keyword : keywords)
				if (string (begin (token_range), end (token_range)) == keyword)
					return token_range;
		return char_range ();
	}
};



matcher int_literal_p
{
	[] (char_range token_range) -> token
	{
		token t;
		t.type = token_type::int_literal;
		t.value = stoi (string (begin (token_range), end (token_range)));
		return t;
	},

	[] (char_range text) -> char_range
	{
		if (!isdigit (text [0]))
			return char_range ();
		if (text [0] == '0' && text.size () > 1 && isdigit (text [1]))
			throw syntax_error ("Multidigit integer literals may not begin with '0'",
			                    begin (text));

		auto token_end = find_if_not (begin (text), end (text), (int (*) (int)) &isdigit);
		if (token_end == end (text) || !isalpha (*token_end))
			return char_range (begin (text), token_end);
		throw syntax_error ("Identifiers must begin with a letter",
		                    begin (text));
	}
};



matcher char_literal_p
{
	[] (char_range token_range) -> token
	{
		token t;
		t.type = token_type::char_literal;
		if (token_range [1] == '\\')
			switch (token_range [2])
			{
			case 'n':
				t.cvalue = '\n';
				break;
			case 't':
				t.cvalue = '\t';
				break;
			case '\'':
			case '\\':
				t.cvalue = token_range [2];
			break;
			default:
				throw syntax_error ("Unrecognized character escape-sequence",
				                    begin (token_range));
			}
		else
			t.cvalue = token_range [1];
		return t;
	},

	[] (char_range text) -> char_range
	{
		if (text [0] != '\'')
			return char_range ();
		if (text.size () < 3 || text [1] == '\n')
			throw syntax_error ("Unterminated character literal",
			                    begin (text));

		// Unescaped character literal
		if (text [1] != '\\')
		{
			if (text [2] != '\'')
			{
				if (text [2] == '\n')
					throw syntax_error ("Unterminated character literal",
					                    begin (text));
				else
					throw syntax_error ("Multicharacter characer literal",
					                    begin (text));
			}
			else
				return char_range (begin (text), begin (text) + 3);
		}

		// Escaped character literal
		if (text.size () < 4 || text [2] == '\n')
			throw syntax_error ("Unterminated character literal",
			                    begin (text));
		if (text [3] != '\'')
			throw syntax_error ("Multicharacter characer literal",
			                    begin (text));
		return char_range (begin (text), begin (text) + 4);
	}
};



matcher string_literal_p
{
	[] (char_range token_range) -> token
	{
		token t;
		t.type = token_type::string_literal;
		token_range.drop_front (1);
		token_range.drop_back (1);

		while (token_range)
		{
			char_range escapeless_substr (begin (token_range), find (begin (token_range),
			                                                          end (token_range),
			                                                          '\\'));
			t.str.append (begin (escapeless_substr), end (escapeless_substr));
			token_range.drop_front (escapeless_substr.size ());
			if (!token_range)
				break;

			// Handle an escape sequence
			switch (token_range [1])
			{
			case 'n':
				t.str.append (1, '\n');
				break;
			case 't':
				t.str.append (1, '\t');
				break;
			case '"':
			case '\\':
				t.str.append (1, token_range [1]);
			break;
			default:
				throw syntax_error ("Unrecognized character escape sequence",
				                    begin (token_range));
			};
			token_range.drop_front (2);
		}

		return t;
	},

	[] (char_range text) -> char_range
	{
		if (text [0] != '"')
			return char_range ();

		auto escaped = [text] (char_range::iterator i) // For determining whether or the double-quote found is escaped
		{
			auto first_non_backslash = find_if_not (reverse_iterator <decltype (i)> (i),
			                                        reverse_iterator <decltype (begin (text))> (begin (text)),
			                                        [] (char c) { return c == '\\'; });
			if ((first_non_backslash - reverse_iterator <decltype (i)> (i)) % 2 == 0) // All immediately adjacent backslashes are escaped
				return false;
			return true;
		};

		auto str_end = begin (text);
		do
		{
			str_end = find_if (str_end + 1, end (text),
			                   [] (char c) { return c == '\n' || c == '"'; });
			if (str_end == end (text))
				throw syntax_error ("Unterminated string literal",
				                    begin (text));
			if (*str_end == '\n')
				throw syntax_error ("Line-terminated string literal",
				                    begin (text));
		} while (escaped (str_end));

		return char_range (begin (text), str_end + 1);
	}
};



unordered_map <string, symbol> symbol_map = {{"+",  symbol::plus},
                                             {"-",  symbol::minus},
                                             {"*",  symbol::asterisk},
                                             {"/",  symbol::solidus},
                                             {"<",  symbol::less_than},
                                             {">",  symbol::greater_than},
                                             {"<=", symbol::lt_equiv},
                                             {">=", symbol::gt_equiv},
                                             {"==", symbol::equivalent},
                                             {"!=", symbol::not_equiv},
                                             {"=",  symbol::equal},
                                             {"!",  symbol::bang},
                                             {"[",  symbol::lbracket},
                                             {"]",  symbol::rbracket},
                                             {"{",  symbol::lbrace},
                                             {"}",  symbol::rbrace},
                                             {"(",  symbol::lparen},
                                             {")",  symbol::rparen},
                                             {",",  symbol::comma},
                                             {";",  symbol::semicolon}};

matcher symbol_p
{
	[] (char_range token_range) -> token
	{
		token t;
		t.type = token_type::symbol;
		t.op = symbol_map.at (string (begin (token_range), end (token_range)));
		return t;
	},

	[] (char_range text) -> char_range
	{
		switch (text [0])
		{
		case '!':
		#ifdef SYNTAX_NO_BANG
			if (text.size () < 2 || text [1] != '=')
				throw syntax_error ("mC does not specify the '!' operator",
				                    begin (text));
		#endif
		case '<':
		case '>':
		case '=':
			if (text.size () > 1 && text [1] == '=')
				return char_range (begin (text), begin (text) + 2);
		case '+':
		case '-':
		case '*':
		case '/':
		case '[':
		case ']':
		case '{':
		case '}':
		case '(':
		case ')':
		case ',':
		case ';':
			return char_range (begin (text), begin (text) + 1);
		default:
			return char_range ();
		}
	}
};



matcher space_p
{
	[] (char_range token_range) -> token
	{
		throw logic_error ("Attempted to construct a token from a non-token range");
	},

	[] (char_range text) -> char_range
	{
		return char_range (begin (text),
		                   find_if_not (begin (text), end (text),
		                                (int (*) (int)) &isspace));
	}
};



matcher comment_p
{
	[] (char_range token_range) -> token
	{
		throw logic_error ("Attempted to construct a token from a non-token range");
	},

	[] (char_range text) -> char_range
	{
		if (text [0] != '/' || text [1] != '*')
			return char_range ();

		auto comment_close = adjacent_find (begin (text), end (text),
		                                    [] (char a, char b) { return a == '*' && b == '/'; });
		if (comment_close == end (text))
			throw syntax_error ("Unterminated comment",
			                    begin (text));
		return char_range (begin (text), comment_close + 2);
	}
};



array <matcher, 6> token_predicates = {{keyword_p, // Check keywords first
                                        identifier_p,
                                        int_literal_p,
                                        char_literal_p,
                                        string_literal_p,
                                        symbol_p}};

array <matcher, 2> nontoken_predicates = {{space_p,
                                           comment_p}};
