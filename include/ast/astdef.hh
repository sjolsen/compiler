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



enum class mc_type
{
	mc_void,
	integer,
	character,
	int_array,
	char_array,
	function
};



struct AST
{
	AST* parent;
	AST_type type;

	AST ();

	virtual std::vector <std::string> contents () const;
};



struct basic_decl
	: AST
{
	virtual std::string get_name () const = 0;
	virtual mc_type get_type () const = 0;
	virtual file_position pos () const = 0;
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



std::string to_string (mc_type t);
std::string to_string (const AST& tree,
                       std::string line_prefix = "");
std::string to_string (AST_type type);



#endif
