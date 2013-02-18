#include "../../include/tokenizer/token_extraction.hh"

using namespace std;



void drop_nontokens (char_range& text)
{
	bool dropped;
	do
	{
		dropped = false;
		for (auto match_p : nontoken_predicates)
		{
			auto nontoken_range = match_p (text);
			if (nontoken_range)
			{
				text.drop_front (nontoken_range.size ());
				dropped = true;
			}
		}
	}
	while (dropped == true);
}



token extract_token (char_range& text)
{
	for (auto match_p : token_predicates)
	{
		auto token_range = match_p (text);
		if (token_range)
		{
			text.drop_front (token_range.size ());
			return match_p.token_constructor (token_range);
		}
	}

	throw syntax_error ("Illegal token", begin (text));
}



vector <token> tokenize (char_range text)
{
	vector <token> tokens;

	char_range working_set = text;
	while (working_set)
	{
		drop_nontokens (working_set);
		if (working_set)
		{
			auto token_start = begin (working_set);
			token t = extract_token (working_set);
			t.pos = file_position (text, token_start);
			tokens.emplace_back (move (t));
		}
	}

	return tokens;
}
