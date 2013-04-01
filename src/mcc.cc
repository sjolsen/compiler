#include <text_processing.hh>
#include <tokenizer.hh>
#include <ast.hh>
#include <semantic.hh>

#include <fstream>
#include <iostream>
#include <iterator>
#include <algorithm>

using namespace std;



const char usage [] =
	"Usage: mcc [ inputfile | - ] [ -Tscanner ]\n";



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



void print_syntax_error (char_range text,
                         const syntax_error& e)
{
	auto error_point = e.where ();
	auto line = containing_line (error_point);

	cerr << "Error: " << e.what () << ' ' << error_point << ':' << endl
	     << line << endl;
	transform (line.begin (), error_point._pos, ostream_iterator <char> (cerr),
	           [] (char c) { return c == '\t'? '\t' : ' '; });
	cerr << '^' << endl;
}



int main (int argc,
          char** argv)
{
	// Process command-line arguments

	if (argc < 2)
	{
		cerr << usage;
		return EXIT_FAILURE;
	}

	bool test_scanner = false;
	bool test_parser = false;

	if (find (argv + 2, argv + argc, string ("-Tscanner")) != argv + argc)
		test_scanner = true;
	if (find (argv + 2, argv + argc, string ("-Tparser")) != argv + argc)
		test_parser = true;

	// Open the input file

	string file;
	try
	{
		file = get_file (argv [1]);
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
	catch (const syntax_error& e)
	{
		print_syntax_error (text, e);
		return EXIT_FAILURE;
	}

	if (test_scanner)
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
	catch (const syntax_error& e)
	{
		print_syntax_error (text, e);
		return EXIT_FAILURE;
	}

	if (test_parser)
	{
		cout << to_string (syntax_tree) << endl;
		return EXIT_SUCCESS;
	}

	// Perform static analysis

	try
	{
		semantic_check (syntax_tree);
	}
	catch (const syntax_error& e)
	{
		print_syntax_error (text, e);
		return EXIT_FAILURE;
	}

	cout << to_string (syntax_tree) << endl;

	return EXIT_SUCCESS;
}
