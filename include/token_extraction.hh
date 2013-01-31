#ifndef TOKEN_EXTRACTION_HH
#define TOKEN_EXTRACTION_HH

#include <vector>
#include "text_processing.hh"
#include "tokendef.hh"
#include "token_predicates.hh"



void drop_nontokens (char_range& text);
token extract_token (char_range& text);
std::vector <token> tokenize (char_range text);



#endif
