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
	AST_type type;
	const token* tokenp;

	AST* parent;
	std::vector <std::shared_ptr <AST>> children;

	AST ();
	AST (const token& t);

	AST (AST&&) = default;
	AST& operator = (AST&&) = default;

	template <typename... Args>
	AST (AST_type node_type,
	     Args&&... args)
		: type (node_type),
		  tokenp (nullptr),
		  children {std::forward <Args> (args)...}
	{
		for (auto& child : children)
			child->parent = this;
	}
};



std::string to_string (const AST& tree,
                       std::string line_prefix = "");
std::string to_string (AST_type type);



#endif
