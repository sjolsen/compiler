#include "../include/char_range.hh"



char_range::char_range (std::string& text)
	: _begin (text.begin ()),
	  _end (text.end ())
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



char& char_range::operator [] (std::size_t n)
{
	return _begin [n];
}



std::size_t char_range::size ()
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



void char_range::drop_front (std::size_t n)
{
	if (n > size ())
		throw std::logic_error ("Called char_range::drop_front with to great an 'n'");
	_begin += n;
}



void char_range::drop_back (std::size_t n)
{
	if (n > size ())
		throw std::logic_error ("Called char_range::drop_back with to great an 'n'");
	_end -= n;
}
