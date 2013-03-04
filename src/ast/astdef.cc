#include <ast/astdef.hh>

#include <algorithm>
#include <unordered_map>

using namespace std;



namespace
{
	vector <string> collect (vector <string>&& v)
	{ return v; }

	template <typename... Rest>
	vector <string> collect (vector <string>&& first,
	                         Rest&&... rest)
	{
		auto&& rest_lines = collect (forward <Rest> (rest)...);
		first.reserve (first.size () + rest_lines.size ());
		for (string& line : rest_lines)
			first.emplace_back (move (line));
		return first;
	}

	vector <string> lines (const AST& tree)
	{
		vector <string> lines = {'(' + to_string (tree.type)};
		for (string& line : tree.contents ())
			lines.emplace_back ("  " + move (line));
		lines.back () += ')';

		return lines;
	}

	vector <string> lines (const AST_node& node)
	{
		return lines (*node);
	}

	template <typename NodeType>
	vector <string> lines (const vector <NodeType>& nodes)
	{
		vector <string> output;
		for (const AST_node& node : nodes)
			output = collect (move (output), lines (node));
		return output;
	}

	vector <string> lines (bool b)
	{ return b ? vector <string> {"[]"} : vector <string> {}; }

	template <typename NodeType>
	bool valid (const vector <NodeType>& nodes)
	{ return !nodes.empty (); }

	bool valid (const AST_node& node)
	{ return node != nullptr; }

	bool valid (bool b)
	{ return b; }
}



AST::AST ()
	: parent (nullptr)
{
}

vector <string> AST::contents () const
{
	return {};
}



#include "auto.astdefs.cc"



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
	  body (_body)
{
	type = AST_type::funDecl;
}

std::vector <std::string> funDecl::contents () const
{
	return collect (valid (type_spec) ? lines (type_spec) : vector <string> {},
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



string to_string (const AST& tree,
                  string line_prefix)
{
	auto tree_lines = lines (tree);

	return accumulate (begin (tree_lines), end (tree_lines), string (),
	                   [&] (const string& output, const string& line) { return output + line_prefix + line + '\n'; });
}

std::string to_string (AST_type type)
{
	switch (type)
	{
	case AST_type::program:
		return "program";
	case AST_type::declList:
		return "declList";
	case AST_type::decl:
		return "decl";
	case AST_type::varDecl:
		return "varDecl";
	case AST_type::typeSpecifier:
		return "typeSpecifier";
	case AST_type::funDecl:
		return "funDecl";
	case AST_type::formalDeclList:
		return "formalDeclList";
	case AST_type::formalDecl:
		return "formalDecl";
	case AST_type::funBody:
		return "funBody";
	case AST_type::localDeclList:
		return "localDeclList";
	case AST_type::statementList:
		return "statementList";
	case AST_type::statement:
		return "statement";
	case AST_type::compoundStmt:
		return "compoundStmt";
	case AST_type::assignStmt:
		return "assignStmt";
	case AST_type::condStmt:
		return "condStmt";
	case AST_type::loopStmt:
		return "loopStmt";
	case AST_type::returnStmt:
		return "returnStmt";
	case AST_type::var:
		return "var";
	case AST_type::expression:
		return "expression";
	case AST_type::relop:
		return "relop";
	case AST_type::addExpr:
		return "addExpr";
	case AST_type::addop:
		return "addop";
	case AST_type::term:
		return "term";
	case AST_type::mulop:
		return "mulop";
	case AST_type::factor:
		return "factor";
	case AST_type::funcCallExpr:
		return "funcCallExpr";
	case AST_type::argList:
		return "argList";
	case AST_type::terminal:
		return "terminal";
	};
}
