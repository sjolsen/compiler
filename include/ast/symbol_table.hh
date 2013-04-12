#ifndef SYMBOL_TABLE_HH
#define SYMBOL_TABLE_HH

#include <ast/astdef.hh>

#include <string>
#include <map>
#include <vector>



struct symbol_entry
{
	std::string name;
	std::vector <mc_type> type;
	basic_decl* decl_node;
};



struct symbol_table
	: std::map <std::string, symbol_entry>
{
};



#endif
