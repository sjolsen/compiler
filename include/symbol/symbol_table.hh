#ifndef SYMBOL_TABLE_HH
#define SYMBOL_TABLE_HH

#include <ast.hh>

#include <string>
#include <unordered_map>



enum class mc_type
{
	mc_void,
	integer,
	character,
	int_array,
	char_array
};



struct symbol_entry
{
	std::string name;
	mc_type type;
	const token* deftoken;
};



struct symbol_table
	: std::unordered_map <std::string, symbol_entry>
{
	symbol_table (const AST& tree);
};



#endif
