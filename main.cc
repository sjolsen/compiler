#include <fstream>
#include <iostream>
#include "include/text_processing.hh"
#include "include/tokenizer.hh"

using namespace std;



indexed_text get_text (string filename)
{
	ifstream fin (filename.c_str ());

	fin.exceptions (ifstream::failbit |
	                ifstream::badbit |
	                ifstream::eofbit);

	return indexed_text (istreambuf_iterator <char> (fin),
	                     istreambuf_iterator <char> ());
}



ostream& operator << (ostream& os, const typename indexed_text::char_iterator& i)
{
	os << '(' << i.line () << ", " << i.col () << ')';
	return os;
}



int main (int argc,
          char** argv)
{
	if (argc < 2)
	{
		cerr << "No input files\n";
		return EXIT_FAILURE;
	}

	indexed_text text;
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
		cerr << e.what () << ' ' << e.where () << endl;
	}
}
