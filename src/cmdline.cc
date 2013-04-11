#include <cmdline.hh>

#include <stdexcept>
#include <algorithm>

using namespace std;



cmdline_args::cmdline_args (const int argc,
                            const char* const* const argv)
	: output_type (elf)
{
	string usage = "Usage: " + string (argv [0]) + " ( inputfile | - ) [ -Tscanner | -Tparser | -Tsemantic | -c | -S ] [ -o outputfile ]";

	if (argc < 2)
		throw runtime_error (usage);

	input_filename = string (argv [1]);
	auto o_opt_iterator = find (argv + 2, argv + argc, string ("-o"));
	if (o_opt_iterator != argv + argc)
	{
		if (o_opt_iterator == argv + argc - 1)
			throw runtime_error (usage);
		output_filename = string (*(o_opt_iterator + 1));
	}

	if (find (argv + 2, argv + argc, string ("-Tscanner")) != argv + argc)
		test_scanner = true;
	if (find (argv + 2, argv + argc, string ("-Tparser")) != argv + argc)
		test_parser = true;
	if (find (argv + 2, argv + argc, string ("-Tsemantic")) != argv + argc)
		test_semantic = true;

	if (find (argv + 2, argv + argc, string ("-c")) != argv + argc)
		output_type = object;
	if (find (argv + 2, argv + argc, string ("-S")) != argv + argc)
		output_type = assembly;
}
