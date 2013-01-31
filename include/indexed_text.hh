#ifndef INDEXED_TEXT_HH
#define INDEXED_TEXT_HH

#include <vector>
#include <string>



class indexed_text
{
	struct itext_iter;
	std::vector <std::string> text;

public:
	typedef itext_iter char_iterator;
	typedef typename std::vector <std::string>::iterator line_iterator;

	template <typename InputIter>
	indexed_text (InputIter first,
	              InputIter last,
	              char delim = '\n')
		: text {""} // Maybe one day we'll get non-static in-class initializers
	{
		for (InputIter i = first; i != last; ++i)
		{
			text.back ().push_back (*i);
			if (*i == delim)
				text.emplace_back ();
		}
	}

	indexed_text ();
	indexed_text (indexed_text&& other);

	indexed_text& operator = (indexed_text&& other);

	char_iterator char_begin ();
	char_iterator char_end ();
	char_iterator begin ();
	char_iterator end ();

	line_iterator line_begin ();
	line_iterator line_end ();

	std::size_t size ();

	char_iterator construct_iterator (std::size_t line,
	                                  std::size_t col);
	line_iterator construct_iterator (std::size_t line);

	const std::vector <std::string>& impl_text () const
	{ return text; }
};



#include "itext_iter.hh"



#endif
