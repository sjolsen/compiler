#include <fstream>
#include <iostream>
#include <algorithm>
#include "../../include/text_processing.hh"
#include "../../include/tokenizer.hh"

using namespace std;



string get_text (istream& fin)
{
	if (!fin.good ())
		throw runtime_error ("Bad input file");

	return string (istreambuf_iterator <char> (fin),
	               istreambuf_iterator <char> ());
}



ostream& operator << (ostream& os, file_position pos)
{
	return os << '(' << pos.first << ", " << pos.second << ')';
}



int main (int argc,
          char** argv)
{
	string file;

	if (argc < 2)
		file = get_text (cin);
	else
	{
		try
		{
			ifstream fin (argv [1]);
			file = get_text (fin);
		}
		catch (const runtime_error& e)
		{
			cerr << e.what () << endl;
			return EXIT_FAILURE;
		}
	}

	char_range text (file);
	try
	{
		auto tokens = tokenize (text);
		for (auto& token : tokens)
			cout << to_string (token) << endl;
	}
	catch (const syntax_error& e)
	{
		cerr << "Syntax error: " << e.what () << ' ' << file_position (text, e.where ()) << ":\n"
		     << string (e.where (), find (e.where (), text.end (), '\n')) << endl;
		return EXIT_FAILURE;
	}
}
