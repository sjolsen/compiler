#ifndef TOKEN_EXTRACTION_HH
#define TOKEN_EXTRACTION_HH

#include "tokendef.hh"
#include "token_predicates.hh"
#include "../text_processing.hh"
#include <vector>



void drop_nontokens (char_range& text);
token extract_token (char_range& text);
std::vector <token> tokenize (char_range text);



#endif
