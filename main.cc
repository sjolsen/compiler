#include <fstream>
#include <iostream>
#include "include/indexed_text.hh"
#include "include/char_range.hh"

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
	reverse (r.begin (), r.end ());
	for (char c : text)
		cout << c;
}
