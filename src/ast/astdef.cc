#include <ast/astdef.hh>

#include <algorithm>

using namespace std;



namespace
{
	vector <string> collect (vector&& <string> v)
	{ return v; }

	template <typename... Rest>
	vector <string> collect (vector&& <string> first,
	                         Rest&&... rest)
	{
		auto&& rest_lines = collect (forward <Rest> (rest)...);
		first.reserve (first.size () + rest_lines.size ());
		for (string& line : rest_lines)
			first.emplace_back (move (line));
		return first;
	}

	vector <string> lines (const vector <AST_node>& nodes)
	{
		vector <string> output;
		for (const AST_node& node : nodes)
			output = collect (move (output), node.to_string ());
		return output;
	}

	bool operator bool (const vector <AST_node>& nodes)
	{ return !nodes.empty (); }
}



AST::AST ()
	: parent (nullptr),
{
}

virtual vector <string> AST::contents ()
{
	return {};
}



#include "auto.astdefs.cc"



terminal::terminal (const token& t)
	: token_ref (t)
{
}

virtual vector <string> terminal::contents ()
{
	return {to_string (token_ref)};
}



vector <string> lines (const AST_node& node)
{
	vector <string> lines = {'(' + to_string (tree.type)};
	for (string& line : node->contents ())
		lines.emplace_back ("  " + move (line));
	lines.back () += ')';

	return lines;
}

string to_string (const AST& tree,
                  string line_prefix)
{
	auto lines = tree.to_string ();

	return accumulate (begin (lines), end (lines), "",
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
	};
}
