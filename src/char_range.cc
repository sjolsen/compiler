#include "../include/char_range.hh"



char_range::char_range (indexed_text& text)
	: _begin (text.char_begin ()),
	  _end (text.char_end ()),
	  _size (_end - _begin)
{
}



char_range::char_range (iterator first,
                        iterator last)
	: _begin (first),
	  _end (last),
	  _size (_end - _begin)
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
	return begin () [n];
}



std::size_t char_range::size ()
{
	return _size;
}



bool char_range::empty ()
{
	return !_size;
}



char_range::operator bool ()
{
	return !empty ();
}



void char_range::drop_front (std::size_t n)
{
	if (n > _size)
		throw std::logic_error ("Called char_range::drop_front with to great an 'n'");
	_begin += n;
	_size -= n;
}



void char_range::drop_back (std::size_t n)
{
	if (n > _size)
		throw std::logic_error ("Called char_range::drop_back with to great an 'n'");
	_end -= n;
	_size -= n;
}
