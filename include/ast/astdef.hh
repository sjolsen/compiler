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
	terminal
};



struct AST
{
	AST* parent;
	AST_type type;

	AST ();

	virtual std::vector <std::string> contents () const;
};



typedef std::shared_ptr <AST> AST_node;
template <typename NodeType>
struct Node
	: std::shared_ptr <NodeType>
{
	template <typename... Args>
	Node (Args&&... args)
		: std::shared_ptr <NodeType> (std::forward <Args> (args)...)
	{
	}
};



struct program;
struct declList;
struct decl;
struct varDecl;
struct typeSpecifier;
struct funDecl;
struct formalDeclList;
struct formalDecl;
struct funBody;
struct localDeclList;
struct statementList;
struct statement;
struct compoundStmt;
struct assignStmt;
struct condStmt;
struct loopStmt;
struct returnStmt;
struct var;
struct expression;
struct relop;
struct addExpr;
struct addop;
struct term;
struct mulop;
struct factor;
struct funcCallExpr;
struct argList;
struct terminal;



#include "auto.astdecls.hh"



struct terminal
	: AST
{
	const token& token_ref;

	terminal (const token& t);
	virtual std::vector <std::string> contents () const ;
};


std::string to_string (const AST& tree,
                       std::string line_prefix = "");
std::string to_string (AST_type type);



#endif
