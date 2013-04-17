#include <ast/astdef.hh>
#include <ast/symbol_table.hh>
#include <ast/astprint.hh>

#include <algorithm>
#include <unordered_map>
#include <stdexcept>

using namespace std;



mc_type::mc_type (basic_mc_type _type,
                  int _size)
	: type (_type),
	  size (_size)
{
}

bool mc_type::operator == (const mc_type& other) const
{
	return type == other.type && size == other.size;
}

bool mc_type::operator != (const mc_type& other) const
{
	return !(*this == other);
}



AST::AST ()
	: parent (nullptr)
{
}

vector <string> AST::contents () const
{
	return {};
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

	throw logic_error ("Bad enumeration value (to_string (AST_type))");
}



string to_string (basic_mc_type t)
{
	switch (t)
	{
	case basic_mc_type::mc_void:
		return "void";
	case basic_mc_type::mc_int:
		return "int";
	case basic_mc_type::mc_char:
		return "char";
	}

	throw logic_error ("Bad enumeration value (to_string (mc_type))");
}



string to_string (mc_type t)
{
	if (t.size == 0)
		return to_string (t.type);
	if (t.size == -1)
		return to_string (t.type) + " []";
	return to_string (t.type) + " [" + to_string (t.size) + "]";
}



string to_string (const vector <mc_type>& parameter_types,
                  bool is_function)
{
	string type_string = to_string (parameter_types [0]);

	if (is_function)
		return type_string + " (" + accumulate (begin (parameter_types) + 1, end (parameter_types), string (),
		                                        [] (const string& type_string, mc_type t) { return (type_string.empty () ? string ("") : type_string + ' ' ) + to_string (t); }) + ")";
	return type_string;
}
