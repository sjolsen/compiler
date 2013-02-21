#ifndef TOKEN_RANGE_HH
#define TOKEN_RANGE_HH

#include <tokenizer/tokendef.hh>

#include <vector>



class token_range
{
public:
	typedef typename std::vector <token>::const_iterator iterator;
	typedef typename std::vector <token>::const_reverse_iterator reverse_iterator;

private:
	iterator _begin;
	iterator _end;

public:
	token_range () = default;
	token_range (const token_range& other) = default;
	token_range (const std::vector <token>& tokens);
	token_range (iterator first,
	             iterator last);

	iterator begin () const;
	iterator end () const;
	reverse_iterator rbegin () const;
	reverse_iterator rend () const;

	const token& operator [] (std::size_t n);

	std::size_t size ();
	bool empty ();
	operator bool ();

	void drop_front (std::size_t n);
	void drop_back (std::size_t n);
};



#endif
