#include "../include/indexed_text.hh"



indexed_text::indexed_text ()
	: text {""}
{
}



indexed_text::indexed_text (indexed_text&& other)
	: text (std::move (other.text))
{
}



indexed_text& indexed_text::operator = (indexed_text&& other)
{
	using std::swap;
	swap (text, other.text);
	return *this;
}



typename indexed_text::char_iterator
indexed_text::char_begin ()
{
	return char_iterator (text, 1, 1);
}



typename indexed_text::char_iterator
indexed_text::char_end ()
{
	return char_iterator (text, text.size (), text.back ().size () + 1);
}



typename indexed_text::char_iterator
indexed_text::begin ()
{
	return char_begin ();
}



typename indexed_text::char_iterator
indexed_text::end ()
{
	return char_end ();
}



typename indexed_text::line_iterator
indexed_text::line_begin ()
{
	return text.begin ();
}



typename indexed_text::line_iterator
indexed_text::line_end ()
{
	return text.end ();
}



std::size_t
indexed_text::size ()
{
	size_t size = 0;
	for (const std::string& line : text)
		size += line.size ();
	return size;
}



typename indexed_text::char_iterator
indexed_text::construct_iterator (std::size_t line,
                                  std::size_t col)
{
	return char_iterator (text, line, col);
}



typename indexed_text::line_iterator
indexed_text::construct_iterator (std::size_t line)
{
	return text.begin () + (line - 1);
}
