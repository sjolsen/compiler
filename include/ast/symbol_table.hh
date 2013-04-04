#ifndef SYMBOL_TABLE_HH
#define SYMBOL_TABLE_HH

#include <ast/astdef.hh>

#include <string>
#include <unordered_map>



struct symbol_entry
{
	std::string name;
	mc_type type;
	basic_decl* decl_node;
};



struct symbol_table
	: std::unordered_map <std::string, symbol_entry>
{
};



#endif
