#include <ast/ast_nodes.hh>
#include <ast/astprint.hh>

#include <unordered_map>

using namespace std;



namespace
{
	static unordered_map <string, basic_mc_type> type_map = {{"void", basic_mc_type::mc_void},
	                                                         {"int", basic_mc_type::mc_int},
	                                                         {"char", basic_mc_type::mc_char}};

	template <typename Parent>
	void link (const Parent*)
	{
	}

	template <typename Parent, typename Child, typename... Rest>
	void link (const Parent* parent, Node <Child>& child, Rest&&... rest)
	{
		if (child)
			child->parent = parent;
		link (parent, forward <Rest> (rest)...);
	}

	template <typename Parent, typename Child, typename... Rest>
	void link (const Parent* parent, vector <Node <Child>>& children, Rest&&... rest)
	{
		for (Node <Child>& child : children)
			child->parent = parent;
		link (parent, forward <Rest> (rest)...);
	}
}



#include "auto.astdefs.cc"



// --- program ---

program::program (Node <declList>&& _decl_list)
	: table (new symbol_table),
	  decl_list (move (_decl_list))
{
	type = AST_type::program;
	decl_list->parent = this;
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

varDecl::varDecl (Node <typeSpecifier>&& _type_spec,
                  Node <terminal>&& _name,
                  Node <terminal>&& _size)
	: type_spec (move (_type_spec)),
	  name (move (_name)),
	  size (move (_size))
{
	type = AST_type::varDecl;
	link (this, type_spec, name, size);
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

vector <mc_type> varDecl::get_type () const
{
	try
	{
		return vector <mc_type> {mc_type (type_map.at (type_spec->kwd_node->token_ref.str), size ? size->token_ref.value : 0)};
	}
	catch (const exception&)
	{
		throw error ("Encountered an illegal variable declaration",
		             type_spec->kwd_node->token_ref.pos);
	}
}

file_position varDecl::pos () const
{
	return name->pos ();
}



// --- funDecl ---

funDecl::funDecl (Node <typeSpecifier>&& _type_spec,
                  Node <terminal>&& _name,
                  Node <formalDeclList>&& _decl_list,
                  Node <funBody>&& _body)
	: param_table (new symbol_table),
	  local_table (new symbol_table),
	  type_spec (move (_type_spec)),
	  name (move (_name)),
	  decl_list (move (_decl_list)),
	  body (move (_body))
{
	type = AST_type::funDecl;
	link (this, type_spec, name, decl_list, body);
}

vector <string> funDecl::contents () const
{
	return collect (lines (*param_table),
	                lines (*local_table),
	                valid (type_spec) ? lines (type_spec) : vector <string> {},
	                valid (name) ? lines (name) : vector <string> {},
	                valid (decl_list) ? lines (decl_list) : vector <string> {},
	                valid (body) ? lines (body) : vector <string> {});
}

string funDecl::get_name () const
{
	return name->token_ref.str;
}

vector <mc_type> funDecl::get_type () const
{
	vector <mc_type> parameter_types {mc_type (type_map.at (type_spec->kwd_node->token_ref.str), 0)};

	if (decl_list)
		for (const Node <formalDecl>& decl_node : decl_list->decls)
			parameter_types.emplace_back (decl_node->get_type () [0]);

	return parameter_types;
}

file_position funDecl::pos () const
{
	return type_spec->kwd_node->token_ref.pos;
}



// --- formalDecl ---

formalDecl::formalDecl (Node <typeSpecifier>&& _type_spec,
                        Node <terminal>&& _name,
                        bool _is_array)
	: type_spec (move (_type_spec)),
	  name (move (_name)),
	  is_array (_is_array)
{
	type = AST_type::formalDecl;
	link (this, type_spec, name);
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

vector <mc_type> formalDecl::get_type () const
{
	try
	{
		return vector <mc_type> {mc_type (type_map.at (type_spec->kwd_node->token_ref.str), is_array ? -1 : 0)};
	}
	catch (const exception&)
	{
		throw error ("Encountered an illegal variable declaration",
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

file_position terminal::pos () const
{
	return token_ref.pos;
}



// --- Symbol table population ---

void populate_table (symbol_table& table,
                     const declList& decl_list)
{
	for (const Node <decl>& decl_node : decl_list.decls)
	{
		if (table.count (decl_node->sub_decl->get_name ()) > 0)
			throw error ("Redefined identifier (first defined at " +
			             to_string (table [decl_node->sub_decl->get_name ()].decl_node->pos ()) + ')',
			             decl_node->sub_decl->pos ());

		table [decl_node->sub_decl->get_name ()] = symbol_entry {decl_node->sub_decl->get_name (),
		                                                         decl_node->sub_decl->get_type (),
		                                                         decl_node->sub_decl.get ()};
	}
}

void populate_table (symbol_table& table,
                     const formalDeclList& decl_list)
{
	for (const Node <formalDecl>& decl_node : decl_list.decls)
	{
		if (table.count (decl_node->get_name ()) > 0)
			throw error ("Redefined identifier (first defined at " +
			             to_string (table [decl_node->get_name ()].decl_node->pos ()) + ')',
			             decl_node->pos ());

		table [decl_node->get_name ()] = symbol_entry {decl_node->get_name (),
		                                               decl_node->get_type (),
		                                               decl_node.get ()};
	}
}

void populate_table (symbol_table& local_table,
                     const localDeclList& decl_list,
                     symbol_table& param_table)
{
	for (const Node <varDecl>& decl_node : decl_list.decls)
	{
		if (local_table.count (decl_node->get_name ()) > 0)
			throw error ("Redefined identifier (first defined at " +
			             to_string (local_table [decl_node->get_name ()].decl_node->pos ()) + ')',
			             decl_node->pos ());

		if (param_table.count (decl_node->get_name ()) > 0)
			throw error ("Redefined identifier (first defined at " +
			             to_string (param_table [decl_node->get_name ()].decl_node->pos ()) + ')',
			             decl_node->pos ());

		local_table [decl_node->get_name ()] = symbol_entry {decl_node->get_name (),
		                                                     decl_node->get_type (),
		                                                     decl_node.get ()};
	}
}
