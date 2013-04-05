#ifndef SYNTAX_ERROR_HH
#define SYNTAX_ERROR_HH

#include <text_processing.hh>

#include <stdexcept>



class error
	: public std::runtime_error
{
private:
	file_position _where_pos;

public:
	error (const std::string& what_str,
	              file_position where_pos);
	error (const char* const what_str,
	              file_position where_pos);

	virtual file_position where () const noexcept;
};



#endif
