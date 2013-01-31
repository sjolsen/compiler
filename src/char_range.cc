#include "../include/char_range.hh"
#include <stdexcept>
#include <iterator>

using namespace std;



// Just using begin and end int char_range::char_range (string&)
// results in a resolution error. This is used instead of string::begin
// and string::end to preserve ADL
inline string::iterator begin_proxy (string& s)
{ return begin (s); }
inline string::iterator end_proxy (string& s)
{ return end (s); }



char_range::char_range (string& text)
	: _begin (begin_proxy (text)),
	  _end (end_proxy (text))
{
}



char_range::char_range (iterator first,
                        iterator last)
	: _begin (first),
	  _end (last)
{
}



typename char_range::iterator
char_range::begin ()
{
	return _begin;
}



typename char_range::iterator
char_range::end ()
{
	return _end;
}



char& char_range::operator [] (size_t n)
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
