#include <fstream>
#include <iostream>
#include <iterator>
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
	return os << '(' << pos._line << ", " << pos._col << ')';
}



char_range containing_line (file_position pos)
{
	auto line_begin = find (reverse_iterator <typename char_range::iterator> (pos._pos),
	                        reverse_iterator <typename char_range::iterator> (begin (pos._file)),
	                        '\n').base ();
	auto line_end = find (pos._pos, end (pos._file), '\n');
	return char_range (line_begin, line_end);
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
		auto error_point = file_position (text, e.where ());
		cerr << "Syntax error: " << e.what () << ' ' << error_point << ':' << endl
		     << containing_line (error_point) << endl;
		fill_n (ostream_iterator <char> (cerr), error_point._col - 1, ' ');
		cerr << '^' << endl;
		return EXIT_FAILURE;
	}
}
