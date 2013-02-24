#include <ast/astdef.hh>

#include <algorithm>

using namespace std;



AST::AST ()
	: parent (nullptr),
	  tokenp (nullptr)
{
}



AST::AST (const token& t)
	: type (AST_type::terminal),
	  parent (nullptr),
	  tokenp (&t)
{
}



namespace
{
	vector <string> AST_to_string_impl (const AST& tree)
	{
		if (tree.type == AST_type::terminal)
			return {to_string (*tree.tokenp)};

		// if (tree.children.size () == 1)
		// 	return AST_to_string_impl (*tree.children.front ());

		vector <string> lines = {'(' + to_string (tree.type)};
		for (auto child : tree.children)
			for (string& line : AST_to_string_impl (*child))
				lines.emplace_back ("  " + move (line));

		lines.back () += ')';
		return lines;
	}
}



string to_string (const AST& tree,
                  string line_prefix)
{
	auto lines = AST_to_string_impl (tree);
	string output;
	for (const string& line : lines)
		output += line_prefix + line + '\n';

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
	};
}
