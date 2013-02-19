#ifndef CHAR_RANGE_HH
#define CHAR_RANGE_HH

#include <string>
#include <iostream>



class char_range
{
public:
	typedef typename std::string::const_iterator iterator;
	typedef typename std::string::const_reverse_iterator reverse_iterator;

private:
	iterator _begin;
	iterator _end;

public:
	char_range () = default;
	char_range (const char_range& other) = default;
	char_range (const std::string& text);
	char_range (iterator first,
	            iterator last);

	iterator begin () const;
	iterator end () const;
	reverse_iterator rbegin () const;
	reverse_iterator rend () const;

	const char& operator [] (std::size_t n);

	std::size_t size ();
	bool empty ();
	operator bool ();

	void drop_front (std::size_t n);
	void drop_back (std::size_t n);
};



std::string to_string (const char_range& r);
std::ostream& operator << (std::ostream& os,
                           const char_range& r);



#endif
