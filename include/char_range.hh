#ifndef CHAR_RANGE_HH
#define CHAR_RANGE_HH

#include <stdexcept>
#include "indexed_text.hh"



class char_range
{
public:
	typedef typename indexed_text::char_iterator iterator;

private:
	iterator _begin;
	iterator _end;
	std::size_t _size;

public:
	char_range () = default;
	char_range (const char_range& other) = default;
	char_range (indexed_text& text);
	char_range (iterator first,
	            iterator last);

	iterator begin ();
	iterator end ();

	std::size_t size ();
	bool empty ();
	operator bool ();

	void drop_front (std::size_t n);
};



#endif
