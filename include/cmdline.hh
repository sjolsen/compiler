#ifndef CMDLINE_HH
#define CMDLINE_HH

#include <string>



struct cmdline_args
{
	std::string input_filename,
	            output_filename;

	bool test_scanner,
	     test_parser,
	     test_semantic;

	enum
	{
		assembly,
		object,
		elf
	} output_type;

	cmdline_args (const int argc,
	              const char* const* const argv);
};



#endif
