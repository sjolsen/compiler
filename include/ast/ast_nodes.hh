#ifndef AST_NODES_HH
#define AST_NODES_HH

#include <ast/astdef.hh>
#include <ast/symbol_table.hh>



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



void populate_table (symbol_table& table,
                     const declList& decl_list);
void populate_table (symbol_table& table,
                     const formalDeclList& decl_list);
void populate_table (symbol_table& table,
                     const localDeclList& decl_list);



#endif
