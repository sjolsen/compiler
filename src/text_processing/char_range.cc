#include "../../include/text_processing/char_range.hh"
#include <stdexcept>
#include <iterator>
#include <algorithm>

using namespace std;



char_range::char_range (const string& text)
	: _begin (text.cbegin ()),
	  _end (text.cend ())
{
}



char_range::char_range (iterator first,
                        iterator last)
	: _begin (first),
	  _end (last)
{
}



typename char_range::iterator
char_range::begin () const
{
	return _begin;
}



typename char_range::iterator
char_range::end () const
{
	return _end;
}



typename char_range::reverse_iterator
char_range::rbegin () const
{
	return reverse_iterator (_end);
}



typename char_range::reverse_iterator
char_range::rend () const
{
	return reverse_iterator (_begin);
}



const char& char_range::operator [] (size_t n)
{
	return _begin [n];
}



size_t char_range::size ()
{
	return _end - _begin;
}



bool char_range::empty ()
{
	return !size ();
}



char_range::operator bool ()
{
	return !empty ();
}



void char_range::drop_front (size_t n)
{
	if (n > size ())
		throw logic_error ("Called char_range::drop_front with to great an 'n'");
	_begin += n;
}



void char_range::drop_back (size_t n)
{
	if (n > size ())
		throw logic_error ("Called char_range::drop_back with to great an 'n'");
	_end -= n;
}



string to_string (const char_range& r)
{
	return string (begin (r), end (r));
}



ostream& operator << (ostream& os,
                      const char_range& r)
{
	copy (begin (r), end (r), ostream_iterator <char> (os));
	return os;
}
