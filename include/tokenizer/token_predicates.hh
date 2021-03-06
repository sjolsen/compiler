#ifndef TOKEN_PREDICATES_HH
#define TOKEN_PREDICATES_HH

#include <text_processing.hh>
#include <tokenizer/error.hh>
#include <tokenizer/tokendef.hh>

#include <array>



struct matcher
{
	token (*token_constructor) (char_range token_range,
	                            char_range file);
	char_range (*match_p) (char_range text,
	                       char_range file);

	inline char_range operator () (char_range text,
	                               char_range file)
	{ return (*match_p) (text, file); }
};



extern matcher identifier_p;
extern matcher keyword_p;
extern matcher int_literal_p;
extern matcher char_literal_p;
extern matcher string_literal_p;
extern matcher symbol_p;

extern matcher space_p;
extern matcher comment_p;

extern std::array <matcher, 6> token_predicates;
extern std::array <matcher, 2> nontoken_predicates;



#endif
