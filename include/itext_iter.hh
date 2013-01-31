#ifndef ITEXT_ITER_HH
#define ITEXT_ITER_HH

#include <stdexcept>
#include <iterator>



class indexed_text::itext_iter
	: public std::iterator <std::random_access_iterator_tag,
	                        char>
{
	friend class indexed_text;

	std::vector <std::string>* _text;
	std::size_t _line;
	std::size_t _col;

public:
	itext_iter ();
	itext_iter (std::vector <std::string>& text,
	            std::size_t line,
	            std::size_t col);

	itext_iter (const itext_iter&) = default;
	itext_iter (itext_iter&&) = default;

	char& operator *();
	itext_iter& operator->();
	char& operator [] (std::size_t n);

	itext_iter& operator = (const itext_iter& other) = default;

	itext_iter& operator ++();
	itext_iter operator++ (int);
	itext_iter& operator --();
	itext_iter operator-- (int);

	itext_iter& operator += (std::ptrdiff_t);
	itext_iter operator + (std::ptrdiff_t n);
	itext_iter& operator -= (std::ptrdiff_t n);
	itext_iter operator - (std::ptrdiff_t n);

	std::size_t operator - (const itext_iter& other);

	bool operator == (const itext_iter& other);
	bool operator != (const itext_iter& other);
	bool operator < (const itext_iter& other);
	bool operator > (const itext_iter& other);
	bool operator <= (const itext_iter& other);
	bool operator >= (const itext_iter& other);

	std::size_t line () const
	{ return _line; }
	std::size_t col () const
	{ return _col; }
};



#endif
