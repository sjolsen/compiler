#include <tokenizer/token_range.hh>

#include <stdexcept>
#include <iterator>
#include <algorithm>

using namespace std;



token_range::token_range (const vector <token>& tokens)
	: _begin (tokens.cbegin ()),
	  _end (tokens.cend ())
{
}



token_range::token_range (iterator first,
                          iterator last)
	: _begin (first),
	  _end (last)
{
}



typename token_range::iterator
token_range::begin () const
{
	return _begin;
}



typename token_range::iterator
token_range::end () const
{
	return _end;
}



typename token_range::reverse_iterator
token_range::rbegin () const
{
	return reverse_iterator (_end);
}



typename token_range::reverse_iterator
token_range::rend () const
{
	return reverse_iterator (_begin);
}



const token& token_range::operator [] (size_t n)
{
	return _begin [n];
}



size_t token_range::size ()
{
	return _end - _begin;
}



bool token_range::empty ()
{
	return !size ();
}



token_range::operator bool ()
{
	return !empty ();
}



void token_range::drop_front (size_t n)
{
	if (n > size ())
		throw logic_error ("Called token_range::drop_front with too great an 'n'");
	_begin += n;
}



void token_range::drop_back (size_t n)
{
	if (n > size ())
		throw logic_error ("Called token_range::drop_back with too great an 'n'");
	_end -= n;
}
