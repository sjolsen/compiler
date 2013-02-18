#include "../../include/symbol/symbol_table.hh"

using namespace std;



symbol_table::symbol_table (const vector <token>& tokens)
{
	for (const token& token : tokens)
		if (token.type == token_type::identifier)
			(*this) [token.str] = symbol_entry {token.str};
}
