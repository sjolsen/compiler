#include <ast/astdef.hh>
#include <ast/symbol_table.hh>
#include <ast/astprint.hh>

#include <algorithm>
#include <unordered_map>

using namespace std;



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
}



string to_string (mc_type t)
{
	switch (t)
	{
	case mc_type::mc_void:
		return "void";
	case mc_type::integer:
		return "int";
	case mc_type::character:
		return "char";
	case mc_type::int_array:
		return "int []";
	case mc_type::char_array:
		return "char []";
	case mc_type::function:
		return "function";
	}
}
