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



struct basic_decl
	: AST
{
	virtual std::string get_name () const = 0;
	virtual mc_type get_type () const = 0;
	virtual file_position pos () const = 0;
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


struct symbol_table;

struct program
	: AST
{
	std::shared_ptr <symbol_table> table;

	Node <declList> decl_list;

	program ();
	program (Node <declList> _decl_list);
	virtual std::vector <std::string> contents () const;
};



struct varDecl
	: basic_decl
{
	Node <typeSpecifier> type_spec;
	Node <terminal> name;
	Node <terminal> size;

	varDecl (Node <typeSpecifier> _type_spec,
	         Node <terminal> _name,
	         Node <terminal> _size);

	virtual std::vector <std::string> contents () const;
	virtual std::string get_name () const;
	virtual mc_type get_type () const;
	virtual file_position pos () const;
};



struct funDecl
	: basic_decl
{
	std::shared_ptr <symbol_table> table;

	Node <typeSpecifier> type_spec;
	Node <terminal> name;
	Node <formalDeclList> decl_list;
	Node <funBody> body;

	funDecl (Node <typeSpecifier> _type_spec,
	         Node <terminal> _name,
	         Node <formalDeclList> _decl_list,
	         Node <funBody> _body);

	virtual std::vector <std::string> contents () const;
	virtual std::string get_name () const;
	virtual mc_type get_type () const;
	virtual file_position pos () const;
};



struct formalDecl
	: basic_decl
{
	Node <typeSpecifier> type_spec;
	Node <terminal> name;
	bool is_array;

	formalDecl (Node <typeSpecifier> _type_spec,
	            Node <terminal> _name,
	            bool _is_array);

	virtual std::vector <std::string> contents () const;
	virtual std::string get_name () const;
	virtual mc_type get_type () const;
	virtual file_position pos () const;
};



struct terminal
	: AST
{
	const token& token_ref;

	terminal (const token& t);
	virtual std::vector <std::string> contents () const;
};


std::string to_string (mc_type t);
std::string to_string (const AST& tree,
                       std::string line_prefix = "");
std::string to_string (AST_type type);



#endif
