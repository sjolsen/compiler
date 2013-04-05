#include <tokenizer/token_predicates.hh>

#include <cctype>
#include <algorithm>
#include <unordered_map>
#include <iterator>

// Disallow the '!' operator
#define __SYNTAX_NO_BANG
// Disallow C99-style comments
//#define __SYNTAX_NO_C99_COMMENTS

using namespace std;



matcher identifier_p
{
	[] (char_range token_range,
	    char_range) -> token
	{
		token t;
		t.type = token_type::identifier;
		t.str = to_string (token_range);
		return t;
	},

	[] (char_range working_set,
	    char_range file) -> char_range
	{
		if (*begin (working_set) == '_')
			throw error ("Identifiers may not contain underscores",
			                    file_position (file, begin (working_set)));
		if (!isalpha (*begin (working_set)))
			return char_range ();

		auto token_end = find_if_not (begin (working_set), end (working_set), (int (*) (int)) &isalnum);
		if (token_end != end (working_set) && *token_end == '_')
			throw error ("Identifiers may not contain underscores",
			                    file_position (file, token_end));

		return char_range (begin (working_set), token_end);

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
	[] (char_range token_range,
	    char_range) -> token
	{
		token t;
		t.type = token_type::keyword;
		t.str = to_string (token_range);
		return t;
	},

	[] (char_range working_set,
	    char_range file) -> char_range
	{
		// Treat keywords as a subset of identifiers
		auto token_range = identifier_p (working_set, file);
		if (token_range)
			for (const string& keyword : keywords)
				if (to_string (token_range) == keyword)
					return token_range;
		return char_range ();
	}
};



matcher int_literal_p
{
	[] (char_range token_range,
	    char_range file) -> token
	{
		token t;
		t.type = token_type::int_literal;

		try
		{
			t.value = stoi (to_string (token_range));
		}
		catch (const out_of_range& e)
		{
			throw error ("Integer literal exceeds value limits",
			                    file_position (file, begin (token_range)));
		}

		return t;
	},

	[] (char_range working_set,
	    char_range file) -> char_range
	{
		if (!isdigit (working_set [0]))
			return char_range ();
		if (working_set [0] == '0' && working_set.size () > 1 && isdigit (working_set [1]))
			throw error ("Multidigit integer literals may not begin with '0'",
			                    file_position (file, begin (working_set)));

		auto token_end = find_if_not (begin (working_set), end (working_set), (int (*) (int)) &isdigit);
		if (token_end == end (working_set) || !isalpha (*token_end))
			return char_range (begin (working_set), token_end);
		throw error ("Identifiers must begin with a letter",
		                    file_position (file, begin (working_set)));
	}
};



matcher char_literal_p
{
	[] (char_range token_range,
	    char_range file) -> token
	{
		token t;
		t.type = token_type::char_literal;
		t.literal = to_string (token_range);

		if (token_range [1] == '\\')
			switch (token_range [2])
			{
			case 'a':
				t.value = '\a';
				break;
			case 'b':
				t.value = '\b';
				break;
			case 'f':
				t.value = '\f';
				break;
			case 'n':
				t.value = '\n';
				break;
			case 'r':
				t.value = '\r';
				break;
			case 't':
				t.value = '\t';
				break;
			case 'v':
				t.value = '\v';
				break;
			case '0':
				t.value = '\0';
				break;
			case '\?':
			case '\'':
			case '\\':
				t.value = token_range [2];
			break;
			default:
				throw error ("Unrecognized character escape-sequence",
				                    file_position (file, begin (token_range) + 1));
			}
		else
			t.value = token_range [1];

		return t;
	},

	[] (char_range working_set,
	    char_range file) -> char_range
	{
		if (working_set [0] != '\'')
			return char_range ();
		if (working_set.size () < 3 || working_set [1] == '\n')
			throw error ("Unterminated character literal",
			                    file_position (file, begin (working_set)));

		// Unescaped character literal
		if (working_set [1] != '\\')
		{
			if (working_set [2] != '\'')
			{
				if (working_set [2] == '\n')
					throw error ("Unterminated character literal",
					                    file_position (file, begin (working_set)));
				else
					throw error ("Multicharacter characer literal",
					                    file_position (file, begin (working_set)));
			}
			else
				return char_range (begin (working_set), begin (working_set) + 3);
		}

		// Escaped character literal
		if (working_set.size () < 4 || working_set [2] == '\n')
			throw error ("Unterminated character literal",
			                    file_position (file, begin (working_set)));
		if (working_set [3] != '\'')
			throw error ("Multicharacter characer literal",
			                    file_position (file, begin (working_set)));
		return char_range (begin (working_set), begin (working_set) + 4);
	}
};



matcher string_literal_p
{
	[] (char_range token_range,
	    char_range file) -> token
	{
		token t;
		t.type = token_type::string_literal;
		t.literal = to_string (token_range);

		token_range.drop_front (1);
		token_range.drop_back (1);

		while (token_range)
		{
			// Pull out the most unescaped working_set possible
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
			case 'a':
				t.str.append (1, '\a');
				break;
			case 'b':
				t.str.append (1, '\b');
				break;
			case 'f':
				t.str.append (1, '\f');
				break;
			case 'n':
				t.str.append (1, '\n');
				break;
			case 'r':
				t.str.append (1, '\r');
				break;
			case 't':
				t.str.append (1, '\t');
				break;
			case 'v':
				t.str.append (1, '\v');
				break;
			case '0':
				t.str.append (1, '\0');
				break;
			case '\?':
			case '"':
			case '\\':
				t.str.append (1, token_range [1]);
			break;
			default:
				throw error ("Unrecognized character escape sequence",
				                    file_position (file, begin (token_range)));
			};
			token_range.drop_front (2);
		}

		return t;
	},

	[] (char_range working_set,
	    char_range file) -> char_range
	{
		if (working_set [0] != '"')
			return char_range ();

		auto escaped = [working_set] (char_range::iterator i) -> bool // For determining whether not or the double-quote found is escaped
		{
			auto first_non_backslash = find_if_not (typename char_range::reverse_iterator (i),
			                                        working_set.rend (),
			                                        [] (char c) { return c == '\\'; });
			if ((first_non_backslash - typename char_range::reverse_iterator (i)) % 2 == 0) // All immediately adjacent backslashes are escaped
				return false;
			return true;
		};

		auto str_end = begin (working_set);
		do
		{
			str_end = find_if (str_end + 1, end (working_set),
			                   [] (char c) { return c == '\n' || c == '"'; });
			if (str_end == end (working_set))
				throw error ("Unterminated string literal",
				                    file_position (file, begin (working_set)));
			if (*str_end == '\n')
				throw error ("Line-terminated string literal",
				                    file_position (file, begin (working_set)));
		} while (escaped (str_end));

		return char_range (begin (working_set), str_end + 1);
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
	[] (char_range token_range,
	    char_range) -> token
	{
		token t;
		t.type = token_type::symbol;
		t.op = symbol_map.at (to_string (token_range));
		return t;
	},

	[] (char_range working_set,
	    char_range file) -> char_range
	{
		switch (working_set [0])
		{
		case '!':
		#ifdef __SYNTAX_NO_BANG
			if (working_set.size () < 2 || working_set [1] != '=')
				throw error ("mC does not specify the '!' operator",
				                    file_position (file, begin (working_set)));
		#endif
		case '<':
		case '>':
		case '=':
			if (working_set.size () > 1 && working_set [1] == '=')
				return char_range (begin (working_set), begin (working_set) + 2);
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
			return char_range (begin (working_set), begin (working_set) + 1);
		default:
			return char_range ();
		}
	}
};



matcher space_p
{
	[] (char_range,
	    char_range) -> token
	{
		throw logic_error ("Attempted to construct a token from a non-token range");
	},

	[] (char_range working_set,
	    char_range) -> char_range
	{
		return char_range (begin (working_set),
		                   find_if_not (begin (working_set), end (working_set),
		                                (int (*) (int)) &isspace));
	}
};



matcher comment_p
{
	[] (char_range,
	    char_range) -> token
	{
		throw logic_error ("Attempted to construct a token from a non-token range");
	},

	[] (char_range working_set,
	    char_range file) -> char_range
	{
		if (working_set [0] != '/' || working_set.size () < 2)
			return char_range ();

		#ifndef __SYNTAX_NO_C99_COMMENTS
		if (working_set [1] == '/')
			return char_range (begin (working_set), find (begin (working_set), end (working_set), '\n'));
		#endif

		if (working_set [1] != '*')
			return char_range ();

		auto comment_close = adjacent_find (begin (working_set), end (working_set),
		                                    [] (char a, char b) { return a == '*' && b == '/'; });
		if (comment_close == end (working_set))
			throw error ("Unterminated comment",
			                    file_position (file, begin (working_set)));
		return char_range (begin (working_set), comment_close + 2);
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
