#include <fstream>
#include <iostream>
#include "include/text_processing.hh"
#include "include/tokenizer.hh"

#include <algorithm>

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

	char_range r (text);
	char_range token_r = identifier_p (r);
	cout << string (begin (token_r), end (token_r)) << endl;
}
