#ifndef SYNTAX_ERROR_HH
#define SYNTAX_ERROR_HH

#include <stdexcept>
#include "text_processing.hh"



class syntax_error
	: public std::runtime_error
{
private:
	typename char_range::iterator _where_iter;

public:
	syntax_error (const std::string& what_str,
	              typename char_range::iterator where_iter);
	syntax_error (const char* const what_str,
	              typename char_range::iterator where_iter);

	virtual typename char_range::iterator where () const noexcept;
};



#endif
