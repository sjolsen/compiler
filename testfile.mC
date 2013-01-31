token extract token (char range ref text)
{
	for (auto match p in token predicates)
	{
		auto token range = match p (text);
		if (token range)
		{
			text drop front (token range dot size ());
			return match p token constructor (token range);
		}
	}

	throw syntax error ("Illegal token", text dot begin ());
}

/* With the non-mC syntax removed */

std vector <token> tokenize (char range text)
{
	std vector <token> tokens;

	while (text)
	{
		drop nontokens (text);
		tokens dot push back (extract token (text));
	}

	return tokens;
}
