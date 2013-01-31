#include "../include/token_extraction.hh"



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

	throw syntax_error ("Illegal token", text.begin ());
}



std::vector <token> tokenize (char_range text)
{
	std::vector <token> tokens;

	while (text)
	{
		drop_nontokens (text);
		tokens.push_back (extract_token (text));
	}

	return tokens;
}
