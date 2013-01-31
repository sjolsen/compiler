#include <fstream>
#include <iostream>
#include <algorithm>
#include "include/text_processing.hh"
#include "include/tokenizer.hh"

using namespace std;



string get_text (string filename)
{
	ifstream fin (filename.c_str ());

	fin.exceptions (ifstream::failbit |
	                ifstream::badbit |
	                ifstream::eofbit);

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
	if (argc < 2)
	{
		cerr << "No input files\n";
		return EXIT_FAILURE;
	}

	string text;
	try
	{
		text = get_text (argv [1]);
	}
	catch (typename ios_base::failure)
	{
		cerr << "Invalid filename\n";
		return EXIT_FAILURE;
	}

	try
	{
		auto tokens = tokenize (char_range (text));
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
