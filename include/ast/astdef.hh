#ifndef ASTDEF_HH
#define ASTDEF_HH

#include <tokenizer.hh>

#include <vector>
#include <memory>
#include <string>



enum class AST_type
{
	program,
	declList,
	decl,
	varDeclStmt,
	varDecl,
	stringDecl,
	intTypeSpec,
	typeSpecifier,
	initBraceList,
	exprList,
	funDecl,
	formalDeclList,
	formalDecl,
	statementList,
	statement,
	compoundStmt,
	condStmt,
	loopStmt,
	returnStmt,
	lvalue,
	rvalue,
	expression,
	equivExpr,
	equivop,
	relExpr,
	relop,
	addExpr,
	addop,
	term,
	mulop,
	funcCallExpr,
	argList,
	terminal,
	epsilon
};



struct AST
{
	AST_type type;
	const token* tokenp;

	std::weak_ptr <AST> parent;
	std::vector <std::shared_ptr <AST>> children;

	AST ();
	AST (const token& t);

	AST (AST&&) = default;

	template <typename... Args>
	AST (AST_type node_type,
	     Args...&& args)
		: type (node_type),
		  tokenp (nullptr),
		  children {std::forward <Args> (args)...}
	{
	}
};



std::string to_string (const AST& tree);
std::string to_string (AST_type type);



#endif
