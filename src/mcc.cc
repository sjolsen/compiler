#include <cmdline.hh>
#include <text_processing.hh>
#include <tokenizer.hh>
#include <ast.hh>
#include <semantic.hh>
#include <code_gen.hh>

#include <fstream>
#include <iostream>
#include <iterator>
#include <algorithm>

using namespace std;



string get_file (const string& filename)
{
	if (filename == "-")
		return string (istreambuf_iterator <char> (cin),
		               istreambuf_iterator <char> ());

	ifstream fin (filename);

	if (!fin.good ())
		throw runtime_error ("Bad input file");

	return string (istreambuf_iterator <char> (fin),
	               istreambuf_iterator <char> ());
}



void print_error (const error& e)
{
	auto error_point = e.where ();
	auto line = containing_line (error_point);

	cerr << "Error: " << e.what () << ' ' << error_point << ':' << endl
	     << line << endl;
	transform (line.begin (), error_point._pos, ostream_iterator <char> (cerr),
	           [] (char c) { return c == '\t' ? '\t' : ' '; });
	cerr << '^' << endl;
}



int main (int argc,
          char** argv)
{
	try
	{
		// Process command-line arguments

		cmdline_args args (argc, argv);

		// Open the input file

		string file;
		try
		{
			file = get_file (args.input_filename);
		}
		catch (const exception& e)
		{
			cerr << e.what () << endl;
			return EXIT_FAILURE;
		}
		char_range text (file);

		// Tokenize the input file

		vector <token> tokens;
		try
		{
			tokens = tokenize (text);
		}
		catch (const error& e)
		{
			print_error (e);
			return EXIT_FAILURE;
		}

		if (args.test_scanner)
		{
			for (auto& token : tokens)
				cout << to_string (token) << endl;
			return EXIT_SUCCESS;
		}

		// Parse the program

		program syntax_tree;
		try
		{
			syntax_tree = program_p (tokens);
		}
		catch (const error& e)
		{
			print_error (e);
			return EXIT_FAILURE;
		}

		if (args.test_parser)
		{
			cout << to_string (syntax_tree) << endl;
			return EXIT_SUCCESS;
		}

		// Perform static analysis

		try
		{
			semantic_check (syntax_tree);
		}
		catch (const error& e)
		{
			print_error (e);
			return EXIT_FAILURE;
		}

		if (args.test_semantic)
		{
			cout << to_string (syntax_tree) << endl;
			return EXIT_SUCCESS;
		}

		// Generate code

		vector <string> assembly = code_gen (syntax_tree);
		for (const string& line : assembly)
			cout << line << '\n';

		return EXIT_SUCCESS;
	}
	catch (const exception& e)
	{
		cerr << e.what () << endl;
		return EXIT_FAILURE;
	}
}
