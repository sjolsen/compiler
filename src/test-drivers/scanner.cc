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
	auto line_begin = find (typename char_range::reverse_iterator (pos._pos),
	                        pos._file.rend (),
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
		auto line = containing_line (error_point);

		cerr << "Syntax error: " << e.what () << ' ' << error_point << ':' << endl
		     << line << endl;
		transform (line.begin (), error_point._pos, ostream_iterator <char> (cerr),
		           [] (char c) { return c == '\t'? '\t' : ' '; });
		cerr << '^' << endl;

		return EXIT_FAILURE;
	}
}
