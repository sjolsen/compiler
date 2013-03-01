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
	varDecl,
	typeSpecifier,
	funDecl,
	formalDeclList,
	formalDecl,
	funBody,
	localDeclList,
	statementList,
	statement,
	compoundStmt,
	assignStmt,
	condStmt,
	loopStmt,
	returnStmt,
	var,
	expression,
	relop,
	addExpr,
	addop,
	term,
	mulop,
	factor,
	funcCallExpr,
	argList,
	terminal,
};



struct AST
{
	AST* parent;
	AST_type type;

	AST ();

	virtual std::vector <std::string> contents ();
};



typedef std::shared_ptr <AST> AST_node;



#include "auto.astdecls.hh"



struct terminal
	: AST
{
	const token& token_ref;

	terminal (const token& t);
	virtual std::vector <std::string> contents () override;
};


std::vector <std::string> lines (const AST_node& node);
std::string to_string (const AST& tree,
                       std::string line_prefix = "");
std::string to_string (AST_type type);



#endif
