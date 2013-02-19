#include <tokenizer/token_extraction.hh>

using namespace std;



void drop_nontokens (char_range& working_set,
                     char_range file)
{
	bool dropped;
	do
	{
		dropped = false;
		for (auto match_p : nontoken_predicates)
		{
			auto nontoken_range = match_p (working_set, file);
			if (nontoken_range)
			{
				working_set.drop_front (nontoken_range.size ());
				dropped = true;
			}
		}
	}
	while (dropped == true);
}



token extract_token (char_range& working_set,
                     char_range file)
{
	for (auto match_p : token_predicates)
	{
		auto token_range = match_p (working_set, file);
		if (token_range)
		{
			working_set.drop_front (token_range.size ());
			return match_p.token_constructor (token_range, file);
		}
	}

	throw syntax_error ("Illegal token", file_position (file, begin (working_set)));
}



deque <token> tokenize (char_range file)
{
	deque <token> tokens;

	char_range working_set = file;
	while (working_set)
	{
		drop_nontokens (working_set, file);
		if (working_set)
		{
			auto token_start = begin (working_set);
			token t = extract_token (working_set, file);
			t.pos = file_position (file, token_start);
			tokens.emplace_back (move (t));
		}
	}

	return tokens;
}
