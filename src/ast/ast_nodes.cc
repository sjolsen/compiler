#include <ast/ast_nodes.hh>
#include <ast/astprint.hh>

using namespace std;



#include "auto.astdefs.cc"



// --- program ---

program::program (Node <declList> _decl_list)
	: decl_list (_decl_list),
	  table (new symbol_table)
{
	type = AST_type::program;
}
program::program ()
	: table (new symbol_table)
{
	type = AST_type::program;
}

vector <string> program::contents () const
{
	return collect (lines (*table),
	                valid (decl_list) ? lines (decl_list) : vector <string> {});
}


// --- varDecl ---

varDecl::varDecl (Node <typeSpecifier> _type_spec,
                  Node <terminal> _name,
                  Node <terminal> _size)
	: type_spec (_type_spec),
	  name (_name),
	  size (_size)
{
	type = AST_type::varDecl;
}

vector <string> varDecl::contents () const
{
	return collect (valid (type_spec) ? lines (type_spec) : vector <string> {},
	                valid (name) ? lines (name) : vector <string> {},
	                valid (size) ? lines (size) : vector <string> {});
}

string varDecl::get_name () const
{
	return name->token_ref.str;
}

mc_type varDecl::get_type () const
{
	static unordered_map <string, mc_type> var_map = {{"void", mc_type::mc_void},
	                                                  {"int", mc_type::integer},
	                                                  {"char", mc_type::character}};
	static unordered_map <string, mc_type> array_map = {{"int", mc_type::int_array},
	                                                    {"char", mc_type::char_array}};

	try
	{
		if (size)
			return array_map.at (type_spec->kwd_node->token_ref.str);
		return var_map.at (type_spec->kwd_node->token_ref.str);
	}
	catch (const exception&)
	{
		throw syntax_error ("Encountered an illegal variable declaration",
		                    type_spec->kwd_node->token_ref.pos);
	}
}

file_position varDecl::pos () const
{
	return type_spec->kwd_node->token_ref.pos;
}



// --- funDecl ---

funDecl::funDecl (Node <typeSpecifier> _type_spec,
                  Node <terminal> _name,
                  Node <formalDeclList> _decl_list,
                  Node <funBody> _body)
	: type_spec (_type_spec),
	  name (_name),
	  decl_list (_decl_list),
	  body (_body),
	  table (new symbol_table)
{
	type = AST_type::funDecl;
}

std::vector <std::string> funDecl::contents () const
{
	return collect (lines (*table),
	                valid (type_spec) ? lines (type_spec) : vector <string> {},
	                valid (name) ? lines (name) : vector <string> {},
	                valid (decl_list) ? lines (decl_list) : vector <string> {},
	                valid (body) ? lines (body) : vector <string> {});
}

std::string funDecl::get_name () const
{
	return name->token_ref.str;
}

mc_type funDecl::get_type () const
{
	return mc_type::function;
}

file_position funDecl::pos () const
{
	return type_spec->kwd_node->token_ref.pos;
}



// --- formalDecl ---

formalDecl::formalDecl (Node <typeSpecifier> _type_spec,
                        Node <terminal> _name,
                        bool _is_array)
	: type_spec (_type_spec),
	  name (_name),
	  is_array (_is_array)
{
	type = AST_type::formalDecl;
}

vector <string> formalDecl::contents () const
{
	return collect (valid (type_spec) ? lines (type_spec) : vector <string> {},
	                valid (name) ? lines (name) : vector <string> {},
	                valid (is_array) ? lines (is_array) : vector <string> {});
}

string formalDecl::get_name () const
{
	return name->token_ref.str;
}

mc_type formalDecl::get_type () const
{
	static unordered_map <string, mc_type> var_map = {{"void", mc_type::mc_void},
	                                                  {"int", mc_type::integer},
	                                                  {"char", mc_type::character}};
	static unordered_map <string, mc_type> array_map = {{"int", mc_type::int_array},
	                                                    {"char", mc_type::char_array}};

	try
	{
		if (is_array)
			return array_map.at (type_spec->kwd_node->token_ref.str);
		return var_map.at (type_spec->kwd_node->token_ref.str);
	}
	catch (const exception&)
	{
		throw syntax_error ("Encountered an illegal variable declaration",
		                    type_spec->kwd_node->token_ref.pos);
	}
}

file_position formalDecl::pos () const
{
	return type_spec->kwd_node->token_ref.pos;
}



// --- terminal ---

terminal::terminal (const token& t)
	: token_ref (t)
{
	type = AST_type::terminal;
}

vector <string> terminal::contents () const
{
	return {to_string (token_ref)};
}



void populate_table (symbol_table& table,
                     const declList& decl_list)
{
	for (const Node <decl>& decl_node : decl_list.decls)
	{
		if (table.count (decl_node->sub_decl->get_name ()) > 0)
			throw syntax_error ("Redefined identifier (first defined at " +
			                    to_string (table [decl_node->sub_decl->get_name ()].decl_node->pos ()) + ')',
			                    decl_node->sub_decl->pos ());

		table [decl_node->sub_decl->get_name ()] = symbol_entry {decl_node->sub_decl->get_name (),
		                                                         decl_node->sub_decl->get_type (),
		                                                         decl_node->sub_decl};
	}
}

void populate_table (symbol_table& table,
                     const formalDeclList& decl_list)
{
	for (const Node <formalDecl>& decl_node : decl_list.decls)
	{
		if (table.count (decl_node->get_name ()) > 0)
			throw syntax_error ("Redefined identifier (first defined at " +
			                    to_string (table [decl_node->get_name ()].decl_node->pos ()) + ')',
			                    decl_node->pos ());

		table [decl_node->get_name ()] = symbol_entry {decl_node->get_name (),
		                                               decl_node->get_type (),
		                                               decl_node};
	}
}

void populate_table (symbol_table& table,
                     const localDeclList& decl_list)
{
	for (const Node <varDecl>& decl_node : decl_list.decls)
	{
		if (table.count (decl_node->get_name ()) > 0)
			throw syntax_error ("Redefined identifier (first defined at " +
			                    to_string (table [decl_node->get_name ()].decl_node->pos ()) + ')',
			                    decl_node->pos ());

		table [decl_node->get_name ()] = symbol_entry {decl_node->get_name (),
		                                               decl_node->get_type (),
		                                               decl_node};
	}
}
