#include "../include/indexed_text.hh"



indexed_text::itext_iter::itext_iter ()
	: _text (nullptr),
	  _line (0),
	  _col (0)
{
}



indexed_text::itext_iter::itext_iter (std::vector <std::string>& text,
                                      std::size_t line,
                                      std::size_t col)
	: _text (&text),
	  _line (line),
	  _col (col)
{
	if (line > text.size ())
		throw std::logic_error ("In indexed_text::iterator constructor: invalid line argument");
	else if (line == text.size ()) // To construct the end iterator
	{
		if (col > text [line - 1].size () + 1)
			throw std::logic_error ("In indexed_text::iterator constructor: invalid col argument");
	}
	else
		if (col > text [line - 1].size ())
			throw std::logic_error ("In indexed_text::iterator constructor: invalid col argument");
}



char&
indexed_text::itext_iter::operator *()
{
	return (*_text) [_line - 1][_col - 1];
}



typename indexed_text::itext_iter&
indexed_text::itext_iter::operator->()
{
	return *this;
}



typename indexed_text::itext_iter&
indexed_text::itext_iter::operator ++()
{
	if (_line == (*_text).size ())
		if (_col > (*_text) [_line - 1].size ())
			throw std::logic_error ("Incremented an indexed_text::iterator past the end");
		else
			++_col;
	else
		if (_col == (*_text) [_line - 1].size ())
		{
			++_line;
			_col = 1;
		}
		else
			++_col;


	return *this;
}



typename indexed_text::itext_iter
indexed_text::itext_iter::operator++ (int)
{
	auto old_iter = *this;
	++*this;
	return old_iter;
}



typename indexed_text::itext_iter&
indexed_text::itext_iter::operator --()
{
	if (_col == 1)
		if (_line == 1)
			throw std::logic_error ("Decremented an indexed_text::iterator past the beginning");
		else
		{
			--_line;
			_col = (*_text) [_line - 1].size ();
		}
	else
		--_col;
	return *this;
}



typename indexed_text::itext_iter
indexed_text::itext_iter::operator-- (int)
{
	auto old_iter = *this;
	--*this;
	return old_iter;
}



typename indexed_text::itext_iter&
indexed_text::itext_iter::operator += (std::ptrdiff_t n)
{
	if (n < 0)
		return *this -= -n;

	n += _col;
	_col = 0;
	while (_line < (*_text).size ())
	{
		if (n > (*_text) [_line - 1].size ())
		{
			n -= (*_text) [_line - 1].size ();
			++_line;
		}
		else
		{
			_col += n;
			return *this;
		}
	}

	if (n > (*_text).back ().size () + 1)
		throw std::logic_error ("Incremented an indexed_text::iterator past the end");
	_col += n;
	return *this;
}


typename indexed_text::itext_iter
indexed_text::itext_iter::operator + (std::ptrdiff_t n)
{
	auto new_iter = *this;
	new_iter += n;
	return new_iter;
}



typename indexed_text::itext_iter&
indexed_text::itext_iter::operator -= (std::ptrdiff_t n)
{
	if (n < 0)
		return *this += -n;

	n -= _col;
	_col = 0;
	while (_line > 1)
	{
		if (n >= 0)
		{
			--_line;
			n -= (*_text) [_line - 1].size ();
		}
		else
		{
			_col += -n;
			return *this;
		}
	}
	if (n >= (*_text).front ().size ())
		throw std::logic_error ("Decremented an indexed_text::iterator past the beginning");
	_col += (*_text).front ().size () - n;
	return *this;
}



typename indexed_text::itext_iter
indexed_text::itext_iter::operator - (std::ptrdiff_t n)
{
	auto new_iter = *this;
	new_iter -= n;
	return new_iter;
}



std::size_t
indexed_text::itext_iter::operator - (const itext_iter& other)
{
	std::size_t difference = 0;

	difference += _col;
	for (typename std::vector <std::string>::size_type i = 0; i < _line - 1; ++i)
		difference += (*_text) [i].size ();
	difference -= other._col;
	for (typename std::vector <std::string>::size_type i = 0; i < other._line - 1; ++i)
		difference -= (*other._text) [i].size ();

	return difference;
}



bool
indexed_text::itext_iter::operator == (const itext_iter& other)
{
	if (_text == other._text)
		return _line == other._line && _col == other._col;
	return false;
}



bool
indexed_text::itext_iter::operator != (const itext_iter& other)
{
	return !(*this == other);
}



bool
indexed_text::itext_iter::operator < (const itext_iter& other)
{
	if (_line < other._line)
		return true;
	if (_line > other._line)
		return false;

	if (_col < other._col)
		return true;
	return false;
}



bool
indexed_text::itext_iter::operator > (const itext_iter& other)
{
	if (_line > other._line)
		return true;
	if (_line < other._line)
		return false;

	if (_col > other._col)
		return true;
	return false;
}



bool
indexed_text::itext_iter::operator <= (const itext_iter& other)
{
	return !(*this > other);
}



bool
indexed_text::itext_iter::operator >= (const itext_iter& other)
{
	return !(*this < other);
}
