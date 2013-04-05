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



enum class basic_mc_type
{
	mc_void,
	mc_int,
	mc_char
};

struct mc_type
{
	basic_mc_type type;
	int size;

	mc_type (basic_mc_type _type = basic_mc_type::mc_void,
	         int _size = 0);

	bool operator == (const mc_type& other);
	bool operator != (const mc_type& other);
};



struct AST
{
	const AST* parent;
	AST_type type;

	AST ();

	virtual std::vector <std::string> contents () const;
	virtual std::vector <mc_type> get_type () const { return std::vector <mc_type> (); }; // FIXME
	virtual file_position pos () const { return file_position (); }; // FIXME
};



struct basic_decl
	: AST
{
	virtual std::string get_name () const = 0;
};



template <typename NodeType>
struct Node
	: std::unique_ptr <NodeType>
{
	template <typename... Args>
	Node (Args&&... args)
		: std::unique_ptr <NodeType> (std::forward <Args> (args)...)
	{
	}
};
typedef Node <AST> AST_node;




std::string to_string (basic_mc_type t);
std::string to_string (mc_type t);
std::string to_string (const std::vector <mc_type>& parameter_types,
                       bool is_function = false);
std::string to_string (const AST& tree,
                       std::string line_prefix = "");
std::string to_string (AST_type type);



#endif
