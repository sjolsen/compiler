#ifndef SYMBOL_TABLE_HH
#define SYMBOL_TABLE_HH

#include <ast.hh>
#include <text_processing.hh>

#include <string>
#include <unordered_map>



struct symbol_entry
{
	std::string name;
	mc_type type;
	Node <basic_decl> decl_node;
};



struct symbol_table
	: std::unordered_map <std::string, symbol_entry>
{
};



void populate_table (symbol_table& table,
                     const program& tree);
void populate_table (symbol_table& table,
                     const funDecl& tree);



#endif
