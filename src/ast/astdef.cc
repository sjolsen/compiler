#include "../../include/ast/astdef.hh"
#include <algorithm>

using namespace std;



namespace
{
	vector <string> AST_to_string_impl (const AST& tree)
	{
		if (tree.type == AST_type::terminal)
			return {to_string (tree.terminal_data)};

		vector <string> lines;
		for (auto child : tree.children)
			for (string& line : AST_to_string_impl (*child))
				lines.emplace_back (move (line));

		auto sexp_prefix = '(' + to_string (tree.type) + ' ';
		lines.front () = sexp_prefix + lines.front ();
		transform (begin (lines) + 1, end (lines), begin (lines) + 1,
		           [&] (const string& line) { return string (sexp_prefix.size (), ' ') + line; });

		lines.back () += ')';
		return lines;
	}
}



string to_string (const AST& tree)
{
	auto lines = AST_to_string_impl (tree);
	string output;
	for (const string& line : lines)
		output += line + '\n';

	return output;
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
	case AST_type::varDeclStmt:
		return "varDeclStmt";
	case AST_type::varDecl:
		return "varDecl";
	case AST_type::intTypeSpec:
		return "intTypeSpec";
	case AST_type::typeSpecifier:
		return "typeSpecifier";
	case AST_type::initBraceList:
		return "initBraceList";
	case AST_type::exprList:
		return "exprList";
	case AST_type::funDecl:
		return "funDecl";
	case AST_type::formalDeclList:
		return "formalDeclList";
	case AST_type::formalDecl:
		return "formalDecl";
	case AST_type::statementList:
		return "statementList";
	case AST_type::statement:
		return "statement";
	case AST_type::compoundStmt:
		return "compoundStmt";
	case AST_type::condStmt:
		return "condStmt";
	case AST_type::loopStmt:
		return "loopStmt";
	case AST_type::returnStmt:
		return "returnStmt";
	case AST_type::lvalue:
		return "lvalue";
	case AST_type::rvalue:
		return "rvalue";
	case AST_type::expression:
		return "expression";
	case AST_type::equivExpr:
		return "equivExpr";
	case AST_type::equivop:
		return "equivop";
	case AST_type::relExpr:
		return "relExpr";
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
	case AST_type::funcCallExpr:
		return "funcCallExpr";
	case AST_type::argList:
		return "argList";
	};
}
