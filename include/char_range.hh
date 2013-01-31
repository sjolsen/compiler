#ifndef CHAR_RANGE_HH
#define CHAR_RANGE_HH

#include <string>



class char_range
{
public:
	typedef typename std::string::iterator iterator;

private:
	iterator _begin;
	iterator _end;

public:
	char_range () = default;
	char_range (const char_range& other) = default;
	char_range (std::string& text);
	char_range (iterator first,
	            iterator last);

	iterator begin ();
	iterator end ();

	char& operator [] (std::size_t n);

	std::size_t size ();
	bool empty ();
	operator bool ();

	void drop_front (std::size_t n);
	void drop_back (std::size_t n);
};



#endif
