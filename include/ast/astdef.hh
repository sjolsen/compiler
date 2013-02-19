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
	terminal
};



struct AST
{
	AST_type type;
	token terminal_data;

	std::weak_ptr <AST> parent;
	std::vector <std::shared_ptr <AST>> children;
};



std::string to_string (const AST& tree);
std::string to_string (AST_type type);



#endif
