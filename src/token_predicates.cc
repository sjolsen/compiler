#include "../include/token_predicates.hh"



matcher identifier_p
{
	[] (char_range token_range) -> token
	{
		token t;
		t.type = token_type::identifier;
		t.str = std::string (token_range.begin (), token_range.end ());
		return t;
	},

	[] (char_range text) -> char_range
	{
		if (!std::isalpha (*text.begin ()))
			return char_range ();
		return char_range (text.begin (),
		                   std::find_if_not (text.begin (), text.end (), (int (*) (int)) &std::isalnum));
	}
};



const std::array <const std::string, 8> keywords = {"if",
                                                    "else",
                                                    "while",
                                                    "int",
                                                    "string",
                                                    "char",
                                                    "return",
                                                    "void"};

matcher keyword_p
{
	[] (char_range token_range) -> token
	{
		token t;
		t.type = token_type::keyword;
		t.str = std::string (token_range.begin (), token_range.end ());
		return t;
	},

	[] (char_range text) -> char_range
	{
		auto token_range = identifier_p (text);
		if (token_range)
			for (const std::string& keyword : keywords)
				if (std::string (token_range.begin (), token_range.end ()) == keyword)
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
		t.value = std::stoi (std::string (token_range.begin (), token_range.end ()));
		return t;
	},

	[] (char_range text) -> char_range
	{
		if (!std::isdigit (text [0]))
			return char_range ();
		if (text [0] == '0' && text.size () > 1 && isdigit (text [1]))
			throw syntax_error ("Multidigit integer literals may not begin with '0'",
			                    text.begin ());
		return char_range (text.begin (),
		                   std::find_if_not (text.begin (), text.end (), (int (*) (int)) &std::isdigit));
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
				t.value = '\n';
				break;
			case 't':
				t.value = '\t';
				break;
			case '\'':
			case '\\':
				t.value = token_range [2];
				break;
			default:
				throw syntax_error ("Unrecognized character escape-sequence",
				                    token_range.begin ());
			}
		else
			t.value = token_range [1];
		return t;
	},

	[] (char_range text) -> char_range
	{
		if (text [0] != '\'')
			return char_range ();

		// Unescaped character literal
		if (text.size () < 3 || text [1] == '\n')
			throw syntax_error ("Unterminated character literal",
			                    text.begin ());
		if (text [1] != '\\')
			if (text [2] != '\'')
				throw syntax_error ("Multicharacter characer literal",
				                    text.begin ());
			else
				return char_range (text.begin (), text.begin () + 3);

		// Escaped character literal
		if (text.size () < 4 || text [2] == '\n')
			throw syntax_error ("Unterminated character literal",
			                    text.begin ());
		if (text [3] != '\'')
			throw syntax_error ("Multicharacter characer literal",
			                    text.begin ());
		return char_range (text.begin (), text.begin () + 4);
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
			char_range escapeless_substr (token_range.begin (), find (token_range.begin (),
			                                                          token_range.end (),
			                                                          '\\'));
			t.str.append (escapeless_substr.begin (), escapeless_substr.end ());
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
				                    token_range.begin ());
			};
			token_range.drop_front (2);
		}

		return t;
	},

	[] (char_range text) -> char_range
	{
		if (text [0] != '"')
			return char_range ();

		auto str_end = text.begin ();
		do
		{
			str_end = std::find_if (str_end + 1, text.end (),
			                        [] (char c) { return c == '\n' || c == '"'; });
			if (str_end == text.end ())
				throw syntax_error ("Unterminated string literal",
				                    text.begin ());
			if (*str_end == '\n')
				throw syntax_error ("Line-terminated string literal",
				                    text.begin ());
		} while (*(str_end - 1) == '\\');

		return char_range (text.begin (), str_end + 1);
	}
};



std::unordered_map <std::string, symbol> symbol_map = {{"+",  symbol::plus},
                                                       {"-",  symbol::minus},
                                                       {"*",  symbol::asterisk},
                                                       {"/",  symbol::solidus},
                                                       {"<",  symbol::less_than},
                                                       {">",  symbol::greater_than},
                                                       {"<=", symbol::lt_equiv},
                                                       {">=", symbol::gt_equiv},
                                                       {"==", symbol::equivalenet},
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
		t.op = symbol_map.at (std::string (token_range.begin (), token_range.end ()));
		return t;
	},

	[] (char_range text) -> char_range
	{
		switch (text [0])
		{
		case '<':
		case '>':
		case '=':
		case '!':
			if (text.size () > 1 && text [1] == '=')
				return char_range (text.begin (), text.begin () + 2);
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
			return char_range (text.begin (), text.begin () + 1);
		default:
			return char_range ();
		}
	}
};



std::array <matcher, 6> token_predicates = {identifier_p,
                                            keyword_p,
                                            int_literal_p,
                                            char_literal_p,
                                            string_literal_p,
                                            symbol_p};
